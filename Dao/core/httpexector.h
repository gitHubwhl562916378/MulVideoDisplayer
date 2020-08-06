#ifndef NETHTTPEXECTOR_H
#define NETHTTPEXECTOR_H

#include "basenetexector.h"
#include <vector>
namespace DLL {
class HttpExector : public BaseHttpExector
{
public:
    bool cancelRequestFlag_ = false;

    HttpExector();
    ~HttpExector();
    void cancelRequest(){cancelRequestFlag_ = true;}
    virtual int progress(double totalDownLoad,double downloaded,double totalUpload, double uploaded) = 0;

protected:
    virtual void setheader(std::vector<std::string> &headers);
    int submitFormData(const std::string &uri, void *formpost);
    int send(HTTP_METHORD methord, const std::string &url, const std::string &data, unsigned long timeout = 1) override;
    const std::string & responseData() override;

private:
    CURL *_curl = nullptr;
    curl_slist *_headers = nullptr;
    std::string _recvData;
};
}

#endif // NETHTTPEXECTOR_H
