#ifndef SERVICEI_H
#define SERVICEI_H

#include <QObject>
#include <QThread>

QT_FORWARD_DECLARE_CLASS(QTreeWidgetItem)
class RestServiceI;
class VideoEncodeI;
class NotifyServiceI;
class ServiceFactoryI
{
public:
    enum RestInterfaceType{
        Concurrent
    };
    enum NotifyInterfaceType{
        Mqtt,
        Kafka,
        WebSocket
    };
    enum VideoEncodeType{
        FFmpeg
    };

    virtual ~ServiceFactoryI(){}
    virtual NotifyServiceI* makeNotifyServiceI(NotifyInterfaceType s = Mqtt) = 0;
    virtual RestServiceI* makeRestServiceI(RestInterfaceType s = Concurrent) = 0;
    virtual VideoEncodeI* makeVideoEncodeI(VideoEncodeType t = FFmpeg) = 0;
};

class RestServiceI : public QObject
{
    Q_OBJECT
public:
    struct AddCameraParams
    {
        int groupId;
        QString tableName;
        QString cameraName;
        QString rtsp;
        QString groupName;
    };
    RestServiceI(QObject *parent = nullptr):QObject(parent){
    }

    virtual void login(const QString &user, const QString &password) = 0;
    virtual void addCamera(const AddCameraParams &) = 0;

signals:
    void sigError(QString);
    void sigLoginSuccessed(QString);
    void sigAddCamera(int);
};

class VideoEncodeI : public QThread
{
    Q_OBJECT

public:
    struct EncodeParams
    {
         char input_file[258];
         char out_file[258];
         int fps;
         int code_id;
         int media_type;
         int pix_fmt;
         int width;
         int height;
         int gop_size;
         int keyint_min;
         int thread_count;
         int me_range = 16;
         int max_qdiff;
         float qcompress;
         int max_b_frames;
         bool b_frame_strategy;
         int qmin;
         int qmax;
         int bit_rate;
    };

    VideoEncodeI(QObject* parent = nullptr): QThread(parent){
    }

    virtual void startEncode(const EncodeParams& params) = 0;
    virtual void stopEncode() = 0;

signals:
    void sigError(QString);
    void sigStoped();
    void sigStarted();
};

#endif // SERVICEI_H
