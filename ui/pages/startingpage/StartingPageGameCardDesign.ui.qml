import QtQuick 2.15
import Fonts 1.0
import Colors 1.0
import Components 1.0

Rectangle {
    id: rctGameCard

    implicitHeight: txtGameDescription.y + txtGameDescription.implicitHeight + 52

    property string gameName: ""
    property string gameDescription: ""
    property string gameCategory: ""
    property string gameImage: ""
    property string buttonText: qsTr("JOGAR AGORA")
    property bool hover: true

    property alias mouseAreaCard: mouseAreaCard
    property alias rctButton: rctButton

    border.width: 2
    border.color: hover ? Colors.secondary : Colors.yellow100
    color: Colors.primary

    Rectangle {
        id: rctImage

        anchors {
            top: parent.top
            topMargin: 16
            leftMargin: 16
            left: rctGameCard.left
        }

        height: 72
        width: rctGameCard.width - 32
        border.width: 2
        border.color: Colors.secondary
        color: Colors.background

        Image {
            height: 48
            width: 48
            anchors.centerIn: parent
            source: gameImage
        }
    }

    Text {
        id: txtGameCategory

        width: rctGameCard.width - 32
        text: gameCategory
        font: Fonts.secondaryText8bit
        color: Colors.secondaryGreen

        anchors {
            top: rctImage.bottom
            topMargin: 16
            leftMargin: 16
            left: rctGameCard.left
        }
    }

    Text {
        id: txtGameName

        width: rctGameCard.width - 32
        text: gameName
        font: Fonts.text8bit
        color: Colors.yellow200
        wrapMode: Text.WordWrap

        anchors {
            top: txtGameCategory.bottom
            topMargin: 16
            leftMargin: 16
            left: rctGameCard.left
        }
    }

    Text {
        id: txtGameDescription

        width: rctGameCard.width - 32
        text: gameDescription
        font: Fonts.secondaryText8bit
        color: Colors.secondaryGreen
        wrapMode: Text.WordWrap
        lineHeight: 1.3
        lineHeightMode: Text.ProportionalHeight

        anchors {
            top: txtGameName.bottom
            topMargin: 16
            leftMargin: 16
            left: rctGameCard.left
        }
    }

    ComponentButton {
        id: rctButton

        componentWidth: parent.width - 32
        componentHeight: 40
        componentBorderColor: rctGameCard.hover ? Colors.secondary : Colors.yellow100
        componentBorderWidth: 2
        componentBtnText: buttonText

        anchors {
            left: parent.left
            leftMargin: 16
            bottom: rctGameCard.bottom
            bottomMargin: -( rctButton.height / 2 )
        }

    }

    MouseArea {
        id: mouseAreaCard

        hoverEnabled: true
        anchors.fill: parent
    }
}
