#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickView>
#include "debug.h"
#include "invisox_common.h"

#include "cscriptstore.h"
#include "csettingstore.h"

#include "cossystem.h"

#if 0
#	include "cinterpenvstore_win.h"
#	include <stdlib.h>
#endif

#include "engine.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	qmlRegisterType<CScriptStore>("ScriptStore", 1, 0, "ScriptStore");
	qmlRegisterType<CSettingStore>("SettingStore", 1, 0, "SettingStore");
	qmlRegisterType<COSSystem>("OSSystem", 1, 0, "OSSystem");

#if 0
	CInterpEnvStore envStore;
	envStore.slotParser();
#endif

	QQmlApplicationEngine engine;
	engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));
    QQuickWindow *wnd = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    HWND hwnd;
    if (nullptr != wnd) {
        hwnd = reinterpret_cast<HWND>(wnd->winId());
        engRegisterClass(hwnd);
    }

	app.setWindowIcon(QIcon("qrc:///image/res/png/mainicon.png"));

    int result = app.exec();
    engUnregisterClass(hwnd);
    return result;
}
