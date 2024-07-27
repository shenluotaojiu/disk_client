#ifndef TCPKERNEL_H
#define TCPKERNEL_H

#include "ikernel.h"
#include <QObject>
#include "client/tcpclient.h"

class TCPKernel : public iKernel
{
    Q_OBJECT
private:
    explicit TCPKernel(QObject *parent = nullptr);
    ~TCPKernel();
    TCPKernel(const TCPKernel&) = delete;
    TCPKernel& operator=(const TCPKernel&) = delete;
signals:
public:
    virtual bool open();
    virtual void close();
    virtual void dealData(SOCKET sock,char* szbuf);
    virtual void sendData(SOCKET sock,char* szbuf);
    static iKernel* getKernel();

private:
    Inet *m_pNet;
    static iKernel* m_pKernel;
};

iKernel* TCPKernel::m_pKernel = new TCPKernel();

#endif // TCPKERNEL_H
