#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define RECVBUFSIZE     4096
#define PERROR(s) { perror(s); exit(-1); }



// Globalvar
struct sockaddr_in servAddr;
int clientSock;                 // server socket
char *servIP; 		        // server address - internal: "127.0.0.1"
int servPort;         	        // server port - we are using 1234



void *ThreadFunction(){
    /* Rudimentar Wait function*/
    for(int w=0; w<10; w++){
        for(int k=0; k<0xFFFFFF; k++);
    }

    /* Define the message to send */
    char message[4096];
    sprintf(message, "#auto-message to establish connection\n");
    size_t messageLenght = strlen(message)+1;
    /* Send the message to the server */
    if (sendto(clientSock, message, messageLenght, 0, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1)
      PERROR("sendto() failed");
    memset(message, '\0', strlen(message));


    for(;;){
        scanf("%s", message);
        strcat(message, "\n");
        messageLenght = strlen(message)+1;
        /* Send the message to the server */
        if (sendto(clientSock, message, strlen(message)+10, 0, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1)
          PERROR("sendto() failed");
        memset(message, '\0', strlen(message));
    }

    /*NEVER REACH*/
    return(NULL);
}



// Client program
int main(int argc, char *argv[]){

  if (argc != 3){                   /* Test for correct number of arguments */
    fprintf(stderr, "\nExpected: %s <Server IP> <Server PORT>\n    e.g.: %s 127.0.0.1 1234\n\n", argv[0], argv[0]);
    exit(1);
  }

  servIP = argv[1];             	/* First arg: server IP address (dotted quad) */
  servPort = atoi(argv[2]);         /* Second arg: server port */

  printf ("Chat UDP Client: try to connect to server %s port %d\n", servIP, servPort);

  /* Create socket for incoming connections */
  if ((clientSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    PERROR("socket() failed");

  /* Define the server address */
  memset(&servAddr, 0, sizeof(servAddr));       /* Zero out structure */
  servAddr.sin_family = AF_INET;                /* Internet address family */
  servAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
  servAddr.sin_port = htons(servPort);          /* Local port */


  /*Sendto will be treated in a thread, while the Recvfrom in the main*/
{
    pthread_t threadID;
    if (pthread_create(&threadID, NULL, ThreadFunction, NULL) != 0)
        PERROR("pthread_create() failed");
}


  /* Receive a response */
  struct sockaddr_in responseAddr;
  unsigned int responseAddrSize = sizeof(responseAddr);
  char responseBuffer[RECVBUFSIZE+1];
  int responseStringLenght;

  /*Recvfrom will be treated in the main, while the Sendto in a thread*/
  for(;;){
    if ((responseStringLenght = recvfrom(clientSock, responseBuffer, RECVBUFSIZE, 0, (struct sockaddr *) &responseAddr, &responseAddrSize)) <0)
      PERROR("recvfrom() failed");

    if (servAddr.sin_addr.s_addr != responseAddr.sin_addr.s_addr) {
      fprintf(stderr,"Error: received a packet from unknown source.\n");
      exit(1);
    }

    /* null-terminate the received data */
    responseBuffer[responseStringLenght] = '\0';
    /* Print received message arg */
    printf("%s\n", responseBuffer);                 //<<<<<<<<< big issue: '\n' necessary, but break all formatation from the server: sugestion: use "nc -u <ip> <port>"
  }


  /*NEVER REACH*/
  close(clientSock);
  return 0;
}
