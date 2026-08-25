import QtQuick 2.15

ProfilePageDesign {
	signal signOut()
	signal userNameUpdated(string newUserName)
	signal userEmailUpdated(string newEmail)
	signal userAvatarUpdated(int avatarIndex, int avatarColorIndex)
	signal showLoading(bool show)
	signal sessionRefreshed(string refreshToken)

	property string editingField: ""
	property int pendingAvatarIndex: -1
	property int pendingAvatarColorIndex: -1

	function parseBalance(value) {
		var normalized = (value || "").replace(/[^0-9,.-]/g, "")
		normalized = normalized.replace(/\./g, "").replace(/,/g, ".")

		var parsed = parseFloat(normalized)
		return isNaN(parsed) ? 0 : parsed
	}

	canWithdraw: parseBalance(userBalance) > 0

	btnExit.onClicked: signOut()

	profileData.onEditUserNameRequested: {
		editingField = "userName"
		popupEditUserName.errorText = ""
		popupEditUserName.successText = ""
		popupEditUserName.open()
	}

	profileData.onEditEmailRequested: {
		editingField = "userEmail"
		popupEditEmail.errorText = ""
		popupEditEmail.successText = ""
		popupEditEmail.open()
	}

	popupEditUserName.onConfirm: {
		profileControl.userName = userName
		profileControl.changeUserName(popupEditUserName.fldInput.text)
	}

	popupEditUserName.onCancel: {
		editingField = ""
	}

	popupEditEmail.onConfirm: {
		profileControl.userName = userName
		profileControl.changeEmail(popupEditEmail.fldInput.text)
	}

	popupEditEmail.onCancel: {
		editingField = ""
	}

	profileCard.btnChangePfp.onClicked: {
		editingField = "avatar"
		popupSelectAvatar.errorText = ""
		popupSelectAvatar.successText = ""
		popupSelectAvatar.selectedAvatarIndex = userAvatarIndex
		popupSelectAvatar.selectedColorIndex = userAvatarColorIndex
		popupSelectAvatar.open()
	}

	popupSelectAvatar.onConfirm: {
		pendingAvatarIndex = popupSelectAvatar.selectedAvatarIndex
		pendingAvatarColorIndex = popupSelectAvatar.selectedColorIndex
		profileControl.userName = userName
		profileControl.changeAvatar(popupSelectAvatar.selectedAvatarIndex, popupSelectAvatar.selectedColorIndex)
	}

	popupSelectAvatar.onCancel: {
		editingField = ""
	}

	profileControl.onSuccess: {
		if (editingField === "userName") {
			popupEditUserName.errorText = ""
			popupEditUserName.successText = qsTr("Nome de usuário alterado com sucesso.")
			popupEditUserName.fldInput.enabled = false
			userNameUpdated(profileControl.userName)
		} else if (editingField === "userEmail") {
			popupEditEmail.errorText = ""
			popupEditEmail.successText = qsTr("E-mail alterado com sucesso.")
			popupEditEmail.fldInput.enabled = false
			userEmailUpdated(popupEditEmail.fldInput.text)
		} else if (editingField === "avatar") {
			popupSelectAvatar.errorText = ""
			popupSelectAvatar.successText = qsTr("Avatar alterado com sucesso.")
			userAvatarUpdated(pendingAvatarIndex, pendingAvatarColorIndex)
		}
	}

	popupEditEmail.onOpened: {
		popupEditEmail.fldInput.enabled = true
		popupEditEmail.fldInput.text = ""
	}
	popupEditUserName.onOpened: {
		popupEditUserName.fldInput.enabled = true
		popupEditUserName.fldInput.text = ""
	}

	profileControl.onShowLoading: {
		showLoading(show)
	}

	profilePagePassword.onShowLoading: {
		showLoading(show)
	}

	profilePagePassword.onSessionRefreshed: function(refreshToken) {
		sessionRefreshed(refreshToken)
	}

	profileControl.onFail: {
		if (editingField === "userName") {
			popupEditUserName.errorText = msg
		} else if (editingField === "userEmail") {
			popupEditEmail.errorText = msg
		} else if (editingField === "avatar") {
			popupSelectAvatar.errorText = msg
		}
	}
}
