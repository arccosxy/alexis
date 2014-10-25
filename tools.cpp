// Written by Pioz.
// Compile with: gcc -o autoclick autoclick.c -lX11
 
#include "tools.h"

#define DefRootWin DefaultRootWindow(disp)
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
  Window root;
  root = DefaultRootWindow(display);
  XWarpPointer (display, None, root, 0,0,0,0, x, y);
  XFlush (display);
  usleep (1);
}

void get_window_geom(Display *disp, Window win, Geometry *geom)
{
    int x, y;
    unsigned int bw, depth;
    Window root;
    memset(geom, 0, sizeof(Geometry));
    XGetGeometry(disp, win, &root, &x ,&y, &geom->w, &geom->h, &bw, &depth);
    XTranslateCoordinates(disp, win, root, x, y, &geom->x, &geom->y, &root);
}

void send_keystrokes(Display *disp, Window win, const char*keys)
{
  Bool escaped=0;
  char numkeys_upper[]="~!@#$%^&*()_+|";
  char numkeys_lower[]="`1234567890-=\\";
  int navkeys[]={XK_Insert,XK_End,XK_Down,XK_Page_Down,XK_Left,-1,XK_Right,XK_Home,XK_Up,XK_Page_Up};
  int funkeys[]={XK_F1,XK_F2,XK_F3,XK_F4,XK_F5,XK_F6,XK_F7,XK_F8,XK_F9,XK_F10,XK_F11,XK_F12};
  unsigned const char*p;
  char*n;
  XEvent ev;
  memset(&ev.xkey,0,sizeof(XKeyEvent));
  ev.xkey.subwindow=None;
  ev.xkey.serial=1;
  ev.xkey.display=disp;
  ev.xkey.window=win;
  ev.xkey.root=DefRootWin;
  ev.xkey.same_screen=1;
  for (p=(unsigned const char*)keys; *p; p++) {
    int c;
    switch (*p) {
      case '\n':
        c=XK_Return;
        break;
      case '\t':
        c=XK_Tab;
        break;
      case '\033':
        c=XK_Escape;
        break;
      case '\b':
        c=XK_BackSpace;
        break;
      case '%':
        if (!escaped) {
          escaped=True;
          continue;
        } else {
          c=*p;
          break;
        }
      case '^':
        if (!escaped) {
          ev.xkey.state|=ControlMask;
          continue;
        }
      case '~':
        if (!escaped) {
          ev.xkey.state|=Mod1Mask;
          continue;
        }
      case '+':
        if (!escaped) {
          ev.xkey.state|=ShiftMask;
          continue;
        }
      case 'f':
      case 'F': {
        int f;
        if (escaped && p[1] && strchr("01", p[1]) && (p[2]>='0') && (p[2]<='9')) {
          char num[3]={0,0,0};
          if (p[1]=='1') {
            strncpy(num,(char*)p+1,2);
          } else {
            num[0]=p[2];
          }
          f=atoi(num);
          c= ((f>0)&&(f<13)) ? funkeys[f-1] : *p;
          p+=2;
        } else {
          c=*p;
        }
        break;
      }
      case '.':
        c=escaped?XK_Delete:*p;
        break;
      default:
      c=*p;
    }
    n=strchr(numkeys_upper,c);
    if (n) {
      c=numkeys_lower[n-numkeys_upper];
      ev.xkey.state|=ShiftMask;
    } else {
      if (escaped && (c>='0') && (c<='9') && (c!='5')) {
        c=navkeys[c-48];
      } else {
        ev.xkey.state|=isupper(c)?ShiftMask:0;
      }
    }
    ev.xkey.keycode=XKeysymToKeycode(disp,c);
    ev.xkey.type=KeyPress;
    XSendEvent(disp, win, True, KeyPressMask,&ev);
    usleep(1000);
    XSync(disp, False);
    ev.xkey.time=CurrentTime;
    ev.xkey.type=KeyRelease;
    XSendEvent(disp, win, True, KeyPressMask,&ev);
    usleep(1000);
    XSync(disp, False);
    ev.xkey.state=0;
    escaped=False;
  }
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
