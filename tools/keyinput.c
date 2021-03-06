/*//////////////////////////////////////////
//    Simulated keyboard input program    //
//                by:0x001                //
//////////////////////////////////////////*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <linux/uinput.h>
 
/*#define die(str) do{
        perror(str);
        exit(EXIT_FAILURE);
        } while(0);
*/
void die(char * str)
{
    perror(str);
    exit(EXIT_FAILURE);
}
 
int fd;
struct uinput_user_dev uidev; // uInput device structure
struct input_event     ev;  // Input device structure
 
/* Setup the uinput device */
void setup_uinput_device()
{
    int i=0;
    // Open the input device
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if(fd < 0)
        die("error: open /dev/uinput");
 
    memset(&uidev, 0, sizeof(uidev)); // Intialize the uInput device to NULL
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "uinput-Openkk");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1;
    uidev.id.product = 0x1;
    uidev.id.version = 1;
    // Setup the driver I/O channels
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    for (i=0; i < 256; i++) {
        ioctl(fd, UI_SET_KEYBIT, i);
    }
    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
 
    /* Create input device into input sub-system */
    write(fd, &uidev, sizeof(uidev));
    if(ioctl(fd, UI_DEV_CREATE) < 0)
        die("error: create uinput device");
}
 
// Simulated keyboard input
void simulate_key(int keycode, int keyvalue)
{
    memset(&ev, 0, sizeof(struct input_event));
    gettimeofday(&ev.time, NULL);
    ev.type = EV_KEY;
    ev.code = keycode;
    ev.value = keyvalue;
    //value = 1: press;
    //value = 0: release;
    if(write(fd, &ev, sizeof(struct input_event)) < 0)
    {
            printf("simulate key error\n");
            return;
    }else{
        printf("simulate key %d, %d\n", keycode, keyvalue);
    }     
}
 
void simulate_key_end()
{
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    write(fd, &ev, sizeof(struct input_event));
}
 
//  check the key is control key
int check_valid_control_key(int key)
 {
      int control_key[] = {29, 42, 54, 56, 97, 100, 125, 126};
      int len = sizeof(control_key) / sizeof(control_key[0]);
      int i;
      int ret = 0;
      for ( i = 0; i < len; i++)
      {
          if (control_key[i] == key)
          {
              ret = 1;
              break;
          }
      }
      return ret;
 }
 
int main(int argc, char *argv[])
{ 
    setup_uinput_device();
 
    sleep(5);
    int key;
    int i;
    /*for (i = 1; i < argc; i++)
    {
        key = atoi(argv[i]);
        simulate_key(key, 1);
        if (check_valid_control_key(key) == 0)
        //if not control key, release it
        {
            simulate_key(key, 0);
        }
    }
    //The code is so nice!
    for (i=1; i < argc; i++)
    {
        key = atoi(argv[i]);
        if (check_valid_control_key(key) == 1)
        {
            simulate_key(key, 0);
        }
    }*/
    //KEY_1 corresponding to 2! Look /usr/include/linux/input.h
    //KEY_2 corresponding to 3!
    key = 2;
    simulate_key(key, 1);
    simulate_key(key, 0);

    key = 3;
    simulate_key(key, 1);
    simulate_key(key, 0);
 
    simulate_key_end();
    if(ioctl(fd, UI_DEV_DESTROY) < 0)
        die("error: ioctl");
    close(fd);
 
    return 0;
}
