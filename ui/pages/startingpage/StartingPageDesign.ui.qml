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
        id: rowButtonsDesktop

        visible: !mobileLayout

        anchors {
            top: txtSecondary.bottom
            topMargin: mobileLayout ? 24 : 64
            left: parent.left
            right: parent.right
            leftMargin: mobileLayout ? 16 : 32
            rightMargin: mobileLayout ? 16 : 32
        }

        height: 48

        Text {
            id: subtitleDesktop

            text: qsTr("[ SELECIONE SEU JOGO ]")
            font: Fonts.text8bit
            color: Colors.yellow100
            elide: Text.ElideRight

            Layout.fillWidth: true
            Layout.minimumWidth: 0
            Layout.preferredWidth: Math.max(0, rowButtonsDesktop.width - filtersDesktop.implicitWidth - 16)
            Layout.alignment: mobileLayout ? Qt.AlignLeft : Qt.AlignVCenter
        }

        Item {
            Layout.preferredWidth: filtersDesktop.implicitWidth
            Layout.minimumWidth: filtersDesktop.implicitWidth
            height: 48
            Layout.alignment: Qt.AlignVCenter

            Row {
                id: filtersDesktop

                anchors {
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }

                spacing: 16

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

    Column {
        id: rowButtons

        visible: mobileLayout
        spacing: 16

        anchors {
            top: txtSecondary.bottom
            topMargin: 24
            left: parent.left
            right: parent.right
            leftMargin: 16
            rightMargin: 16
        }

        Text {
            text: qsTr("[ SELECIONE SEU JOGO ]")
            font: Fonts.text8bit
            color: Colors.yellow100
        }

        Flow {
            width: parent.width
            spacing: 8

            Repeater {
                model: gameCategories

                ComponentButton {
                    componentBtnText: modelData
                    componentHeight: 48
                    componentWidth: btnText.contentWidth + 16
                    componentEnabledColor: Colors.background
                    componentDisabledColor: Colors.background
                    componentBorderColor: root.selectedCategory === model.index ? Colors.yellow100 : Colors.secondaryGreen
                    componentTextColor: root.selectedCategory === model.index ? Colors.yellow100 : Colors.secondaryGreen

                    onClicked: selectedCategory = model.index
                }
            }
        }
    }


    Flickable {
        id: gamesFlickable

        anchors {
            top: mobileLayout ? rowButtons.bottom : rowButtonsDesktop.bottom
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

            property real uniformCardHeight: {
                var maxHeight = 240

                for (var childIndex = 0; childIndex < children.length; childIndex++) {
                    maxHeight = Math.max(maxHeight, children[childIndex].implicitHeight)
                }

                return maxHeight
            }

            x: mobileLayout ? 16 : 32
            y: mobileLayout ? 24 : 64
            width: gamesFlickable.width - (mobileLayout ? 32 : 64)
            columns: mobileLayout ? 1 : 2
            spacing: mobileLayout ? 40 : 32

            Repeater {
                model: filteredGames

                StartingPageGameCard {
                    width: mobileLayout ? gamesGrid.width : (gamesGrid.width - gamesGrid.spacing) / 2
                    height: gamesGrid.uniformCardHeight

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
