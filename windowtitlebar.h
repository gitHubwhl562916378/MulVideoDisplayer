#ifndef WINDOWTITLEBAR_H
#define WINDOWTITLEBAR_H

#include <QWidget>
QT_FORWARD_DECLARE_CLASS(QPushButton)
class WindowTitleBar : public QWidget
{
    Q_OBJECT
public:
    WindowTitleBar(QWidget *parent = nullptr);
    void setUserStyle(int s);

private:
    QPushButton *closeBtn_{nullptr};

private slots:
    void slotCloseBtnClicked();
};

#endif // WINDOWTITLEBAR_H
