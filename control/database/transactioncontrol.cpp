#include "transactioncontrol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>

TransactionControl::TransactionControl( QObject* parent ) :
    QObject( parent ) {

    connect( &_supabaseApi, &SupabaseApi::requestFinished, this, &TransactionControl::handleRequestFinished );
    connect( &_supabaseApi, &SupabaseApi::requestFailed, this, &TransactionControl::handleRequestFailed );
}

bool TransactionControl::busy() const {
    return _busy;
}

void TransactionControl::createTransaction( const QString& userName, double amount, int type, int description ) {

    Q_UNUSED( userName ); // o usuário é resolvido no servidor a partir da sessão autenticada (auth.uid())

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

    _pendingAmount = amount;
    _pendingType = parsedType;
    _pendingDescription = parsedDescription;

    _requestType = RequestType::CreateTransaction;
    setBusy( true );
    emit showLoading( true );

    QJsonObject params;

    params[ "p_amount" ] = _pendingAmount;
    params[ "p_type" ] = transactionTypeToString( _pendingType );
    params[ "p_description" ] = transactionDescriptionToString( _pendingDescription );

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

        resetPendingState();

        emit success( QLocale::system().toCurrencyString( newBalance ), newBalance );

        return;
    }

    _requestType = RequestType::None;
    emit showLoading( false );
    setBusy( false );
    resetPendingState();
    emit fail( "Resposta inesperada do Supabase." );
}

void TransactionControl::handleRequestFailed( const QString& error ) {

    _requestType = RequestType::None;

    emit showLoading( false );
    setBusy( false );
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
