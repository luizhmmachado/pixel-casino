#ifndef TRANSACTIONCONTROL_H
#define TRANSACTIONCONTROL_H

#include "supabaseapi.h"

#include <QObject>

class TransactionControl : public QObject {
    Q_OBJECT

    Q_PROPERTY( bool busy READ busy NOTIFY busyChanged )

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

    Q_INVOKABLE void createTransaction( const QString& userName, double amount, int type, int description );

signals:
    void busyChanged();
    void showLoading( bool show );
    void success( const QString& formattedBalance, double balance );
    void fail( const QString& msg );

private slots:
    void handleRequestFinished( const QJsonDocument& response );
    void handleRequestFailed( const QString& error );

private:
    enum class RequestType {
        None,
        CreateTransaction
    };

    void setBusy( bool busy );
    QString transactionTypeToString( TransactionType type ) const;
    QString transactionDescriptionToString( TransactionDescription description ) const;
    bool parseTransactionType( int value, TransactionType& outType ) const;
    bool parseTransactionDescription( int value, TransactionDescription& outDescription ) const;
    void resetPendingState();

    SupabaseApi _supabaseApi;

    RequestType _requestType = RequestType::None;
    bool _busy = false;

    double _pendingAmount = 0.0;
    TransactionType _pendingType = TransactionType::Add;
    TransactionDescription _pendingDescription = TransactionDescription::Deposit;
};

#endif // TRANSACTIONCONTROL_H
