extern "C"
{
#include "libavformat/avformat.h"
}
#include <QOpenGLContext>
#include <QLibrary>
#include <QGuiApplication>
#include "videowidget.h"
#include "render/nv12render.h"
#include "render/yuvrender.h"
#include "videodecode/decodtask.h"
#include "renderthread.h"

std::atomic_bool RenderThread::isInited_;
RenderThread::RenderThread(VideoWidget *glw, QObject *parent):
    QThread (parent),
    gl_widget_(glw)
{
    if(!isInited_.load()){
        renderFactoryInstance()->Register(AV_PIX_FMT_YUV420P, []()->VideoRender*{return new YuvRender;});
        renderFactoryInstance()->Register(AV_PIX_FMT_NV12, []()->VideoRender*{return new Nv12Render;});
        CreateRenderFunc func = nullptr;
        QLibrary dllLoad("Nv12Render_Gpu"); //Nv2RGBRender_Gpu Nv12Render_Gpu
        if(dllLoad.load()){
            func = (CreateRenderFunc)dllLoad.resolve("createRender");
            renderFactoryInstance()->Register(AV_PIX_FMT_CUDA, [=]()->VideoRender*{return func(exte_data_);});
        }

        isInited_.store(true);
    }
}

RenderThread::~RenderThread()
{

}

VideoRender *RenderThread::getRender(int pix)
{
    try {
        return renderFactoryInstance()->CreateObject(pix);
    } catch (const std::exception &e) {
        emit sigError(e.what());
    }
    return nullptr;
}

void RenderThread::Render(const std::function<void (void)> handle)
{
    if(isInterruptionRequested()){
        return;
    }

    QOpenGLContext *ctx = gl_widget_->context();
    if (!ctx){ // QOpenGLWidget not yet initialized
        emit sigError("QOpenGLWidget not yet initialized");
        return;
    }

    // Grab the context.
    m_grabMutex.lock();
    if(!gl_widget_->isFrameSwapped()) //保证程序启动时,RenderThread先拿到锁后，能终止此次渲染。因为这时界面还没显示的话, FrameSwapped还没切换过来
    {
        m_grabMutex.unlock();
        return;
    }
    emit sigContextWanted();
    m_grabCond.wait(&m_grabMutex);
    QMutexLocker lock(&m_renderMutex);
    m_grabMutex.unlock();

    if(isInterruptionRequested()){
        if(ctx->thread() == QThread::currentThread()){
            ctx->moveToThread(qGuiApp->thread());
        }
        return;
    }

    Q_ASSERT(ctx->thread() == QThread::currentThread());
    // Make the context (and an offscreen surface) current for this thread. The
    // QOpenGLWidget's fbo is bound in the context.
    gl_widget_->makeCurrent();

    if(handle)
    {
        handle();
    }

    // Make no context current on this thread and move the QOpenGLWidget's
    // context back to the gui thread.
    gl_widget_->doneCurrent();
    ctx->moveToThread(qGuiApp->thread());

    // Schedule composition. Note that this will use QueuedConnection, meaning
    // that update() will be invoked on the gui thread.
    QMetaObject::invokeMethod(gl_widget_, "update");
}

void RenderThread::release()
{
    if(m_task_){
        m_task_->destroy();
    }
}

void RenderThread::setFileName(QString f)
{
    file_name_ = f;
}

void RenderThread::setDevice(QString d)
{
    device_ = d;
}

void RenderThread::slotMoveContext()
{
    QOpenGLContext *ctx = gl_widget_->context();
    qDebug() << "RenderThread::slotMoveContext" << ctx->thread() << QThread::currentThread() << qGuiApp->thread();
    if(ctx && ctx->thread() == QThread::currentThread()){
        ctx->moveToThread(qGuiApp->thread());
    }
}

void RenderThread::run()
{
    m_task_ = std::shared_ptr<DecodeTask>(DecodeTaskManager::Instance()->makeTask(this, device_));
    m_task_->decode(file_name_);
}

Factory<VideoRender, int> *RenderThread::renderFactoryInstance()
{
    static Factory<VideoRender, int> render_factory;
    return &render_factory;
}
