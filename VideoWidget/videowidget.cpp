#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QDebug>
#include "renderthread.h"
#include "videowidget.h"

VideoWidget::VideoWidget(QWidget *parent)
{
    isFrameSwapped_.store(false);
    connect(this, &QOpenGLWidget::aboutToCompose, this, &VideoWidget::slotAboutToCompose);
    connect(this, &QOpenGLWidget::frameSwapped, this, &VideoWidget::slotFrameSwapped);
    connect(this, &QOpenGLWidget::aboutToResize, this, &VideoWidget::slotAboutToResize);
    connect(this, &QOpenGLWidget::resized, this, &VideoWidget::slotResized);

    m_thread = new RenderThread(this);
    connect(m_thread, &RenderThread::sigContextWanted, this, &VideoWidget::slotGrabContext);

    connect(m_thread, &RenderThread::sigError, this, &VideoWidget::sigError);
    connect(m_thread, &RenderThread::sigVideoStarted, this, [&](int w, int h){m_state_ = Play; emit sigVideoStarted(w, h);});
    connect(m_thread, &RenderThread::finished, this, &VideoWidget::slotFinished);
    connect(m_thread, &RenderThread::sigFps, this, &VideoWidget::sigFps);
    connect(m_thread, &RenderThread::sigCurFpsChanged, this, &VideoWidget::sigCurFpsChanged);
}

VideoWidget::~VideoWidget()
{
    if(m_thread->isRunning())
    {
        disconnect(m_thread, &RenderThread::finished, this, &VideoWidget::slotFinished);
        stopRender();
    }
    delete m_thread;
}

bool VideoWidget::isFrameSwapped() const
{
    return isFrameSwapped_.load();
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
    if(m_thread->isRunning()){
        disconnect(m_thread, &RenderThread::finished, this, &VideoWidget::slotFinished);
        stopRender();
        connect(m_thread, &RenderThread::finished, this, &VideoWidget::slotFinished);
    }
    m_state_ = Ready;
    source_file_ = filename;
    device_name_ = device;
    m_thread->setDevice(device);
    m_thread->setFileName(filename);
    m_thread->start();
}

void VideoWidget::stopRender()
{
    m_thread->requestInterruption();
    m_thread->prepareExit();
    m_thread->quit();
    m_thread->wait();
}

void VideoWidget::resizeGL(int w, int h)
{
    m_thread->lockRenderer();
    context()->functions()->glViewport(0, 0, w, h);
    m_thread->unlockRenderer();
}

void VideoWidget::slotGrabContext()
{
    m_thread->lockRenderer();
    QMutexLocker lock(m_thread->grabMutex());
    context()->moveToThread(m_thread);
    m_thread->grabCond()->wakeAll();
    m_thread->unlockRenderer();
}

void VideoWidget::slotAboutToCompose()
{
    m_thread->lockRenderer();
}

void VideoWidget::slotFrameSwapped()
{
    m_thread->unlockRenderer();
    isFrameSwapped_.store(true);
}

void VideoWidget::slotAboutToResize()
{
    m_thread->lockRenderer();
    isFrameSwapped_.store(false);
}

void VideoWidget::slotResized()
{
    m_thread->unlockRenderer();
}

void VideoWidget::slotFinished()
{
    m_state_ = Stopped;
    m_thread->lockRenderer();
    makeCurrent();
    m_thread->release();
    context()->functions()->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    context()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    doneCurrent();
    m_thread->unlockRenderer();
    update();
    emit sigVideoStopped();
}
