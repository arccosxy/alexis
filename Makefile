CC = g++

CFLAGGS = -g
LIB = /usr/lib/X11
INCLUDE = /usr/include/X11 

appServer: main.o rtspServer.o
	$(CC) -o appServer main.o rtspServer.o 

main.o: main.cpp rtspServer.h
	$(CC) $(CFLAGS) -c main.cpp

rtspServer.o: rtspServer.cpp rtspServer.h
	$(CC) $(CFLAGS) -c rtspServer.cpp

clean:
	-rm main.o rtspServer.o appServer

