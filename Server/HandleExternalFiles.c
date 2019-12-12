#include "UDPchatServer.h"



//For simplicity the user cannot create new rooms, neither delete
void CreateDefaultRoomFolders(void){
    char path[64];

    //Directory /rooms
    sprintf(path, "./rooms");
    struct stat st0 = {0};
    if (stat((const char*)path, &st0) == -1) {
        mkdir(path, 0700);
    }

    //Create room #1: Pixnet
    sprintf(path, "./rooms/1");
    struct stat st1 = {0};
    if (stat((const char*)path, &st1) == -1) {
        mkdir(path, 0700);
    }

    //Create room #2: Sport
    sprintf(path, "./rooms/2");
    struct stat st2 = {0};
    if (stat((const char*)path, &st2) == -1) {
        mkdir(path, 0700);
    }
}



//Give or Recover id attributed to a client
unsigned int searchClientID (char* ip, unsigned short port){
    unsigned int id = 0;
    int nline = 0;
    FILE *fp;
    const char *fname = "./db/clients_ip_port_id.txt";

    //Check if the Database directory existes, if doesn't: create
    struct stat st = {0};
    if (stat("./db", &st) == -1) {
        mkdir("./db", 0700);
    }

    //Check if the file Client_ID existes, if doesn't: create
    if((fp = fopen(fname, "r")) == NULL) {
        id = 1;
		fp = fopen(fname, "w+");
        fprintf(fp, "%-15s %-5s %-5s\n", "IP", "PORT", "ID");       //write a header in the file
        fprintf(fp, "%-15s %-5d %-5d\n", ip, port, id);             //add the new information
        newuser = 1;                                                //flag to indicate new client
    }else{
        char filestr[31];                                           //string with the file's line
        char* wantedstr;                                            //"wanted string"
        char* pointer;                                              //pointer of the filestr in the file

        wantedstr = malloc(31);                                     //allocate temporally memory for the str
        sprintf(wantedstr, "%-15s %-5d ", ip, port);                //build string to look for
        fgets(filestr, 30, fp);                                     //read the first line of the file

        while (!feof(fp)){                                          //while not reach the final of the file
            if((pointer = (strstr(filestr, wantedstr))) != NULL){   //compare substring in the current line
                id = atoi(pointer+strlen(wantedstr));               //convert ID from char type to int
                newuser = 0;                                        //flag to indicate that is not a new client (redundance)
                break;                                              //stop to scan file
            } //end IF compare strings
            nline++;                                                //id should be equal to (nline-1)
            fgets(filestr, 30, fp);                                 //check next line
        } //end WHILE sweep line per line
        free(wantedstr);                                            //release the memory

        //If id still equal to zero, means that it was not found, so it is a new connection
        if(!id){
            id = nline;                                             //id should be equal to nline
            fp = fopen(fname, "a");                                 //allow to write a new line
            fprintf(fp, "%-15s %-5d %-5d\n", ip, port, id);         //add new client to the file
            newuser = 1;                                            //flag to indicate new client
        } //end IF attribute a new ID for the new Client

    } //end ELSE treat <ip,port> for client_id file already existent
    fclose(fp);                                                     //close external file


    return id;
} //end searchClientID()



//Recover client nickname
unsigned short searchClientNAME (unsigned int id, char nickname[]){
    unsigned short flag = 0;                                        //0: unsuccessful    1: success
    int nline = 0;                                                  //line number
    FILE *fp;
    const char *fname = "./db/client_name.txt";

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat("./db", &st) == -1) {
        PERROR("search_name() folder 'db' not found");
    }

    //Check if the file Client_NAME existes, if doesn't: inform
    if((fp = fopen(fname, "r")) == NULL) {
        flag = 0;                                                   //name need to be saved
    }else{
        char filestr[127];                                          //string with the file's line
        char* wantedstr;                                            //"wanted string"
        char* pointer;                                              //pointer of the filestr in the file
        size_t n = 0;                                               //store nickname length

        wantedstr = malloc(7);                                      //allocate temporally memory for the str
        sprintf(wantedstr, "%05d ", id);                            //build string to look for
        fgets(filestr, 128, fp);                                    //read the first line of the file

        while (!feof(fp)){                                          //while not reach the final of the file
            if((pointer = (strstr(filestr, wantedstr))) != NULL){   //compare substring in the current line
                n = strlen(filestr) - strlen(wantedstr) - 1;        //n = how many characters the name has
                strncpy(nickname, pointer+strlen(wantedstr), n);    //get the name in front of the id number
                flag = 1;                                           //indicates that the name was found
                break;                                              //stop to scan file
            } //end IF compare strings
            nline++;                                                //id should be equal to (nline-1)
            fgets(filestr, 128, fp);                                //check next line
        } //end WHILE sweep line per line
        free(wantedstr);                                            //release the memory
        fclose(fp);                                                 //close external file
   } //end ELSE file existes


    return flag;
} //end searchClientNAME()



