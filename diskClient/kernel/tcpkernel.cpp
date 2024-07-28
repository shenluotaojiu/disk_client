#include "tcpkernel.h"

_BEGIN_PROTOCOL_MAP
PM(_default_register_rs,&TCPKernel::registerrs)
_END_PROTOCOL_MAP

TCPKernel::TCPKernel(QObject *parent)
{
    m_pNet = new tcpclient();
}

TCPKernel::~TCPKernel()
{
    delete m_pNet;
}

void TCPKernel::registerrs(char *szbuf)
{
    STRU_REGISTER_RS* srs = (STRU_REGISTER_RS*)szbuf;
    char result = srs->m_szResult;
    // 需要把数据传给widget界面
    emit signals_kernel_registerrs(result);
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

void TCPKernel::dealData(char *szbuf)
{
    // 包映射表
    int count=0;
    while(1)
    {
        if(g_PMEntries[count].m_nType == *szbuf)
        {
            (this->*g_PMEntries[count].m_pFun)(szbuf);
            break;
        }
        else if(g_PMEntries[count].m_nType == 0 && g_PMEntries[count].m_pFun == 0)
            break;

        count++;
    }
}

void TCPKernel::sendData(char *szbuf,int nlen)
{
    m_pNet->sendData(szbuf,nlen);
}
