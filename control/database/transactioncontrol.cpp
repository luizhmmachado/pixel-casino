#include "transactioncontrol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>

TransactionControl::TransactionControl( QObject* parent ) :
    QObject( parent ) {

    connect( &_supabaseApi, &SupabaseApi::requestFinished, this, &TransactionControl::handleRequestFinished );
    connect( &_supabaseApi, &SupabaseApi::requestFailed, this, &TransactionControl::handleRequestFailed );

    _balance = _offlineQueue.cachedBalance();

    _syncTimer.setInterval( 20000 );
    connect( &_syncTimer, &QTimer::timeout, this, &TransactionControl::syncPendingTransactions );
    _syncTimer.start();

    syncPendingTransactions();
}

bool TransactionControl::busy() const {
    return _busy;
}

int TransactionControl::pendingSyncCount() const {
    return _offlineQueue.pendingCount();
}

void TransactionControl::setKnownBalance( double balance ) {

    if ( _offlineQueue.pendingCount() > 0 ) {
        return;
    }

    updateBalance( balance );
}

void TransactionControl::updateBalance( double newBalance ) {
    _balance = newBalance;
    _offlineQueue.setCachedBalance( newBalance );
}

void TransactionControl::setActiveUser( const QString& userName ) {

    _offlineQueue.setActiveUser( userName );
    _balance = _offlineQueue.cachedBalance();

    emit pendingSyncCountChanged();
}

void TransactionControl::createTransaction( const QString& userName, double amount, int type, int description ) {

    _offlineQueue.setActiveUser( userName );

    if ( _busy ) {
        emit fail( "Outra transacao ainda esta sendo processada." );
        return;
    }

    if ( amount <= 0.0 ) {
        emit fail( "Valor da transacao invalido." );
        return;
    }

    TransactionType parsedType;
    TransactionDescription parsedDescription;

    if ( !parseTransactionType( type, parsedType ) ) {
        emit fail( "Tipo de transacao invalido." );
        return;
    }

    if ( !parseTransactionDescription( description, parsedDescription ) ) {
        emit fail( "Descricao de transacao invalida." );
        return;
    }

    if ( parsedType == TransactionType::Subtract && amount > _balance ) {
        emit fail( "Saldo insuficiente." );
        return;
    }

    _pendingAmount = amount;
    _pendingType = parsedType;
    _pendingDescription = parsedDescription;

    setBusy( true );
    emit showLoading( true );

    if ( _offlineQueue.pendingCount() > 0 ) {
        applyOffline();
        return;
    }

    _requestType = RequestType::CreateTransaction;

    QJsonObject params;

    params[ "p_amount" ] = _pendingAmount;
    params[ "p_type" ] = transactionTypeToString( _pendingType );
    params[ "p_description" ] = transactionDescriptionToString( _pendingDescription );

    _supabaseApi.rpc( "create_transaction", params );
}

void TransactionControl::applyOffline() {

    double newBalance = _balance;

    if ( _pendingType == TransactionType::Add ) {
        newBalance += _pendingAmount;
    } else {
        newBalance -= _pendingAmount;
    }

    if ( newBalance < 0.0 ) {
        _requestType = RequestType::None;
        setBusy( false );
        emit showLoading( false );
        resetPendingState();
        emit fail( "Saldo insuficiente." );
        return;
    }

    _offlineQueue.enqueue( _pendingAmount, transactionTypeToString( _pendingType ), transactionDescriptionToString( _pendingDescription ) );
    updateBalance( newBalance );

    _requestType = RequestType::None;
    setBusy( false );
    emit showLoading( false );

    const QString formattedBalance = QLocale::system().toCurrencyString( newBalance );

    resetPendingState();

    emit pendingSyncCountChanged();
    emit success( formattedBalance, newBalance );
}

void TransactionControl::syncPendingTransactions() {

    if ( _busy || _requestType != RequestType::None ) {
        return;
    }

    if ( !SupabaseApi::hasAccessToken() ) {
        return;
    }

    const QVector<OfflineQueue::PendingTransaction> items = _offlineQueue.pending();

    if ( items.isEmpty() ) {
        return;
    }

    const OfflineQueue::PendingTransaction next = items.first();

    _pendingSyncId = next.id;
    _requestType = RequestType::SyncTransaction;
    setBusy( true );

    QJsonObject params;

    params[ "p_amount" ] = next.amount;
    params[ "p_type" ] = next.type;
    params[ "p_description" ] = next.description;

    _supabaseApi.rpc( "create_transaction", params );
}

