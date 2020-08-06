#include "httpexector.h"
#include <functional>
#include <QDebug>

size_t onReply(void *ptr,size_t size, size_t nmemb, void *userData)
{
    std::string *rvData = reinterpret_cast<std::string*>(userData);
    rvData->append(reinterpret_cast<char*>(ptr),size * nmemb);
    return size * nmemb;
}

int progress_func(void *ptr, double totalDownLoad,double downloaded,double totalUpload, double uploaded)
{
    DLL::HttpExector *objPtr = reinterpret_cast<DLL::HttpExector*>(ptr);
    if(objPtr->cancelRequestFlag_){
        return -1;
    }
    objPtr->progress(totalDownLoad,downloaded,totalUpload,uploaded);
    return 0;
}

DLL::HttpExector::HttpExector()
{
    _curl = curl_easy_init();
}

DLL::HttpExector::~HttpExector()
{
    curl_easy_cleanup(_curl);
}

void DLL::HttpExector::setheader(std::vector<std::string> &headers)
{
    if(_headers){
        curl_slist_free_all(_headers);
        _headers = nullptr;
    }

    for(auto &header : headers){
        _headers = curl_slist_append(_headers,header.c_str());
    }
    curl_easy_setopt(_curl,CURLOPT_HTTPHEADER,_headers);
}

int DLL::HttpExector::submitFormData(const std::string &uri, void *formpost)
{
    _recvData.clear();
    curl_easy_setopt(_curl,CURLOPT_URL,uri.c_str());
    curl_easy_setopt(_curl,CURLOPT_WRITEDATA, &_recvData);
    curl_easy_setopt(_curl,CURLOPT_WRITEFUNCTION, onReply);
    curl_easy_setopt(_curl,CURLOPT_TIMEOUT,-1);
    curl_easy_setopt(_curl,CURLOPT_NOPROGRESS,false);
    curl_easy_setopt(_curl,CURLOPT_PROGRESSFUNCTION,progress_func);
    curl_easy_setopt(_curl,CURLOPT_PROGRESSDATA,this);
    curl_easy_setopt(_curl,CURLOPT_HTTPPOST,formpost);

    return curl_easy_perform(_curl);
}

int DLL::HttpExector::send(DLL::HTTP_METHORD methord, const std::string &url, const std::string &data, unsigned long timeout)
{
    curl_easy_setopt(_curl,CURLOPT_URL,url.c_str());
    switch (methord) {
    case DLL::GET:
        curl_easy_setopt(_curl,CURLOPT_POST,0);
        break;
    case DLL::POST:
        curl_easy_setopt(_curl,CURLOPT_POST,1);
        curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, false);
        curl_easy_setopt(_curl,CURLOPT_PROGRESSFUNCTION,progress_func);
        curl_easy_setopt(_curl,CURLOPT_PROGRESSDATA,this);
        curl_easy_setopt(_curl,CURLOPT_POSTFIELDS,data.c_str());
        curl_easy_setopt(_curl,CURLOPT_POSTFIELDSIZE,data.size());
    }

    _recvData.clear();
    curl_easy_setopt(_curl,CURLOPT_WRITEDATA, &_recvData);
    curl_easy_setopt(_curl,CURLOPT_WRITEFUNCTION, onReply);
    curl_easy_setopt(_curl,CURLOPT_TIMEOUT,timeout);

    return curl_easy_perform(_curl);
}

const std::string &DLL::HttpExector::responseData()
{
    return _recvData;
}
