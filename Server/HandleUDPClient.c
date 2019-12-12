#include "UDPchatServer.h"



unsigned int HandleClientID(struct sockaddr_in *ClntAddrStruct){
    unsigned int ID = 0;

    // Makes the relation between <ip,port> with <id>
    ID = searchClientID(inet_ntoa(ClntAddrStruct->sin_addr), ClntAddrStruct->sin_port);

    return ID;
} //end HandleClientID()



unsigned short HandleClientNAME(unsigned int ID, char name_buffer[]){
    unsigned short fname = 0;

    // Makes the relation between <id> with <nickname>
    fname = searchClientNAME(ID, name_buffer);

    return fname;
} //end HandleClientNAME()



unsigned short HandleClientROOM(unsigned int ID){
    unsigned short room = 0xFF;

    // Makes the relation between <id> with <room>
    room = searchClientLOCATION(ID);

    return room;
} //end HandleClientROOM()



void showWelcome(unsigned int ID, char* MSG, struct sockaddr_in *ClntAddrStruct){

    // Inform success in establish the connection
    Transmit((char*)"\n[info] connection established successfully\n", *&ClntAddrStruct, NOtime, __HOMEPAGE);

    // Discard the first message: only utility is to establish connection
    char* discard_warn = malloc(strlen(MSG)+100);
    sprintf(discard_warn, "[info] you're the user number %u\n[warning] previous message discarded: %s", ID, MSG);
    Transmit(discard_warn, *&ClntAddrStruct, NOtime, __HOMEPAGE);
    free(discard_warn);

    // Give the Welcome message and ask his/her name
    const char* welcome = "\n\n\n  Welcome to chat  * Let's Talk! *  :D\n\n\n\n>>Please insert your nickname:  ";
    Transmit((char*)welcome, *&ClntAddrStruct, NOtime, __HOMEPAGE);

} //end showWelcome()



void showHomePage(unsigned int ID, struct sockaddr_in *ClntAddrStruct){

    char homepage[2048];
    users_registered = countUsersREGISTERED();
    sprintf(homepage, "\n\n\n[HOME PAGE]\n\n  There are %u users resgistered in the chat so far (and probably online):", users_registered);
    scanClientREGISTERED_byID(ID, homepage);
    strcat(homepage, "\n\n  Rooms available: 2\n    \t#1 PIXNET\n    \t#2 SPORTS\n");
    strcat(homepage, "\n  General instructions:\n");
    strcat(homepage, "    All messages typed in the Home Page are broadcast messages\n");
    strcat(homepage, "    Messages written inside a room it is only visible there\n");
    strcat(homepage, "    Private messages follow the format:  @<user> <message>\n");
    strcat(homepage, "    Actions e commands follow the format:  !<command>\n");
    strcat(homepage, "    For more infomations type: !help\n");

    Transmit((char*)homepage, *&ClntAddrStruct, NOtime, __HOMEPAGE);

} //end showHomePage()



void broadcastMessage(unsigned int ID, char name[], char text[], unsigned short flag){

    struct sockaddr_in clientsAddr;
    char* ip = malloc(16);
    unsigned int port, id;

    FILE *fp;
    const char *fname = "./db/clients_ip_port_id.txt";          //file with the info of all clients
    fp = fopen(fname, "r");                                     //read only

    char filestr[127];                                          //string with the file's line
    fgets(filestr, 30, fp);                                     //read the first line of the file (skip the header)
    while (!feof(fp)){                                          //until the end of the file
        fscanf(fp, "%15s %5u %5u\n", ip, &port, &id);           //read line per line
        if(ID != id){
            memset(&clientsAddr, 0, sizeof(clientsAddr));       //zero out structure
            clientsAddr.sin_family = AF_INET;                   //internet address family = 2
            clientsAddr.sin_port = port;                        //local port            //problem: how to use htons(port) ?
            clientsAddr.sin_addr.s_addr = inet_addr(ip);        //client IP address     //inet_aton(ip, &clientsAddr.sin_addr.s_addr);
            if(flag == _USER){
                char aux[75];
                sprintf(aux, "$(%s): ", name);
                Transmit(aux, &clientsAddr, SHOWtime, __HOMEPAGE);                      //inform sender
            }
            Transmit((char*)text, &clientsAddr, NOtime, __HOMEPAGE);                    //sendto()
            if(flag == _USER){
                Transmit((char*)">> ", &clientsAddr, NOtime, __HOMEPAGE);               //show "waiting"-token of the system
            }
        } //end IF it is not the requester
    } //end WHILE it is not the end of the file
    free(ip);                                                   //release memory
    fclose(fp);                                                 //close file

} //end broadcastMessage()



