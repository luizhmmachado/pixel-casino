import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQml 2.15
import DataBaseControl 1.0
import Colors 1.0
import Fonts 1.0
import Components 1.0

Item {
    id: root
    anchors.fill: parent

    signal login
    signal success(var balance, string userName, string creationDate, string userCpf, string userEmail, string birthDate, int avatarIndex, int avatarColorIndex)
    signal sessionEstablished(string refreshToken)
    signal showLoading(bool show)

    property int day: -1
    property int month: -1
    property int year: -1
    property bool validAge: false
    property bool validCpf: false
    property bool validPassword: false
    property bool validConfirmPassword: false
    property bool validName: false
    property bool validEmail: false

    property var passwordRequirements: [
        "Contém pelo menos 8 caracteres",
        "Contém letra maiúscula",
        "Contém letra minúscula",
        "Contém número",
        "Contém caractere especial"
    ]

    function hasUppercase(password) {
        return /[A-Z]/.test(password)
    }

    function hasLowercase(password) {
        return /[a-z]/.test(password)
    }

    function hasNumber(password) {
        return /\d/.test(password)
    }

    function hasSpecial(password) {
        return /[^A-Za-z0-9]/.test(password)
    }

    function hasSize(password) {
        return password.length >= 8
    }

    function validatePasswordRequirement(index, password) {
        switch (index) {
        case 0:
            return hasSize(password)
        case 1:
            return hasUppercase(password)
        case 2:
            return hasLowercase(password)
        case 3:
            return hasNumber(password)
        case 4:
            return hasSpecial(password)
        default:
            return false
        }
    }

    function verifyPassword(password) {
        return hasUppercase(password) && hasLowercase(password) && hasNumber(password) && hasSpecial(password) && hasSize(password)
    }

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

    function calculateAge(day, month, year) {
        var today = new Date()
        var birth = new Date(year, month - 1, day)
        var age = today.getFullYear() - birth.getFullYear()
        var monthDifference = today.getMonth() - birth.getMonth()

        if (monthDifference < 0 || (monthDifference === 0 && today.getDate() < birth.getDate()))
            age--

        return age
    }

    function validateBirthDate(day, month, year) {
        if (day < 1 || month < 1 || year < 1900)
            return false

        var date = new Date(year, month - 1, day)

        if (date.getFullYear() !== year || date.getMonth() !== month - 1 || date.getDate() !== day)
            return false

        return calculateAge(day, month, year) >= 18
    }

    function updatePasswordMatch() {
        root.validConfirmPassword = fldConfirmPassword.text.length > 0 && fldConfirmPassword.text === fldPassword.text
    }

    function updateFormValidation() {
        root.validName = fldName.componentText.trim().length >= 8 && fldName.acceptableInput
        root.validEmail = fldEmail.componentText.length > 0 && fldEmail.acceptableInput
        root.validCpf = root.validateCpf(fldCpf.text.replace(/\D/g, ""))
        root.validAge = root.validateBirthDate(root.day, root.month, root.year)
        root.validPassword = root.verifyPassword(fldPassword.componentText)
        updatePasswordMatch()

        btnRegister.enabled = root.validName && root.validEmail && root.validCpf && root.validAge && root.validPassword && root.validConfirmPassword
    }

    function updateBirthDate() {
        var dayText = inputDay.text
        var monthText = inputMonth.text
        var yearText = inputYear.text

        validAge = false

        if (dayText.length > 0 && monthText.length > 0 && yearText.length > 0) {
            var dayNum = parseInt(dayText)
            var monthNum = parseInt(monthText)
            var yearNum = parseInt(yearText)

            if (!isNaN(dayNum) && !isNaN(monthNum) && !isNaN(yearNum)) {
                day = dayNum
                month = monthNum
                year = yearNum

                validAge = validateBirthDate(day, month, year)

                if (validAge)
                    control.birthDt = day + "-" + month + "-" + year
            }
        }

        updateFormValidation()
    }

    Rectangle {
        anchors.fill: parent
        color: Colors.background

        Rectangle {
            id: rctForm
            width: parent.width * 0.6
            height: formColumn.implicitHeight + 128
            anchors.centerIn: parent
            border.width: 2
            border.color: Colors.yellow100
            color: Colors.primary

            Column {
                id: formColumn
                width: parent.width - 64
                anchors.top: parent.top
                anchors.topMargin: 64
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16

                Label {
                    width: parent.width
                    text: qsTr("[ NOVO CADASTRO ]")
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
                        text: qsTr("NOME COMPLETO:")
                        font: Fonts.secondaryText8bit
                        color: Colors.yellow200
                    }

                    ComponentField {
                        id: fldName
                        componentWidth: parent.width
                        componentPlaceholder: qsTr("DIGITE SEU NOME")
                        componentValidator: RegularExpressionValidator { regularExpression: /^(?=.*[A-Za-zÀ-ÖØ-öø-ÿ])[A-Za-zÀ-ÖØ-öø-ÿ' -]+$/ }
                        componentValid: validName

                        onTextChanged: {
                            control.name = text
                            updateFormValidation()
                        }
                    }
                }

                Column {
                    width: parent.width
                    spacing: 8

                    Text {
                        text: qsTr("E-MAIL:")
                        font: Fonts.secondaryText8bit
                        color: Colors.yellow200
                    }

                    ComponentField {
                        id: fldEmail
                        componentWidth: parent.width
                        componentPlaceholder: qsTr("SEU@EMAIL.COM")
                        componentValidator: RegularExpressionValidator { regularExpression: /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-z]{2,}$/ }
                        componentValid: validEmail

                        onTextChanged: {
                            control.email = text
                            updateFormValidation()
                        }
                    }
                }

                Row {
                    width: parent.width
                    spacing: 16

                    Column {
                        width: (parent.width - 16) / 2
                        spacing: 8

                        Text {
                            text: qsTr("CPF:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        ComponentField {
                            id: fldCpf

                            property bool cpfValid: false

                            componentWidth: parent.width
                            componentValid: cpfValid
                            componentPlaceholder: qsTr("000.000.000-00")
                            inputMethodHints: Qt.ImhDigitsOnly

                            onTextChanged: {
                                var numbers = text.replace(/\D/g, "")

                                if (numbers.length > 11)
                                    numbers = numbers.substring(0, 11)

                                var formatted = ""

                                if (numbers.length > 0)
                                    formatted += numbers.substring(0, 3)

                                if (numbers.length >= 4)
                                    formatted += "." + numbers.substring(3, 6)

                                if (numbers.length >= 7)
                                    formatted += "." + numbers.substring(6, 9)

                                if (numbers.length >= 10)
                                    formatted += "-" + numbers.substring(9, 11)

                                if (formatted !== text) {
                                    text = formatted
                                }

                                cpfValid = root.validateCpf(numbers)
                                root.validCpf = cpfValid
                                control.cpf = numbers
                                root.updateFormValidation()
                            }
                        }
                    }

                    Column {
                        width: (parent.width - 16) / 2
                        spacing: 8

                        Text {
                            text: qsTr("NASCIMENTO:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        Row {
                            width: parent.width
                            spacing: 8

                            ComponentField {
                                id: inputDay
                                componentWidth: 48
                                componentPlaceholder: "DD"
                                componentValid: validAge
                                inputMethodHints: Qt.ImhDigitsOnly

                                onTextChanged: {
                                    updateBirthDate()
                                }
                            }

                            ComponentField {
                                id: inputMonth
                                componentWidth: 48
                                componentPlaceholder: "MM"
                                componentValid: validAge
                                inputMethodHints: Qt.ImhDigitsOnly

                                onTextChanged: {
                                    updateBirthDate()
                                }
                            }

                            ComponentField {
                                id: inputYear
                                componentWidth: parent.width - 112
                                componentPlaceholder: "AAAA"
                                componentValid: validAge
                                inputMethodHints: Qt.ImhDigitsOnly

                                onTextChanged: {
                                    updateBirthDate()
                                }
                            }
                        }
                    }
                }

                Row {
                    width: parent.width
                    spacing: 16

                    Column {
                        width: (parent.width - 16) / 2
                        spacing: 8

                        Text {
                            text: qsTr("SENHA:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        ComponentField {
                            id: fldPassword

                            componentWidth: parent.width
                            componentPlaceholder: qsTr("••••••••")
                            componentEchoMode: TextInput.Password
                            componentValidator: RegularExpressionValidator { regularExpression: /.{8,20}/ }
                            componentValid: root.validPassword

                            onTextChanged: {
                                control.password = text
                                root.validPassword = root.verifyPassword(componentText)
                                root.updatePasswordMatch()
                                root.updateFormValidation()
                            }
                        }
                    }

                    Column {
                        width: (parent.width - 16) / 2
                        spacing: 8

                        Text {
                            text: qsTr("CONFIRMAR SENHA:")
                            font: Fonts.secondaryText8bit
                            color: Colors.yellow200
                        }

                        ComponentField {
                            id: fldConfirmPassword

                            componentWidth: parent.width
                            componentPlaceholder: qsTr("••••••••")
                            componentEchoMode: TextInput.Password
                            componentValid: root.validConfirmPassword

                            onTextChanged: {
                                root.updatePasswordMatch()
                                root.updateFormValidation()
                            }
                        }
                    }
                }

                Item {
                    width: 1
                    height: 8
                }

                Rectangle {
                    id: rctPasswordRequirements
                    width: parent.width
                    height: passwordRequirementsColumn.implicitHeight + 20
                    color: Colors.background
                    border.width: 2
                    border.color: Colors.secondary

                    Column {
                        id: passwordRequirementsColumn
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
                                    text: validatePasswordRequirement(index, fldPassword.componentText) ? "✓" : "x"
                                    color: validatePasswordRequirement(index, fldPassword.componentText) ? Colors.success : Colors.error
                                    font: Fonts.secondaryText8bit
                                }

                                Text {
                                    text: passwordRequirements[index]
                                    color: Colors.secondary
                                    font.family: "Press Start 2P"
                                    font.pixelSize: 12
                                    font.strikeout: validatePasswordRequirement(index, fldPassword.componentText)
                                }
                            }
                        }
                    }
                }

                Item {
                    width: 1
                    height: 8
                }

                Text {
                    id: txtError
                    width: parent.width
                    visible: text.length > 0
                    color: Colors.error
                    font: Fonts.secondaryText8bit
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }

                ComponentButton {
                    id: btnRegister
                    componentWidth: parent.width
                    componentHeight: 48
                    componentBtnText: qsTr("[ CADASTRAR ]")
                    enabled: false

                    onClicked: {
                        control.insert()
                    }
                }

                Text {
                    id: txtLogin
                    width: parent.width
                    text: qsTr("Já tem conta? Faça Login")
                    font: Fonts.underlinedText8bit
                    color: Colors.yellow200
                    horizontalAlignment: Text.AlignHCenter

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            root.login()
                        }
                    }
                }
            }
        }
    }

    DataBaseControl {
        id: control

        onShowLoading: {
            root.showLoading(show)
            btnRegister.enabled = !show && validName && validEmail && validCpf && validAge && validPassword && validConfirmPassword

            if (show){
                txtError.visible = false
            }
        }

        onSuccess: {
            root.success(formattedBalance, userName, creationDate, cpf, email, birthDate, avatarIndex, avatarColorIndex)
        }

        onSessionEstablished: function(refreshToken) {
            root.sessionEstablished(refreshToken)
        }

        onFail: {
            txtError.text = msg
            txtError.visible = true
            btnRegister.enabled = validName && validEmail && validCpf && validAge && validPassword && validConfirmPassword
        }
    }
}
