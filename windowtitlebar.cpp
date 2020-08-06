#include <QPushButton>
#include <QHBoxLayout>
#include <QEvent>
#include <QDebug>
#include "windowtitlebar.h"

WindowTitleBar::WindowTitleBar(QWidget *parent):
    QWidget(parent)
{
    closeBtn_ = new QPushButton(tr("×"));
    QHBoxLayout *mainLay = new QHBoxLayout;
    mainLay->addWidget(closeBtn_);
    mainLay->setAlignment(Qt::AlignRight);
    mainLay->setContentsMargins(0,5,5,5);
    setLayout(mainLay);

    connect(closeBtn_,SIGNAL(clicked(bool)),this,SLOT(slotCloseBtnClicked()));
    setUserStyle(0);
}

void WindowTitleBar::setUserStyle(int s)
{
    QFont f;
    if( s == 0){
        closeBtn_->setStyleSheet("QPushButton{"
                                 "color: rgba(255,255,255,191);"
                                 "background-color: rgba(255,255,255,0.1);"
                                 "}");
    }
}

void WindowTitleBar::slotCloseBtnClicked()
{
    if(parentWidget()){
        parentWidget()->close();
    }
}