//Save client nickname
void saveClientNAME (unsigned int id, char nickname[]){
    FILE *fp;
    const char *fname = "./db/client_name.txt";

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat("./db", &st) == -1) {
        PERROR("save_name() folder 'db' not found");
    }

    //Check if the file Client_NAME existes, if doesn't: create
    if((fp = fopen(fname, "r")) == NULL) {
        fp = fopen(fname, "w+");
        fprintf(fp, "%-5s %s\n", "ID", "NAME");                 //write a header in the first line
        fprintf(fp, "%05d %s\n", id, nickname);                 //add the new information to the file
    }else{
        fp = fopen(fname, "a");                                 //allow to write a new line
        fprintf(fp, "%05d %s\n", id, nickname);                 //add new client to the file
    } //end ELSE just add the new information to the file
    fclose(fp);                                                 //close external file

} //end saveClientNAME()



//Scan Clients Registered for the Home Page
void scanClientREGISTERED_byID (unsigned int id, char buffer[]){
    FILE *fp;
    const char *fname = "./db/client_name.txt";

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat("./db", &st) == -1) {
        PERROR("scan_name() folder 'db' not found");
    }

    //Check if the file Client_NAME existes, if doesn't: fatal error
    if((fp = fopen(fname, "r")) == NULL) {
        PERROR("scan_name() folder 'client_name' not found");
    }else{
        char filestr[128];                                          //string with the file's line
        char* wantedstr;                                            //"wanted string"
        size_t n = 0;

        wantedstr = malloc(7);                                      //allocate temporally memory for the str
        sprintf(wantedstr, "%05d ", id);                            //build string to look for
        fgets(filestr, 127, fp);                                    //read the first line of the file
        fgets(filestr, 127, fp);                                    //read the second line of the file

        while (!feof(fp)){                                          //while not reach the final of the file
            strcat(buffer, "\n    -> ");                            //add visual format in the string
            n = strlen(filestr+strlen(wantedstr)) - 1;
            strncat(buffer, filestr+strlen(wantedstr), n);          //add the names to the string
            if(strstr(filestr, wantedstr) != NULL){                 //compare if is the name of who request the list
                strcat(buffer, " (you)");                           //add the identifier "you, for self recognition"
            } //end IF compare strings
            fgets(filestr, 127, fp);                                //check next line
        } //end WHILE sweep line per line
        free(wantedstr);                                            //release the memory
        fclose(fp);                                                 //close external file
   } //end ELSE file existes

} //end searchClientNAME_byID()



//Scan Clients Registered for private message
unsigned int scanClientREGISTERED_byNAME (char nickname[]){
    unsigned int id = 0;
    FILE *fp;
    const char *fname = "./db/client_name.txt";

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat("./db", &st) == -1) {
        PERROR("scan_name() folder 'db' not found");
    }

    //Check if the file Client_NAME existes, if doesn't: fatal error
    if((fp = fopen(fname, "r")) == NULL) {
        PERROR("scan_name() folder 'client_name' not found");
    }else{
        char filestr[128];                                          //string with the file's line
        char* wantedstr;                                            //"wanted string"

        wantedstr = malloc(64);                                     //allocate temporally memory for the str
        fgets(filestr, 127, fp);                                    //read the first line of the file, to skip the header
        while (!feof(fp)){                                          //while not reach the final of the file
            fscanf(fp, "%5u %s\n", &id, wantedstr);                 //read line per line
            if(!strcmp(wantedstr, nickname)){                       //compare if is the name requested
                free(wantedstr);                                    //release the auxiliary memory allocated
                fclose(fp);                                         //if found, close external file
                return id;                                          //return id
            } //end IF compare strings
        } //end WHILE sweep line per line
        free(wantedstr);                                            //release the memory
        fclose(fp);                                                 //close external file
   } //end ELSE file existes

   return 0;                                                        //if name not found return 0
} //end searchClientNAME_byNAME()



