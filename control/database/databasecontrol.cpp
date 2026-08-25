#include "databasecontrol.h"

#include <QDebug>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

DataBaseControl::DataBaseControl( QObject* parent ) :
    QObject( parent ) {

    connect( &_supabaseApi, &SupabaseApi::requestFinished, this, &DataBaseControl::handleRequestFinished );
    connect( &_supabaseApi, &SupabaseApi::requestFailed, this, &DataBaseControl::handleRequestFailed );
}

void DataBaseControl::insert() {

    if ( _email.isEmpty() || _password.isEmpty() || _name.isEmpty() ) {
        emit fail( "Nome, e-mail e senha são obrigatórios." );
        return;
    }

    _requestType = RequestType::SignUp;

    emit showLoading( true );

    const QString userName = normalizeUserName( _name );

    QJsonObject metadata;

    metadata[ "name" ] = _name;
    metadata[ "user" ] = userName;

    if ( !_cpf.isEmpty() ) {
        metadata[ "cpf" ] = _cpf;
    }

    if ( !_birthDt.isEmpty() ) {
        metadata[ "birth_date" ] = _birthDt;
    }

    _supabaseApi.authSignUp( _email, _password, metadata );
}

void DataBaseControl::authenticate() {

    if ( _email.isEmpty() || _password.isEmpty() ) {
        emit fail( "E-mail, usuário ou CPF e senha são obrigatórios." );
        return;
    }

    _requestType = RequestType::ResolveLoginEmail;

    emit showLoading( true );

    QJsonObject params;

    params[ "p_identifier" ] = _email.trimmed();

    _supabaseApi.rpc( "resolve_login_email", params );
}

void DataBaseControl::validateSession( const QString& refreshToken ) {

    if ( refreshToken.isEmpty() ) {
        emit sessionValidated( false, "", "", "", "", "", "", 0, 0 );
        return;
    }

    _requestType = RequestType::RefreshSession;

    _supabaseApi.authRefreshToken( refreshToken );
}

void DataBaseControl::logout() {

    _supabaseApi.authSignOut();

    SupabaseApi::clearAccessToken();
}

void DataBaseControl::fetchProfile() {

    _supabaseApi.get( "profiles?select=user,balance,creation_date,cpf,email,birth_date,avatar_index,avatar_color_index&limit=1" );
}

void DataBaseControl::emitProfile( const QJsonObject& profile, bool asSessionValidated ) {

    auto toProfileDate = []( const QJsonObject& user ) {
        const QString rawDate = user.value( "creation_date" ).toString();

        if ( rawDate.isEmpty() ) {
            return QString();
        }

        QDateTime parsedDate = QDateTime::fromString( rawDate, Qt::ISODateWithMs );

        if ( !parsedDate.isValid() ) {
            parsedDate = QDateTime::fromString( rawDate, Qt::ISODate );
        }

        if ( parsedDate.isValid() ) {
            return parsedDate.date().toString( "dd/MM/yyyy" );
        }

        return rawDate;
    };

    auto toBirthDate = []( const QJsonObject& user ) {
        const QString rawDate = user.value( "birth_date" ).toString();
        const QDate parsedDate = QDate::fromString( rawDate, "yyyy-MM-dd" );

        return parsedDate.isValid() ? parsedDate.toString( "dd/MM/yyyy" ) : rawDate;
    };

    const double balance = profile.value( "balance" ).toDouble();
    const QString userName = profile.value( "user" ).toString();
    const QString creationDate = toProfileDate( profile );
    const QString cpf = profile.value( "cpf" ).toString();
    const QString email = profile.value( "email" ).toString();
    const QString birthDate = toBirthDate( profile );
    const int avatarIndex = profile.value( "avatar_index" ).toInt( 0 );
    const int avatarColorIndex = profile.value( "avatar_color_index" ).toInt( 0 );
    const QString formattedBalance = QLocale::system().toCurrencyString( balance );

    emit sessionEstablished( _pendingRefreshToken );

    if ( asSessionValidated ) {
        emit sessionValidated( true, formattedBalance, userName, creationDate, cpf, email, birthDate, avatarIndex, avatarColorIndex );
    } else {
        emit success( formattedBalance, userName, creationDate, cpf, email, birthDate, avatarIndex, avatarColorIndex );
    }
}

