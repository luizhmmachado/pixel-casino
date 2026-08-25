#ifndef TRANSACTIONCONTROL_H
#define TRANSACTIONCONTROL_H

#include "supabaseapi.h"
#include "offlinequeue.h"

#include <QObject>
#include <QTimer>

class TransactionControl : public QObject {
    Q_OBJECT

    Q_PROPERTY( bool busy READ busy NOTIFY busyChanged )
    Q_PROPERTY( int pendingSyncCount READ pendingSyncCount NOTIFY pendingSyncCountChanged )

public:
    explicit TransactionControl( QObject* parent = nullptr );

    enum class TransactionType {
        Add = 0,
        Subtract
    };
    Q_ENUM( TransactionType )

    enum class TransactionDescription {
        Deposit = 0,
        Withdraw,
        BetLoss,
        BetWin
    };
    Q_ENUM( TransactionDescription )

    bool busy() const;
    int pendingSyncCount() const;

    Q_INVOKABLE void setActiveUser( const QString& userName );
    Q_INVOKABLE void createTransaction( const QString& userName, double amount, int type, int description );
    Q_INVOKABLE void setKnownBalance( double balance );
    Q_INVOKABLE void syncPendingTransactions();

signals:
    void busyChanged();
    void pendingSyncCountChanged();
    void showLoading( bool show );
    void success( const QString& formattedBalance, double balance );
    void fail( const QString& msg );

private slots:
    void handleRequestFinished( const QJsonDocument& response );
    void handleRequestFailed( const QString& error, bool isOffline );

private:
    enum class RequestType {
        None,
        CreateTransaction,
        SyncTransaction
    };

    void setBusy( bool busy );
    QString transactionTypeToString( TransactionType type ) const;
    QString transactionDescriptionToString( TransactionDescription description ) const;
    bool parseTransactionType( int value, TransactionType& outType ) const;
    bool parseTransactionDescription( int value, TransactionDescription& outDescription ) const;
    void resetPendingState();
    void applyOffline();
    void updateBalance( double newBalance );

    SupabaseApi _supabaseApi;
    OfflineQueue _offlineQueue;
    QTimer _syncTimer;

    RequestType _requestType = RequestType::None;
    bool _busy = false;

    double _balance = 0.0;
    double _pendingAmount = 0.0;
    TransactionType _pendingType = TransactionType::Add;
    TransactionDescription _pendingDescription = TransactionDescription::Deposit;
    qint64 _pendingSyncId = 0;
};

#endif
