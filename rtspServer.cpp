#include "rtspServer.h"
#include <string>

int handle_setup(struct req *req)
{
    string res = "";
    char sessionId[30];
    global_sessionID ++;
    int len = sprintf(sessionId, "%ld", global_sessionID);
    sessionId[len] = '\0';
    res = res + "RTSP/1.0 " + "200 " + "OK\r\n";
    
}

int main()
{
    int running = 1;
    int msgid;
    int msg_to_receive = 0;
    struct req req_data;
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
    }
    return 0;
}
