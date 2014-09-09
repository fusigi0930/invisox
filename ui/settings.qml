import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Dialog {
    id: settingDialog
    modality: Qt.ApplicationModal

    signal sigEditSetting(variant info);
    signal sigUpdateSetting(variant info);

    Component.onCompleted: {
        initFocus();
    }

    onSigEditSetting: {
        for (var prop in info) {
            if ("script" === prop) {
                editScriptFile.text=info[prop];
            }
            else if ("stop_key" === prop) {
                detectAction.text=info[prop];
            }
            else if ("method" === prop) {
                comboMethod.currentIndex=info[prop];
            }
            else if ("plug_in" === prop.substr(0,7)) {
                console.log("plug_in: "+info[prop]);
                var pluginsFile;
                pluginsFile={"file":info[prop]};
                listPluginsItem.append(pluginsFile);
            }
        }
    }

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
                initFocus();
            }
            onRejected: {
                initFocus();
            }
        }

        onClicked: {
            fileDlg.open();
        }
    }

    Text {
        id: labelMethod
        x: 5
        y: labelScriptFile.paintedHeight+labelScriptFile.y+15
        text: qsTr("Work Method:")
    }

    ComboBox {
        id: comboMethod
        x: labelMethod.x+labelMethod.paintedWidth +3
        y: labelMethod.y-3
        currentIndex: 0

        model: [qsTr("Normal"), qsTr("Send Message")]
        onCurrentIndexChanged: {
            initFocus();
        }
    }

    Text {
        id: labelSpecProc
        x: comboMethod.x+comboMethod.width +13
        y: labelMethod.y
        text: qsTr("Process:")
    }

    ComboBox {
        id: comboSpecProc
        x: labelSpecProc.x+labelSpecProc.width +3
        y: labelSpecProc.y-3

        model: ["none"]
        enabled: comboMethod.currentIndex != 0
    }

    Text {
        id: labelHotkey
        x: 5
        y: labelMethod.paintedHeight+labelMethod.y+15
        text: qsTr("Ternimal Key:")
    }

    Rectangle {
        id: rectHotkey
        x: labelHotkey.paintedWidth+labelHotkey.x+3
        y: labelHotkey.y-3
        width: settingDialog.width/1.5
        height: labelMethod.paintedHeight+6

        border.width: 1
        border.color: "#8080FF"

        Item {
            id: keyHandleAction
            focus: true
            enabled: true
            Keys.onPressed: {
                var szAction="";
                console.log("key: " + event.key + " f1: " + Qt.Key_F1);
                if ((event.key >= Qt.Key_F1) && (event.key <= Qt.Key_F12)) {
                    szAction="";
                    if (event.modifiers & Qt.ControlModifier) {
                        szAction += "ctrl+"
                    }
                    if (event.modifiers & Qt.AltModifier) {
                        szAction += "alt+"
                    }
                    if (event.modifiers & Qt.ShiftModifier) {
                        szAction += "shift+"
                    }
                    if (event.modifiers !== Qt.NoModifier) {
                        szAction += "F" + (event.key-Qt.Key_F1+1)
                        detectAction.text=szAction
                        event.accepted=true
                    }

                }
                else if (((event.key >= Qt.Key_0) && (event.key <= Qt.Key_9)) ||
                         ((event.key >= Qt.Key_A) && (event.key <= Qt.Key_Z)) ||
                         (event.key === Qt.Key_Equal)) {
                    szAction="";
                    if (event.modifiers & Qt.ControlModifier) {
                        szAction += "ctrl+"
                    }
                    if (event.modifiers & Qt.AltModifier) {
                        szAction += "alt+"
                    }
                    if (event.modifiers & Qt.ShiftModifier) {
                        szAction += "shift+"
                    }
                    console.log("modified: " + event.modifiers + " no: " + Qt.NoModifier)
                    if (event.modifiers !== Qt.NoModifier) {
                        szAction += String.fromCharCode(event.key)
                        detectAction.text=szAction
                        event.accepted=true
                    }

                }
            }

        }

        Text {
            id: detectAction
            anchors.fill:parent
            visible: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                keyHandleAction.focus=true
            }
        }
    }

    Button {
        id: buttonCancel
        x: settingDialog.width-width-25
        y: buttonOK.y
        text: qsTr("Cancel")

        onClicked: {
            close();
        }
    }

    Rectangle {
        id: rectPlugins
        x: labelHotkey.x
        y: rectHotkey.y+rectHotkey.height+15

        border.width: 2
        border.color: "#7070F0"

        width: settingDialog.width-2*x-20
        height: buttonCancel.y-y-30

        TableView {
            id: listPlugins
            x: 0
            y: 0
            width: parent.width
            height: parent.height
            model: listPluginsItem

            TableViewColumn{ role: "enabled"; title: ("v"); width: 25; delegate: checkEnable }
            TableViewColumn{ role: "file"; title: qsTr("file name"); width: 500 }
        }

        ListModel {
            id: listPluginsItem

            //ListElement {
            //    enabled: "" ; file: "test"
            //}
        }

        Component {
            id: checkEnable
            CheckBox {
                id: cboxEnable
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        cboxEnable.checked=!cboxEnable.checked
                    }
                }
            }
        }

    }

    Button {
        id: buttonOK
        x: buttonCancel.x-width-15
        y: settingDialog.height-buttonOK.height-30
        text: qsTr("OK")
        onClicked: {
            var info;
            info={"script":editScriptFile.text,"stop_key":detectAction.text,"method":comboMethod.currentIndex};
            for (var i=0; i<listPluginsItem.count; i++) {
                info["plug_in"+i]=listPluginsItem.get(i)["file"]
            }

            sigUpdateSetting(info);
            close();
        }
    }

    function initFocus() {
        keyHandleAction.focus=true
    }
}
