#ifndef SUPABASEAPI_H
#define SUPABASEAPI_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>

#include <functional>

class SupabaseApi : public QObject {
    Q_OBJECT

public:
    explicit SupabaseApi( QObject* parent = nullptr );

    void get( const QString& endpoint, bool isRetry = false );
    void post( const QString& endpoint, const QJsonObject& data, bool isRetry = false );
    void patch( const QString& endpoint, const QJsonObject& data, bool isRetry = false );
    void rpc( const QString& functionName, const QJsonObject& params, bool isRetry = false );

    void authSignUp( const QString& email, const QString& password, const QJsonObject& metadata );
    void authSignInWithPassword( const QString& email, const QString& password );
    void authRefreshToken( const QString& refreshToken );
    void authUpdateUser( const QJsonObject& data, bool isRetry = false );
    void authSignOut();

    static void setAccessToken( const QString& accessToken );
    static void setSession( const QString& accessToken, const QString& refreshToken );
    static void clearAccessToken();
    static bool hasAccessToken();

signals:
    void requestFinished( const QJsonDocument& response );
    void requestFailed( const QString& error, bool isOffline );

private:
    struct PendingRequest {
        bool retryable = false;
        bool isRetry = false;
        bool isRefreshForRetry = false;
        std::function<void()> resend;
    };

    void handleReply( QNetworkReply* reply );
    void refreshAndRetry( const PendingRequest& original );
    static QString bearerToken();

private:
    QNetworkAccessManager _networkManager;
    QHash<QNetworkReply*, PendingRequest> _pending;
    static QString s_accessToken;
    static QString s_refreshToken;
};

#endif
