#include "UDPchatServer.h"


// Structure of arguments to pass to client thread
struct ThreadArgs{
    char* msgClient;                        // recvMsg
};




void createThread(char* ClientMsg){
    pthread_t threadID;                     // Thread ID from pthread_create()
    struct ThreadArgs *clntInfos;           // Pointer to argument structure for thread

    // Create separate memory for client argument
    if ((clntInfos = (struct ThreadArgs *) malloc(sizeof(struct ThreadArgs))) == NULL)
        PERROR("malloc() failed");

    // Collect the arguments to a struct, to pass to the ThreadFunction
    clntInfos->msgClient = ClientMsg;

    // Create client thread
    if (pthread_create(&threadID, NULL, ThreadFunction, (void *) clntInfos) != 0)
        PERROR("pthread_create() failed");

//  printf("[DEBUG] Thread %ld is running\n", (long int) threadID);
} //end createThread()




void *ThreadFunction(void *threadArgs){
//  printf("[DEBUG] Thread %ld is running\n", pthread_self());

    char msg[RECVBUFSIZE];                  // copy of Client's message received
    struct sockaddr_in thrClientAddr;       // copy of ClientAddr
    struct ClientInfo{
        unsigned int clntID;                // socket descriptor for client connection
        char clntNAME[64];                  // nickname
        unsigned short clntRoom;            // current room
    } user;
    unsigned short int fName;               // flag to indicate, there is a name inside clntNAME[]


    // Guarantees that thread resources are deallocated upon return
    pthread_detach(pthread_self());

    // Copy the client's message before release the memory for new messages
    strcpy(msg, ((struct ThreadArgs *) threadArgs)->msgClient);
    thrClientAddr = ClientAddr;
    // Release rx buffer for new messages  and  Deallocate memory for argument
    memset(recvMsg, '\0', RECVBUFSIZE);
    free(threadArgs);



    // Clean and initialize variables
    user.clntID = 0;
    memset(user.clntNAME, '\0', sizeof(user.clntNAME));
    user.clntRoom = 0;
    fName = 0;



    // Virtual client socket
    user.clntID = HandleClientID(&thrClientAddr);

    // Save log
    saveLog(user.clntID, msg);

    // If it was the first message of a new client
    if(newuser){
        newuser = 0;
        showWelcome(user.clntID, msg, &thrClientAddr);
        return (NULL);
    }

    // Check if the client already has a nickname: (user.clntNAME = nickname)
    fName = HandleClientNAME(user.clntID, user.clntNAME);

    // If the is no name registered, accept this message as the name informed
    if(!fName){
        strncpy(user.clntNAME, msg, strlen(msg)-1);
        saveClientNAME(user.clntID, user.clntNAME);
        Transmit((char*)"[info] name successfully registered\n", &thrClientAddr, NOtime, 0);
        showHomePage(user.clntID, &thrClientAddr);
        Transmit((char*)"\n\n\n>> ", &thrClientAddr, NOtime, 0);
        updateClntRoom(user.clntID, __HOMEPAGE);

        char broadcastInform[128];
        sprintf(broadcastInform, "\n\n[info] new user *%s* is connected\n\n>> ", user.clntNAME);
        broadcastMessage(user.clntID, user.clntNAME, broadcastInform, _SYST);   //send to ip 255.255.255.255 didn't work

        return (NULL);
    }

    // Check the current room the user is
    user.clntRoom = HandleClientROOM(user.clntID);



    ///TREAT/REDIRECT MESSAGES:

    //Treat private messages
    if(msg[0]=='@'){
        signed short sflag;                                                     //-1: not found; 0: echo; 1: transmited; -2: no msg
        sflag = privateMessage(user.clntID, user.clntNAME, msg);

        if(sflag == -1){                                                        //fail
            Transmit((char*)"[info] user not found\n", &thrClientAddr, NOtime, 0);
        }else if(sflag == 0){                                                   //loop back
            Transmit((char*)">> ", &thrClientAddr, NOtime, 0);
            Transmit((char*)"$(*echo*): ", &thrClientAddr, SHOWtime, 0);
            Transmit((msg+strlen(user.clntNAME)+2), &thrClientAddr, NOtime, 0);
        }else if(sflag == -2){                                                  //no message detected
            char aux[75];
            char atname[64];
            size_t n = strlen(msg)-2;                                           //discard @ and \n
            strncpy(atname, msg+1, n);
            sprintf(aux, "[info] *%s* is online\n", atname);
            Transmit(aux, &thrClientAddr, NOtime, 0);
        }else{                                                                  //success
            //
        }

    }

    // Treat command
    else if(msg[0]=='!'){
        TreatCommand(user.clntID, user.clntRoom, msg, &thrClientAddr);
    }

    // Redirect the message to the other users (if is not a command nor a private msg)
    else{
        if(user.clntRoom == __HOMEPAGE){
            broadcastMessage(user.clntID, user.clntNAME, msg, _USER);
        }
        else{
            roomMessage(user.clntID, user.clntNAME, user.clntRoom, msg, _USER);
        }
    }

    Transmit((char*)">> ", &thrClientAddr, NOtime, 0);                          //sendto()

    return (NULL);
} //end ThreadFunction()
