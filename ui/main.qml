import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import ScriptStore 1.0

ApplicationWindow {
    visible: true
    id: mainWindow
    width: Screen.width * 2/3
    height: Screen.height * 2/3
    title: qsTr("Hello World")

    // invisOX's classes
    ScriptStore {
        id: invisScripts
        xmlName: "test/stest.xml"

    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            ToolButton {
                text: qsTr("add script")
                tooltip: qsTr("add a new script file to invisOX")
                onClicked: invisScripts.slotParser();

            }
            ToolButton {
                text: qsTr("remove script")
                tooltip: qsTr("remove the selected script from invisOX")
            }
            ToolButton {
                text: qsTr("run script")
                tooltip: qsTr("run the selected script file")
            }
            ToolButton {
                text: qsTr("stop script")
                tooltip: qsTr("stop all running scripts")
            }
            ToolButton {
                text: qsTr("settings")
                tooltip: qsTr("settings")
            }
        }
    }

    Rectangle {
        id: rectScriptList
        x: 5
        y: 5

        border.width: 2
        border.color: "#7070F0"

        width: mainWindow.width-2*x
        height: mainWindow.contentItem.height-2*y

        TableView {
            id: listScript
            x: 0
            y: 0
            width: parent.width
            height: parent.height

            model: listScriptItems

            TableViewColumn{role: "status" ; title: qsTr("Status") ; width: 50}
            TableViewColumn{role: "actions" ; title: qsTr("Actions") ; width: 150}
            TableViewColumn{role: "script" ; title: qsTr("Script Path") ; width: 350}
            TableViewColumn{role: "desc" ; title: qsTr("Descriptiton") ; width: 350}
            TableViewColumn{role: "lang" ; title: qsTr("Type") ; width: 150}

        }

        // content for listScript TableView
        ListModel {
            id: listScriptItems
            ListElement{ actions: "A" ; script: "b" }
        }

    }


}
