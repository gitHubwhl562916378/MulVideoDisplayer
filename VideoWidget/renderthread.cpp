extern "C"
{
#include "libavformat/avformat.h"
}
#include <QOpenGLContext>
#include <QMutexLocker>
#include <QLibrary>
#include <QDateTime>
#include "render/nv12render.h"
#include "render/yuvrender.h"
#include "videodecode/decodtask.h"
#include "renderthread.h"

std::atomic_bool RenderThread::isInited_;
RenderThread::RenderThread(QSurface *surface, QOpenGLContext *ctx, QObject *parent):
    QThread (parent),
    surface_(surface)
{
    context_ = new QOpenGLContext();
    context_->setFormat(ctx->format());
    context_->setShareContext(ctx);
    context_->create();
    context_->moveToThread(this);

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
    context_->doneCurrent();
    context_->deleteLater();
}

VideoRender *RenderThread::getRender(int pix)
{
    try {
        QMutexLocker lock(renderLocker());
        render_ = renderFactoryInstance()->CreateObject(pix);
        return render_;
    } catch (const std::exception &e) {
        emit sigError(e.what());
    }
    return nullptr;
}

void RenderThread::Render(const std::function<void (void)> handle)
{
    context_->makeCurrent(surface_);
    if(handle)
    {
        handle();
        emit sigTextureReady();
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

void RenderThread::run()
{
    auto m_task = std::shared_ptr<DecodeTask>(DecodeTaskManager::Instance()->makeTask(this, device_));
    m_task->decode(file_name_);

    context_->makeCurrent(surface_);
    QMutexLocker lock(renderLocker());
    delete  render_;
    render_ = nullptr;
}

Factory<VideoRender, int> *RenderThread::renderFactoryInstance()
{
    static Factory<VideoRender, int> render_factory;
    return &render_factory;
}