signed short privateMessage(unsigned int ID, char senderName[],char text[]){
    signed short sflag;                                             //-1: not found;  0: echo;  1: success;

    struct sockaddr_in recipientAddr;
    char* r_ip = malloc(16);
    unsigned int r_port, r_id;
    char* r_name = malloc(64);
    char* s_msg;

    strtok_r (text, " ", &s_msg);                                   //split string:
    if(!strlen(s_msg)){                                             //if there is no message
        sflag = -2;
        free(r_ip);                                                 //release memory
        free(r_name);                                               //release memory
        return sflag;                                               //no message detected
    }
    strcpy(r_name, text+1);                                         //ignore symbol '@'

    r_id = scanClientREGISTERED_byNAME(r_name);

    if(!r_id){                                                      //client not found
        sflag = -1;
    }
    else if(r_id == ID){                                            //it is yourself
        sflag = 0;
    }
    else{
        unsigned int id;
        FILE *fp;
        const char *fname = "./db/clients_ip_port_id.txt";          //file with the info of all clients
        fp = fopen(fname, "r");                                     //read only

        char filestr[127];                                          //string with the file's line
        fgets(filestr, 30, fp);                                     //read the first line of the file (skip the header)
        while (!feof(fp)){                                          //until the end of the file
            fscanf(fp, "%15s %5u %5u\n", r_ip, &r_port, &id);       //read line per line
            if(r_id == id){
                memset(&recipientAddr, 0, sizeof(recipientAddr));   //zero out structure
                recipientAddr.sin_family = AF_INET;                 //internet address family = 2
                recipientAddr.sin_port = r_port;                    //local port
                recipientAddr.sin_addr.s_addr = inet_addr(r_ip);    //receiver IP address

                break;                                              //finish search
            } //end IF it is not the requester
        } //end WHILE it is not the end of the file
        fclose(fp);                                                 //close file

        char aux[75];
        sprintf(aux, "[pvt] $(%s): ", senderName);
        Transmit(aux, &recipientAddr, SHOWtime, 0);
        Transmit(s_msg, &recipientAddr, NOtime, 0);
        Transmit((char*)">> ", &recipientAddr, NOtime, 0);

        sflag = 1;
    }

    free(r_ip);                                                     //release memory
    free(r_name);                                                   //release memory

    return sflag;
} //end privateMessage()



void TreatCommand(unsigned int ID, unsigned short room, char command[], struct sockaddr_in *ClntAddrStruct){
    char inform[2048];

    for(uint i=0; i<strlen(command); i++)
        command[i] = tolower(command[i]);                           //convert characters to lower case

    if(!strcmp(command, "!help\n")){                                            //-show commands
        sprintf(inform, "\n  Commands available:\n"
                        "    !status          show status of the server\n"
                        "    !online          show registered users\n"
                        "    !#<room number>  enter room\n"
                        "    !home            return to home page\n\n");
    }else if(!strcmp(command, "!status\n")){                                    //-show status
        up_time = time(NULL) - start_time;
        sprintf(inform, "\n  Server status:\n"
                        "    uptime: %lu seconds\n"
                        "    messages processed: %lu\n"
                        "    rooms available: 2\n"
                        "    \t#1 PIXNET\n"
                        "    \t#2 SPORTS\n\n", up_time, n_msg_processed);
    }else if(!strcmp(command, "!online\n")){                                    //-show clients registered
        users_registered = countUsersREGISTERED();
        sprintf(inform, "\n  There are %u users resgistered in the chat so far (and probably online):", users_registered);
        scanClientREGISTERED_byID(ID, inform);
        strcat(inform, "\n\n");
    }else if(!strcmp(command, "!home\n")){                                      //-send user to HOME PAGE
        if(room != __HOMEPAGE){                                                 //if is changing room
            getOutRoom(ID, room);
            updateClntRoom(ID, __HOMEPAGE);
        }
        showHomePage(ID, *&ClntAddrStruct);                                     //receive the auto-welcome of the room
        sprintf(inform, "\n\n");
    }else if(!strcmp(command, "!#1\n")){                                        //-send user to ROOM #1 PAGE
        if(room == __PixnetPG){                                                 //if already inside, do nothing
            sprintf(inform, "[info] you are already inside the requested room\n");
        }else{                                                                  //changing location
            getOutRoom(ID, room);                                               //if was in another room before, leave
            updateClntRoom(ID, __PixnetPG);                                     //update client location
            getInRoom(ID, __PixnetPG);                                          //enter the new room
            showRoomPage(ID, __PixnetPG, inform);                               //receive the auto-welcome of the room
        }
    }else if(!strcmp(command, "!#2\n")){                                        //-send user to ROOM #2 PAGE
        if(room == __SportPAGE){
            sprintf(inform, "[info] you are already inside the requested room\n");
        }else{
            getOutRoom(ID, room);
            updateClntRoom(ID, __SportPAGE);
            getInRoom(ID, __SportPAGE);
            showRoomPage(ID, __SportPAGE, inform);
        }
    }else{                                                                      //-inform unknow command
        char* aux = malloc(strlen(command));
        strncpy(aux, command+1, strlen(command)-2);
        sprintf(inform, "[info] command <%s> is not available;\ttry: !help\n", aux);
        free(aux);
    }

    Transmit((char*)inform, *&ClntAddrStruct, NOtime, 0);

} //end TreatCommand()



