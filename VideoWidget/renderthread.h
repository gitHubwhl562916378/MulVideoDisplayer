#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "render/factory.h"
#include "render/videorender.h"
QT_FORWARD_DECLARE_CLASS(QOpenGLContext)
QT_FORWARD_DECLARE_CLASS(DecodeTask)
QT_FORWARD_DECLARE_CLASS(QSurface)
class RenderThread : public QThread
{
    Q_OBJECT
public:
    RenderThread(QSurface *surface, QOpenGLContext *ctx = nullptr, QObject *parent = nullptr);
    ~RenderThread() override;
    VideoRender* currentRender() {return  render_;}

    virtual VideoRender* getRender(int);
    virtual void Render(const std::function<void (void)>);
    virtual void setExtraData(void *ctx){exte_data_ = ctx;}

    void setFileName(QString);
    void setDevice(QString);

signals:
    void sigError(QString);
    void sigVideoStarted(int, int);
    void sigFps(int);
    void sigCurFpsChanged(int);

    void sigTextureReady();

protected:
    void run() override;

private:
    static Factory<VideoRender, int>* renderFactoryInstance();
    static std::atomic_bool isInited_;

    QOpenGLContext *context_;
    QSurface *surface_;
    VideoRender *render_ = nullptr;

    QString file_name_, device_;
    std::shared_ptr<DecodeTask> m_task_{nullptr};
    void *exte_data_{nullptr};
};

#endif // RENDERTHREAD_H
