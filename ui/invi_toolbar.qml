import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import "qrc:/modules/"

ToolBar {
	id: rootBar

	signal sigClickRemove()
	signal sigClickAdd(var text)
	signal sigClickRun()
	signal sigClickStop()
	signal sigClickSettings(var text)
	signal sigClickTest()

	RowLayout {
		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("add")
			labelText: qsTr("add")
			iconSource: "/image/res/png/add.png"
			tooltip: qsTr("add a new script file to invisOX")
			onSigClicked: rootBar.sigClickAdd(buttonText)
		}

		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("remove")
			labelText: qsTr("remove")
			iconSource: "/image/res/png/delete.png"
			tooltip: qsTr("remove the selected script from invisOX")
			onSigClicked: rootBar.sigClickRemove()
		}

		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("run")
			labelText: qsTr("run")
			iconSource: "/image/res/png/run.png"
			tooltip: qsTr("run the selected script file")
		}

		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("stop")
			labelText: qsTr("stop")
			iconSource: "/image/res/png/stop.png"
			tooltip: qsTr("stop all running scripts")
		}

		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("settings")
			labelText: qsTr("settings")
			iconSource: "/image/res/png/setting.png"
			tooltip: qsTr("settings")
			onSigClicked: rootBar.sigClickSettings(buttonText)
		}

		InviToolButton {
			width: 48
			height: 40
			buttonText: qsTr("test")
			labelText: qsTr("test")
			tooltip: qsTr("test")
			onSigClicked: rootBar.sigClickTest()
		}
	}
}