void DataBaseControl::handleRequestFinished( const QJsonDocument& response ) {

    if ( _requestType == RequestType::None ) {
        return;
    }

    qInfo() << "DataBaseControl::handleRequestFinished";

    if ( _requestType == RequestType::ResolveLoginEmail ) {

        _requestType = RequestType::None;

        if ( !response.isArray() || response.array().isEmpty() ) {
            emit showLoading( false );
            emit fail( "E-mail ou senha inválidos." );
            return;
        }

        const QString resolvedEmail = response.array().first().toObject().value( "email" ).toString();

        if ( resolvedEmail.isEmpty() ) {
            emit showLoading( false );
            emit fail( "E-mail ou senha inválidos." );
            return;
        }

        _requestType = RequestType::SignIn;

        _supabaseApi.authSignInWithPassword( resolvedEmail, _password );

        return;
    }

    if ( _requestType == RequestType::SignUp || _requestType == RequestType::SignIn ) {

        const QJsonObject session = response.object();
        const QString accessToken = session.value( "access_token" ).toString();
        const QString refreshToken = session.value( "refresh_token" ).toString();

        if ( accessToken.isEmpty() ) {
            _requestType = RequestType::None;
            emit showLoading( false );
            emit fail( "Não foi possível iniciar a sessão." );
            return;
        }

        SupabaseApi::setSession( accessToken, refreshToken );
        _pendingRefreshToken = refreshToken;

        _requestType = RequestType::FetchProfile;

        _password.clear();
        emit passwordChanged();

        fetchProfile();

        return;
    }

    if ( _requestType == RequestType::FetchProfile ) {

        _requestType = RequestType::None;

        emit showLoading( false );

        if ( !response.isArray() || response.array().isEmpty() ) {
            emit fail( "Não foi possível carregar o perfil." );
            return;
        }

        emitProfile( response.array().first().toObject(), false );

        return;
    }

    if ( _requestType == RequestType::RefreshSession ) {

        const QJsonObject session = response.object();
        const QString accessToken = session.value( "access_token" ).toString();
        const QString refreshToken = session.value( "refresh_token" ).toString();

        if ( accessToken.isEmpty() ) {
            _requestType = RequestType::None;
            emit sessionValidated( false, "", "", "", "", "", "", 0, 0 );
            return;
        }

        SupabaseApi::setSession( accessToken, refreshToken );
        _pendingRefreshToken = refreshToken;

        _requestType = RequestType::FetchProfileAfterRefresh;

        fetchProfile();

        return;
    }

    if ( _requestType == RequestType::FetchProfileAfterRefresh ) {

        _requestType = RequestType::None;

        if ( !response.isArray() || response.array().isEmpty() ) {
            emit sessionValidated( false, "", "", "", "", "", "", 0, 0 );
            return;
        }

        emitProfile( response.array().first().toObject(), true );

        return;
    }

    _requestType = RequestType::None;

    qWarning() << "DataBaseControl::handleRequestFinished Tipo de requisição desconhecido.";

    emit showLoading( false );

    emit fail( "Resposta inesperada do Supabase." );
}

void DataBaseControl::handleRequestFailed( const QString& error ) {

    const RequestType requestType = _requestType;

    _requestType = RequestType::None;

    if ( requestType == RequestType::None ) {
        return;
    }

    emit showLoading( false );

    qWarning() << "DataBaseControl::handleRequestFailed:" << error;

    if ( requestType == RequestType::ResolveLoginEmail || requestType == RequestType::SignIn ) {
        emit fail( "E-mail ou senha inválidos." );
        return;
    }

    if ( requestType == RequestType::SignUp ) {
        const QString lowerError = error.toLower();

        if ( lowerError.contains( "duplicate key" ) || lowerError.contains( "unique constraint" ) ||
             lowerError.contains( "23505" ) || lowerError.contains( "already registered" ) ||
             lowerError.contains( "user_already_exists" ) ) {
            emit fail( "E-mail, CPF ou usuário já registrado no sistema." );
            return;
        }

        emit fail( error );
        return;
    }

    if ( requestType == RequestType::FetchProfile ) {
        emit fail( "Não foi possível carregar o perfil." );
        return;
    }

    if ( requestType == RequestType::RefreshSession || requestType == RequestType::FetchProfileAfterRefresh ) {
        emit sessionValidated( false, "", "", "", "", "", "", 0, 0 );
        return;
    }

    emit fail( error );
}

QString DataBaseControl::normalizeUserName( const QString& fullName ) const {

    QString userName = fullName;

    userName = userName.toLower();
    userName.remove( QRegularExpression( "\\s+" ) );

    return userName;
}


QString DataBaseControl::email() const {
    return _email;
}

void DataBaseControl::setEmail( const QString& email ) {

    if ( _email == email )
        return;

    _email = email;

    emit emailChanged();
}

QString DataBaseControl::password() const {
    return _password;
}

void DataBaseControl::setPassword( const QString& password ) {

    if ( _password == password )
        return;

    _password = password;

    emit passwordChanged();
}

QString DataBaseControl::cpf() const {
    return _cpf;
}

void DataBaseControl::setCpf( const QString& cpf ) {

    if ( _cpf == cpf )
        return;

    _cpf = cpf;

    emit cpfChanged();
}

QString DataBaseControl::birthDt() const {
    return _birthDt;
}

void DataBaseControl::setBirthDt( const QString& birthDt ) {

    const QDate date = QDate::fromString( birthDt, "d-M-yyyy" );

    if ( !date.isValid() ) {
        return;
    }

    const QString formattedBirthDt = date.toString( "yyyy-MM-dd" );

    if ( _birthDt == formattedBirthDt ) {
        return;
    }

    _birthDt = formattedBirthDt;

    emit birthDtChanged();
}

QString DataBaseControl::name() const {
    return _name;
}

void DataBaseControl::setName( const QString& name ) {

    if ( _name == name )
        return;

    _name = name;

    emit nameChanged();
}
