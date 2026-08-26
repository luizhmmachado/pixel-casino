import QtQuick 2.15
import Colors 1.0
import Fonts 1.0
import Components 1.0

Rectangle {
    id: rctDeposit

    property bool mobileLayout: width < 700

    property string userName: ""
    property string userEmail: ""
    property string userBirthDate: ""
    property string userCpf: ""
    property string formattedCpf: ""

    property alias mouseAreaEditUserName: mouseAreaEditUserName
    property alias mouseAreaEditEmail: mouseAreaEditEmail

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
            text: qsTr( "[ DADOS PESSOAIS ]" )
            font: Fonts.text8bit
            color: Colors.textColor
        }

        Grid {
            width: parent.width
            columns: mobileLayout ? 1 : 2
            spacing: 32

            Column {
                id: clmData1

                width: mobileLayout ? parent.width : (parent.width - parent.spacing) / 2
                spacing: 8

                Row{
                    width: clmData1.width

                    Text {
                        id: txtLabelUsername

                        text: qsTr("Nome de usuário:")
                        font: Fonts.secondaryText8bit
                        color: Colors.yellow200
                    }

                    Item {
                        width: parent.width - txtLabelUsername.width - 16
                        height: 1
                    }


                    Image {
                        height: 16
                        width: 16
                        source: "qrc:/resources/images/icons/edit.svg"

                        MouseArea {
                            id: mouseAreaEditUserName
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                }

                ComponentField {
                    id: fldUserName

                    componentWidth: clmData1.width
                    componentPlaceholder: userName
                    borderColor: Colors.secondary
                    enabled: false
                }

                Item {
                    width: 1
                    height: 8
                }

                Text {
                    text: qsTr("CPF:")
                    font: Fonts.secondaryText8bit
                    color: Colors.yellow200
                }

                ComponentField {
                    id: fldCpf

                    componentWidth: clmData1.width
                    componentPlaceholder: formattedCpf
                    borderColor: Colors.secondary
                    enabled: false
                }
            }

            Column {
                id: clmData2

                width: mobileLayout ? parent.width : (parent.width - parent.spacing) / 2
                spacing: 8

                Row {
                    width: clmData2.width

                    Text {
                        id: txtLabelEmail
                        text: qsTr("E-mail:")
                        font: Fonts.secondaryText8bit
                        color: Colors.yellow200
                    }

                    Item {
                        width: parent.width - txtLabelEmail.width - 16
                        height: 1
                    }


                    Image {
                        height: 16
                        width: 16
                        source: "qrc:/resources/images/icons/edit.svg"

                        MouseArea {
                            id: mouseAreaEditEmail
                            anchors.fill: parent
                            cursorShape: Qt.PointingHandCursor
                        }
                    }
                }

                ComponentField {
                    id: fldEmail
                    componentWidth: clmData2.width
                    componentPlaceholder: userEmail
                    borderColor: Colors.secondary
                    enabled: false
                }

                Item {
                    width: 1
                    height: 8
                }

                Text {
                    text: qsTr("Data de nascimento:")
                    font: Fonts.secondaryText8bit
                    color: Colors.yellow200
                }

                ComponentField {
                    componentWidth: clmData2.width
                    componentPlaceholder: userBirthDate
                    borderColor: Colors.secondary
                    enabled: false
                }
            }
        }
    }
}
