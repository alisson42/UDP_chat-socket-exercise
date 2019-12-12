#include "UDPchatServer.h"



// Create server socket() and bind()
int CreateUDPServerSocket(unsigned short port){
	int servSockt;                                     // socket to create
	struct sockaddr_in udpServAddr;                    // Local address

	// Create socket for incoming connections //
	if ((servSockt = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
		PERROR("socket() failed");
	}

	// Construct local address structure //
	memset(&udpServAddr, 0, sizeof(udpServAddr));      // Zero out structure
	udpServAddr.sin_family = AF_INET;                  // Internet address family
	udpServAddr.sin_addr.s_addr = htonl(INADDR_ANY);   // Any incoming interface
	udpServAddr.sin_port = htons(port);                // Local port

	// Bind to the local address //
	if (bind(servSockt, (struct sockaddr *) &udpServAddr, sizeof(udpServAddr)) < 0){
		PERROR("bind() failed");
	}

	printf ("Chat server - UDP: binding on port %d\n", port);

	return servSockt;
} //end CreateUDPServerSocket()




void Transmit(char* data, struct sockaddr_in *ClntAddrStruct, unsigned short show_time, unsigned short show_room){
	ssize_t dataSize;

	if(show_time){
		current_time = time(NULL);
	    c_time = *localtime(&current_time);
		char tempo[30];
		sprintf(tempo, "[%04d-%02d-%02d %02d:%02d:%02d] ", c_time.tm_year+1900, c_time.tm_mon+1, c_time.tm_mday, c_time.tm_hour, c_time.tm_min, c_time.tm_sec);

		dataSize = strlen(tempo)+1;
		if(sendto(servSock, tempo, strlen(tempo)+1, 0, (struct sockaddr *) *&ClntAddrStruct, sizeof(*ClntAddrStruct)) != dataSize)
			PERROR("sendto()#1 failed");
	} //end IF show time

	if(show_room){
		char room[64];
		switch(show_room){
			case __PixnetPG:
				sprintf(room, "[%s] ", "PIXNET");
				break;

			case __SportPAGE:
				sprintf(room, "[%s] ", "SPORTS");
				break;
		}

		dataSize = strlen(room)+1;
		if(sendto(servSock, room, strlen(room)+1, 0, (struct sockaddr *) *&ClntAddrStruct, sizeof(*ClntAddrStruct)) != dataSize)
			PERROR("sendto()#2 failed");
	} //end IF show room

	dataSize = strlen(data)+1;
	if(sendto(servSock, data, strlen(data)+1, 0, (struct sockaddr *) *&ClntAddrStruct, sizeof(*ClntAddrStruct)) != dataSize)
		PERROR("sendto()#3 failed");
} //end Transmit()
