#ifndef ERRORLABEL_H
#define ERRORLABEL_H

#include <QLabel>

class ErrorLabel : public QLabel
{
    Q_OBJECT
public:
    ErrorLabel(QWidget *parent = nullptr);

protected:
    bool event(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
};

#endif // ERRORLABEL_H
