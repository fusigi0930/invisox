#include <QApplication>
#include <QQmlApplicationEngine>

#include <QQmlEngine>
#include <QQmlComponent>

#include "cscriptstore.h"
#include "csettingstore.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<CScriptStore>("ScriptStore", 1, 0, "ScriptStore");
    qmlRegisterType<CSettingStore>("SettingStore", 1, 0, "SettingStore");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:///qml/main.qml")));

    return app.exec();
}
