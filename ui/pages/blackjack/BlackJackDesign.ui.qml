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
    property alias betValue: betValue
    property alias blackjackCards: blackjackCards
    property alias btnBuy: btnBuy
    property alias btnHold: btnHold
    property alias popupRoundResult: popupRoundResult

    Rectangle{
        anchors.fill: parent
        anchors.horizontalCenter: root.horizontalCenter
        color: Colors.background

        Flickable {
            id: flkMain

            anchors.fill: parent
            anchors.margins: 16
            clip: true
            contentWidth: width
            contentHeight: clmMain.implicitHeight
            boundsBehavior: Flickable.StopAtBounds

            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AlwaysOff
            }

            Column{
                id: clmMain

                width: flkMain.width
                spacing: 32

                ComponentTitle {
                    id: title

                    componentText: qsTr("BlackJack")
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                ComponentBetValue {
                    id: betValue

                    availableBalance: root.userBalance
                    width: root.mobileLayout ? parent.width : ( parent.width / 3 ) * 2
                    height: betValue.implicitHeight
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                BlackJackCards {
                    id: blackjackCards

                    width: root.mobileLayout ? parent.width : ( parent.width / 3 ) * 2
                    height: 340
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Row {
                    id: rowActions

                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 16

                    ComponentButton {
                        id: btnBuy

                        componentWidth: btnText.contentWidth + 16
                        componentHeight: 48
                        componentBtnText: qsTr("[ BUY ]")
                    }

                    ComponentButton {
                        id: btnHold

                        componentWidth: btnText.contentWidth + 16
                        componentHeight: 48
                        componentBtnText: qsTr("[ HOLD ]")
                    }
                }
            }
        }

        ComponentPopupConfirmation {
            id: popupRoundResult

            canCancel: false
            showInput: false
            componentConfirmBtnText: qsTr("Apostar novamente")
            anchors.centerIn: parent
        }
    }
}
