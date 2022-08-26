#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QQmlContext>
#include "qsettingini.h"
#include "qfileutils.h"
#include "qqclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QSettingIni qSettingIni("config.ini");
    QFileUtils qFileUtils;
    ClientInfo clientInfo;

    QQmlApplicationEngine engine;
    QQmlContext* root = engine.rootContext();

    root->setContextProperty("Config", &qSettingIni);
    root->setContextProperty("QFileUtils", &qFileUtils);
    root->setContextProperty("ClientInfo", &clientInfo);

    const QUrl url(QStringLiteral("qrc:/LoginWindow.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    //C++后台通讯
    QQClient* qqClient=new QQClient(&engine,&app);

    return app.exec();
}
