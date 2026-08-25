#include "supabaseapi.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

namespace {

const QString SUPABASE_URL =
    "https://iokafhbtumsomvxknyju.supabase.co";

const QString SUPABASE_KEY =
    "sb_publishable_QVgUk_xaOWb0T5JThyJt9w_4IpRGsTL";

}

QString SupabaseApi::s_accessToken;
QString SupabaseApi::s_refreshToken;

SupabaseApi::SupabaseApi( QObject* parent ) :
    QObject( parent ) {

    connect(
        &_networkManager,
        &QNetworkAccessManager::finished,
        this,
        &SupabaseApi::handleReply
        );
}

void SupabaseApi::setAccessToken( const QString& accessToken ) {
    s_accessToken = accessToken;
}

void SupabaseApi::setSession( const QString& accessToken, const QString& refreshToken ) {
    s_accessToken = accessToken;
    s_refreshToken = refreshToken;
}

void SupabaseApi::clearAccessToken() {
    s_accessToken.clear();
    s_refreshToken.clear();
}

bool SupabaseApi::hasAccessToken() {
    return !s_accessToken.isEmpty();
}

QString SupabaseApi::bearerToken() {
    return s_accessToken.isEmpty() ? SUPABASE_KEY : s_accessToken;
}

void SupabaseApi::get( const QString& endpoint, bool isRetry ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/rest/v1/" + endpoint )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    QNetworkReply* reply = _networkManager.get( request );

    PendingRequest pending;
    pending.retryable = !s_accessToken.isEmpty();
    pending.isRetry = isRetry;
    pending.resend = [this, endpoint]() { get( endpoint, true ); };

    _pending.insert( reply, pending );
}

void SupabaseApi::post(
    const QString& endpoint,
    const QJsonObject& data,
    bool isRetry ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/rest/v1/" + endpoint )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    request.setRawHeader(
        "Prefer",
        "return=representation"
        );

    const QJsonDocument document( data );

    QNetworkReply* reply = _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );

    PendingRequest pending;
    pending.retryable = !s_accessToken.isEmpty();
    pending.isRetry = isRetry;
    pending.resend = [this, endpoint, data]() { post( endpoint, data, true ); };

    _pending.insert( reply, pending );
}

void SupabaseApi::patch(
    const QString& endpoint,
    const QJsonObject& data,
    bool isRetry ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/rest/v1/" + endpoint )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    request.setRawHeader(
        "Prefer",
        "return=representation"
        );

    const QJsonDocument document( data );

    QNetworkReply* reply = _networkManager.sendCustomRequest(
        request,
        "PATCH",
        document.toJson( QJsonDocument::Compact )
        );

    PendingRequest pending;
    pending.retryable = !s_accessToken.isEmpty();
    pending.isRetry = isRetry;
    pending.resend = [this, endpoint, data]() { patch( endpoint, data, true ); };

    _pending.insert( reply, pending );
}

void SupabaseApi::rpc(
    const QString& functionName,
    const QJsonObject& params,
    bool isRetry ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/rest/v1/rpc/" + functionName )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    const QJsonDocument document( params );

    QNetworkReply* reply = _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );

    PendingRequest pending;
    pending.retryable = !s_accessToken.isEmpty();
    pending.isRetry = isRetry;
    pending.resend = [this, functionName, params]() { rpc( functionName, params, true ); };

    _pending.insert( reply, pending );
}

void SupabaseApi::authSignUp(
    const QString& email,
    const QString& password,
    const QJsonObject& metadata ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/signup" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + SUPABASE_KEY.toUtf8()
        );

    QJsonObject body;

    body[ "email" ] = email;
    body[ "password" ] = password;
    body[ "data" ] = metadata;

    const QJsonDocument document( body );

    _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );
}

void SupabaseApi::authSignInWithPassword(
    const QString& email,
    const QString& password ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/token?grant_type=password" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + SUPABASE_KEY.toUtf8()
        );

    QJsonObject body;

    body[ "email" ] = email;
    body[ "password" ] = password;

    const QJsonDocument document( body );

    _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );
}

void SupabaseApi::authRefreshToken( const QString& refreshToken ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/token?grant_type=refresh_token" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + SUPABASE_KEY.toUtf8()
        );

    QJsonObject body;

    body[ "refresh_token" ] = refreshToken;

    const QJsonDocument document( body );

    _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );
}

void SupabaseApi::authUpdateUser( const QJsonObject& data, bool isRetry ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/user" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    const QJsonDocument document( data );

    QNetworkReply* reply = _networkManager.sendCustomRequest(
        request,
        "PUT",
        document.toJson( QJsonDocument::Compact )
        );

    PendingRequest pending;
    pending.retryable = !s_accessToken.isEmpty();
    pending.isRetry = isRetry;
    pending.resend = [this, data]() { authUpdateUser( data, true ); };

    _pending.insert( reply, pending );
}

void SupabaseApi::authSignOut() {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/logout" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + bearerToken().toUtf8()
        );

    QNetworkReply* reply = _networkManager.post( request, QByteArray() );

    _pending.insert( reply, PendingRequest() );
}

void SupabaseApi::refreshAndRetry( const PendingRequest& original ) {

    QNetworkRequest request(
        QUrl( SUPABASE_URL + "/auth/v1/token?grant_type=refresh_token" )
        );

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        "application/json"
        );

    request.setRawHeader(
        "apikey",
        SUPABASE_KEY.toUtf8()
        );

    request.setRawHeader(
        "Authorization",
        "Bearer " + SUPABASE_KEY.toUtf8()
        );

    QJsonObject body;
    body[ "refresh_token" ] = s_refreshToken;

    const QJsonDocument document( body );

    QNetworkReply* reply = _networkManager.post(
        request,
        document.toJson( QJsonDocument::Compact )
        );

    PendingRequest pending;
    pending.isRefreshForRetry = true;
    pending.resend = original.resend;

    _pending.insert( reply, pending );
}

void SupabaseApi::handleReply( QNetworkReply* reply ) {

    const PendingRequest pending = _pending.take( reply );
    const int statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt();
    const QByteArray responseData = reply->readAll();
    const bool hasError = reply->error() != QNetworkReply::NoError;
    const bool isOffline = hasError && statusCode == 0;

    reply->deleteLater();

    if ( pending.isRefreshForRetry ) {

        const QJsonObject session = QJsonDocument::fromJson( responseData ).object();
        const QString accessToken = session.value( "access_token" ).toString();
        const QString refreshToken = session.value( "refresh_token" ).toString();

        if ( hasError || accessToken.isEmpty() ) {

            if ( !isOffline ) {
                clearAccessToken();
            }

            emit requestFailed( isOffline ? "Sem conexão com a internet." : "Sessão expirada. Faça login novamente.", isOffline );
            return;
        }

        setSession( accessToken, refreshToken );

        if ( pending.resend ) {
            pending.resend();
        }

        return;
    }

    if ( hasError ) {

        if ( statusCode == 401 && pending.retryable && !pending.isRetry && !s_refreshToken.isEmpty() ) {
            refreshAndRetry( pending );
            return;
        }

        emit requestFailed(
            reply->errorString() +
            " | " +
            QString::fromUtf8( responseData ),
            isOffline
            );

        return;
    }

    const QJsonDocument response =
        QJsonDocument::fromJson( responseData );

    if ( response.isNull() && !responseData.isEmpty() ) {
        emit requestFailed(
            "Resposta JSON inválida: " +
            QString::fromUtf8( responseData ),
            false
            );

        return;
    }

    emit requestFinished( response );
}
