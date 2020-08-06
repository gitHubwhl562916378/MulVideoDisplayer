#ifndef TCPSOCKETEXECTOR_H
#define TCPSOCKETEXECTOR_H

#include "basenetexector.h"
#if WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
namespace DLL {
class TcpSocketExector : public BaseSocketExector
{
public:
    TcpSocketExector();
    ~TcpSocketExector();

protected:
    int createSocket() override;
    int connect(std::string& ip, int port) override;
    size_t recvData(void *buffer, unsigned long size) override;
    void close() override;
    int sendData(std::string &data) override;

private:
    int m_sockfd;
};
}

#endif // TCPSOCKETEXECTOR_H

