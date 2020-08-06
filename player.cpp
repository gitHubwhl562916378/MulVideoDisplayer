#include <QTimer>
#include <QDebug>
#include "errorlabel.h"
#include "player.h"

Player::Player(QWidget *parent):
    VideoWidget (parent)
{
    errorL_ = new ErrorLabel(this);
    errorL_->hide();
    connect(this, SIGNAL(sigError(QString)), this, SLOT(slotError(QString)));
    connect(this, &VideoWidget::sigVideoStarted, this, [&](int w, int h){errorL_->hide();});
}

void Player::startPlay(const QString &url, const QString &device)
{
    errorL_->hide();
    VideoWidget::slotPlay(url, device);
}

void Player::slotError(QString str)
{
    errorL_->setText(str);
    errorL_->show();
    QTimer::singleShot(5000, this, [&]{
        slotPlay(url(), deviceName());
    });
}

void Player::slotStoped()
{

}
