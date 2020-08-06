#include <QEvent>
#include <QDebug>
#include "errorlabel.h"

ErrorLabel::ErrorLabel(QWidget *parent):
    QLabel (parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Foreground, Qt::red);
    setPalette(pal);

    setAlignment(Qt::AlignCenter);
    if(parent){
        parent->installEventFilter(this);
    }
}

bool ErrorLabel::event(QEvent *event)
{
    if(!parentWidget())return QWidget::event(event);
    if(event->type() == QEvent::Show){
        QRect r = rect();
        r.moveCenter(parentWidget()->rect().center());
        move(r.topLeft());
    }
    return QLabel::event(event);
}

bool ErrorLabel::eventFilter(QObject *watched, QEvent *event)
{
    if(!parentWidget()){
        return QLabel::eventFilter(watched,event);
    }

    QWidget *pw = qobject_cast<QWidget*>(watched);
    if(pw == parentWidget() && event->type() == QEvent::Resize){
        QRect r = rect();
        r.moveCenter(parentWidget()->rect().center());
        move(r.topLeft());
    }

    return QLabel::eventFilter(watched,event);
}
