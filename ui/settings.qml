import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Dialog {
    id: settingDialog
    modality: Qt.ApplicationModal

    Text {
        id: labelScriptFile
        x: 5
        y: 2
        text: qsTr("Script List:")
    }

    Rectangle {
        id: rectScriptFile
        x: labelScriptFile.paintedWidth+labelScriptFile.x+3
        y: labelScriptFile.y-3
        width: settingDialog.width/1.5
        height: labelScriptFile.paintedHeight+6

        border.width: 1
        border.color: "#8080FF"

        TextInput {
            id: editScriptFile
            anchors.fill:parent
            visible: true
            focus: false
        }

    }

    Button {
        id: buttonBrowse
        x: rectScriptFile.x+rectScriptFile.width +3
        y: rectScriptFile.y
        text: qsTr("Browse")

        FileDialog {
            id: fileDlg
            selectMultiple: false
            selectFolder: false
            selectExisting: true
            title: qsTr("Select a script list file...")

            nameFilters: [ "xml files (*.xml)", "All files (*)" ]

            onAccepted: {
                editScriptFile.text=fileDlg.fileUrl.toString();
                initFoucs();
            }
            onRejected: {
                initFoucs();
            }
        }

        onClicked: {
            fileDlg.open();
        }
    }
}
