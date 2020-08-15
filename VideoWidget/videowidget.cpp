#include <QOpenGLContext>
#include <QGuiApplication>
#include <QOpenGLFunctions>
#include <QDebug>
#include <QOffscreenSurface>
#include "renderthread.h"
#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent)
{
}

VideoWidget::~VideoWidget()
{
    if(m_thread->isRunning())
    {
        disconnect(m_thread, &RenderThread::finished, this, &VideoWidget::slotFinished);
        disconnect();
        slotStop();
    }
    delete m_thread;
}

QString VideoWidget::url() const
{
    return source_file_;
}

QString VideoWidget::deviceName() const
{
    return device_name_;
}

VideoWidget::PlayState VideoWidget::state() const
{
    return m_state_;
}

void VideoWidget::slotPlay(QString filename, QString device)
{
    slotStop();
    m_state_ = Ready;
    source_file_ = filename;
    device_name_ = device;
    m_thread->setDevice(device);
    m_thread->setFileName(filename);
    m_thread->start();
}

void VideoWidget::slotStop()
{
    if(m_thread->isRunning()){
        disconnect(m_thread, SIGNAL(finished()), this, SLOT(slotFinished()));
        slotFinished();
        connect(m_thread, SIGNAL(finished()), this, SLOT(slotFinished()), Qt::UniqueConnection);
    }
}

void VideoWidget::initializeGL()
{
    auto context = QOpenGLContext::currentContext();
    auto surface = context->surface();

    auto render_surface = new QOffscreenSurface(nullptr, this);
    render_surface->setFormat(surface->format());
    render_surface->create();

    context->doneCurrent();
    m_thread = new RenderThread(render_surface, context);
    context->makeCurrent(surface);

    connect(m_thread, SIGNAL(sigTextureReady()), this, SLOT(update()));
    connect(m_thread, &RenderThread::sigError, this, &VideoWidget::sigError);
    connect(m_thread, &RenderThread::sigVideoStarted, this, [&](int w, int h){
        m_state_ = Play;
        emit sigVideoStarted(w, h);
    });
    connect(m_thread, &RenderThread::sigFps, this, &VideoWidget::sigFps);
    connect(m_thread, &RenderThread::sigCurFpsChanged, this, &VideoWidget::sigCurFpsChanged);
    connect(m_thread, SIGNAL(finished()), this, SLOT(slotFinished()), Qt::UniqueConnection);
}

void VideoWidget::resizeGL(int w, int h)
{
    context()->functions()->glViewport(0, 0, w, h);
}

void VideoWidget::paintGL()
{
    if(!m_thread->currentRender())
    {
        return;
    }

    m_thread->currentRender()->draw();
}

void VideoWidget::slotFinished()
{
    m_state_ = Stopped;
    m_thread->requestInterruption();
    m_thread->quit();
    m_thread->wait();

    if(sender() == qobject_cast<QObject*>(m_thread)){
        emit sigVideoStopped();
    }
}
