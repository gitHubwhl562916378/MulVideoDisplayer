#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "widgetinterface.h"
QT_FORWARD_DECLARE_CLASS(RealPlayManager)
QT_FORWARD_DECLARE_CLASS(QMenu)
QT_FORWARD_DECLARE_CLASS(QTimer)
class MainWindow : public WidgetI
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setUserStyle(int s = 0) override;

protected:
    bool event(QEvent *event) override;

private slots:
    void slotVideoTimeout();

private:
    RealPlayManager *playManager_;
    QMenu *menu_;
    QList<QPair<QString, QString>> urlList_;
    bool isFirst_ = true;
    int playIndex_ = 0;
    int screenIndex_ = 0;
    QTimer *playTimer_;
};

#endif // MAINWINDOW_H
