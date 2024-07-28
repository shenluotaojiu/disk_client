#include "tcpkernel.h"

_BEGIN_PROTOCOL_MAP

_END_PROTOCOL_MAP

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
    int count = 0;
    while(1)
    {
        if(g_PMEntries[count].m_nType == *szbuf)
        {
            (this->*g_PMEntries[count].m_pFun)(sock,szbuf);
            break;
        }
        else if(g_PMEntries[count].m_nType == 0
             && g_PMEntries[count].m_nType == 0)
            break;
        count++;
    }
}

void TCPKernel::sendData(SOCKET sock, char *szbuf)
{

}

// iKernel *TCPKernel::getKernel()
// {
//     return m_pKernel;
// }
