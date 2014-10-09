#include "./rtspClient.h"
#include <stdio.h>

char	streamUri[] = "rtsp://appserver234.comcast.com:554/98765";
char      servIP[] = "127.0.0.1";
unsigned int    port = 8554;

int main(int argc, char *argv[])
{
    char command[2];
    memset(command, 0, 2);
    rtspClient client(streamUri, servIP, port);
    while(1){
        
	fprintf(stdout, "please input the command:\n"
		"-[method option]:\n"
		"-1 setup         \n"
		"-2 play          \n"
		"-3 teardown      \n"
		"-w move up       \n"
		"-s move down     \n"
		"-a move left     \n"
		"-d move right    \n"
		"-c click         \n"
		"-q quit          \n");
	fprintf(stdout, ">");
	fscanf(stdin, "%s", command);
        switch(command[0]){
		case '1':
			client.Setup();
			break;
		case '2':
			client.Play();
			break;
		case '3':
			client.Teardown();
			break;
		case 'w':
			client.Play('w');
			break;
		case 's':
			client.Play('s');
			break;
		case 'a':
			client.Play('a');
			break;
		case 'd':
			client.Play('d');
			break;
		case 'c':
			client.Play('c');
			break;
		case 'q':
			return 0;
			break;
		default:
			fprintf(stdout, "Illegal Input!\n");
			break;
	}
    }
    return 0;
  
}
