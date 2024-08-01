#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include <QObject>
#include "ikernel.h"
#include "../client/tcpclient.h"
#include "Packdef.h"

class TCPKernel;

typedef void(TCPKernel::*PFUN)(char*);
struct MyProtocolMap
{
    char m_nType;
    PFUN m_pFun;
};


class TCPKernel : public QObject, public iKernel
{
    Q_OBJECT
public:
    explicit TCPKernel(QObject *parent = nullptr);
    ~TCPKernel();

    void registerrs(char *szbuf);
    void loginrs(char *szbuf);
    void getfilelistrs(char *szbuf);
    void uploadinfors(char *szbuf);

signals:        //信号
    void signals_kernel_registerrs(STRU_REGISTER_RS*);
    void signals_kernel_loginrs(STRU_LOGIN_RS*);
    void signals_kernel_getfilelistrs(STRU_GETFILELIST_RS*);
    void signals_kernel_uploadinfors(STRU_UPLOADFILEINFO_RS*);

public:
    virtual bool open();
    virtual void close();
    virtual void dealData(char* szbuf);
    virtual void sendData(char* szbuf,int nlen);


private:
    Inet *m_pNet;


};

#endif // TCPKERNEL_H
