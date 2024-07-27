#ifndef INET_H
#define INET_H
#include<Winsock2.h>

class Inet
{
public:
    Inet() = default;
    virtual ~Inet() = default;

public:
    virtual bool initNetWork(const char* szip = "127.0.0.1",unsigned short nport = 1234) = 0;
    virtual void unInitNetWork(const char* szerr = "") = 0;                                         //卸载网络
    virtual bool sendData(const char* szbuf,int nlen) = 0;                                          //向服务器发送数据
    virtual void recvData() = 0;                                                                    //接收数据
};


#endif // INET_H
