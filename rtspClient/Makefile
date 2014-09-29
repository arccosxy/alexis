CC = g++

CFLAGGS = -g 

rtspClient: main.o rtspClient.o
	$(CC) -o rtspClient main.o rtspClient.o

main.o: main.cpp rtspClient.h
	$(CC) $(CFLAGS) -c main.cpp

rtspClient.o: rtspClient.cpp rtspClient.h
	$(CC) $(CFLAGS) -c rtspClient.cpp

clean:
	-rm main.o rtspClient.o rtspClient
