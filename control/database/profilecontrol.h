#ifndef PROFILECONTROL_H
#define PROFILECONTROL_H

#include "supabaseapi.h"

#include <QObject>

class ProfileControl : public QObject {
    Q_OBJECT

    Q_PROPERTY( QString userName READ userName WRITE setUserName NOTIFY userNameChanged )
    Q_PROPERTY( QString email READ email WRITE setEmail NOTIFY emailChanged )
    Q_PROPERTY( QString currentPassword READ currentPassword WRITE setCurrentPassword NOTIFY currentPasswordChanged )
    Q_PROPERTY( QString newPassword READ newPassword WRITE setNewPassword NOTIFY newPasswordChanged )

public:
    explicit ProfileControl( QObject* parent = nullptr );

    enum class Avatar {
        Card = 0,
        Crown,
        Diamond,
        Horse,
        Profile,
        Star
    };
    Q_ENUM( Avatar )

    enum class AvatarColor {
        Red = 0,
        Blue,
        Yellow,
        Green,
        Orange
    };
    Q_ENUM( AvatarColor )

    QString userName() const;
    void setUserName( const QString& userName );

    QString email() const;
    void setEmail( const QString& email );

    QString currentPassword() const;
    void setCurrentPassword( const QString& currentPassword );

    QString newPassword() const;
    void setNewPassword( const QString& newPassword );

public slots:
    void changePassword();
    void changeUserName( const QString& newUserName );
    void changeEmail( const QString& newEmail );
    void changeAvatar( int avatarIndex, int avatarColorIndex );

signals:
    void userNameChanged();
    void emailChanged();
    void currentPasswordChanged();
    void newPasswordChanged();
    void showLoading( bool show );
    void success();
    void sessionRefreshed( const QString& refreshToken );
    void fail( const QString& msg );

private slots:
    void handleRequestFinished( const QJsonDocument& response );
    void handleRequestFailed( const QString& error );

private:
    enum class RequestType {
        None,
        VerifyCurrentPassword,
        UpdatePassword,
        UpdateUserName,
        UpdateEmailAuth,
        UpdateEmailProfile,
        UpdateAvatar
    };

    SupabaseApi _supabaseApi;

    RequestType _requestType = RequestType::None;

    QString _userName;
    QString _email;
    QString _currentPassword;
    QString _newPassword;
    QString _pendingValue;
};

#endif // PROFILECONTROL_H
