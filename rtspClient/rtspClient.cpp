#include "./rtspClient.h"

rtspClient::rtspClient(const char *pStreamUri, const char *pServerIp, short iServerPort)
{
    int result;
    struct sockaddr_in servaddr;

    m_iCSeq = 0;
    memset(m_szStreamUri, 0, 50);
    strcpy(m_szStreamUri, pStreamUri);
    memset(m_szSession, 0, 50);

    m_tcpSocketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(m_tcpSocketDesc == -1)
        perror("Create Socket error");
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(iServerPort);
    servaddr.sin_addr.s_addr = inet_addr(pServerIp);

    result = connect(m_tcpSocketDesc, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(result == -1){
        perror("Connect error");
    }
    else
        printf("Connect to server!\n");
 
}

rtspClient::~rtspClient()
{
    if(m_tcpSocketDesc != -1)       
        close(m_tcpSocketDesc);
}

//功能：从返回消息中取出session字段值
void rtspClient::parseResMessage(char *pMessage)
{
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
    	m_szSession[i] = *pResult;
        pResult ++;
    }
    m_szSession[i] = '\0';
    printf("session is: %s\n", m_szSession);
}

//功能：从setup请求的响应中取出SDP字段
void rtspClient::getSDP(char *pMessage, char *fileName)
{
    char *pStr = pMessage;
    const char *pSubStr = "v=0";
    char tmp[1024];
    char *pResult = strstr(pStr, pSubStr);
    int len = sprintf(tmp, "%s", "SDP:\r\n");
    if(pResult != NULL)
    {
        int i;
        for(i = len; *pResult != '\0'; i++)
        {
            tmp[i] = *pResult;
            pResult ++;
        }
        tmp[i] = '\0';
        printf("SDP is: \n%s\n", tmp);
        FILE *pFile = fopen(fileName, "w");
        fwrite(tmp, 1, strlen(tmp), pFile);
        fclose(pFile);
    }
    else{
        perror("get SDP error");
    }
}

size_t rtspClient::Setup()
{
    m_iCSeq++;
    char setupCommand[500];
    sprintf(setupCommand, "SETUP rtsp://sessionmanager2.comcast.com:554/;purchaseToken=c0c2d8b0-cc\
82-11d9-8cd50-800200c9a66;serverID=127.0.0.1 RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Transport: RTP/AVP;unicast;client=00AF123456DE;qam_name=Chicago.Southbend.5\r\n"
            "User-Agent: shawn\r\n"
            "ClientSessionId: 00262254CCCA00000001\r\n", m_iCSeq);
    fprintf(stdout, "Setup request message is:\n%s\n", setupCommand);
    int iRetValue = 0;
    iRetValue = send(m_tcpSocketDesc, setupCommand, strlen(setupCommand) + 1, 0);
    if(iRetValue == -1){
        return -1;
    }
    char setupResponse[1500];
    memset(setupResponse, 0, 1500);
    iRetValue = recv(m_tcpSocketDesc, setupResponse, 1500, 0);
    if(iRetValue == -1){
        return -1;
    }

    fflush(NULL);
    fprintf(stdout, "setup response message is:\n%s\n", setupResponse);
    
    //从返回信息中取出seesion字段    
    parseResMessage(setupResponse);
    //从返回信息中取出SDP字段
    getSDP(setupResponse, "client.sdp");
    return 0;
}

size_t rtspClient::Teardown()
{
    if(m_tcpSocketDesc == -1)
    {
        return -1;	
    }
    m_iCSeq++;
    char teardownCommand[500];
    memset(teardownCommand, 0, 500);
    sprintf(teardownCommand, "TEARDOWN %s RTSP/1.0\r\n"
	    "CSeq: %d\r\n"
   	    "Session: %s\r\n"
    	    "User-Agent: shawn\r\n"
  	    "\r\n", m_szStreamUri, m_iCSeq, m_szSession);
    fprintf(stdout, "teardown request message is:\n%s\n", teardownCommand);
    int iRetValue = 0;
    iRetValue = send(m_tcpSocketDesc, teardownCommand, strlen(teardownCommand)+1, 0);
    if(iRetValue == -1)
    {
        return -1;
    }
    char teardownResponse[1500];
    memset(teardownResponse, 0, 1500);
    iRetValue = recv(m_tcpSocketDesc, teardownResponse, 1500, 0);
    if(iRetValue == -1)
    {
        return -1;
    }
    fflush(NULL);
    fprintf(stdout, "teardown response message is:\n%s\n", teardownResponse);
    return 0;

}

size_t rtspClient::Play()
{
    int video_pid;
    pid_t child_pid;

    if(m_tcpSocketDesc == -1)
    {
        return -1;
    }

    m_iCSeq++;
    char playCommand[1500];
    memset(playCommand, 0, 1500);
    sprintf(playCommand, "PLAY %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
	    "Session: %s\r\n"
	    "Range: npt=0-\r\n"
	    "User-Agent: shawn\r\n"
	    "\r\n", m_szStreamUri, m_iCSeq, m_szSession);
    fprintf(stdout, "play request message is:\n%s\n", playCommand);

    int iRetValue = 0;
    iRetValue = send(m_tcpSocketDesc, playCommand, strlen(playCommand)+1, 0);
    if(iRetValue == -1)
    {
	return -1;
    }
    char playResponse[1500];
    memset(playResponse, 0, 1500);
    iRetValue = recv(m_tcpSocketDesc, playResponse, 1500, 0);
    if(iRetValue == -1)
    {
        return -1;
    }

    fflush(NULL);
    fprintf(stdout, "play response message is:\n%s\n", playResponse);
    if((child_pid = fork()) == 0){
         system("ffplay client.sdp > /dev/null 2>&1");
         exit(0);
    }
    video_pid = child_pid;
    cout << "client video_pid= " << video_pid << endl;

    return 0;

}

size_t rtspClient::Play(char opera)
{
    if(m_tcpSocketDesc == -1)
    {
        return -1;
    }

    m_iCSeq++;
    char playCommand[1500];
    memset(playCommand, 0, 1500);
    sprintf(playCommand, "PLAY %s RTSP/1.0\r\n"
            "CSeq: %d\r\n"
            "Session: %s\r\n"
            "Operation: %c\r\n"
            "\r\n", m_szStreamUri, m_iCSeq, m_szSession, opera);
    fprintf(stdout, "play request message is:\n%s\n", playCommand);

    int iRetValue = 0;
    iRetValue = send(m_tcpSocketDesc, playCommand, strlen(playCommand)+1, 0);
    if(iRetValue == -1)
    {
        return -1;
    }
    char playResponse[1500];
    memset(playResponse, 0, 1500);
    iRetValue = recv(m_tcpSocketDesc, playResponse, 1500, 0);
    if(iRetValue == -1)
    {
        return -1;
    }

    fflush(NULL);
    fprintf(stdout, "play response message is:\n%s\n", playResponse);
    return 0;

}
