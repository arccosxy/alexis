#include "rtspServer.h"
#include <string>

string getMethod(char *pMessage)
{
    string method = "";
    char *pStr = pMessage;
    while(*pStr != ' ')
    {
        method += *pStr;
        pStr ++;
    }
    return method;
}

string getCseq(char *pMessage)
{
    string cseq = "";
    char *pStr = pMessage;
    const char *pSubStr = "CSeq:";
    char *pResult = strstr(pStr, pSubStr);
    if(pResult != NULL)
    {
        pResult += strlen(pSubStr) + 1;
    }
    for(int i = 0; *pResult != '\r'; i++)
    {
        cseq += *pResult;
        pResult ++;
    }
    return cseq;
}

string getClientSessionId(char *pMessage)
{
    string clientSessionId = "";
    char *pStr = pMessage;
    const char *pSubStr = "ClientSessionId:";
    char *pResult = strstr(pStr, pSubStr);
    if(pResult != NULL)
    {
        pResult += strlen(pSubStr) + 1;
    }
    for(int i = 0; *pResult != '\r'; i++)
    {
        clientSessionId += *pResult;
        pResult ++;
    }
    return clientSessionId;
}

int handle_setup(struct req *req)
{
    string res = "";
    char sessionId[30];
    struct sockaddr_in serv, cli;
    socklen_t serv_len = sizeof(serv);
    socklen_t cli_len = sizeof(cli);
    char serv_ip[20], cli_ip[20];
    
    getsockname(req->type, (struct sockaddr *)&cli, &cli_len);
    getpeername(req->type, (struct sockaddr *)&serv, &serv_len);
    inet_ntop(AF_INET, &cli.sin_addr, cli_ip, sizeof(cli_ip));
    inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
    string cliIp(cli_ip);
    string servIp(serv_ip);
    cout << "The servIp is: " << servIp << endl;
    cout << "The cliIp is: " << cliIp << endl;

    global_sessionID ++;
    int len = sprintf(sessionId, "%ld", global_sessionID);
    sessionId[len] = '\0';
    res += "RTSP/1.0 200 OK\r\n";
    res += "CSeq: " + getCseq(req->data) + "\r\n";
    res += "Transport: H264T/DVBC/QAM;unicast\r\n";
    res += "OnDemandSessionId: be074250-cc5a-11d9-8cd5-0800200c9a66\r\n";
    res += "ClientSessionId: " + getClientSessionId(req->data) + "\r\n";
    res += "Content-type: application/sdp\r\n";
    res += "\r\n";
    res += "v=0\r\n";
    res += "o=- 0 0 IN IP4 " + servIp + "\r\n";
    res += "clint\r\n";
    res += "IN IP4 " + cliIp + "\r\n";
    res += "t=0 0\r\n";
    res += "a=tool:libavformat 55.21.101\r\n";
    res += "m=video 5060 RTP/AVP 96\r\n";
    res += "rtpmap:96 H264/90000\r\n";
    res += "a=fmtp:96 packetization-mode=1\r\n"; 
}

int main()
{
    int running = 1;
    int msgid;
    int msg_to_receive = 0;
    struct req req_data;
    int ret;
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if(msgid == -1){
        perror("msgget failed");
    }
    cout << "rtspServer running..." << endl;
    while(running)
    {
        if(msgrcv(msgid, (void *)&req_data, MAXLINE, msg_to_receive, 0) == -1)
        {
	    perror("msgrcv failed");
        }
        printf("msgcv:\n%s", req_data.data);

        if(getMethod(req_data.data) == "SETUP")
            ret = handle_setup(&req_data);
    }
    return 0;
}
