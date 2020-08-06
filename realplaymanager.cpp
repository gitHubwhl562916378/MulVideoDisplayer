#include <QStack>
#include <QGridLayout>
#include <QMouseEvent>
#include <QFocusFrame>
#include <QDebug>
#include "realplaymanager.h"
#include "Service/servicefacetory.h"
#include "player.h"
#include "VideoWidget/videowidget.h"

RealPlayManager::RealPlayManager(QWidget *parent):
    QWidget(parent)
{
    m_mainLay = new QGridLayout;
    m_mainLay->setMargin(1);
    laySpacing_ = 1;
    m_mainLay->setSpacing(laySpacing_);
    setLayout(m_mainLay);
    m_focusFrame = new QFocusFrame(this);
    setFocusStyle(-1);
}

RealPlayManager::~RealPlayManager()
{
}

void RealPlayManager::setFocusStyle(int s)
{
    QPalette pal;
    switch (s) {
    case -1:
        pal = m_focusFrame->palette();
        pal.setColor(QPalette::Foreground,Qt::transparent);
        m_focusFrame->setPalette(pal);
        m_focusFrame->show();
        break;
    case 0:
        pal = m_focusFrame->palette();
        pal.setColor(QPalette::Foreground,QColor(189,56,57));
        m_focusFrame->setPalette(pal);
        m_focusFrame->show();
        break;
    case 1:
        pal = m_focusFrame->palette();
        pal.setColor(QPalette::Foreground,Qt::yellow);
        m_focusFrame->setPalette(pal);
        m_focusFrame->show();
        break;
    default:
        break;
    }
}

int RealPlayManager::screenCount() const
{
//    return curRow * curCol - curBigRowSpan * curBigColSpan + 1 ;
    return  m_mainLay->count();
}

void RealPlayManager::splitScreen(const int rows, const int cols, const int bigFrameRow, const int bigFrameCol, const int bigFrameRowSpan, const int bigFrameColSpan)
{
    if(rows == curRow && cols == curCol && bigFrameRow == curBigRow && bigFrameCol == curBigCol && bigFrameRowSpan == curBigRowSpan && bigFrameColSpan == curBigColSpan){
        return;
    }
    curRow = rows;
    curCol = cols;
    curBigRow = bigFrameRow;
    curBigCol = bigFrameCol;
    curBigRowSpan = bigFrameRowSpan;
    curBigColSpan = bigFrameColSpan;
    QStack<QWidget*> realFrameLayStack;
    int oldCount = m_mainLay->count();
    for(int i = oldCount - 1; i >= 0; --i){
        realFrameLayStack.push(m_mainLay->takeAt(i)->widget());
    }

    int loopCount = bigFrameRow * cols;                                           //bigFrameRow 启示位置0， cols 总列， bigFrameCol 0，rows 总行，bigFrameRowSpan 大窗占有行

    int _row = -1, _col = -1, _rowSpan = -1 , _colSpan = -1;
    for(int i = 0 ; i < loopCount ; i ++){
        _row = i / bigFrameCol; _col = i % bigFrameCol; _rowSpan = 1; _colSpan = 1;
        if(!realFrameLayStack.empty()){
            m_mainLay->addWidget(realFrameLayStack.pop(), _row, _col,_rowSpan,_colSpan);
        }else{
            m_mainLay->addWidget(new Player,_row, _col,_rowSpan,_colSpan);
        }
    }

    loopCount = bigFrameCol * bigFrameRowSpan;
    for(int i = 0; i < loopCount; i ++){
        _row = bigFrameRow + i / bigFrameCol; _col = i % bigFrameCol; _rowSpan = 1; _colSpan = 1;
        if(!realFrameLayStack.empty()){
            m_mainLay->addWidget(realFrameLayStack.pop(), _row, _col,_rowSpan,_colSpan);
        }else{
            m_mainLay->addWidget(new Player, _row, _col,_rowSpan,_colSpan);
        }
    }

    _row = bigFrameRow; _col = bigFrameCol; _rowSpan = bigFrameRowSpan; _colSpan = bigFrameColSpan;
    if(!realFrameLayStack.empty()){
        m_mainLay->addWidget(realFrameLayStack.pop(), _row, _col,_rowSpan,_colSpan);
    }else{
        m_mainLay->addWidget(new Player,_row, _col,_rowSpan,_colSpan);
    }

    int colStart = bigFrameCol + bigFrameColSpan;
    int colCount = cols - colStart;
    loopCount = colCount * bigFrameRowSpan;
    for(int i = 0; i < loopCount; i++){
        _row = bigFrameRow + i / colCount; _col = colStart + i % colCount; _rowSpan = 1; _colSpan = 1;
        if(!realFrameLayStack.empty()){
            m_mainLay->addWidget(realFrameLayStack.pop(), _row, _col,_rowSpan,_colSpan);
        }else{
            m_mainLay->addWidget(new Player,_row, _col,_rowSpan,_colSpan);
        }
    }

    int rowStart = bigFrameRow + bigFrameRowSpan;
    int rowCount = rows - rowStart;
    loopCount = rowCount * cols;
    for(int i = 0; i < loopCount; i++){
        _row = rowStart + i / cols; _col = i % cols; _rowSpan = 1; _colSpan = 1;
        if(!realFrameLayStack.empty()){
            m_mainLay->addWidget(realFrameLayStack.pop(), _row, _col,_rowSpan,_colSpan);
        }else{
            m_mainLay->addWidget(new Player,_row, _col,_rowSpan,_colSpan);
        }
    }

    if(!realFrameLayStack.isEmpty()){
        m_focusFrame->setWidget(m_mainLay->itemAt(0)->widget());
        while (!realFrameLayStack.empty()) {
              QWidget *item = realFrameLayStack.pop();

              delete item;
        }
    }


    if(m_isFullScreen){
        closeVideoFullScreen();
    }

    if(!isInitsized_){
        m_focusFrame->setWidget(m_mainLay->itemAt(0)->widget());
        isInitsized_ = true;
    }
}