//Scan Clients Registered for the Home Page
unsigned int countUsersREGISTERED (void){
    unsigned int users_registered = 0;

    FILE *fp;
    const char *fname = "./db/client_name.txt";
    fp = fopen(fname, "r");
    char filestr[128];
    fgets(filestr, 127, fp);
    while (!feof(fp)){
        fgets(filestr, 127, fp);
        users_registered++;
    }
    users_registered--; //minus the header
    return users_registered;
} //end countUsersREGISTERED()



//Change or Create file with the location of the user inside the plataform
void saveLog (unsigned int id, char* msg){
    FILE *fp;
    char path[64];

    sprintf(path, "./log");

    //Check if the Database directory 'log' existes, if doesn't: create
    struct stat st1 = {0};
    if (stat((const char*)path, &st1) == -1) {
        mkdir(path, 0700);
    }

    sprintf(path, "./log/%u", id);

    //Check if the Database directory '#id' existes, if doesn't: create
    struct stat st2 = {0};
    if (stat((const char*)path, &st2) == -1) {
        mkdir(path, 0700);
    }

    current_time = time(NULL);
    c_time = *localtime(&current_time);
    char tempo[30];
    sprintf(tempo, "[%04d-%02d-%02d %02d:%02d:%02d] ", c_time.tm_year+1900, c_time.tm_mon+1, c_time.tm_mday, c_time.tm_hour, c_time.tm_min, c_time.tm_sec);


    strcat(path, "/messages.txt");

    fp = fopen(path, "a+");                                         //add or create file
    fprintf(fp, "%s%s", tempo, msg);                                //write the time of the message and the message
    fclose(fp);                                                     //close external file

} //end saveLog()



//Change or Create file with the location of the user inside the plataform
void updateClntRoom (unsigned int id, unsigned char room){
    FILE *fp;
    char path[64];

    sprintf(path, "./log/%u", id);

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        PERROR("update_room() folder '/log/id' not found");
    }

    strcat(path, "/current_room.txt");

    fp = fopen(path, "w+");                                         //write or create file
    fprintf(fp, "%u", room);                                        //just write the number of the room
    fclose(fp);                                                     //close external file
} //end updateClntRoom()



//Read the client current room
unsigned short searchClientLOCATION(unsigned int id){
    unsigned int room = 0xFF;

    FILE *fp;
    char path[64];
    sprintf(path, "./log/%u", id);

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        PERROR("read_room() folder '/log/id' not found");
    }

    strcat(path, "/current_room.txt");

    fp = fopen(path, "r");                                          //only read allow
    fscanf(fp, "%u", &room);                                        //just read the number of the room
    fclose(fp);                                                     //close external file

    return room;
} //end searchClientLOCATION()



//Save the messages written inside the rooms
void saveRoomLog (unsigned short idRoom, char* sender, char* msg, unsigned short flag){
    FILE *fp;
    char path[64];

    sprintf(path, "./rooms/%u", idRoom);

    //Check if the Database directory existes, if doesn't: fatal error
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        PERROR("saveRoomLog() folder '/rooms/#roomID' not found");
    }

    strcat(path, "/log.txt");

    current_time = time(NULL);
    c_time = *localtime(&current_time);
    char tempo[30];
    sprintf(tempo, "[%04d-%02d-%02d %02d:%02d:%02d] ", c_time.tm_year+1900, c_time.tm_mon+1, c_time.tm_mday, c_time.tm_hour, c_time.tm_min, c_time.tm_sec);


    fp = fopen(path, "a+");                                         //add or create file
    if(flag == _SYST){
        fprintf(fp, "\n\n%s%s", tempo, msg+9);                      //write when and event
    }else if(flag == _USER){
        fprintf(fp, "%s$(%s) %s", tempo, sender, msg);              //write when, who and what
    }
    fclose(fp);                                                     //close external file
} //end saveRoomLog()



