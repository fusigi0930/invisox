#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQuickView>
#include "debug.h"

#include "cscriptstore.h"
#include "csettingstore.h"

#include "cossystem.h"

#ifdef Q_OS_WIN
#	include "cinterpenvstore_win.h"
#	include <stdlib.h>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<CScriptStore>("ScriptStore", 1, 0, "ScriptStore");
    qmlRegisterType<CSettingStore>("SettingStore", 1, 0, "SettingStore");
    qmlRegisterType<COSSystem>("OSSystem", 1, 0, "OSSystem");

#ifdef Q_OS_WIN
	CInterpEnvStore envStore;
	envStore.slotParser();
#endif

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));

    app.setWindowIcon(QIcon("qrc:///image/res/png/mainicon.png"));

    return app.exec();
}
