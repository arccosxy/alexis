#include "rtspServer.h"
#include <string>

long global_sessionID = 1;

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

string getSession(char *pMessage)
{
    string sessionId = "";
    char *pStr = pMessage;
    const char *pSubStr = "Session:";
    char *pResult = strstr(pStr, pSubStr);
    if(pResult != NULL)
    {
        pResult += strlen(pSubStr) + 1;
    }
    int i;
    for(i = 0; *pResult != '\r'; i++)
    {
        sessionId += *pResult;
        pResult ++;
    }
    return sessionId;
}

string handle_setup(struct req *req, string servIp, string cliIp)
{
    string res = "";
    char sessionId[30];
    string sid;
    global_sessionID ++;
    int len = sprintf(sessionId, "%ld", global_sessionID);
    sessionId[len] = '\0';
    sid = sessionId;
    res += "RTSP/1.0 200 OK\r\n";
    res += "CSeq: " + getCseq(req->data) + "\r\n";
    res += "Session: " + sid  + "\r\n";
    res += "Transport: H264T/DVBC/QAM;unicast\r\n";
    res += "OnDemandSessionId: be074250-cc5a-11d9-8cd5-0800200c9a66\r\n";
    res += "ClientSessionId: " + getClientSessionId(req->data) + "\r\n";
    res += "Content-type: application/sdp\r\n";
    res += "\r\n";
    res += "v=0\r\n";
    res += "o=- 0 0 IN IP4 " + servIp + "\r\n";
    res += "s=client\r\n";
    res += "c=IN IP4 " + cliIp + "\r\n";
    res += "t=0 0\r\n";
    res += "a=tool:libavformat 55.21.101\r\n";
    res += "m=video 5060 RTP/AVP 96\r\n";
    res += "a=rtpmap:96 H264/90000\r\n";
    res += "a=fmtp:96 packetization-mode=1\r\n";

    return res; 
}

string handle_play(struct req *req)
{
    int app_pid, video_pid;
    unsigned long wid;
    pid_t child_pid1, child_pid2;

    string res = "";
    res += "RTSP/1.0 200 OK\r\n";
    res += "CSeq: " + getCseq(req->data) + "\r\n";
    res += "Session: " + getSession(req->data)  + "\r\n";
    res += "Range: npt=0-\r\n";

    if((child_pid1 = fork()) == 0){
        execl("/usr/lib/firefox/firefox", "firefox", (char *)0);
    }
    sleep(5);
    app_pid = child_pid1;
    cout << "app_pid = " << app_pid << endl;
  
    // Start with the root window.
    Display *display = XOpenDisplay(0);

    WindowsMatchingPid match(display, XDefaultRootWindow(display), app_pid);

    // Print the result.
    const list<Window> &result = match.result();
    cout << "reslut size = "<< result.size() << endl;
    cout << "Window id: "<< (unsigned long)(*result.begin()) << endl;
    wid = (unsigned long)(*result.begin());

    //XCloseDisplay (display2);
 
    if((child_pid2 = fork()) == 0){
        char winId[20];
        memset(winId, 0, sizeof(winId));
        sprintf(winId, "%ld", wid);
        cout << "winId= " << winId << endl;
        execl("./ffmpeg.sh", "./ffmpeg.sh", winId, (char *)0);
    }
    else{
        video_pid = child_pid2;
        cout << "video_pid= " << video_pid << endl;
 
        return res;
    }
}
/*
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
*/
