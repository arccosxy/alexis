#ifndef RTSPCLIENT_H
#define RTSPCLIENT_H

#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

class rtspClient{
    public:
        rtspClient(const char *pStreamUri,
                 const char *pServerIp,
                    short iServerPort);
        virtual ~ rtspClient();
        size_t    Setup();
        size_t    Play();
	//new method for control applacations.
        size_t    Play(char opera);
        size_t    Teardown();
        size_t    Heartbeat();
        size_t    Get_parametr();
    protected:
    private:
        char    m_szStreamUri[50];
        char    m_szSession[50];
        int     m_tcpSocketDesc;
        size_t  m_iCSeq;
        struct  sockaddr_in m_sockAddr;
    private:
        void parseResMessage(char *pMessage);
};

#endif
