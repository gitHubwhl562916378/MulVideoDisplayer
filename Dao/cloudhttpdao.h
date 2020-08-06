#ifndef CLOUDHTTPDAO_H
#define CLOUDHTTPDAO_H
#include "core/httpexector.h"
#include "Service/servicei.h"

namespace DLL{
class CloudHttpDao : public HttpExector
{
public:
    CloudHttpDao();
    QString login(const QString &user, const QString &password, QString *secureKey);
    QString addCamera(const RestServiceI::AddCameraParams &params, int *id);

protected:
    int progress(double totalDownLoad,double downloaded,double totalUpload, double uploaded) override;

private:
    QString host_;
};
}

#endif // CLOUDHTTPDAO_H
