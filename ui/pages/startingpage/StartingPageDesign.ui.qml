import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import Fonts 1.0
import Colors 1.0
import Components 1.0

Item {
    property bool mobileLayout: width < 700

    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: Colors.background
    }

    ComponentTitle {
        id: lblTitle

        componentText: qsTr("PIXEL CASINO")
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: mobileLayout ? 24 : 64
        }
    }

    Text {
        id: txtSecondary

        text: qsTr("Retire seus prêmios na hora")
        font: Fonts.text8bit
        color: Colors.secondaryGreen

        anchors {
            horizontalCenter: parent.horizontalCenter
            top: lblTitle.top
            topMargin: mobileLayout ? 48 : 64
        }
    }

    RowLayout {
        id: rowButtons

        anchors {
            top: txtSecondary.bottom
            topMargin: mobileLayout ? 24 : 64
            left: parent.left
            right: parent.right
            leftMargin: mobileLayout ? 16 : 32
            rightMargin: mobileLayout ? 16 : 32
        }

        height: mobileLayout ? 112 : 48

        Text {
            text: qsTr("[ SELECIONE SEU JOGO ]")
            font: Fonts.text8bit
            color: Colors.yellow100

            Layout.alignment: mobileLayout ? Qt.AlignLeft : Qt.AlignVCenter
        }

        Item {
            Layout.fillWidth: true
            height: 48

            Row {
                anchors {
                    right: mobileLayout ? undefined : parent.right
                    left: mobileLayout ? parent.left : undefined
                    top: mobileLayout ? parent.top : undefined
                    verticalCenter: mobileLayout ? undefined : parent.verticalCenter
                    topMargin: mobileLayout ? 48 : 0
                }

                spacing: mobileLayout ? 8 : 16

                Repeater {
                    model: gameCategories

                    ComponentButton {
                        componentBtnText: modelData
                        componentHeight: 48
                        componentWidth: btnText.contentWidth + 16
                        componentEnabledColor: Colors.background
                        componentDisabledColor: Colors.background
                        componentBorderColor: root.selectedCategory === model.index ? Colors.yellow100 : Colors.secondaryGreen
                        componentTextColor:  root.selectedCategory === model.index ? Colors.yellow100 : Colors.secondaryGreen

                        onClicked: selectedCategory = model.index
                    }
                }
            }
        }
    }


    Flickable {
        id: gamesFlickable

        anchors {
            top: rowButtons.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        contentWidth: width
        contentHeight: gamesGrid.y + gamesGrid.height
        clip: true
        interactive: mobileLayout

        Grid {
            id: gamesGrid

            x: mobileLayout ? 16 : 32
            y: mobileLayout ? 24 : 64
            width: gamesFlickable.width - (mobileLayout ? 32 : 64)
            columns: mobileLayout ? 1 : 2
            spacing: mobileLayout ? 24 : 32

            Repeater {
                model: filteredGames

                StartingPageGameCard {
                    width: mobileLayout ? gamesGrid.width : (gamesGrid.width - gamesGrid.spacing) / 2
                    height: 240

                    gameName: modelData.name
                    gameCategory: modelData.category
                    gameDescription: modelData.description
                    gameImage: modelData.image

                    onClicked: root.selectedIndex = model.index
                }
            }
        }
    }

}
