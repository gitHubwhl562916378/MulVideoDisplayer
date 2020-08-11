#include <QTimer>
#include <QDebug>
#include "errorlabel.h"
#include "player.h"

Player::Player(QWidget *parent):
    VideoWidget (parent)
{
    errorL_ = new ErrorLabel(this);
    errorL_->hide();
    timer_ = new QTimer(this);
    timer_->setInterval(5000);
    connect(timer_, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    connect(this, SIGNAL(sigError(QString)), this, SLOT(slotError(QString)));
    connect(this, &VideoWidget::sigVideoStarted, this, [&](int w, int h){errorL_->hide();});
    connect(this, &VideoWidget::sigVideoStopped, this, &Player::slotStoped);
}

void Player::startPlay(const QString &url, const QString &device)
{
    timer_->stop();
    errorL_->hide();
    VideoWidget::slotPlay(url, device);
}

void Player::slotError(QString str)
{
    errorL_->setText(str);
    errorL_->show();
}

void Player::slotStoped()
{
    timer_->start();
}

void Player::slotTimeout()
{
    errorL_->clear();
    errorL_->hide();
    slotPlay(url(), deviceName());
}
