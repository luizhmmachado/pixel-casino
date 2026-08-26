import QtQuick 2.15
import Colors 1.0
import Fonts 1.0
import Components 1.0

Rectangle {
    id: rctDeposit

    property bool mobileLayout: width < 700

    property bool canWithdraw: false

    color: Colors.primary
    border.width: 2
    border.color: Colors.secondary
    radius: 5
    height: clmBalance.implicitHeight + 64

    Column {
        id: clmBalance

        spacing: 16
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 32
        }

        Text {
            text: qsTr ( "Saldo disponível:")
            font: Fonts.secondaryText8bit
            color: Colors.secondaryGreen
        }

        Text {
            text: userBalance

            font: Fonts.title8bit
            color: Colors.textColor
        }

        Item {
            width: 1
            height: 16
        }

        Grid {
            columns: mobileLayout ? 1 : 2
            spacing: 32
            width: parent.width

            ComponentButton {
                componentWidth: mobileLayout ? parent.width : btnText.contentWidth + 16
                componentBtnText: qsTr( "[ DEPOSITAR ]")
            }

            ComponentButton {
                id: btnWithdraw
                componentWidth: mobileLayout ? parent.width : btnText.contentWidth + 16
                componentBtnText: qsTr( "[ SACAR ]")
                enabled: canWithdraw
            }
        }
    }
}
