#ifndef VIDEOITEM_H
#define VIDEOITEM_H

#include <QQuickFramebufferObject>
QT_FORWARD_DECLARE_CLASS(RenderThread)
class VideoItem : public QQuickFramebufferObject
{
    Q_OBJECT
public:
    enum PlayState{
        Ready,
        Play,
        Stopped
    };
    ~VideoItem() override;
    QQuickFramebufferObject::Renderer *createRenderer() const override;

    RenderThread *render_thr_{nullptr};

signals:
    void sigError(QString msg);
    void sigVideoStarted(int width, int height);
    void sigFps(int fps);
    void sigCurFpsChanged(int fps);
    void sigVideoStopped();
    void sigInitized();

public slots:
    QString deviceName();
    QString url();
    void slotInitisze();
    void slotStop();
    void slotPlay(QString filename, QString device);

private slots:
    void slotFinished();

private:
    PlayState m_state_;
    bool is_initized_ = false;
    QString source_file_, device_name_;
};

#endif // VIDEOITEM_H
