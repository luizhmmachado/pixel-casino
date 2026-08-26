#include "offlinequeue.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

namespace {

const char* CONNECTION_NAME = "pixel_casino_offline_queue";

QString databasePath() {

    const QString dir = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );

    QDir().mkpath( dir );

    return dir + "/offline_queue.sqlite3";
}

}

OfflineQueue::OfflineQueue() {
    ensureSchema();
}

void OfflineQueue::ensureSchema() {

    QSqlDatabase db = QSqlDatabase::contains( CONNECTION_NAME )
        ? QSqlDatabase::database( CONNECTION_NAME )
        : QSqlDatabase::addDatabase( "QSQLITE", CONNECTION_NAME );

    if ( !db.isOpen() ) {
        db.setDatabaseName( databasePath() );

        if ( !db.open() ) {
            qWarning() << "OfflineQueue: não foi possível abrir o banco local:" << db.lastError().text();
            _ready = false;
            return;
        }
    }

    QSqlQuery query( db );

    const bool createdTransactions = query.exec(
        "CREATE TABLE IF NOT EXISTS pending_transactions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_name TEXT NOT NULL, "
        "amount REAL NOT NULL, "
        "type TEXT NOT NULL, "
        "description TEXT NOT NULL, "
        "created_at TEXT NOT NULL"
        ")"
        );

    const bool createdState = query.exec(
        "CREATE TABLE IF NOT EXISTS local_state ("
        "user_name TEXT NOT NULL, "
        "key TEXT NOT NULL, "
        "value TEXT, "
        "PRIMARY KEY (user_name, key)"
        ")"
        );

    if ( !createdTransactions || !createdState ) {
        qWarning() << "OfflineQueue: falha ao criar schema local:" << query.lastError().text();
        _ready = false;
        return;
    }

    bool hasTransactionUser = false;
    if ( query.exec( "PRAGMA table_info(pending_transactions)" ) ) {
        while ( query.next() ) {
            if ( query.value( 1 ).toString() == "user_name" ) {
                hasTransactionUser = true;
                break;
            }
        }
    }

    if ( !hasTransactionUser && !query.exec( "ALTER TABLE pending_transactions ADD COLUMN user_name TEXT NOT NULL DEFAULT ''" ) ) {
        qWarning() << "OfflineQueue: falha ao atualizar schema de transações:" << query.lastError().text();
        _ready = false;
        return;
    }

    _ready = true;
}

bool OfflineQueue::isReady() const {
    return _ready;
}

void OfflineQueue::setActiveUser( const QString& userName ) {
    _activeUser = userName.trimmed().toLower();

    if ( !_ready || _activeUser.isEmpty() ) {
        return;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );
    query.prepare( "UPDATE pending_transactions SET user_name = ? WHERE user_name = ''" );
    query.bindValue( 0, _activeUser );

    if ( !query.exec() ) {
        qWarning() << "OfflineQueue::setActiveUser falhou ao migrar transações:" << query.lastError().text();
    }
}

double OfflineQueue::cachedBalance() const {

    if ( !_ready || _activeUser.isEmpty() ) {
        return 0.0;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    query.prepare( "SELECT value FROM local_state WHERE user_name = :user AND key = 'balance'" );
    query.bindValue( ":user", _activeUser );

    if ( !query.exec() || !query.next() ) {
        return 0.0;
    }

    return query.value( 0 ).toDouble();
}

void OfflineQueue::setCachedBalance( double balance ) {

    if ( !_ready || _activeUser.isEmpty() ) {
        return;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    query.prepare( "INSERT INTO local_state (user_name, key, value) VALUES (:user, 'balance', :insert_value) "
                   "ON CONFLICT(user_name, key) DO UPDATE SET value = :update_value" );
    query.bindValue( ":user", _activeUser );
    query.bindValue( ":insert_value", QString::number( balance, 'f', 2 ) );
    query.bindValue( ":update_value", QString::number( balance, 'f', 2 ) );

    if ( !query.exec() ) {
        qWarning() << "OfflineQueue::setCachedBalance falhou:" << query.lastError().text();
    }
}

void OfflineQueue::enqueue( double amount, const QString& type, const QString& description ) {

    if ( !_ready || _activeUser.isEmpty() ) {
        return;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    query.prepare( "INSERT INTO pending_transactions (user_name, amount, type, description, created_at) "
                   "VALUES (:user, :amount, :type, :description, :created_at)" );
    query.bindValue( ":user", _activeUser );
    query.bindValue( ":amount", amount );
    query.bindValue( ":type", type );
    query.bindValue( ":description", description );
    query.bindValue( ":created_at", QDateTime::currentDateTimeUtc().toString( Qt::ISODateWithMs ) );

    if ( !query.exec() ) {
        qWarning() << "OfflineQueue::enqueue falhou:" << query.lastError().text();
    }
}

QVector<OfflineQueue::PendingTransaction> OfflineQueue::pending() const {

    QVector<PendingTransaction> result;

    if ( !_ready || _activeUser.isEmpty() ) {
        return result;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    if ( !query.prepare( "SELECT id, amount, type, description FROM pending_transactions WHERE user_name = ? ORDER BY id ASC" ) ) {
        qWarning() << "OfflineQueue::pending falhou ao preparar consulta:" << query.lastError().text();
        return result;
    }

    query.bindValue( 0, _activeUser );

    if ( !query.exec() ) {
        qWarning() << "OfflineQueue::pending falhou:" << query.lastError().text();
        return result;
    }

    while ( query.next() ) {

        PendingTransaction transaction;

        transaction.id = query.value( 0 ).toLongLong();
        transaction.amount = query.value( 1 ).toDouble();
        transaction.type = query.value( 2 ).toString();
        transaction.description = query.value( 3 ).toString();

        result.append( transaction );
    }

    return result;
}

void OfflineQueue::remove( qint64 id ) {

    if ( !_ready || _activeUser.isEmpty() ) {
        return;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    query.prepare( "DELETE FROM pending_transactions WHERE id = :id AND user_name = :user" );
    query.bindValue( ":id", id );
    query.bindValue( ":user", _activeUser );

    if ( !query.exec() ) {
        qWarning() << "OfflineQueue::remove falhou:" << query.lastError().text();
    }
}

int OfflineQueue::pendingCount() const {

    if ( !_ready || _activeUser.isEmpty() ) {
        return 0;
    }

    QSqlQuery query( QSqlDatabase::database( CONNECTION_NAME ) );

    query.prepare( "SELECT COUNT(*) FROM pending_transactions WHERE user_name = :user" );
    query.bindValue( ":user", _activeUser );

    if ( !query.exec() || !query.next() ) {
        return 0;
    }

    return query.value( 0 ).toInt();
}
