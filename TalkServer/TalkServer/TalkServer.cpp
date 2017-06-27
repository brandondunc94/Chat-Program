//Brandon Duncan
//CSC 3350
//Lab 6 - IPC Socket Talk - TalkServer.exe

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define  HOSTNAMELEN   128 

#include <winsock2.h> 
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winbase.h>
#include <conio.h>


int main()
{

	WORD wVersionRequested;  WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);  // Version 2.0  
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		fprintf(stderr, "Process failed on WinSock startup\n");
		ExitProcess(0);
	};

	//Get hostname
	char  serverHostName[HOSTNAMELEN];  // ASCIIZ c-string  
	int result = gethostname(serverHostName, HOSTNAMELEN);

	//Create Socket
	SOCKET skt = socket(AF_INET, SOCK_STREAM, 0);
	if (skt == INVALID_SOCKET)
	{
		printf("Error: Could not create socket : %d", WSAGetLastError());
	}

	//Get host name struct
	struct hostent  *host;
	host = gethostbyname(serverHostName);

	//Create Listener
	SOCKADDR_IN Listener;

	//Let bind select a port number
	u_short port = 0;

	// Configure Listener for this server host and port# 
	ZeroMemory(&Listener, sizeof(SOCKADDR_IN));
		Listener.sin_family = AF_INET;
		Listener.sin_port = htons((u_short)port);  // port # 
		CopyMemory(&Listener.sin_addr,     // Host     
			host->h_addr_list[0], host->h_length);

	//bind to socket
	int code = bind(skt, (const SOCKADDR *)&Listener, sizeof(Listener));

	//Get Listener info
	int len = sizeof(Listener);
	getsockname(skt, (SOCKADDR *)&Listener, &len);
	u_short realPort = ntohs(Listener.sin_port);	//Convert net to host short

	//Begin Listening
	printf("Port #: %d\n", realPort);
	printf("Hostname: %s\n\n", host->h_name);
	printf("Listening... ");
	listen(skt, SOMAXCONN);

	//Wait for client
	SOCKADDR_IN  Client;
	int ClientLen = sizeof(SOCKADDR_IN);
	SOCKET ConnSkt;
	//Accept connection request
	ConnSkt = accept(skt, (SOCKADDR *)&Client, &ClientLen);

	printf("Connection Success!\n\nPlease type a message and hit enter to send. Type 'exit' to quit.\n");

	//Close original listener socket
	closesocket(skt);

	//****************Communication Implementation*******************
	int flags = 0;
	const int bufferSize = 500;
	char  buffer[bufferSize];
	//Set socket to not block
	unsigned long  on = 1; 
	ioctlsocket(ConnSkt, FIONBIO, &on);
	
	while (TRUE)
	{
		if (_kbhit())
		{
			printf(">");
			gets_s(buffer, bufferSize);  // input entire line
			if (strcmp(buffer, "exit") == 0)
			{
				send(ConnSkt, buffer, strlen(buffer) + 1, flags);
				break;
			}
			send(ConnSkt, buffer, strlen(buffer) + 1, flags);
		}
		int nRead = recv(ConnSkt, buffer, bufferSize, flags);
		if (nRead > 0)	//Check to see if anything was received
		{
			buffer[nRead] = '\0';	//add NULL char at end
			if (strcmp(buffer, "exit") == 0 || strcmp(buffer, "exit\n") == 0)
			{
				break;
			}
			printf("<%s\n", buffer);	//print recieved message
		}
		Sleep(500); // Take a 500 ms nap, give the cpu a break
	}                  
	
	printf("Connection Lost. Exiting Program...\n");

	//Close connection socket
	closesocket(ConnSkt);
	WSACleanup();
	return 0;
}