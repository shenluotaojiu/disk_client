#include "tcpkernel.h"

TCPKernel::TCPKernel(QObject *parent)
    : QObject{parent}
{
    m_pNet = new tcpclient(this);
}

_BEGIN_PROTOCOL_MAP
PM(_default_register_rs,&TCPKernel::registerrs)
PM(_default_login_rs,&TCPKernel::loginrs)
PM(_default_getfilelist_rs,&TCPKernel::getfilelistrs)
PM(_default_upload_fileinfo_rs,&TCPKernel::uploadinfors)

_END_PROTOCOL_MAP


    TCPKernel::~TCPKernel()
{
    delete m_pNet;
}

void TCPKernel::registerrs(char *szbuf)
{
    // 需要把数据传给widget界面
    emit signals_kernel_registerrs((STRU_REGISTER_RS*)szbuf);
}

void TCPKernel::loginrs(char *szbuf)
{
    emit signals_kernel_loginrs((STRU_LOGIN_RS*)szbuf);
}

void TCPKernel::getfilelistrs(char *szbuf)
{
    emit signals_kernel_getfilelistrs((STRU_GETFILELIST_RS*)szbuf);
}

void TCPKernel::uploadinfors(char *szbuf)
{
    emit signals_kernel_uploadinfors((STRU_UPLOADFILEINFO_RS*)szbuf);
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

