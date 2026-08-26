import QtQuick 2.15
import QtQuick.Controls 2.15
import Components 1.0
import Colors 1.0
import Fonts 1.0

Popup {
    id: popup

    property string titlePopup: ""
    property string componentText: ""
    property string componentCancelBtnText: qsTr( "Cancelar" )
    property string componentConfirmBtnText: qsTr( "Confirmar" )
    property bool canCancel: true
    property bool showInput: false
    property bool canConfirm: true
    property string errorText: ""
    property string successText: ""
    property bool mobileLayout: popup.parent ? popup.parent.width < 700 : false
    property alias btnCancel: btnCancel
    property alias btnConfirm: btnConfirm
    property alias fldInput: fldInput
    property alias extraContent: clmExtraContent.data

    width: popup.parent && popup.parent.width < 700
        ? popup.parent.width - 32
        : Math.max( 360, cmpTitle.implicitWidth + 64 )
    height: popup.parent ? Math.min( clmContent.implicitHeight + 64, popup.parent.height - 64 ) : clmContent.implicitHeight + 64
    padding: 0
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose

    background: Rectangle {
        color: Colors.primary
        radius: 5
        border.width: 1
        border.color: Colors.yellow200
    }

    Overlay.modal: Rectangle {
        color: Colors.popupDim
    }

    Flickable {
        id: flkContent

        anchors {
            fill: parent
            margins: 32
        }
        clip: true
        contentWidth: width
        contentHeight: clmContent.implicitHeight
        interactive: contentHeight > height
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: clmContent

            width: parent.width
            spacing: 16

            ComponentTitle {
                id: cmpTitle

                componentText: titlePopup
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
            }

            Item {
                width: 1
                height: 32
            }

            Text {
                width: parent.width
                wrapMode: Text.WordWrap
                height: contentHeight
                font: Fonts.text8bit
                color: Colors.yellow100
                horizontalAlignment: Text.AlignHCenter
                lineHeight: 1.5
                lineHeightMode: Text.ProportionalHeight
                text: componentText
            }

            Column {
                id: clmExtraContent

                width: parent.width
                spacing: 16
            }

            ComponentField {
                id: fldInput

                visible: popup.showInput
                componentWidth: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                width: parent.width
                visible: popup.errorText.length > 0
                wrapMode: Text.WordWrap
                font: Fonts.secondaryText8bit
                color: Colors.error
                horizontalAlignment: Text.AlignHCenter
                text: popup.errorText
            }

            Text {
                width: parent.width
                visible: popup.successText.length > 0
                wrapMode: Text.WordWrap
                font: Fonts.secondaryText8bit
                color: Colors.success
                horizontalAlignment: Text.AlignHCenter
                text: popup.successText
            }

            Grid {
                id: rowButtons
                width: parent.width
                columns: popup.mobileLayout ? 1 : 2
                spacing: 32

                ComponentButton {
                    id: btnCancel

                    componentBtnText: popup.successText.length > 0 ? qsTr( "Fechar" ) : popup.componentCancelBtnText
                    componentWidth: popup.successText.length > 0 || popup.mobileLayout ? parent.width : (parent.width - parent.spacing) / 2
                    visible: popup.canCancel || popup.successText.length > 0
                    componentBorderColor: Colors.error
                    componentTextColor: Colors.error
                }

                ComponentButton {
                    id: btnConfirm

                    componentBtnText: popup.componentConfirmBtnText
                    componentWidth: btnCancel.visible && !popup.mobileLayout ? (parent.width - parent.spacing) / 2 : parent.width
                    visible: popup.successText.length === 0
                    enabled: popup.canConfirm && (!popup.showInput || fldInput.componentValid)
                }
            }
        }
    }
}
