#include "UDPchatServer.h"

/*______________________________________________________________.
Server:                         |Client:                        |
         TCP      UDP           |        TCP      UDP           |
--------------------------------|-------------------------------|
        socket   socket()       |       socket   socket()       |
               v                |              v                |
        bind     bind()	        |         -        -            |
               v                |              v                |
        listen     -            |         -        -            |
               v                |              v                |
        accept     -            |       connect  connect()      |
               v                |              v                |
        recv     recvfrom()     <       send     sendto()       |
               v                |              v                |
        send     sendto()       >       recv     recvfrom()     |
               v                |              v                |
        close    close()        |       close    close()        |
================================================================|
Server:	make > ./chatserver                                     |
Client:	nc -u localhost 1234                                    |
###############################################################*/



// Server program
int main() {
    start_time = time(NULL);
    users_registered = 0;
    n_msg_processed = 0;

    unsigned int ClientLen = sizeof(ClientAddr);    // size of Client structure
    ssize_t recvMsgSize;                            // length of the recv message in bytes

    //Create rooms
    CreateDefaultRoomFolders();

    //Create socket and bind
    servSock = 0;
    servSock = CreateUDPServerSocket(SERVERPORT);


    //Inifinity loop
    for (;;){
        // Wait receive a new message (recvfrom is a block function)
        if ((recvMsgSize = recvfrom(servSock, recvMsg, RECVBUFSIZE, 0, (struct sockaddr *) &ClientAddr, &ClientLen)) < 0)
            PERROR("recvfrom() failed");
        n_msg_processed++;

//      printf("\n%s", recvMsg);                // only for debug

        // Create Thread to Handle message
        if(recvMsgSize > 1)                         // if the person just didn't press Enter
            createThread(&recvMsg[0]);

    } //end FOR (inf loop)

    // hopefully never reached
    close(servSock);
    return 0;
} //end main()
