#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include "filehelper.h"

FileHelper::FileHelper(QObject *parent) : QObject(parent)
{

}

QJsonValue FileHelper::slotReadIni(const QString file_name)
{
    QJsonObject resJson;
    QJsonObject default_obj;
    QJsonArray array;
    QSettings setting(file_name, QSettings::IniFormat);
    resJson.insert("timeout", setting.value("VideoScreen/timeout").toInt() * 1000);
    int default_screen = setting.value("VideoScreen/default").toInt();
    int count = setting.beginReadArray("VideoScreen");
    for(int i = 0; i < count; i++){
        setting.setArrayIndex(i);
        QStringList scrennInfoList = setting.value("screen").toString().split(' ');

        if(scrennInfoList.size() != 6){
            continue;
        }
        int rows = scrennInfoList.first().toInt();
        int cols = scrennInfoList.at(1).toInt();
        int bigFrameRowSpan = scrennInfoList.at(4).toInt();
        int bigFrameColSpan = scrennInfoList.at(5).toInt();
        QJsonObject info{{"rows", scrennInfoList.first().toInt()},
                         {"cols", scrennInfoList.at(1).toInt()},
                         {"bigFrameRow", scrennInfoList.at(2).toInt()},
                         {"bigFrameCol", scrennInfoList.at(3).toInt()},
                         {"bigFrameRowSpan", scrennInfoList.at(4).toInt()},
                         {"bigFrameColSpan", scrennInfoList.at(5).toInt()},
                         {"showCount", rows * cols - bigFrameRowSpan * bigFrameColSpan + 1}};
        array.append(info);

        if(i == default_screen)
        {
            default_obj = info;
        }
    }
    resJson.insert("default_screen", default_obj);
    resJson.insert("screens", array);
    setting.endArray();

    return resJson;
}

QJsonValue FileHelper::slotReadUrl(const QString file_name)
{
    QJsonArray resJson;
    QFile urlFile(file_name);
    if(!urlFile.open(QIODevice::ReadOnly))
        return resJson;

    while (!urlFile.atEnd()) {
        QByteArray line = urlFile.readLine().simplified();
        QList<QByteArray> values = line.split(' ');
        if(values.count() != 2){
            continue;
        }

        resJson.append(QJsonObject{{"url", QString(values.at(0))},{"device", QString(values.at(1))}});
    }
    urlFile.close();

    return  resJson;
}
