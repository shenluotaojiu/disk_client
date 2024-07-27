#include "tcpkernel.h"

TCPKernel::TCPKernel(QObject *parent)
{
    m_pNet = new tcpclient();
}

TCPKernel::~TCPKernel()
{
    delete m_pNet;
}

bool TCPKernel::open()
{
    if(!m_pNet->initNetWork(ip,port))
    {
        printf("inet init flase\n");
        return false;
    }

    return true;
}

void TCPKernel::close()
{
    m_pNet->unInitNetWork();
}

void TCPKernel::dealData(SOCKET sock, char *szbuf)
{

}

void TCPKernel::sendData(SOCKET sock, char *szbuf)
{

}

iKernel *TCPKernel::getKernel()
{
    return m_pKernel;
}
