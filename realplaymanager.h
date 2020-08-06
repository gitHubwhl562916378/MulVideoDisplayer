#ifndef REALPLAYMANAGER_H
#define REALPLAYMANAGER_H

#include <QWidget>
QT_FORWARD_DECLARE_CLASS(QGridLayout)
QT_FORWARD_DECLARE_CLASS(QFocusFrame)
QT_FORWARD_DECLARE_CLASS(Player)
class RealPlayManager : public QWidget
{
    Q_OBJECT
public:
    explicit RealPlayManager(QWidget*parent = nullptr);
    ~RealPlayManager() override;
    void setFocusStyle(int s = 0);
    int screenCount()const;
    void splitScreen(const int rows, const int cols, const int bigFrameRow, const int bigFrameCol, const int bigFrameRowSpan, const int bigFrameColSpan);
    Player* indexWidget(int pos);
    Player* focusWidget();

protected:
    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;

private:
    QGridLayout *m_mainLay{nullptr};
    bool m_isFullScreen = false,isInitsized_ = false;
    QFocusFrame *m_focusFrame{nullptr};

    int curRow = 0,curCol = 0,curBigRow = 0,curBigCol = 0,curBigRowSpan = 0,curBigColSpan = 0, laySpacing_  = 0;

    void videoShowFullScreen(QWidget *);
    void closeVideoFullScreen();
};

#endif // REALPLAYMANAGER_H
