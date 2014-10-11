#include "unp.h"
#include "rtspServer.h"
#include <limits.h>
#include <sys/msg.h>

#define SERV_PORT 8554
#define SERV_IP "127.0.0.1"
#ifndef OPEN_MAX
#define OPEN_MAX 1024
#endif
#define CONTROLLEN sizeof(struct cmsghdr) + sizeof(int)
/*
struct req 
{
    int type;
    char data[MAXLINE];
};
*/
ssize_t send_fd(int sockfd, int fd_to_send)
{
    //char tmpbuf[CONTROLLEN];
    //struct cmsghdr *cmptr = (struct cmsghdr *)tmpbuf;

#ifdef HAVE_MSGHDR_MSG_CONTROL
    union{
        struct cmsghdr cm;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct iovec iov[1];
    struct msghdr msg;
    char buf[1];
    struct cmsghdr *cmptr;
   
    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *)CMSG_DATA(cmptr)) = fd_to_send;
#else
    msg.msg_accrights = (caddr_t)&fd_to_send;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    
    iov[0].iov_base = buf;
    iov[0].iov_len = 1;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    
    return(sendmsg(sockfd, &msg, 0));
}

int recv_fd(int sockfd, int *fd_to_recv)
{
    //char tmpbuf[CONTROLLEN];
    //struct cmsghdr *cmptr = (struct cmsghdr *)tmpbuf;
    struct iovec iov[1];
    struct msghdr msg;
    ssize_t n;
    char buf[1];
#ifdef HAVE_MSGHDR_MSG_CONTROL
    union{
        struct cmsghdr *cmptr;
        char control[CMSG_SPACE(sizeof(int))];
    }control_un;
    struct cmsghdr *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    int newfd;
    msg.msg_accrights = (caddr_t)&newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    
    iov[0].iov_base = buf;
    iov[0].iov_len = 1;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if((n = recvmsg(sockfd, &msg, 0)) <= 0)
        return(n);
#ifdef HAVE_MSGHDR_MSG_CONTROL
    if((cmptr = CMSG_FIRSTHDR(&msg)) != NULL && cmptr->cmsg_len == CMSG_LEN(sizeof(int))){
        if(cmptr->cmsg_level != SOL_SOCKET)
            perror("control level != SOL_SOKECT");
        if(cmptr->cmsg_type != SCM_RIGHTS)
            perror("control type != SCM_RIGHTS");
        *fd_to_recv = *(int *)CMSG_DATA(cmptr);
    }
    else
        *fd_to_recv = -1;
#else 
    if(msg.msg_accrightslen == sizeof(int))
        *fd_to_recv = newfd;
    else
        *fd_to_recv = -1;
#endif
    return(n);
}

int x_sock_set_block(int sockfd, int on)
{
    int val, ret;
    val = fcntl(sockfd, F_GETFL, 0);
    if(on){
        ret = fcntl(sockfd, F_SETFL, ~O_NONBLOCK & val);
    }
    else{
        ret = fcntl(sockfd, F_SETFL, O_NONBLOCK | val);
    }
    if(ret){
        return errno;
    }
    return 0;
}

int main(int argc, char **argv)
{
    printf("-------------------------------------------------------------\n");
    printf("App Sever running...\n");
    printf("Server IP: %s Port: %d \n", SERV_IP, SERV_PORT);
    printf("-------------------------------------------------------------\n");
    
    int sockpair[2];
    int ret, fd;
    pid_t pid;

    ret = socketpair(AF_UNIX, SOCK_STREAM, 0, sockpair);
    if(ret < 0){
        perror("Call socketpair error");
        exit(0);
    }
    //system("./rtspServer &");
    pid = fork();
    if(pid == 0){
        //child process
        cout << "in child..." << endl;
        close(sockpair[1]);

        int running = 1;
        int msgid;
        int msg_to_receive = 0;
        struct req req_data;
        int ret;
        char response[MAXLINE];
        string res;

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
            
            /*ret = x_sock_set_block(sockpair[0], 1);
            if(ret != 0){
                perror("x_sock_set_block error");
            }*/
            ret = recv_fd(sockpair[0], &fd);
            if(ret <= 0)
                cout << "recv_fd failed." << endl;
            cout << "Recv sockfd is: " << fd << endl;
            if(getMethod(req_data.data) == "SETUP")
            {  
                struct sockaddr_in serv, cli;
                socklen_t serv_len = sizeof(serv);
                socklen_t cli_len = sizeof(cli); 
                char serv_ip[20], cli_ip[20];

                getsockname(fd, (struct sockaddr *)&cli, &cli_len);
                getpeername(fd, (struct sockaddr *)&serv, &serv_len);
                inet_ntop(AF_INET, &cli.sin_addr, cli_ip, sizeof(cli_ip));
                inet_ntop(AF_INET, &serv.sin_addr, serv_ip, sizeof(serv_ip));
                string cliIp(cli_ip);
                string servIp(serv_ip);
                cout << "The servIp is: " << servIp << endl;
                cout << "The cliIp is: " << cliIp << endl;
                res = handle_setup(&req_data, servIp, cliIp);
            }
            if(getMethod(req_data.data) == "PLAY")
            {
                res = handle_play(&req_data);
            }
            strcpy(response, res.c_str());
            cout << "response is:\r\n" << response;
            send(fd, response, strlen(response)+1, 0);
            close(fd);
        }

        return 0;
    }
    //parent process
    close(sockpair[0]);
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
		    //send(sockfd, buf, strlen(buf)+1, 0);

		    //Add message queue.
		    int msgid;
		    struct req req_data;
		    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
		    if(msgid == -1){
			perror("msgget failed");
		    }
		    req_data.type = sockfd;
                    cout << "The sockfd is: " << sockfd << endl;
		    strcpy(req_data.data, buf);
                    
                    if(send_fd(sockpair[1], sockfd) < 0){
                        perror("send_fd failed");
                    }
                    cout << "send_fd..." << endl;

		    if(msgsnd(msgid, (void*)&req_data, MAXLINE, 0) == -1){
			perror("msgsnd failed");
		    }
                    //close(sockfd); 		
		}
		if(--nready <= 0)
		    break;
	    }
	}	
    }
} 
