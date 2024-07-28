#include "tcpclient.h"
#include "kernel/tcpkernel.h"

tcpclient::tcpclient()
{
    sockclient = 0;
    m_hthread = nullptr;
    m_bFlagQuit = true;
}

tcpclient::~tcpclient()
{

}

DWORD WINAPI tcpclient::ThreadProc(_In_ LPVOID lpParameter)
{
    tcpclient* pthis = (tcpclient*)lpParameter;
    pthis->recvData();
    return 0;
}

bool tcpclient::initNetWork(const char *szip, unsigned short nport)
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {

        // printf("WSAStartup failed with error: %d\n", err);
        unInitNetWork("WSAStartup failed with error");
        return 0;
    }

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        //printf("Could not find a usable version of Winsock.dll\n");
        unInitNetWork("Could not find a usable version of Winsock.dll\n");
        return 0;
    }
    else
        printf("The Winsock 2.2 dll was found okay\n");

    sockclient = socket(AF_INET, SOCK_STREAM, 0);
    if (sockclient == INVALID_SOCKET)
    {
        // printf("socket err\n");
        unInitNetWork("socket err\n");
        return 0;
    }

    sockaddr_in addrserver;
    addrserver.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addrserver.sin_port = htons(1234);
    addrserver.sin_family = AF_INET;

    if (SOCKET_ERROR == connect(sockclient, (sockaddr*)&addrserver, sizeof(addrserver)))
    {
        printf("connect err\n");
        WSACleanup();
        return 0;
    }

    //
    m_hthread = CreateThread(0,0,&ThreadProc,this,0,0);
    return true;
}



void tcpclient::unInitNetWork(const char* szerr)
{
    printf(szerr);
    if(sockclient)
    {
        closesocket(sockclient);
        sockclient = 0;
    }
    WSACleanup();

    m_bFlagQuit = false;
    if(m_hthread)
    {
        if(WaitForSingleObject(m_hthread,100) == WAIT_TIMEOUT)
            TerminateThread(m_hthread,-1);
        CloseHandle(m_hthread);
        m_hthread = nullptr;
    }
}

void tcpclient::recvData()
{
    int nPackSize;
    int nReadNum;
    char* pszbuf;
    while(m_bFlagQuit)
    {
        nReadNum = recv(sockclient,(char*)&nPackSize,sizeof(nPackSize),0);
        if(nReadNum <= 0 || nPackSize <= 0)
            continue;

        pszbuf = new char[nPackSize];
        int offset = 0;
        while(nPackSize)
        {
            nReadNum = recv(sockclient,pszbuf+offset,nPackSize,0);
            if(nReadNum > 0)
            {
                nPackSize -= nReadNum;
                offset += nReadNum;
            }
        }

        // todo
        TCPKernel m_kernel;
        m_kernel.dealData(pszbuf);

        delete[] pszbuf;
        pszbuf = nullptr;
    }
}

bool tcpclient::sendData(const char *szbuf, int nlen)
{
    //先发包大小
    if(send(sockclient,(const char*)&nlen,sizeof(nlen),0) <= 0)
        return false;
    //再发包内容
    if(send(sockclient,szbuf,nlen,0) <= 0)
        return false;
    return true;
}
