#ifndef IKERNEL_H
#define IKERNEL_H

#include"client/tcpclient.h"

class iKernel
{
public:
    iKernel() = default;
    virtual ~iKernel() = default;

public:
    virtual bool open() = 0;
    virtual void close() = 0;
    virtual void dealData(SOCKET sock,char* szbuf) = 0;
    void set_inet_parameter(const char* ip,unsigned short port)
    {
        this->ip = ip;
        this->port = port;
    }
protected:


protected:
    const char *ip = nullptr;
    unsigned short port = 0;
};

#endif // IKERNEL_H
