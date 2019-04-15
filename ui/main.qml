import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import ScriptStore 1.0
import SettingStore 1.0
import "qrc:/modules/"

ApplicationWindow {
	visible: true
	id: mainWindow
	width: Screen.width * 2/3
	height: Screen.height * 2/3
	title: qsTr("invisOX")

	objectName: "invisoxMainWindow"

	signal sigEditItemToDialog(variant info);
	signal sigEditSetting(variant info);

	Component.onCompleted: {
		invisScripts.slotEngineReady();
		invisSettings.slotParser();
	}

	onClosing: {
		invisScripts.slotEngineStop();
		invisSettings.slotStore();
		invisScripts.slotStore();
	}

	Loader {
		id: addDialogloader
		// using the connections to get the dialog result
	}

	Loader {
		id: settingDialogLoader
	}

	Connections {
		target: addDialogloader.item
		onSigUpdateInfo: doAddScript(info)
		onSigUpdateEditInfo: doEditScript(editInfo)
	}

	Connections {
		target: settingDialogLoader.item
		onSigUpdateSetting: {
			invisSettings.slotSetSettingInfo(info)
		}
		onSigRemovePluginReq: {
			if(invisSettings.slotRemovePlugin(info))
				settingDialogLoader.item.sigRemovePluginRes(true);
		}
		onSigAddPluginReq: {
			if (invisSettings.slotInsertPlugin(info)) {
				console.log("filename: "+ info)
				settingDialogLoader.item.sigAddPluginRes(info);
			}
		}
	}

	onSigEditItemToDialog: addDialogloader.item.sigInitEditInfo(info)
	onSigEditSetting: settingDialogLoader.item.sigEditSetting(info)

	function doAddScript(addInfo) {
		console.log("add new script");
		listScriptItems.addItemFromDialog(addInfo);
	}

	function doEditScript(editInfo) {
		console.log("do edit script");
		listScriptItems.editItem(editInfo);
	}

	// invisOX's classes
	ScriptStore {
		id: invisScripts
		xmlName: invisSettings.xml_script
		onSigAddListItem: listScriptItems.addItem();
		onSigUpdateItemStatus: listScriptItems.updateItemStatus(updateItem)
		stopKey: invisSettings.xml_stopKey
		onSigUpdateGenScript:  {
			console.log(gen_script);
		}
	}

	SettingStore {
		id: invisSettings

	}

	// ui controls

	menuBar: MenuBar {
		Menu {
			title: qsTr("File")
			MenuItem {
				text: qsTr("Exit")
				onTriggered: Qt.quit();
			}
		}
	}

	toolBar: InviToolBar {
		id: inviToolBar
		height: 52

		onSigClickRemove: {
			var listInfo=listScriptItems.get(listScript.currentRow);
			console.log("curr: " + listScript.currentRow + " actions: " + listInfo["actions"]);
			var info = {"actions":listInfo["actions"],"desc":listInfo["desc"],"script":listInfo["script"],"lang":listInfo["lang"]};
			if (0 ===invisScripts.slotRemoveItem(info))
				listScriptItems.remove(listScript.currentRow);
		}

		onSigClickAdd: {
			console.log("text: " + text);
			var dlg=addDialogloader.setSource("/qml/new_script.qml", {
							visible: true,
							title: text,
							width: mainWindow.width / 2,
							height: mainWindow.height / 2,
							focus: false
							} )
		}

		onSigClickSettings: {
			var dlg=settingDialogLoader.setSource("/qml/settings.qml", {
							visible: true,
							title: text,
							width: mainWindow.width/2,
							height: mainWindow.height/2,
							focus: false
							} )

			sigEditSetting(invisSettings.slotGetSettings());
		}

		onSigClickRun: {
			var listInfo=listScriptItems.get(listScript.currentRow);
			console.log("curr: " + listScript.currentRow + " actions: " + listInfo["actions"]);
			var info = {"actions":listInfo["actions"],"desc":listInfo["desc"],"script":listInfo["script"],"lang":listInfo["lang"]};
			console.log(info);
			invisScripts.slotRunItem(info);
		}

		onSigClickRec: {
			console.log("rec type: ", type);
			if (type === "record") {
				console.log("start record script");
				invisScripts.slotEngineRecReady();
			}
			else if (type === "stop-record") {
				console.log("stop record script");
				invisScripts.slotEngineRecStop();
				invisScripts.slotEngineReady();
			}
		}

		onSigClickTest: {
			invisScripts.testcling();
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
					if (mouse.button === Qt.RightButton) {
						console.log("mouse x: "+ mouse.x + " y: " + mouse.y + " row: " + listScript.rowAt(mouse.x, mouse.y))
						listScript.selection.clear();
						listScript.selection.select(listScript.rowAt(mouse.x, mouse.y));
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

				function updateItemStatus(statusInfo) {
					console.log("item count: " + listScriptItems.count);
					for (var i=0; i<count; i++) {
						var obj = listScriptItems.get(i);
						if (obj.actions === statusInfo.actions) {
							var info = {"status": statusInfo.status, "actions":obj.actions, "desc":obj.desc, "script":obj.script, "lang":obj.lang};
							console.log("update item: ", i, " status: ", info.status);
							listScriptItems.set(i, info);
							break;
						}
					}
				}

			}

		}

	}


}