void TransactionControl::handleRequestFinished( const QJsonDocument& response ) {

    if ( _requestType == RequestType::CreateTransaction ) {

        _requestType = RequestType::None;
        emit showLoading( false );
        setBusy( false );

        if ( !response.isArray() || response.array().isEmpty() ) {
            resetPendingState();
            emit fail( "Não foi possível concluir a transação." );
            return;
        }

        const QJsonObject result = response.array().first().toObject();
        const double newBalance = result.value( "balance" ).toDouble();

        updateBalance( newBalance );
        resetPendingState();

        emit success( QLocale::system().toCurrencyString( newBalance ), newBalance );

        return;
    }

    if ( _requestType == RequestType::SyncTransaction ) {

        _requestType = RequestType::None;
        setBusy( false );

        if ( response.isArray() && !response.array().isEmpty() ) {
            const double newBalance = response.array().first().toObject().value( "balance" ).toDouble();
            updateBalance( newBalance );
        }

        _offlineQueue.remove( _pendingSyncId );
        emit pendingSyncCountChanged();

        syncPendingTransactions();

        return;
    }

    _requestType = RequestType::None;
    emit showLoading( false );
    setBusy( false );
    resetPendingState();
    emit fail( "Resposta inesperada do Supabase." );
}

void TransactionControl::handleRequestFailed( const QString& error, bool isOffline ) {

    if ( _requestType == RequestType::SyncTransaction ) {

        _requestType = RequestType::None;
        setBusy( false );

        if ( isOffline ) {
            return;
        }

        const QString lowerError = error.toLower();
        const bool isKnownRejection = lowerError.contains( "insufficient_balance" ) || lowerError.contains( "invalid_amount" ) ||
            lowerError.contains( "invalid_type" ) || lowerError.contains( "invalid_description" ) || lowerError.contains( "user_not_found" );

        if ( !isKnownRejection ) {
            qWarning() << "TransactionControl: falha inesperada ao sincronizar, tentará novamente:" << error;
            return;
        }

        qWarning() << "TransactionControl: descartando transação offline pendente após erro do servidor:" << error;
        _offlineQueue.remove( _pendingSyncId );
        emit pendingSyncCountChanged();
        syncPendingTransactions();

        return;
    }

    _requestType = RequestType::None;

    emit showLoading( false );
    setBusy( false );

    if ( isOffline ) {
        applyOffline();
        return;
    }

    resetPendingState();

    const QString lowerError = error.toLower();

    if ( lowerError.contains( "insufficient_balance" ) ) {
        emit fail( "Saldo insuficiente." );
        return;
    }

    if ( lowerError.contains( "user_not_found" ) ) {
        emit fail( "Usuario nao encontrado." );
        return;
    }

    emit fail( error );
}

void TransactionControl::setBusy( bool busy ) {

    if ( _busy == busy ) {
        return;
    }

    _busy = busy;
    emit busyChanged();
}

QString TransactionControl::transactionTypeToString( TransactionType type ) const {

    switch ( type ) {
        case TransactionType::Add:
            return "ADD";
        case TransactionType::Subtract:
            return "SUBTRACT";
    }

    return "ADD";
}

QString TransactionControl::transactionDescriptionToString( TransactionDescription description ) const {

    switch ( description ) {
        case TransactionDescription::Deposit:
            return "DEPOSIT";
        case TransactionDescription::Withdraw:
            return "WITHDRAW";
        case TransactionDescription::BetLoss:
            return "BET_LOSS";
        case TransactionDescription::BetWin:
            return "BET_WIN";
    }

    return "DEPOSIT";
}

bool TransactionControl::parseTransactionType( int value, TransactionType& outType ) const {

    if ( value == static_cast<int>( TransactionType::Add ) ) {
        outType = TransactionType::Add;
        return true;
    }

    if ( value == static_cast<int>( TransactionType::Subtract ) ) {
        outType = TransactionType::Subtract;
        return true;
    }

    return false;
}

bool TransactionControl::parseTransactionDescription( int value, TransactionDescription& outDescription ) const {

    if ( value == static_cast<int>( TransactionDescription::Deposit ) ) {
        outDescription = TransactionDescription::Deposit;
        return true;
    }

    if ( value == static_cast<int>( TransactionDescription::Withdraw ) ) {
        outDescription = TransactionDescription::Withdraw;
        return true;
    }

    if ( value == static_cast<int>( TransactionDescription::BetLoss ) ) {
        outDescription = TransactionDescription::BetLoss;
        return true;
    }

    if ( value == static_cast<int>( TransactionDescription::BetWin ) ) {
        outDescription = TransactionDescription::BetWin;
        return true;
    }

    return false;
}

void TransactionControl::resetPendingState() {
    _pendingAmount = 0.0;
    _pendingType = TransactionType::Add;
    _pendingDescription = TransactionDescription::Deposit;
}
