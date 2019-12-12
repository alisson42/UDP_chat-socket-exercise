##This is a simplified version of a chat using UDP protocol, developed by
	Alisson Rodrigues de Paula


##General Instructions:
	#in the Ubuntu Terminal, access the folder of the project (using 'cd'), wich has two sub-directories:
		- Client
		- Server

	#inside the directory -Server- use the commands:
		make
		./chatserver &

	the command "make" will compile all *.c and *.h files, and the command "./chatserver" will execute the server. Since the server doesn't show (print) anything on the screen, it can run in background (&).

	#inside the directory -Client- use the commands:
		gcc chat_client-UDP.c -o client -pthread
		./client

	the first command line will generate the executable of the client and the second will run it
	However, to "print" any message on the Terminal screen it is necessary to jump a line in the end of each message, what breaks the visual formatation of the chat. So we suggest to run the Client(s) using the Ubuntu-Terminal command:
		nc -u localhost 1234

	"nc -u" will guarantee a UDP connection, "localhost" is the internal IP of the computer (127.0.0.1) and "1234" is the fixed port used by our Server (random choice of the developers).
