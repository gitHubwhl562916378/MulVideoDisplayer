#include "cloudhttpdao.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QSettings>
#include <QUrl>
#include <QBuffer>
#include <QRect>
#include <QDebug>
#include <QDir>
#include <QFileInfoList>
#include <QApplication>

DLL::CloudHttpDao::CloudHttpDao()
{
    std::vector<std::string> headers;
    headers.emplace_back("Accept-Language: en-US,en;q=0.9");
    headers.emplace_back("Content-Type: application/json");
    headers.emplace_back("User-Agent:Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/55.0.2883.87 Safari/537.36");
    // 7d1e52d3cf0142e19b5901eb1ef91372 第三方token
    headers.emplace_back("token: 221792ffb8cc4130a8e3034396cfb63b");
    headers.emplace_back("where: a8d11ce5b78a47d381e0bb62d3229c7bwe0caa");
    headers.emplace_back("Expect:");
    setheader(headers);

    QSettings config("config.ini",QSettings::IniFormat);
    host_ = config.value("Http/host").toString();
}

QString DLL::CloudHttpDao::login(const QString &user, const QString &password, QString *secureKey)
{
    QJsonObject jsObj{{"username", user},{"password", password}};
    QJsonDocument jsDoc(jsObj);
    QByteArray js_byte_array = jsDoc.toJson();
    std::string body_data = js_byte_array.toStdString();
    int code = send(POST, (host_ + "/experience/api/v1/client/login").toStdString(), body_data, 4);
    if(code != CURLE_OK)
    {
        return curl_easy_strerror(CURLcode(code));
    }

    QJsonParseError jsError;
    jsDoc = QJsonDocument::fromJson(QByteArray::fromStdString(responseData()), &jsError);
    if(jsError.error != QJsonParseError::NoError)
    {
        return jsError.errorString();
    }
    jsObj = jsDoc.object();
    int status = jsObj.value("status").toInt();
    if(status != 200)
    {
        return jsObj.value("message").toString() + " status " + QString::number(status);
    }
    jsObj = jsObj.value("data").toObject();
    *secureKey = jsObj.value("secureKey").toString();
    return QString();
}

QString DLL::CloudHttpDao::addCamera(const RestServiceI::AddCameraParams &params, int *id)
{
    QJsonObject jsObj{{"tableName", params.tableName},
                      {"nickname", params.cameraName},
                      {"rtsp", params.rtsp},
                      {"groupId", params.groupId}};
    if(!params.groupName.isEmpty())
    {
        jsObj.insert("groupName", params.groupName);
    }
    QJsonDocument jsDoc(jsObj);
    QByteArray js_byte_array = jsDoc.toJson();
    std::string body_data = js_byte_array.toStdString();
    int code = send(POST, (host_ + "/app/api/v1/dp/insertInfo").toStdString(), body_data, 4);
    if(code != CURLE_OK)
    {
        return curl_easy_strerror(CURLcode(code));
    }

    QJsonParseError jsError;
    jsDoc = QJsonDocument::fromJson(QByteArray::fromStdString(responseData()), &jsError);
    if(jsError.error != QJsonParseError::NoError)
    {
        return jsError.errorString();
    }
    jsObj = jsDoc.object();
    int status = jsObj.value("status").toInt();
    if(status != 200)
    {
        return  jsObj.value("message").toString() + " status " + QString::number(status);
    }
    jsObj = jsObj.value("data").toObject();
    *id = jsObj.value("id").toInt();
    return QString();
}

int DLL::CloudHttpDao::progress(double totalDownLoad, double downloaded, double totalUpload, double uploaded)
{
    return 0;
}
