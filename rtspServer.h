#include "unp.h"
#include "tools.h"
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>
#include <map>

using namespace std;

struct req
{
    int type;
    char data[MAXLINE];
};

typedef struct _Geometry
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
} Geometry;

typedef struct sValue
{
    string cliIp;
    int x;
    int y;
    int app_pid;
    int video_pid;
    unsigned long wid;
}sValue;

string getMethod(char *pMessage);
string getCsq(char *pMessage);
string getClientSessionId(char *pMessage);
string getSession(char *pMessage);
string getOperation(char *pMessage);
string handle_setup(struct req *req, string servIp, string cliIp);
string handle_play(struct req *req, string cliIp);
string handle_operate(struct req *req);
string handle_teardown(struct req *req);
