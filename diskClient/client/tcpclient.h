#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <winsock2.h>
#include <windows.h>
#include "inet.h"
#include<stdio.h>
#include<stdlib.h>

class tcpclient : public Inet
{
private:
    SOCKET sockclient;
    HANDLE m_hthread;
    bool m_bFlagQuit;
public:
    tcpclient();
    ~tcpclient();

    bool initNetWork(const char* szip = "127.0.0.1",unsigned short nport = 1234);
    void unInitNetWork(const char* szerr = "");
    void recvData();
    bool sendData(const char* szbuf,int nlen);
    static DWORD WINAPI ThreadProc(_In_ LPVOID lpParameter);
};

#endif // TCPCLIENT_H
