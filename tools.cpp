// Written by Pioz.
// Compile with: gcc -o autoclick autoclick.c -lX11
 
#include "tools.h"

// Simulate mouse click
void
click (Display *display, int button)
{
  // Create and setting up the event
  XEvent event;
  memset (&event, 0, sizeof (event));
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (display);
  while (event.xbutton.subwindow)
    {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer (display, event.xbutton.window,
		     &event.xbutton.root, &event.xbutton.subwindow,
		     &event.xbutton.x_root, &event.xbutton.y_root,
		     &event.xbutton.x, &event.xbutton.y,
		     &event.xbutton.state);
    }
  // Press
  event.type = ButtonPress;
  if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
  // Release
  event.type = ButtonRelease;
  if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
  usleep (1);
}
 
// Get mouse coordinates
void
coords (Display *display, int *x, int *y)
{
  XEvent event;
  XQueryPointer (display, DefaultRootWindow (display),
                 &event.xbutton.root, &event.xbutton.window,
                 &event.xbutton.x_root, &event.xbutton.y_root,
                 &event.xbutton.x, &event.xbutton.y,
                 &event.xbutton.state);
  *x = event.xbutton.x;
  *y = event.xbutton.y;
}
 
// Move mouse pointer (relative) 设置默认值需要g++
void
move (Display *display, int x, int y)
{
  XWarpPointer (display, None, None, 0,0,0,0, x, y);
  XFlush (display);
  usleep (1);
}
 
// Move mouse pointer (absolute)
void
move_to (Display *display, int x, int y)
{
  int cur_x, cur_y;
  //coords (display, &cur_x, &cur_y);
  //XWarpPointer (display, None, None, 0,0,0,0, -cur_x, -cur_y);
  //XWarpPointer (display, None, None, 0,0,0,0, x, y);
  XWarpPointer (display, None, None, 0,0,0,0, x, y);
  XFlush (display);
  usleep (1);
}
/*
void
str_echo(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE], buf2[MAXLINE];
	int i;
 
  // Open X display
  Display *display = XOpenDisplay (NULL);
  if (display == NULL)
    {
      fprintf (stderr, "Can't open display!\n");
      //return -1;
    }

again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0){
		printf("%d\n", n);

		for(i=0; i<n-1; i++){
			if(buf[i] == 'a'){		
				move (display, -10, 0);
				printf("move left!\n");
			}
			else if(buf[i] == 'd'){
				move (display, 10, 0);
				printf("move right!\n");	
			}
			else if(buf[i] == 'w'){
				move (display, 0, -10);
				printf("move up!\n");
			}
			else if(buf[i] == 's'){
				move (display, 0, 10);
				printf("move down!\n");
			}
			else if(buf[i] == 'p'){
				click (display, Button1);
				printf("mouse click!\n");
			}
		}		
		writen(sockfd, buf, n);
		memset(buf, 0x00, sizeof(buf));
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_echo: read error");

	XCloseDisplay (display);
	  // Close X display and exit
}
*/
/* 
// START HERE
int
main(int argc, char **argv)
{
	int			listenfd, connfd, app_pid;
	unsigned long		wid; 
	pid_t			childpid, cchildpid1, cchildpid2;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (SA *) &cliaddr, &clilen);

		if ( (childpid = fork()) == 0) {	// child process
			close(listenfd);	// close listening socket

			if((cchildpid1 = fork()) == 0){
				execl("/usr/lib/firefox/firefox", "firefox", (char *)0);			
			}
			sleep(5);
			app_pid = cchildpid1;
			printf("app_pid = %d\n", app_pid);

			// Start with the root window.
    			Display *display = XOpenDisplay(0);

    			WindowsMatchingPid match(display, XDefaultRootWindow(display), cchildpid1);

			// Print the result.
    			const list<Window> &result = match.result();
			cout << "reslut size = "<< result.size() << endl;
    			//for(list<Window>::const_iterator it = result.begin(); it != result.end(); it++)
    				//cout << "Window #" << (unsigned long)(*it) << endl;
    			cout << "Window id: "<< (unsigned long)(*result.begin()) << endl;
			wid = (unsigned long)(*result.begin());
			
			//XCloseDisplay (display2);

			if((cchildpid2 = fork()) == 0){
				//printf("child_app_pid = %d\n", app_pid);

				char xx[20];
				memset(xx,0,sizeof(xx));
				sprintf(xx, "%ld", wid);
				printf("zifuchuan: %s\n", xx);
				//char *const ff_argv[] = {}
				execl("./ffmpeg.sh", "./ffmpeg.sh", xx, (char*)0);
			}
			str_echo(connfd);	// process the request
			exit(0);
		}
		close(connfd);			// parent closes connected socket
	}
	
	return 0;
}
*/
