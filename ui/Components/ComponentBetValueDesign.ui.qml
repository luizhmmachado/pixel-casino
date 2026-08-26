import QtQuick 2.15
import QtQuick.Controls 2.15
import Colors 1.0
import Fonts 1.0
import Components 1.0

Item {
	id: root
	implicitHeight: clmContent.implicitHeight + 64

	property string titleText: qsTr("APOSTA:")
	property string availableDisplayText: ""
	property var quickBetLabels: []
	property bool mobileLayout: width < 700

	property alias inputBet: inputBet
	property alias btnMinus: btnMinus
	property alias btnPlus: btnPlus

	signal quickBetClicked(int index)
	signal maxClicked()

	Rectangle {
		anchors.fill: parent
		color: Colors.primary
		border.width: 2
		border.color: Colors.secondary
		radius: 4

		Column {
			id: clmContent

			anchors.fill: parent
			anchors {
				left: parent.left
				right: parent.right
				top: parent.top
				leftMargin: 32
				rightMargin: 32
				topMargin: 32
				bottomMargin: 32
			}
			spacing: 0

			Text {
				text: root.titleText
				color: Colors.yellow100
				font: Fonts.text8bit
			}

			Item {
				width: 1
				height: 32
			}

			Row {
				width: parent.width
				spacing: 12

				ComponentButton {
					id: btnMinus

					componentWidth: mobileLayout ? 40 : 52
					componentHeight: 52
					componentBtnText: "-"
					componentTextFont: Fonts.title8bit
					componentEnabledColor: Colors.background
					componentBorderColor: Colors.yellow100
					componentBorderColorOnHovered: Colors.yellow100
					componentTextColor: Colors.yellow100
					componentTextColorOnHovered: Colors.yellow100
					enableHover: true
				}

				Column {
					width: parent.width - btnMinus.width - btnPlus.width - parent.spacing * 2
						spacing: 16

					TextField {
						id: inputBet

						width: parent.width
						height: 56
						topPadding: 0
						bottomPadding: 0
						horizontalAlignment: Text.AlignHCenter
						verticalAlignment: Text.AlignVCenter
						font: Fonts.title8bit
						color: Colors.yellow100
						selectByMouse: true

						background: Rectangle {
							color: "transparent"
							border.width: 0
							radius: 0
						}
					}

					Text {
						width: parent.width
						text: root.availableDisplayText
						color: Colors.secondaryGreen
						font: Fonts.secondaryText8bit
						horizontalAlignment: Text.AlignHCenter
					}
				}

				ComponentButton {
					id: btnPlus

					componentWidth: mobileLayout ? 40 : 52
					componentHeight: 52
					componentBtnText: "+"
					componentTextFont: Fonts.title8bit
					componentEnabledColor: Colors.background
					componentBorderColor: Colors.yellow100
					componentBorderColorOnHovered: Colors.yellow100
					componentTextColor: Colors.yellow100
					componentTextColorOnHovered: Colors.yellow100
					enableHover: true
				}
			}

			Item {
				width: 1
				height: 32
			}

			Flow {
				id: quickBetRow

				width: parent.width
				anchors.horizontalCenter: parent.horizontalCenter
				spacing: 10
				layoutDirection: Qt.LeftToRight

				Repeater {
					model: root.quickBetLabels

					ComponentButton {
						componentWidth: btnText.contentWidth + 16
						componentHeight: 36
						componentBtnText: modelData
						componentTextFont: Fonts.secondaryText8bit
						componentEnabledColor: Colors.background
						componentBorderColor: Colors.yellow100
						componentBorderColorOnHovered: Colors.yellow100
						componentTextColor: Colors.yellow100
						componentTextColorOnHovered: Colors.yellow100
						enableHover: true

						onClicked: root.quickBetClicked(index)
					}
				}
			}

			Item {
				width: 1
				height: 10
			}

			ComponentButton {
				anchors.horizontalCenter: parent.horizontalCenter
				componentWidth: Math.min( quickBetRow.implicitWidth, parent.width )
				componentHeight: 36
				componentBtnText: qsTr("[ MAX  ]")
				componentTextFont: Fonts.secondaryText8bit
				componentEnabledColor: Colors.background
				componentBorderColor: Colors.yellow100
				componentBorderColorOnHovered: Colors.yellow100
				componentTextColor: Colors.yellow100
				componentTextColorOnHovered: Colors.yellow100
				enableHover: true

				onClicked: root.maxClicked()
			}
		}
	}

}
