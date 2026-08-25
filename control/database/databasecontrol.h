#ifndef DATABASECONTROL_H
#define DATABASECONTROL_H

#include "supabaseapi.h"

#include <QObject>

class DataBaseControl : public QObject {
    Q_OBJECT

    Q_PROPERTY( QString cpf READ cpf WRITE setCpf NOTIFY cpfChanged )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString email READ email WRITE setEmail NOTIFY emailChanged )
    Q_PROPERTY( QString password READ password WRITE setPassword NOTIFY passwordChanged )
    Q_PROPERTY( QString birthDt READ birthDt WRITE setBirthDt NOTIFY birthDtChanged )

public:
    explicit DataBaseControl( QObject* parent = nullptr );

    QString email() const;
    void setEmail( const QString& email );

    QString password() const;
    void setPassword( const QString& password );

    QString cpf() const;
    void setCpf( const QString& cpf );

    QString birthDt() const;
    void setBirthDt( const QString& birthDt );

    QString name() const;
    void setName( const QString& name );

public slots:
    void insert();
    void authenticate();
    void validateSession( const QString& refreshToken );
    void logout();

signals:
    void emailChanged();
    void passwordChanged();
    void cpfChanged();
    void birthDtChanged();
    void nameChanged();
    void showLoading( bool show );
    void success( const QString& formattedBalance, const QString& userName, const QString& creationDate, const QString& cpf, const QString& email, const QString& birthDate, int avatarIndex, int avatarColorIndex );
    void sessionValidated( bool isValid, const QString& formattedBalance, const QString& userName, const QString& creationDate, const QString& cpf, const QString& email, const QString& birthDate, int avatarIndex, int avatarColorIndex );
    void sessionEstablished( const QString& refreshToken );
    void fail( const QString& msg );

private slots:
    void handleRequestFinished( const QJsonDocument& response );
    void handleRequestFailed( const QString& error );

private:
    enum class RequestType {
        None,
        ResolveLoginEmail,
        SignUp,
        SignIn,
        FetchProfile,
        RefreshSession,
        FetchProfileAfterRefresh
    };

    SupabaseApi _supabaseApi;

    QString normalizeUserName( const QString& fullName ) const;
    void fetchProfile();
    void emitProfile( const QJsonObject& profile, bool asSessionValidated );

    RequestType _requestType = RequestType::None;

    QString _email;
    QString _password;
    QString _cpf;
    QString _birthDt;
    QString _name;
    QString _pendingRefreshToken;
};

#endif // DATABASECONTROL_H
