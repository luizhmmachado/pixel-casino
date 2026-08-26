import QtQuick 2.15
import QtQuick.Controls 2.15
import Fonts 1.0
import Colors 1.0
import Components 1.0

Item {
    id: root

    anchors.fill: parent

    property string userBalance: ""
    property bool mobileLayout: width < 700
    property alias horseSelection: horseSelection
    property alias horseRace: horseRace
    property alias horsePopup: horsePopup

    Rectangle{
        anchors.fill: parent
        anchors.horizontalCenter: root.horizontalCenter
        color: Colors.background

        Column{
            anchors.fill: parent
            anchors.margins: 16
            spacing: 32

            ComponentTitle {
                id: title

                componentText: qsTr("Corrida de Cavalos")
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            HorseSelection{
                id: horseSelection

                width: root.mobileLayout ? parent.width : ( parent.width / 3 ) * 2
                height: parent.height - title.height
                horsesList: control.horsesList
                betValue.availableBalance: root.userBalance
                anchors.horizontalCenter: parent.horizontalCenter
            }

            HorseRace{
                id: horseRace

                width: parent.width
                height: parent.height - title.height
                horsesList: control.horsesList
                visible: false
                anchors.horizontalCenter: parent.horizontalCenter
            }

            ComponentPopupConfirmation {
                id: horsePopup

                width: root.mobileLayout ? parent.width - 32 : parent.width / 2
                canCancel: false
                showInput: false
                componentConfirmBtnText: qsTr("Apostar novamente")
                anchors.centerIn: parent
            }
        }
    }
}
