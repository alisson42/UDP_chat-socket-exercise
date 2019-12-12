#ifndef UDPCHATSERVER_H
#define UDPCHATSERVER_H

///Libraries
#include <stdio.h>              // for perror(), printf() and fprintf()
#include <stdlib.h>             // for exit() and atoi()
#include <time.h>               // for uptime
#include <sys/socket.h>         // for socket() and bind()
#include <arpa/inet.h>          // for sockaddr_in and inet_ntoa()
#include <string.h>             // for memset()
#include <unistd.h>             // for close()
#include <sys/types.h>          // for the pthread.h
#include <pthread.h>            // for POSIX threads
#include <sys/stat.h>           // for stat() and mkdir()
#include <netinet/in.h>         // for htons()
#include <ctype.h>              // for tolower() toupper()




///Globaldef
#define RECVBUFSIZE 4096
#define PERROR(s) { perror(s); exit(-1); }
#define SERVERPORT  1234        //don`t take any Official or Reserved   //https://en.wikipedia.org/wiki/List_of_TCP_and_UDP_port_numbers
#define NOtime      0
#define SHOWtime    1
#define __HOMEPAGE  0           //also works as a flag: don't show room (in Transmit() function)
#define __PixnetPG  1
#define __SportPAGE 2
#define _SYST       0
#define _USER       1



///Globalvar
time_t start_time, current_time, up_time;           // receive time()
struct tm c_time;                                   // time split in yy-mm-dd hh:mm:ss ...
int servSock;                                       // server socket
char recvMsg[RECVBUFSIZE];                          // RX buffer
struct sockaddr_in ClientAddr;                      // Client structure info
unsigned short int newuser;                         // indicates first msg of a need client
unsigned int users_registered;                      // last ID
unsigned long int n_msg_processed;                  // number of messages processed



///Prototypes
//HandleUDPLowLayer.c
int CreateUDPServerSocket(unsigned short port);                                         // Create UDP server socket
void Transmit(char*, struct sockaddr_in*, unsigned short, unsigned short);              // Sendto()
//HandleThreads.c
void createThread(char* ClientMsg);                                                     // Crete Thread function
void *ThreadFunction(void *threadArgs);                                                 // Thread function
//HandleUDPClient.c
unsigned int HandleClientID(struct sockaddr_in *ClntAddrStruct);                        // Delegate Cliend ID
unsigned short HandleClientNAME(unsigned int ID, char name_buffer[]);                   // Read the stored client's name
unsigned short HandleClientROOM(unsigned int ID);                                       // Read the stored client's room
void showWelcome(unsigned int ID, char* MSG, struct sockaddr_in *ClntAddrStruct);       // Print the Welcome message
void showHomePage(unsigned int ID, struct sockaddr_in *ClntAddrStruct);                 // Print the Home Page
void broadcastMessage(unsigned int ID, char name[], char text[], unsigned short flag);  // Treat broadcast messages
signed short privateMessage(unsigned int ID, char senderName[], char text[]);           // Treat private messages
void TreatCommand(unsigned int, unsigned short, char*, struct sockaddr_in*);            // Treat commands
void showRoomPage(unsigned int IDclnt, unsigned short idRoom, char roompage[]);         // Print the rooms page
void roomMessage(unsigned int, char*, unsigned short, char*, unsigned short);           // Treat room messages
//HandleExternalFiles.c
void CreateDefaultRoomFolders(void);                                                    // Create default rooms
unsigned int searchClientID (char* ip, unsigned short port);                            // Access external file relate to ID
unsigned short searchClientNAME (unsigned int id, char nickname[]);                     // Recover the Client nickname
void saveClientNAME (unsigned int id, char nickname[]);                                 // Save Client nickname
unsigned short searchClientLOCATION(unsigned int id);                                   // Recover the Client room
void scanClientREGISTERED_byID (unsigned int id, char buffer[]);                        // Read for print all clients registered (give id, receive name)
unsigned int scanClientREGISTERED_byNAME (char nickname[]);                             // Look for a client ID (give name, receive id)
unsigned int countUsersREGISTERED (void);                                               // Sweep the database to count how many users
void saveLog (unsigned int id, char* msg);                                              // Save the clients message in a log file
void updateClntRoom (unsigned int id, unsigned char room);                              // Update the client location in the chat
void saveRoomLog (unsigned short idRoom, char* sender, char* msg, unsigned short flag); // Save room log
void getOutRoom(unsigned int id, unsigned short idRoom);                                // Leave the room
void getInRoom(unsigned int id, unsigned short idRoom);                                 // Enter in a room
void scanRoomClients_byID (unsigned int id, unsigned short idRoom, char buffer[]);      // Show who is inside the room



#endif
