QT += quick
QT += sql
QT += concurrent

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        control/blackjack/blackjackcontrol.cpp \
        control/database/databasecontrol.cpp \
        control/database/profilecontrol.cpp \
        control/database/transactioncontrol.cpp \
        control/database/supabaseapi.cpp \
        control/horserace/horsemodel.cpp \
        control/horserace/horseracecontrol.cpp \
        main.cpp

RESOURCES += qml.qrc \
    resources.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH += \
    $$PWD/ui

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    control/blackjack/blackjackcontrol.h \
    control/database/databasecontrol.h \
    control/database/profilecontrol.h \
    control/database/transactioncontrol.h \
    control/database/supabaseapi.h \
    control/horserace/horsemodel.h \
    control/horserace/horseracecontrol.h
