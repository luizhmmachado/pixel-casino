#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFontDatabase>
#include <QDebug>

#include <control/blackjack/blackjackcontrol.h>
#include <control/database/databasecontrol.h>
#include <control/database/profilecontrol.h>
#include <control/database/transactioncontrol.h>
#include <control/horserace/horseracecontrol.h>
#include <control/horserace/horsemodel.h>

int main( int argc, char* argv[] ) {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling );
#endif
    QGuiApplication app( argc, argv );
    Q_INIT_RESOURCE( resources );

    QCoreApplication::setOrganizationName( "PixelCasino" );
    QCoreApplication::setOrganizationDomain( "pixelcasino.local" );
    QCoreApplication::setApplicationName( "pixel-casino" );

    DataBaseControl databaseControl;

    QFontDatabase::addApplicationFont( ":/ui/theme/fonts/PressStart2P-Regular.ttf" );

    qmlRegisterType<BlackJackControl>( "BlackJackControl", 1, 0, "BlackJackControl" );
    qmlRegisterType<DataBaseControl>( "DataBaseControl", 1, 0, "DataBaseControl" );
    qmlRegisterType<ProfileControl>( "ProfileControl", 1, 0, "ProfileControl" );
    qmlRegisterType<TransactionControl>( "TransactionControl", 1, 0, "TransactionControl" );
    qmlRegisterType<HorseRaceControl>( "HorseRaceControl", 1, 0, "HorseRaceControl" );
    qmlRegisterType<HorseModel>( "HorseModel", 1, 0, "HorseModel" );
    qmlRegisterSingletonType( QUrl( QStringLiteral( "qrc:/ui/theme/Colors.qml" ) ),
                              "Colors", 1, 0, "Colors" );
    qmlRegisterSingletonType( QUrl( QStringLiteral( "qrc:/ui/theme/Fonts.qml" ) ),
                              "Fonts", 1, 0, "Fonts" );

    QQmlApplicationEngine engine;
    engine.addImportPath( ":/ui" );
    const QUrl url( QStringLiteral( "qrc:/main.qml" ) );
    QObject::connect( &engine, &QQmlApplicationEngine::objectCreated,
                      &app, [url]( QObject* obj, const QUrl& objUrl ) {
        if ( !obj && url == objUrl )
            QCoreApplication::exit( -1 );
    }, Qt::QueuedConnection );
    engine.load( url );

    return app.exec();
}
