#include "tcpsocketexector.h"
#include <functional>
#include <QDebug>
#ifdef linux
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

DLL::TcpSocketExector::TcpSocketExector()
{
}

DLL::TcpSocketExector::~TcpSocketExector()
{
    close();
}


int DLL::TcpSocketExector::connect(std::string &ip, int port)
{
#ifdef WIN32
    SOCKADDR_IN addr;
    addr.sin_family = PF_INET;
    addr.sin_port = ::htons(port);
    addr.sin_addr.S_un.S_addr = ::inet_addr(ip.data());
    int ret = ::connect(m_sockfd,(SOCKADDR*)&addr,sizeof(SOCKADDR));
    if(ret == -1){
        qDebug() << "socket connect error sockfd: " << m_sockfd << __FILE__ << __LINE__;
    }
    return ret;
#else
    sockaddr_in addr;
    ::memset(&addr,0,sizeof(addr));
    addr.sin_family = PF_INET;
    addr.sin_port = ::htons(port);
    addr.sin_addr.s_addr = ::inet_addr(ip.data());
    int ret = ::connect(m_sockfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret == -1){
        qDebug() << "socket connect error sockfd: " << m_sockfd << __FILE__ << __LINE__;
    }
    return ret;
#endif
}

int DLL::TcpSocketExector::sendData(std::string &data)
{
    char *p = const_cast<char*>(data.data());
    unsigned int length = data.size();
    unsigned int bytes = 0;
    int ret = 0;
    while (bytes < length) {
        ret = ::send(m_sockfd,p + bytes,length - bytes,0);
        if(ret == -1){
            return -1;
        }
        bytes += ret;
    }
    return bytes;
}

int DLL::TcpSocketExector::createSocket()
{
    close();
#ifdef WIN32
    WSADATA wsa;
    if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        qDebug() << "WSAStartup failed";
        return -1;
    }
    m_sockfd = int(::socket(PF_INET,SOCK_STREAM,IPPROTO_TCP));
#else
    m_sockfd = ::socket(PF_INET,SOCK_STREAM,0);
#endif
    if(m_sockfd == -1){
        qDebug() << "socket error sockfd: " << m_sockfd << __FILE__ << __LINE__;
        return -1;
    }
    return m_sockfd;
}

size_t DLL::TcpSocketExector::recvData(void *buffer, unsigned long size)
{
    return ::recv(m_sockfd, reinterpret_cast<char*>(buffer),size,0);
}

void DLL::TcpSocketExector::close()
{
#ifdef WIN32
    ::closesocket(m_sockfd);
    WSACleanup();
#else
    ::close(m_sockfd);
#endif
}
