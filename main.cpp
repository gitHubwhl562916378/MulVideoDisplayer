#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <Windows.h>
#include "VideoWidget/videoitem.h"
#include "Service/servicefacetory.h"
#include "common_data.h"
#include "filehelper.h"

extern "C" {
//确保连接了nvidia的显示器
//    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
//确保连接了amd的显示器
//    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}
#pragma execution_character_set("utf-8")
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<VideoItem>("SceneGraphRendering", 1, 0, "VideoItem");
    qmlRegisterType<FileHelper>("SceneGraphRendering", 1, 0, "FileHelper");

    ServiceFactoryI *facetoryI = new ServiceFactory;
    app.setProperty(FACETORY_KEY, reinterpret_cast<unsigned long long>(facetoryI));

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    QObject::connect(qGuiApp, &QGuiApplication::aboutToQuit, [&]{
        delete facetoryI;
    });
    return app.exec();
}
