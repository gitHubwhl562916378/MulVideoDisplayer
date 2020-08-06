#ifndef SERVICEFACETORY_H
#define SERVICEFACETORY_H

#include "servicei.h"
#include "Service/restserviceconcurrent.h"
#include "videoencodebyffmpeg.h"
class ServiceFactory : public ServiceFactoryI
{
public:
    ServiceFactory() = default;
    inline NotifyServiceI* makeNotifyServiceI(NotifyInterfaceType s) override{
        switch (s) {
        default:
            break;
        }
        return nullptr;
    }
    inline RestServiceI* makeRestServiceI(RestInterfaceType s) override{
        RestServiceI *ptr{nullptr};
        switch (s) {
        case Concurrent:
            ptr = new RestServiceConcurrent;
            break;
        }
        return ptr;
    }
    inline VideoEncodeI* makeVideoEncodeI(VideoEncodeType t = FFmpeg) override{
        VideoEncodeI *ptr{nullptr};
        switch (t) {
        case FFmpeg:
            ptr = new VideoEncodeByFFmpeg;
            break;
        }
        return ptr;
    }
};
#endif // SERVICEFACETORY_H