//Insert client id in room register and inform who joined the room
void getInRoom(unsigned int id, unsigned short idRoom){
    FILE *fp;
    char path[50];
    sprintf(path, "./rooms/%u/clients_connected.txt", idRoom);      //file with the client's id inside the room
    fp = fopen(path, "a+");                                         //add or create file
    fprintf(fp, "%u\n", id);
    fclose(fp);


    //Inform other users who enter the room
    char name[64]; HandleClientNAME(id, name);                      //get the name just for inform other users
    char localInform[128];
    sprintf(localInform, "\n\n[info] *%s* joined the room\n\n", name);
    roomMessage(id, name, idRoom, localInform, _SYST);
} //end getInRoom()



//Delete the ID client from the list of who is inside the room
void getOutRoom(unsigned int id, unsigned short idRoom){
    if(idRoom != __HOMEPAGE){                                       //if client was in homepage: nothing to do
        unsigned int ids_read[500];                                 //store the id's read from the file
        unsigned int i=0;                                           //walk in the array ids_read[]


        FILE *fp;
        char path[50];
        sprintf(path, "./rooms/%u/clients_connected.txt", idRoom);  //file with the client's id inside the room

        fp = fopen(path, "r");                                      //read file
        while (!feof(fp)){                                          //read ALL file
            fscanf(fp, "%u\n", &ids_read[i++]);                     //store in the vector
        }
        fclose(fp);


        fp = fopen(path, "w");                                      //write file
        char aux[2];
        memset(aux, '\0', strlen(aux));
        fprintf(fp, "%s", aux);
        fclose(fp);

        if(i>1){
            fp = fopen(path, "a");                                      //write file
            for(uint j=0; j<i; j++){
                if(ids_read[j] != id){
                    fprintf(fp, "%u\n", ids_read[j]);
                }
            }
            fclose(fp);
        }



        //Inform other users who leaved the room
        char name[64]; HandleClientNAME(id, name);                  //get the name just for inform other users
        char localInform[128];
        sprintf(localInform, "\n\n[info] *%s* leaved the room\n\n", name);
        roomMessage(id, name, idRoom, localInform, _SYST);
    } //end IF was in another room before
} //end getOutRoom()



//Scan Clients Registered in the Room for the 'Welcome Room Page'
void scanRoomClients_byID (unsigned int id, unsigned short idRoom, char buffer[]){
    unsigned int id_reg;                                                //receive the id read from clients_connected.txt
    FILE *f_general, *f_room;
    const char *fname_all = "./db/client_name.txt";                     //file with the name of all clients
    char* fname_room = malloc(50);
    sprintf(fname_room, "./rooms/%u/clients_connected.txt", idRoom);    //file with the client's id inside the room

    unsigned int id_read;                                               //id read from the general file
    char nick_read[64];                                                 //name read from the file
    char filestr[127];                                                  //string with the file's line
    //Sweep all file "clients_connected.txt"
    f_room = fopen(fname_room, "r");                                    //read only
    while (!feof(f_room)){                                              //until the end of the file
        fscanf(f_room, "%u\n", &id_reg);                                //read line per line all ids registered
        //Sweep file "client_name.txt" search for the user's name
        f_general = fopen(fname_all, "r");                              //read only - Guarantees a reading started at the beginning
        fgets(filestr, 127, f_general);                                 //read the first line of the file (skip the header)
        while (!feof(f_general)){                                       //until (maximum) the end of the file
            fscanf(f_general, "%05u %s\n", &id_read, nick_read);        //read line per line (until find the user)
            if(id_read == id_reg){                                      //if user found
                strcat(buffer, "\n    -> ");                            //add some visual formatation
                strcat(buffer, nick_read);                              //add name into the string
                if(id == id_reg){                                       //compare if is the name of who request the list
                    strcat(buffer, " (you)");                           //add the identifier "you", for self recognition
                } //end IF it is who requested the message
                fclose(f_general);                                      //close info file
                break;                                                  //finish here to see if there is more people in the room
            } //end IF id in INFO file found
        } //end WHILE it is not the end of the INFO file
    } //end WHILE it is not the end of the CONNECTED USERS TO THE ROOM file
    fclose(f_room);                                                     //close conn file
    free(fname_room);                                                   //release memory

} //end scanRoomClients_byID()
