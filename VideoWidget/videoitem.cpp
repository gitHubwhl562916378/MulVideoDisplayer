#include <QOpenGLFramebufferObjectFormat>
#include <QOpenGLContext>
#include <QOffscreenSurface>
#include <QQuickWindow>
#include <QGuiApplication>
#include "videoitem.h"
#include "renderthread.h"

class VideoFboRenderer : public QQuickFramebufferObject::Renderer
{
public:
    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override
    {
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        format.setSamples(4);
        return new QOpenGLFramebufferObject(size, format);
    }

    void render() override
    {
        QMutexLocker lock(m_obj_->render_thr_->renderLocker());
         if(!m_obj_->render_thr_->currentRender()){
             return;
         }
        m_obj_->render_thr_->currentRender()->draw();
    }

    void synchronize(QQuickFramebufferObject *item) override
    {
        if(!m_obj_)
        {
            m_obj_ = qobject_cast<VideoItem*>(item);
        }
        if(!m_obj_->render_thr_)
        {
            m_obj_->render_thr_ = new RenderThread;

            auto context = QOpenGLContext::currentContext();
            context->doneCurrent();
            m_obj_->render_thr_->setContext(context);
            context->makeCurrent(m_obj_->window());

            QMetaObject::invokeMethod(item, "slotInitisze");
        }
    }

private:
    VideoItem *m_obj_ = nullptr;
};

VideoItem::~VideoItem()
{
    if(is_initized_)
    {
        slotStop();
        delete render_thr_;
    }
}

QQuickFramebufferObject::Renderer *VideoItem::createRenderer() const
{
    return new VideoFboRenderer;
}

QString VideoItem::deviceName()
{
    return device_name_;
}

QString VideoItem::url()
{
    return source_file_;
}

void VideoItem::slotInitisze()
{
    auto render_surface = new QOffscreenSurface(nullptr, this);
    render_surface->setFormat(render_thr_->context()->format());
    render_surface->create();
    render_thr_->setSurface(render_surface);

    connect(render_thr_, SIGNAL(sigTextureReady()), this, SLOT(update()));
    connect(render_thr_, SIGNAL(sigError(QString)), this, SIGNAL(sigError(QString)));
    connect(render_thr_, &RenderThread::sigVideoStarted, this, [&](int w, int h){
        m_state_ = Play;
        emit sigVideoStarted(w, h);
    });
    connect(render_thr_, SIGNAL(sigFps(int)), this, SIGNAL(sigFps(int)));
    connect(render_thr_, SIGNAL(sigCurFpsChanged(int)), this, SIGNAL(sigCurFpsChanged(int)));
    connect(render_thr_, SIGNAL(finished()), this, SLOT(slotFinished()), Qt::UniqueConnection);
    is_initized_ = true;
    emit sigInitized();
}

void VideoItem::slotStop()
{
    if(!is_initized_)return;
    render_thr_->requestInterruption();
    render_thr_->quit();
    render_thr_->wait();
}

void VideoItem::slotFinished()
{
    m_state_ = Stopped;
    emit sigVideoStopped();
}

void VideoItem::slotPlay(QString filename, QString device)
{
    if(is_initized_)
    {
        slotStop();
        m_state_ = Ready;
        source_file_ = filename;
        device_name_ = device;
        render_thr_->setDevice(device);
        render_thr_->setFileName(filename);
        render_thr_->start();
    }else{
        connect(this, &VideoItem::sigInitized, this, [=]{
            slotStop();
            m_state_ = Ready;
            source_file_ = filename;
            device_name_ = device;
            render_thr_->setDevice(device);
            render_thr_->setFileName(filename);
            render_thr_->start();
        });
    }
}
