//Brandon Duncan
//CSC 3350
//Lab 6 - IPC Socket Talk - TalkClient.exe

#define  HOSTNAMELEN   128 

#include <winsock2.h> 
#include <string.h>
#include <windows.h>
#include <winbase.h>
#include <stdio.h>
#include <conio.h>

int main(int argc, char *argv[])
{
	WORD wVersionRequested;  WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);  // Version 2.0  
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		fprintf(stderr, "Process failed on WinSock startup\n");
		ExitProcess(0);
	};

	char serverName[HOSTNAMELEN];
	char *x;
	int portNum;
	struct hostent  *host; //Create host

	//Check to see if a hostname was given
	if (argc == 3)
	{
		//Hostname given
		host = gethostbyname(argv[2]);
		portNum = strtol(argv[1], &x, 10);
	}
	else
	{
		//Hostname not specified, get local hostname
		int result = gethostname(serverName, HOSTNAMELEN);
		host = gethostbyname(serverName);
		portNum = strtol(argv[1], &x, 10);
	}
	

	SOCKADDR_IN  theSrvr; 
	ZeroMemory(&theSrvr, sizeof(SOCKADDR_IN));   
		theSrvr.sin_family = AF_INET;    
		theSrvr.sin_port = htons((u_short)portNum);    
		CopyMemory(&theSrvr.sin_addr,     // Host          
			host->h_addr_list[0], host->h_length); 

	//Create client socket
	SOCKET ConnSkt = socket(AF_INET, SOCK_STREAM, 0);
	if (ConnSkt == INVALID_SOCKET)
	{
		printf("Error: Could not create socket : %d", WSAGetLastError());
	}

	//Make connection request
	int result = connect(ConnSkt, (const SOCKADDR *)&theSrvr, sizeof(theSrvr));

	printf("Connection Success!\n\nPlease type a message and hit enter to send it. Type 'exit' to quit.\n");
	//****************Communication Implementation*******************
	int flags = 0;
	const int bufferSize = 500;
	char  buffer[bufferSize];
	bool check = true;	//Set socket to not block
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