Player *RealPlayManager::indexWidget(int pos)
{
    int count = screenCount();
    if(pos < 0 || pos >= count)
    {
        return nullptr;
    }

    return dynamic_cast<Player*>(m_mainLay->itemAt(pos)->widget());
}

Player *RealPlayManager::focusWidget()
{
    return dynamic_cast<Player*>(m_focusFrame->widget());
}

void RealPlayManager::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QWidget *videoW = childAt(ev->pos());
    if(!videoW)return;
    if(!m_isFullScreen){
        videoShowFullScreen(videoW);
    }else{
        closeVideoFullScreen();
    }

}

void RealPlayManager::mousePressEvent(QMouseEvent *ev)
{
    QWidget *videoW = childAt(ev->pos());
    if(!videoW)return;
    m_focusFrame->setWidget(videoW);
}

void RealPlayManager::videoShowFullScreen(QWidget *vw)
{
    if(m_isFullScreen)return;
    for(int i = 0; i < m_mainLay->count(); i++){
        QWidget *w = m_mainLay->itemAt(i)->widget();
        w->setMaximumSize(0,0);
    }
    m_mainLay->setSpacing(0);
    vw->setMaximumSize(width() - layout()->margin() * 2, height() - layout()->margin() * 2);
    vw->setMinimumSize(width() - layout()->margin() * 2, height() - layout()->margin() * 2);
    updateGeometry();
    m_isFullScreen = true;
}

void RealPlayManager::closeVideoFullScreen()
{
    if(!m_isFullScreen)return;
    for(int i = 0; i < m_mainLay->count(); i++){
        QWidget *w = m_mainLay->itemAt(i)->widget();
        w->setMinimumSize(0,0);
        w->setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    }
    m_mainLay->setSpacing(laySpacing_);
    updateGeometry();
    m_isFullScreen = false;
}
