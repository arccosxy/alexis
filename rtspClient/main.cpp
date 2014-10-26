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
		"-b setup         \n"
		"-p play          \n"
		"-t teardown      \n"
		"-w move up       \n"
		"-s move down     \n"
		"-a move left     \n"
		"-d move right    \n"
		"-c click         \n"
                "-0~9 input number\n"
                "-e backspace     \n"
                "-n enter         \n"
		"-q quit          \n");
	fprintf(stdout, ">");
	fscanf(stdin, "%s", command);
        switch(command[0]){
		case 'b':
			client.Setup();
			break;
		case 'p':
			client.Play();
			break;
		case 't':
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
		case '0':
			client.Play('0');
			break;
		case '1':
			client.Play('1');
			break;
		case '2':
			client.Play('2');
			break;
		case '3':
			client.Play('3');
			break;
		case '4':
			client.Play('4');
			break;
		case '5':
			client.Play('5');
			break;
		case '6':
			client.Play('6');
			break;
		case '7':
			client.Play('7');
			break;
		case '8':
			client.Play('8');
			break;
		case '9':
			client.Play('9');
			break;
		case 'e':
			client.Play('e');
			break;
		case 'n':
			client.Play('n');
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
