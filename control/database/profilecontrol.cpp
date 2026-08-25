#include "profilecontrol.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

ProfileControl::ProfileControl( QObject* parent ) :
    QObject( parent ) {

    connect( &_supabaseApi, &SupabaseApi::requestFinished, this, &ProfileControl::handleRequestFinished );
    connect( &_supabaseApi, &SupabaseApi::requestFailed, this, &ProfileControl::handleRequestFailed );
}

void ProfileControl::changePassword() {

    if ( _userName.isEmpty() || _email.isEmpty() || _currentPassword.isEmpty() || _newPassword.isEmpty() ) {
        emit fail( "Preencha a senha atual e a nova senha." );
        return;
    }

    _requestType = RequestType::VerifyCurrentPassword;

    emit showLoading( true );

    _supabaseApi.authSignInWithPassword( _email, _currentPassword );
}

void ProfileControl::changeUserName( const QString& newUserName ) {

    const QString normalizedUserName = newUserName.trimmed().toLower();

    if ( _userName.isEmpty() || normalizedUserName.isEmpty() ) {
        emit fail( "Informe um nome de usuário válido." );
        return;
    }

    if ( normalizedUserName == _userName.trimmed().toLower() ) {
        emit fail( "O novo nome de usuário deve ser diferente do atual." );
        return;
    }

    _requestType = RequestType::UpdateUserName;
    _pendingValue = normalizedUserName;

    emit showLoading( true );

    const QString encodedUserName = QString::fromUtf8( QUrl::toPercentEncoding( _userName.trimmed().toLower() ) );
    const QString endpoint = "profiles?user=eq." + encodedUserName;

    QJsonObject data;
    data[ "user" ] = normalizedUserName;

    _supabaseApi.patch( endpoint, data );
}

void ProfileControl::changeEmail( const QString& newEmail ) {

    const QString normalizedEmail = newEmail.trimmed();

    if ( _userName.isEmpty() || normalizedEmail.isEmpty() ) {
        emit fail( "Informe um e-mail válido." );
        return;
    }

    _requestType = RequestType::UpdateEmailAuth;
    _pendingValue = normalizedEmail;

    emit showLoading( true );

    QJsonObject data;
    data[ "email" ] = normalizedEmail;

    _supabaseApi.authUpdateUser( data );
}

void ProfileControl::changeAvatar( int avatarIndex, int avatarColorIndex ) {

    if ( _userName.isEmpty() ) {
        emit fail( "Usuário inválido." );
        return;
    }

    if ( avatarIndex < 0 || avatarIndex > static_cast<int>( Avatar::Star ) ||
         avatarColorIndex < 0 || avatarColorIndex > static_cast<int>( AvatarColor::Orange ) ) {
        emit fail( "Avatar inválido." );
        return;
    }

    _requestType = RequestType::UpdateAvatar;

    emit showLoading( true );

    const QString encodedUserName = QString::fromUtf8( QUrl::toPercentEncoding( _userName.trimmed().toLower() ) );
    const QString endpoint = "profiles?user=eq." + encodedUserName;

    QJsonObject data;
    data[ "avatar_index" ] = avatarIndex;
    data[ "avatar_color_index" ] = avatarColorIndex;

    _supabaseApi.patch( endpoint, data );
}

