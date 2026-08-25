import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import DataBaseControl 1.0
import Colors 1.0
import Fonts 1.0
import Components 1.0

Item {
    id: root

    signal register
    signal success( var balance, string userName, string creationDate, string userCpf, string userEmail, string birthDate, int avatarIndex, int avatarColorIndex )
    signal sessionEstablished( string refreshToken )
    signal showLoading( bool show )

    property bool updatingIdentifier: false

    function validateCpf(cpf) {
        var numbers = cpf.replace(/\D/g, "")

        if (numbers.length !== 11)
            return false

        if (/^(\d)\1{10}$/.test(numbers))
            return false

        var sum = 0

        for (var i = 0; i < 9; i++)
            sum += parseInt(numbers.charAt(i)) * (10 - i)

        var remainder = sum % 11
        var digit1 = remainder < 2 ? 0 : 11 - remainder

        if (digit1 !== parseInt(numbers.charAt(9)))
            return false

        sum = 0

        for (var j = 0; j < 10; j++)
            sum += parseInt(numbers.charAt(j)) * (11 - j)

        remainder = sum % 11
        var digit2 = remainder < 2 ? 0 : 11 - remainder

        return digit2 === parseInt(numbers.charAt(10))
    }

    function formatCpf(cpf) {
        var numbers = cpf.replace(/\D/g, "")

        if (numbers.length !== 11)
            return cpf

        return numbers.substring(0, 3) + "." + numbers.substring(3, 6) + "." + numbers.substring(6, 9) + "-" + numbers.substring(9, 11)
    }

    anchors.fill: parent

    Rectangle {
        anchors.fill: parent
        color: Colors.background

        Rectangle {
            id: rctForm

            width: parent.width * 0.4
            height: formColumn.implicitHeight + 128

            anchors.centerIn: parent

            border.width: 2
            border.color: Colors.yellow100
            color: Colors.primary

            Column {
                id: formColumn

                width: parent.width - 64

                anchors {
                    top: parent.top
                    topMargin: 64
                    horizontalCenter: parent.horizontalCenter
                }

                spacing: 16

                Label {
                    width: parent.width

                    text: qsTr( "[ LOGIN ]" )
                    font: Fonts.title8bit
                    color: Colors.yellow200

                    horizontalAlignment: Text.AlignHCenter
                }

                Item {
                    width: 1
                    height: 16
                }

                Column {
                    width: parent.width

                    spacing: 8

                    Text {
                        text: qsTr( "E-MAIL / USUÁRIO / CPF:" )
                        font: Fonts.text8bit
                        color: Colors.yellow200
                    }

                    ComponentField {
                        id: fldIdentifier
                        componentWidth: parent.width
                        componentValid: text.length > 0

                        onTextChanged: {
                            if (root.updatingIdentifier) {
                                control.email = text
                                return
                            }

                            var typedText = text
                            var cpfDigits = typedText.replace(/\D/g, "")

                            if (cpfDigits.length === 11 && root.validateCpf(cpfDigits)) {
                                var maskedCpf = root.formatCpf(cpfDigits)

                                if (maskedCpf !== typedText) {
                                    root.updatingIdentifier = true
                                    text = maskedCpf
                                    root.updatingIdentifier = false
                                    typedText = maskedCpf
                                }
                            }

                            control.email = typedText
                        }
                    }

                }

                Column {
                    width: parent.width

                    spacing: 8

                    Text {
                        text: qsTr( "SENHA:" )
                        font: Fonts.text8bit
                        color: Colors.yellow200
                    }

                    ComponentField {
                        id: fldPassword
                        componentWidth: parent.width
                        componentEchoMode: TextInput.Password
                        componentValidator: RegularExpressionValidator {
                            regularExpression: /.{8,20}/
                        }
                        componentValid: text.length > 0 && acceptableInput

                        onTextChanged: control.password = text
                    }
                }

                Text {
                    id: txtError

                    visible: false

                    color: Colors.error
                    font: Fonts.secondaryText8bit
                }

                Item {
                    width: parent.width
                    height: 64

                    ComponentButton {
                        id: btnLogin

                        anchors.bottom: parent.bottom

                        componentWidth: parent.width
                        componentHeight: 48
                        componentBtnText: qsTr( "[ ENTRAR ]" )

                        onClicked: {
                            control.authenticate()
                        }
                    }
                }

                Text {
                    id: txtRegister

                    width: parent.width

                    text: qsTr( "Não tem conta? Cadastre-se" )
                    font: Fonts.underlinedText8bit
                    color: Colors.yellow200

                    horizontalAlignment: Text.AlignHCenter

                    anchors.bottomMargin: 64

                    MouseArea {
                        anchors.fill: parent

                        onClicked: root.register()
                    }
                }


            }
        }
    }

    DataBaseControl {
        id: control

        onShowLoading: {
            root.showLoading(show)
            btnLogin.enabled = !show
        }

        onSuccess: {
            root.success( formattedBalance, userName, creationDate, cpf, email, birthDate, avatarIndex, avatarColorIndex )
        }

        onSessionEstablished: function(refreshToken) {
            root.sessionEstablished(refreshToken)
        }

        onFail: {
            txtError.text = msg
            txtError.visible = true
        }
    }
}
