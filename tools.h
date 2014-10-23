#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <unistd.h>

#include "unp.h"
//#include "error.h"
//#include "rw.h"

#include <X11/Xatom.h>
#include <iostream>
#include <list>
#include <stdlib.h>

using namespace std;

typedef struct _Geometry
{
    int x;
    int y;
    unsigned int w;
    unsigned int h;
} Geometry;

class WindowsMatchingPid
{
public:
    WindowsMatchingPid(Display *display, Window wRoot, unsigned long pid)
        : _display(display)
        , _pid(pid)
    {
    // Get the PID property atom.
        _atomPID = XInternAtom(display, "_NET_WM_PID", True);
        if(_atomPID == None)
        {
                cout << "No such atom" << endl;
                return;
        }

        search(wRoot);
    }

    const list<Window> &result() const { return _result; }

private:
    unsigned long  _pid;
    Atom           _atomPID;
    Display       *_display;
    list<Window>   _result;

    void search(Window w)
    {
    // Get the PID for the current Window.
        Atom           type;
        int            format;
        unsigned long  nItems;
        unsigned long  bytesAfter;
        unsigned char *propPID = 0;
        if(Success == XGetWindowProperty(_display, w, _atomPID, 0, 1, False, XA_CARDINAL,
                                         &type, &format, &nItems, &bytesAfter, &propPID))
        {
                if(propPID != 0)
                {
                // If the PID matches, add this window to the result set.
                        if(_pid == *((unsigned long *)propPID))
                                _result.push_back(w);

                        XFree(propPID);
                }
        }

    // Recurse into child windows.
        Window    wRoot;
        Window    wParent;
        Window   *wChild;
        unsigned  nChildren;
        if(0 != XQueryTree(_display, w, &wRoot, &wParent, &wChild, &nChildren))
        {
                for(unsigned i = 0; i < nChildren; i++)
                        search(wChild[i]);
        }
    }
};

// Simulate mouse click
void click (Display *display, int button);
// Get mouse coordinates
void coords (Display *display, int *x, int *y);
// Move mouse pointer (relative) 设置默认值需要g++
void move (Display *display, int x, int y);
// Move mouse pointer (absolute)
void move_to (Display *display, int x, int y);
void get_window_geom(Display *disp, Window win, Geometry *geom);
