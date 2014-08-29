import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Dialog {
    id: scriptDialog
    modality: Qt.ApplicationModal

    signal sigUpdateInfo(variant info)

    Component.onCompleted: {
        initFoucs();
    }


    Text {
        id: labelAction
        x: 5
        y: 2
        text: qsTr("Action:")
    }

    ComboBox {
        id: comboAction
        x: rectAction.x+rectAction.width +3
        y: rectAction.y
        currentIndex: 0

        model: [qsTr("Hotkey"), qsTr("Net command")]
        onCurrentIndexChanged: {
            initFoucs();
        }
    }

    Rectangle {
        id: rectAction
        x: labelAction.paintedWidth+labelAction.x+3
        y: labelAction.y-3
        width: scriptDialog.width/1.5
        height: labelAction.paintedHeight+6

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
                        szAction += "ctrl + "
                    }
                    if (event.modifiers & Qt.AltModifier) {
                        szAction += "alt + "
                    }
                    if (event.modifiers & Qt.ShiftModifier) {
                        szAction += "shift + "
                    }
                    if (event.modifiers !== Qt.NoModifier) {
                        szAction += "F" + (event.key-Qt.Key_F1+1)
                        detectAction.text=szAction
                        event.accepted=true
                    }

                }
                else if (((event.key >= Qt.Key_0) && (event.key <= Qt.Key_9)) ||
                         ((event.key >= Qt.Key_A) && (event.key <= Qt.Key_Z))) {
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

        TextInput {
            id: editAction
            anchors.fill:parent
            visible: false
            focus: false
        }

        Text {
            id: detectAction
            anchors.fill:parent
            visible: true
        }
    }

    Text {
        id: labelFile
        x: labelAction.x
        y: labelAction.paintedHeight+labelAction.y+15
        text: qsTr("File:")
    }

    Rectangle {
        id: rectFile
        x: labelFile.paintedWidth+labelFile.x+3
        y: labelFile.y-3
        width: scriptDialog.width/1.5
        height: labelFile.paintedHeight+6

        border.width: 1
        border.color: "#8080FF"

        TextInput {
            id: editFile
            anchors.fill:parent
            visible: true
            focus: false
        }

    }

    Button {
        id: buttonBrowse
        x: rectFile.x+rectFile.width +3
        y: rectFile.y
        text: qsTr("Browse")

        FileDialog {
            id: fileDlg
            selectMultiple: false
            selectFolder: false
            selectExisting: true
            title: qsTr("Select a script file...")

            nameFilters: [ "Cpp files (*.cpp)", "php files (*.php)", "basic files (*.bas)", "js file (*.js)", "All files (*)" ]

            onAccepted: {
                editFile.text=fileDlg.fileUrl.toString();
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

    Text {
        id: labelInterp
        x: labelFile.x
        y: labelFile.paintedHeight+labelFile.y+15
        text: qsTr("Interpreter:")
    }

    ComboBox {
        id: comboInterp
        x: labelInterp.x+labelInterp.paintedWidth +3
        y: labelInterp.y-3
        currentIndex: 0

        model: [qsTr("C/C++"), qsTr("PHP"), qsTr("Basic"), qsTr("Java script")]
        onCurrentIndexChanged: {
            initFoucs();
        }
    }

    Rectangle {
        id: rectDesc
        x: labelInterp.x
        y: labelInterp.paintedHeight+labelInterp.y+15
        width: scriptDialog.width-28
        height: scriptDialog.height-y-30

        border.width: 1
        border.color: "#8080FF"

        Flickable {
            id: flickDesc
            anchors.fill: parent
            width: rectDesc.width
            height: rectDesc.height

            contentWidth: editDesc.width
            contentHeight: editDesc.height

            function ensureVisible(rect) {
                 if (contentX >= rect.x)
                     contentX = rect.x;
                 else if (contentX+width <= rect.x+rect.width)
                     contentX = rect.x+rect.width-width;
                 if (contentY >= rect.y)
                     contentY = rect.y;
                 else if (contentY+height <= rect.y+rect.height)
                     contentY = rect.y+rect.height-height;
            }


            TextEdit {
                id: editDesc
                x: 2
                y: 1
                width: flickDesc.width-4
                transformOrigin: Item.TopLeft
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                wrapMode: Text.WordWrap

                onCursorRectangleChanged: flickDesc.ensureVisible(cursorRectangle)
            }
        }
    }

    Button {
        id: buttonCancel
        x: scriptDialog.width-width-25
        y: buttonOK.y
        text: qsTr("Cancel")

        onClicked: {
            close();
        }
    }

    Button {
        id: buttonOK
        x: buttonCancel.x-width-15
        y: comboInterp.y
        text: qsTr("OK")
        property variant info

        onClicked: {
            if (editFile.text.trim() === "") {
                return;
            }

            var actionCmd;
            var interpreterCmd;
            switch (comboAction.currentIndex) {
                default:
                case 0: // hotkey
                    if (detectAction.text.trim() === "") {
                        return;
                    }

                    actionCmd=detectAction.text
                    break;
                case 1: // necommand
                    if (editAction.text.trim() === "") {
                        return;
                    }

                    actionCmd="nc:"+editAction.text
                    break;
            }

            switch (comboInterp.currentIndex) {
                default:
                case 0: // c/c++
                    interpreterCmd="c/c++";
                    break;
                case 1: // php
                    interpreterCmd="php";
                    break;
                case 2: // basic
                    interpreterCmd="basic";
                    break;
                case 3: // javascript
                    interpreterCmd="javascript";
                    break;
            }

            info = [
               {"actions":actionCmd,"desc":editDesc.text,"script":editFile.text,"lang":interpreterCmd}
            ]

            sigUpdateInfo(info)
            close();
        }
    }

    function initFoucs() {
        switch (comboAction.currentIndex) {
            default:
                break;
            case 0: // Hotkey
                editAction.visible=false;
                editAction.focus=false;
                detectAction.visible=true;
                keyHandleAction.focus=true;
                break;
            case 1: // Net command
                editAction.visible=true;
                detectAction.visible=false;
                keyHandleAction.focus=false;
                editAction.focus=true;
                break;
        }
    }

}
