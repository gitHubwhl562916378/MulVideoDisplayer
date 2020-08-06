#include <QFutureWatcher>
#include <QtConcurrent>
#include <QDebug>
#include "restserviceconcurrent.h"

RestServiceConcurrent::RestServiceConcurrent(QObject *parent)
{
    http_dao_ = new DLL::CloudHttpDao;
}

RestServiceConcurrent::~RestServiceConcurrent()
{
    delete http_dao_;
}

void RestServiceConcurrent::login(const QString &user, const QString &password)
{
    QString *secretKey = new QString;
    QFutureWatcher<QString> *fwatcher = new QFutureWatcher<QString>(this);
    connect(fwatcher, &QFutureWatcher<QString>::finished, this, [=]{
        if(fwatcher->result().isEmpty())
        {
            emit sigLoginSuccessed(*secretKey);
        }else{
            emit sigError(fwatcher->result());
        }
        delete secretKey;
    });
    connect(fwatcher, SIGNAL(finished()), this, SLOT(deleteLater()));
    fwatcher->setFuture(QtConcurrent::run(http_dao_, &DLL::CloudHttpDao::login, user, password, secretKey));
}

void RestServiceConcurrent::addCamera(const AddCameraParams &params)
{
    int *id = new int;
    QFutureWatcher<QString> *fwatcher = new QFutureWatcher<QString>(this);
    connect(fwatcher, &QFutureWatcher<QString>::finished, this, [=]{
        if(fwatcher->result().isEmpty())
        {
            emit sigAddCamera(*id);
        }else{
            emit sigError(fwatcher->result());
        }
        delete id;
    });
    connect(fwatcher, SIGNAL(finished()), this, SLOT(deleteLater()));
    fwatcher->setFuture(QtConcurrent::run(http_dao_, &DLL::CloudHttpDao::addCamera, params, id));
}