void ProfileControl::handleRequestFinished( const QJsonDocument& response ) {

    if ( _requestType == RequestType::VerifyCurrentPassword ) {

        _requestType = RequestType::None;

        const QJsonObject session = response.object();
        const QString accessToken = session.value( "access_token" ).toString();
        const QString refreshToken = session.value( "refresh_token" ).toString();

        if ( accessToken.isEmpty() ) {
            emit showLoading( false );
            emit fail( "Senha atual incorreta." );
            return;
        }

        SupabaseApi::setSession( accessToken, refreshToken );

        emit sessionRefreshed( refreshToken );

        _requestType = RequestType::UpdatePassword;

        QJsonObject data;
        data[ "password" ] = _newPassword;

        _supabaseApi.authUpdateUser( data );

        return;
    }

    if ( _requestType == RequestType::UpdatePassword ) {

        _requestType = RequestType::None;

        emit showLoading( false );

        _currentPassword.clear();
        _newPassword.clear();

        emit currentPasswordChanged();
        emit newPasswordChanged();

        emit success();

        return;
    }

    if ( _requestType == RequestType::UpdateUserName ) {

        _requestType = RequestType::None;

        emit showLoading( false );

        if ( !response.isArray() || response.array().isEmpty() ) {
            emit fail( "Não foi possível atualizar o nome de usuário." );
            return;
        }

        _userName = _pendingValue;

        emit userNameChanged();
        emit success();

        return;
    }

    if ( _requestType == RequestType::UpdateEmailAuth ) {

        _requestType = RequestType::UpdateEmailProfile;

        const QString encodedUserName = QString::fromUtf8( QUrl::toPercentEncoding( _userName.trimmed().toLower() ) );
        const QString endpoint = "profiles?user=eq." + encodedUserName;

        QJsonObject data;
        data[ "email" ] = _pendingValue;

        _supabaseApi.patch( endpoint, data );

        return;
    }

    if ( _requestType == RequestType::UpdateEmailProfile ) {

        _requestType = RequestType::None;

        emit showLoading( false );

        emit success();

        return;
    }

    if ( _requestType == RequestType::UpdateAvatar ) {

        _requestType = RequestType::None;

        emit showLoading( false );

        if ( !response.isArray() || response.array().isEmpty() ) {
            emit fail( "Não foi possível atualizar o avatar." );
            return;
        }

        emit success();

        return;
    }

    _requestType = RequestType::None;

    emit showLoading( false );
    emit fail( "Resposta inesperada do Supabase." );
}

void ProfileControl::handleRequestFailed( const QString& error, bool isOffline ) {

    const RequestType requestType = _requestType;

    _requestType = RequestType::None;

    emit showLoading( false );

    qWarning() << "ProfileControl::handleRequestFailed:" << error;

    if ( isOffline ) {
        emit fail( "Sem conexão com a internet. Tente novamente mais tarde." );
        return;
    }

    const QString lowerError = error.toLower();
    const bool isDuplicate = lowerError.contains( "duplicate key" ) || lowerError.contains( "unique constraint" ) || lowerError.contains( "23505" );

    if ( requestType == RequestType::VerifyCurrentPassword ) {
        emit fail( "Senha atual incorreta." );
        return;
    }

    if ( requestType == RequestType::UpdatePassword ) {
        emit fail( "Não foi possível alterar a senha." );
        return;
    }

    if ( requestType == RequestType::UpdateUserName ) {
        emit fail( isDuplicate ? "Nome de usuário já está em uso." : "Não foi possível atualizar o nome de usuário." );
        return;
    }

    if ( requestType == RequestType::UpdateEmailAuth || requestType == RequestType::UpdateEmailProfile ) {
        emit fail( isDuplicate || lowerError.contains( "already" ) ? "E-mail já está em uso." : "Não foi possível atualizar o e-mail." );
        return;
    }

    if ( requestType == RequestType::UpdateAvatar ) {
        emit fail( "Não foi possível atualizar o avatar." );
        return;
    }

    emit fail( "Não foi possível concluir a operação." );
}

QString ProfileControl::userName() const {
    return _userName;
}

void ProfileControl::setUserName( const QString& userName ) {

    if ( _userName == userName )
        return;

    _userName = userName;

    emit userNameChanged();
}

QString ProfileControl::email() const {
    return _email;
}

void ProfileControl::setEmail( const QString& email ) {

    if ( _email == email )
        return;

    _email = email;

    emit emailChanged();
}

QString ProfileControl::currentPassword() const {
    return _currentPassword;
}

void ProfileControl::setCurrentPassword( const QString& currentPassword ) {

    if ( _currentPassword == currentPassword )
        return;

    _currentPassword = currentPassword;

    emit currentPasswordChanged();
}

QString ProfileControl::newPassword() const {
    return _newPassword;
}

void ProfileControl::setNewPassword( const QString& newPassword ) {

    if ( _newPassword == newPassword )
        return;

    _newPassword = newPassword;

    emit newPasswordChanged();
}
