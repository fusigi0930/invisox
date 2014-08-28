import QtQuick 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2

Dialog {
    id: scriptDialog
    modality: Qt.ApplicationModal

    Component.onCompleted: {
        initFoucs();
    }

    Text {
        id: labelAction
        x: 5
        y: 2
        text: qsTr("Action:")
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
                console.log(event.text);
                detectAction.text=event.text;
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
