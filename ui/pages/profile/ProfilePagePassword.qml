import QtQuick 2.15
import Colors 1.0

ProfilePagePasswordDesign {
    id: root

    signal showLoading(bool show)
    signal anyTextChanged()
    signal sessionRefreshed(string refreshToken)
    property bool suppressMessageReset: false

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

    function verifyPassword(password) {
        return hasUppercase(password) && hasLowercase(password) && hasNumber(password) && hasSpecial(password) && hasSize(password)
    }

    function updatePasswordRequirementsStatus() {
        var password = fldNewPassword.text

        root.passwordRequirementsValid = [
            hasSize(password),
            hasUppercase(password),
            hasLowercase(password),
            hasNumber(password),
            hasSpecial(password)
        ]
    }

    function updateConfirmNewPasswordValidity() {
        root.validConfirmNewPassword = fldConfirmNewPassword.text.length > 0 && fldConfirmNewPassword.text === fldNewPassword.text
    }

    fldNewPassword.onTextChanged: {
        root.anyTextChanged()
        root.validNewPassword = root.verifyPassword(fldNewPassword.text)
        root.updatePasswordRequirementsStatus()
        root.updateConfirmNewPasswordValidity()
    }

    fldConfirmNewPassword.onTextChanged: {
        root.anyTextChanged()
        root.updateConfirmNewPasswordValidity()
    }

    btnChangePassword.onClicked: {
        control.userName = root.userName
        control.email = root.userEmail
        control.currentPassword = fldActualPassword.text
        control.newPassword = fldNewPassword.text

        control.changePassword()
    }

    control.onShowLoading: {
        root.showLoading(show)
        root.isChangingPassword = show
    }

    control.onSessionRefreshed: function(refreshToken) {
        root.sessionRefreshed(refreshToken)
    }

    control.onSuccess: {
        txtPasswordError.text = qsTr( "Senha alterada com sucesso." )
        txtPasswordError.color = Colors.success

        suppressMessageReset = true
        fldActualPassword.text = ""
        fldNewPassword.text = ""
        fldConfirmNewPassword.text = ""
        suppressMessageReset = false
    }

    control.onFail: {
        txtPasswordError.text = msg
        txtPasswordError.color = Colors.error
    }

    fldActualPassword.onTextChanged: root.anyTextChanged()

    onAnyTextChanged: {
        if (suppressMessageReset) {
            return
        }

        if (fldActualPassword.text === "" && fldNewPassword.text === "" && fldConfirmNewPassword.text === "") {
            return
        }

        txtPasswordError.text = ""
    }
}
