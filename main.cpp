#include "unp.h"
#include "rtspServer.h"
#include <limits.h>
#include <sys/msg.h>

#define SERV_PORT 554
#define SERV_IP "127.0.0.1"
#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif
/*
struct req 
{
    int type;
    char data[MAXLINE];
};
*/
int main(int argc, char **argv)
{
    printf("-------------------------------------------------------------\n");
    printf("App Sever running...\n");
    printf("Server IP: %s Port: %d \n", SERV_IP, SERV_PORT);
    printf("-------------------------------------------------------------\n");

    system("./rtspServer &");
    int i, maxi, listenfd, connfd, sockfd;
    int nready;
    ssize_t recvbytes;
    char buf[MAXLINE];
    socklen_t clilen;
    struct pollfd client[OPEN_MAX];
    struct sockaddr_in cliaddr, servaddr;
    
    if(-1 == (listenfd = socket(AF_INET, SOCK_STREAM, 0))){
	perror("error in create socket");
	exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    if(-1 == bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))){
	perror("bind error");
	exit(0);
    }
    printf("bind success\n");

    if(-1 == listen(listenfd, LISTENQ)){
	perror("listen error");
	exit(0);
    }
    printf("The server is listenning...\n");

    client[0].fd = listenfd;
    client[0].events = POLLRDNORM;
    for(i = 1; i < OPEN_MAX; i++)
        client[i].fd = -1;
    maxi = 0;

    for(;;){
        nready = poll(client, maxi+1, INFTIM);
	if(client[0].revents & POLLRDNORM){
	    clilen = sizeof(cliaddr);
	    if(-1 == (connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen))){
		perror("accept error");
	    }
	    else{
		printf("accept connect.\n");
	        printf("accept connfd is %d\n", connfd);
	    }
	    for(i = 1; i < OPEN_MAX; i++){
		if(client[i].fd < 0){
		    client[i].fd = connfd;
		    break;
		}
	    }
	    if(i == OPEN_MAX)
		perror("too many clients");
	    client[i].events = POLLRDNORM;
	    if(i > maxi)
		maxi = i;
	    if(--nready <= 0)
		continue;
	}

	for(i =1; i <= maxi; i++){
	    if((sockfd = client[i].fd) < 0)
		continue;
	    if(client[i].revents & (POLLRDNORM | POLLERR)){
		if((recvbytes = recv(sockfd, buf, MAXLINE, 0)) < 0){
		    if(errno == ECONNRESET){
			close(sockfd);
			client[i].fd = -1;
		    } else
			perror("read error");
		} 
		else if(recvbytes == 0){
		    close(sockfd);
		    client[i].fd = -1;
		}
		else{
		    buf[recvbytes] = '\0';
		    printf("%d bytes receive from connect:\n%s\n", recvbytes, buf);
		    send(sockfd, buf, strlen(buf)+1, 0);

		    //Add message queue.
		    int msgid;
		    struct req req_data;
		    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
		    if(msgid == -1){
			perror("msgget failed");
		    }
		    req_data.type = sockfd;
		    strcpy(req_data.data, buf);

		    if(msgsnd(msgid, (void*)&req_data, MAXLINE, 0) == -1){
				perror("msgsnd failed");
		    }		
		}
		if(--nready <= 0)
		    break;
	    }
	}	
    }
} 
