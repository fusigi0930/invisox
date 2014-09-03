import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import ScriptStore 1.0
import SettingStore 1.0

ApplicationWindow {
    visible: true
    id: mainWindow
    width: Screen.width * 2/3
    height: Screen.height * 2/3
    title: qsTr("invisOX")

    signal sigEditItemToDialog(variant info);

    Component.onCompleted: {
        invisScripts.slotParser();

    }

    onClosing: {
        invisScripts.slotStore();
    }

    Loader {
        id: addDialogloader
        // using the connections to get the dialog result
    }

    Connections {
        target: addDialogloader.item
        onSigUpdateInfo: doAddScript(info)
        onSigUpdateEditInfo: doEditScript(editInfo)
    }

    onSigEditItemToDialog: addDialogloader.item.sigInitEditInfo(info)

    function doAddScript(addInfo) {
        listScriptItems.addItemFromDialog(addInfo);
    }

    function doEditScript(editInfo) {
        listScriptItems.editItem(editInfo);
    }


    // invisOX's classes
    ScriptStore {
        id: invisScripts
        xmlName: "test/stest.xml"
        onSigAddListItem: listScriptItems.addItem();

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
                iconSource: "/image/res/png/add.png"
                tooltip: qsTr("add a new script file to invisOX")
                onClicked: {
                    var dlg=addDialogloader.setSource("/qml/new_script.qml", {
                                    visible: true,
                                    title: text,
                                    width: mainWindow.width/2,
                                    height: mainWindow.height/2,
                                    focus: false
                                    } )
                }

            }
            ToolButton {
                text: qsTr("remove script")
                iconSource: "/image/res/png/delete.png"
                tooltip: qsTr("remove the selected script from invisOX")
                onClicked: {
                    var listInfo=listScriptItems.get(listScript.currentRow);
                    console.log("curr: " + listScript.currentRow + " actions: " + listInfo["actions"]);
                    var info = {"actions":listInfo["actions"],"desc":listInfo["desc"],"script":listInfo["script"],"lang":listInfo["lang"]};
                    if (0 ===invisScripts.slotRemoveItem(info))
                        listScriptItems.remove(listScript.currentRow);
                }
            }
            ToolButton {
                text: qsTr("run script")
                tooltip: qsTr("run the selected script file")
                iconSource: "/image/res/png/run.png"
            }
            ToolButton {
                text: qsTr("stop script")
                tooltip: qsTr("stop all running scripts")
                iconSource: "/image/res/png/stop.png"
            }
            ToolButton {
                text: qsTr("settings")
                tooltip: qsTr("settings")
                iconSource: "/image/res/png/setting.png"
            }
        }
    }

    // child controls
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

            property int n_scriptListRightClickPos: -1;

            // initial the header bar
            TableViewColumn{role: "status" ; title: qsTr("Status") ; width: 50}
            TableViewColumn{role: "actions" ; title: qsTr("Actions") ; width: 150}
            TableViewColumn{role: "script" ; title: qsTr("Script Path") ; width: 350}
            TableViewColumn{role: "desc" ; title: qsTr("Descriptiton") ; width: 350}
            TableViewColumn{role: "lang" ; title: qsTr("Language") ; width: 150}

            // context menu
            Menu {
                id: menuScriptList

                MenuItem {
                    text: qsTr("remove")
                    onTriggered: {
                        var listInfo=listScriptItems.get(listScript.n_scriptListRightClickPos);
                        var info = {"actions":listInfo["actions"],"desc":listInfo["desc"],"script":listInfo["script"],"lang":listInfo["lang"]};
                        if (0 ===invisScripts.slotRemoveItem(info))
                            listScriptItems.remove(listScript.n_scriptListRightClickPos);
                    }
                }
                MenuItem {
                    text: qsTr("edit")
                    onTriggered: {
                        var dlg=addDialogloader.setSource("/qml/new_script.qml", {
                            visible: true,
                            title: text,
                            width: mainWindow.width/2,
                            height: mainWindow.height/2,
                            focus: false
                            } )

                        sigEditItemToDialog(listScriptItems.get(listScript.n_scriptListRightClickPos));
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.RightButton
                onClicked: {
                    if (mouse.button == Qt.RightButton) {
                        console.log("mouse x: "+ mouse.x + " y: " + mouse.y + " row: " + listScript.rowAt(mouse.x, mouse.y))
                        if (listScript.rowAt(mouse.x, mouse.y) !== -1) {
                            listScript.n_scriptListRightClickPos= listScript.rowAt(mouse.x, mouse.y)
                            menuScriptList.popup()
                        }
                    }
                }
            }

            // content for listScript TableView
            ListModel {
                id: listScriptItems


                function addItem() {
                    append(invisScripts.item);
                }

                function addItemFromDialog(addInfo) {
                    if (0 === invisScripts.slotAddItem(addInfo))
                        append(addInfo);
                }

                function editItem(editInfo) {
                    if (0 === invisScripts.slotEditItem(editInfo))
                        listScriptItems.set(listScript.n_scriptListRightClickPos, editInfo);
                }

            }

        }

    }


}
