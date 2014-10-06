CC = g++

CFLAGGS = -g 

testAppServer: main.o rtspServer.o
	$(CC) -o testAppServer main.o rtspServer.o

main.o: main.cpp rtspServer.h
	$(CC) $(CFLAGS) -c main.cpp

rtspClient.o: rtspServer.cpp rtspServer.h
	$(CC) $(CFLAGS) -c rtspServer.cpp

clean:
	-rm main.o rtspServer.o rtspServer
