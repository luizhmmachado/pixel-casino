import QtQuick 2.15
import Colors 1.0
import Fonts 1.0
import Components 1.0
import ProfileControl 1.0

Rectangle {
    id: rctDeposit

    property bool mobileLayout: width < 700

    property string userName: ""
    property string userEmail: ""
    property string userBirthDate: ""
    property string userCpf: ""
    property string formattedCpf: ""

    property alias fldActualPassword: fldActualPassword
    property alias fldNewPassword: fldNewPassword
    property alias fldConfirmNewPassword: fldConfirmNewPassword
    property alias btnChangePassword: btnChangePassword
    property alias txtPasswordError: txtPasswordError
    property alias control: control

    property bool validNewPassword: false
    property bool validConfirmNewPassword: false
    property bool isChangingPassword: false
    property var passwordRequirementsValid: [false, false, false, false, false]
    property var passwordRequirements: [
        "Contém pelo menos 8 caracteres",
        "Contém letra maiúscula",
        "Contém letra minúscula",
        "Contém número",
        "Contém caractere especial"
    ]

    color: Colors.primary
    border.width: 2
    border.color: Colors.secondary
    radius: 5
    height: clmMain.implicitHeight + 64

    Column {
        id: clmMain

        spacing: 32
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 32
        }

        Text {
            text: qsTr( "[ SENHA ]" )
            font: Fonts.text8bit
            color: Colors.textColor
        }

        Row {
            width: parent.width
            spacing: 32

            Column {
                id: clmPassword

                width: (parent.width - parent.spacing)
                spacing: 16


                Text {
                    id: txtLabelActualPassword

                    text: qsTr("Senha atual:")
                    font: Fonts.secondaryText8bit
                    color: Colors.yellow200
                }

                ComponentField {
                    id: fldActualPassword

                    componentWidth: clmPassword.width + parent.spacing
                    componentEchoMode: TextInput.Password
                    componentValidator: RegularExpressionValidator {
                        regularExpression: /.{8,20}/
                    }
                    componentValid: text.length > 0 && acceptableInput
                }

                Grid {
                    width: parent.width
                    columns: mobileLayout ? 1 : 2
                    spacing: 16

                    Column {
                        width: mobileLayout ? parent.width : (parent.width - parent.spacing) / 2
                        spacing: 8

                        Text {
                            text: qsTr("Nova senha:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        ComponentField {
                            id: fldNewPassword

                            componentWidth: parent.width
                            componentEchoMode: TextInput.Password
                            componentValidator: RegularExpressionValidator {
                                regularExpression: /.{8,20}/
                            }
                            componentValid: validNewPassword
                        }
                    }

                    Column {
                        width: mobileLayout ? parent.width : (parent.width - parent.spacing) / 2
                        spacing: 8

                        Text {
                            text: qsTr("Confirmar nova senha:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        ComponentField {
                            id: fldConfirmNewPassword

                            componentWidth: parent.width
                            componentEchoMode: TextInput.Password
                            componentValidator: RegularExpressionValidator {
                                regularExpression: /.{8,20}/
                            }
                            componentValid: validConfirmNewPassword
                        }
                    }
                }

                Rectangle {
                    id: rctPasswordRequirementsh

                    width: parent.width + parent.spacing
                    height: clmPasswordRequirements.implicitHeight + 20
                    color: Colors.background
                    border.width: 2
                    border.color: Colors.secondary

                    Column {
                        id: clmPasswordRequirements

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 10
                        spacing: 6

                        Repeater {
                            model: passwordRequirements.length

                            delegate: Row {
                                width: parent.width
                                spacing: 8

                                Text {
                                    width: 12
                                    text: passwordRequirementsValid[index] ? "✓" : "x"
                                    color: passwordRequirementsValid[index] ? Colors.success : Colors.error
                                    font: Fonts.secondaryText8bit
                                }

                                Text {
                                    text: passwordRequirements[index]
                                    color: Colors.secondary
                                    font.family: "Press Start 2P"
                                    font.pixelSize: 12
                                    font.strikeout: passwordRequirementsValid[index]
                                }
                            }
                        }
                    }
                }
            }
        }

        Text {
            id: txtPasswordError
            width: parent.width
            visible: text.length > 0
            color: Colors.error
            font: Fonts.secondaryText8bit
            wrapMode: Text.WordWrap
        }

        ComponentButton {
            id: btnChangePassword

            componentBtnText: qsTr( "[ ALTERAR SENHA ]" )
            componentWidth: btnText.contentWidth + 16
            enabled: !isChangingPassword && fldActualPassword.componentValid && validNewPassword && validConfirmNewPassword
        }
    }

    ProfileControl {
        id: control
    }
}
