QT += quick
QT += sql
QT += concurrent

SOURCES += \
        control/blackjack/blackjackcontrol.cpp \
        control/database/databasecontrol.cpp \
        control/database/profilecontrol.cpp \
        control/database/transactioncontrol.cpp \
        control/database/supabaseapi.cpp \
        control/database/offlinequeue.cpp \
        control/horserace/horsemodel.cpp \
        control/horserace/horseracecontrol.cpp \
        main.cpp

RESOURCES += qml.qrc \
    resources.qrc

QML_IMPORT_PATH += \
    $$PWD/ui

QML_DESIGNER_IMPORT_PATH =

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    control/blackjack/blackjackcontrol.h \
    control/database/databasecontrol.h \
    control/database/profilecontrol.h \
    control/database/transactioncontrol.h \
    control/database/supabaseapi.h \
    control/database/offlinequeue.h \
    control/horserace/horsemodel.h \
    control/horserace/horseracecontrol.h