void showRoomPage(unsigned int IDclnt, unsigned short idRoom, char roompage[]){

    switch(idRoom){
      case __PixnetPG:
        sprintf(roompage,   "\n\n\n[PIXNET ROOM]\n\n"
                            "  Welcome to PIXNET room!\n\n");
        break;

      case __SportPAGE:
        sprintf(roompage,   "\n\n\n[SPORTS ROOM]\n\n"
                            "  Welcome to SPORTS room!\n\n");
        break;

      default:
        PERROR("Unexpected room!");
    }

    strcat(roompage,"  The messages typed here will only be visualized by who is also in this room.\n"
                    "  You will continually receive the messages typed in the Home Page\n"
                    "  User currently inside the room:");
    scanRoomClients_byID(IDclnt, idRoom, roompage);
    strcat(roompage, "\n\n\n");

} //end showRoomPage()



void roomMessage(unsigned int IDclnt, char name[], unsigned short idRoom, char text[], unsigned short flag){
    saveRoomLog(idRoom, name, text, flag);                              //save log

    unsigned int id_reg;                                                //receive the id read from clients_connected.txt
    FILE *f_info, *f_conn;
    const char *fname_addr = "./db/clients_ip_port_id.txt";             //file with the info of all clients
    char* fname_insid = malloc(50);
    sprintf(fname_insid, "./rooms/%u/clients_connected.txt", idRoom);   //file with the client's id inside the room


    //Sweep all file "clients_connected.txt"
    f_conn = fopen(fname_insid, "r");                                   //read only
    while (!feof(f_conn)){                                              //until the end of the file
        fscanf(f_conn, "%u\n", &id_reg);                                //read line per line all ids registered
        if(id_reg != IDclnt){                                           //if it is an id different of the sender
            f_info = fopen(fname_addr, "r");                            //read only
            //Sweep file "clients_ip_port_id.txt" search for the infos from the user id_reg
            char filestr[127];                                          //string with the file's line
            char* ip = malloc(16);                                      //declare variable to store ip
            unsigned int port, id;                                      //declare variables to store port and id read from the file
            struct sockaddr_in clientsAddr;                             //create struct to send message to the user

            fgets(filestr, 127, f_info);                                //read the first line of the file (skip the header)
            while (!feof(f_info)){                                      //until the end of the file
                fscanf(f_info, "%15s %5u %5u\n", ip, &port, &id);       //read line per line (until find the user)
                if(id == id_reg){                                       //if user found
                    memset(&clientsAddr, 0, sizeof(clientsAddr));       //zero out structure
                    clientsAddr.sin_family = AF_INET;                   //internet address family = 2
                    clientsAddr.sin_port = port;                        //local port            //problem: how to use htons(port) ?
                    clientsAddr.sin_addr.s_addr = inet_addr(ip);        //client IP address     //inet_aton(ip, &clientsAddr.sin_addr.s_addr);
                    if(flag == _USER){
                        char aux[75];
                        sprintf(aux, "$(%s): ", name);
                        Transmit(aux, &clientsAddr, SHOWtime, idRoom);  //inform sender
                    }
                    Transmit((char*)text, &clientsAddr, NOtime, 0);     //sendto()
                    Transmit((char*)">> ", &clientsAddr, NOtime, 0);    //show "waiting"-token of the system

                    free(ip);                                           //release memory
                    fclose(f_info);                                     //close info file
                    break;                                              //finish here to see if there is more people in the room
                } //end IF id in INFO file found
            } //end WHILE it is not the end of the INFO file
        } //end IF avoid echo
    } //end WHILE it is not the end of the CONNECTED USERS TO THE ROOM file
    fclose(f_conn);                                                     //close conn file
    free(fname_insid);                                                  //release memory

} //end roomMessage()
