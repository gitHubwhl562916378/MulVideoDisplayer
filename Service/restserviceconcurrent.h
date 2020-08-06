#ifndef RESTSERVICECONCURRENT_H
#define RESTSERVICECONCURRENT_H

#include "servicei.h"
#include "Dao/cloudhttpdao.h"
class RestServiceConcurrent : public RestServiceI
{
    Q_OBJECT
public:
    RestServiceConcurrent(QObject *parent = nullptr);
    ~RestServiceConcurrent() override;
    void login(const QString &user, const QString &password) override;
    void addCamera(const AddCameraParams &) override;

private:
    DLL::CloudHttpDao *http_dao_;
};

#endif // RESTSERVICECONCURRENT_H
