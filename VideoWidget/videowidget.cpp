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
    slotStop();
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
    if(is_initized_)
    {
        slotStop();
        m_state_ = Ready;
        source_file_ = filename;
        device_name_ = device;
        m_thread->setDevice(device);
        m_thread->setFileName(filename);
        m_thread->start();
    }else{
        connect(this, &VideoWidget::sigInitized, this, [=]{
            slotStop();
            m_state_ = Ready;
            source_file_ = filename;
            device_name_ = device;
            m_thread->setDevice(device);
            m_thread->setFileName(filename);
            m_thread->start();
        });
    }
}

void VideoWidget::slotStop()
{
    m_thread->requestInterruption();
    m_thread->quit();
    m_thread->wait();
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
    is_initized_ = true;
    emit sigInitized();
}

void VideoWidget::resizeGL(int w, int h)
{
    context()->functions()->glViewport(0, 0, w, h);
}

void VideoWidget::paintGL()
{
    QMutexLocker lock(m_thread->renderLocker());
    if(!m_thread->currentRender()){
        QOpenGLContext::currentContext()->functions()->glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        QOpenGLContext::currentContext()->functions()->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        return;
    }

    m_thread->currentRender()->draw();
}

void VideoWidget::slotFinished()
{
    m_state_ = Stopped;
    update();
    emit sigVideoStopped();
}
