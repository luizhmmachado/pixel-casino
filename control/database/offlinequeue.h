#ifndef OFFLINEQUEUE_H
#define OFFLINEQUEUE_H

#include <QVector>
#include <QString>

class OfflineQueue {

public:
    struct PendingTransaction {
        qint64 id = 0;
        double amount = 0.0;
        QString type;
        QString description;
    };

    explicit OfflineQueue();

    bool isReady() const;

    void setActiveUser( const QString& userName );

    double cachedBalance() const;
    void setCachedBalance( double balance );

    void enqueue( double amount, const QString& type, const QString& description );
    QVector<PendingTransaction> pending() const;
    void remove( qint64 id );
    int pendingCount() const;

private:
    void ensureSchema();

    QString _activeUser;
    bool _ready = false;
};

#endif
