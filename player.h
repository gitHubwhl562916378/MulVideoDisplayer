#ifndef PLAYER_H
#define PLAYER_H

#include "VideoWidget/videowidget.h"
QT_FORWARD_DECLARE_CLASS(ErrorLabel)
class Player : public VideoWidget
{
    Q_OBJECT
public:
    Player(QWidget* parent = nullptr);
    void startPlay(const QString &url, const QString &device);

private slots:
    void slotError(QString);
    void slotStoped();

private:
    ErrorLabel *errorL_;
};

#endif // PLAYER_H
