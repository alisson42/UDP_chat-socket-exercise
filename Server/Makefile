CC=gcc
CFLAGS=-I -std=c99 -pedantic -Wall -Wextra -Wwrite-strings -Werror -pthread
DEPS=UDPchatServer.h
OBJ=HandleUDPLowLayer.o HandleExternalFiles.o HandleThreads.o HandleUDPClient.o chat_server-UDP.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

chatserver:$(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f $(OBJ) *.o chatserver
