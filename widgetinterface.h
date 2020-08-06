#ifndef WIDGETINTERFACE_H
#define WIDGETINTERFACE_H

#include <QWidget>
#include <QDynamicPropertyChangeEvent>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QDebug>
#include "common_data.h"
#include "windowtitlebar.h"

class WidgetI : public QWidget
{
    Q_OBJECT
public:
    WidgetI(QWidget *parent = nullptr):QWidget(parent){
        titleBar_ = new WindowTitleBar(this);
        titleBar_->hide();
    }
    void setLayout(QLayout *lay){
        QVBoxLayout *mainLay = new QVBoxLayout;
        mainLay->addWidget(titleBar_);
        mainLay->addLayout(lay);
        mainLay->setMargin(0);
        mainLay->setSpacing(0);
        QWidget::setLayout(mainLay);
    }
    virtual void setUserStyle(int s = 0) = 0;
    virtual int userStyle() const{return curStyle_;}

protected:
    bool event(QEvent *event) override{
        if(event->type() == QEvent::DynamicPropertyChange){
            QDynamicPropertyChangeEvent *ev = dynamic_cast<QDynamicPropertyChangeEvent*>(event);
            curStyle_ = ev->propertyName().toInt();
            setUserStyle(curStyle_);
            ev->accept();
        }else if(event->type() == QEvent::Show){
            if(!isWindow() || !parentWidget())return QWidget::event(event);
            resize(width(),height() + titleBar_->height());
            titleBar_->setVisible(true);

            QRect rt = rect();
            rt.moveCenter(parentWidget()->window()->rect().center());
            move(rt.topLeft());
        }
        return QWidget::event(event);
    }

    int curStyle_ = 0;
    WindowTitleBar *titleBar_{nullptr};
    QLayout *centerLay_{nullptr};
};
#endif // WIDGETINTERFACE_H
