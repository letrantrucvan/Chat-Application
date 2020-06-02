#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define LenMsg 1024
#define PORT "8080"

using namespace std;

int main()
{
	WSADATA wsaData;
	int iResult;

	SOCKET Client = INVALID_SOCKET;
	SOCKET Server = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	Server = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (Server == INVALID_SOCKET)
	{
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(Server, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(Server);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(Server, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(Server);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	Client = accept(Server, NULL, NULL);
	if (Client == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(Server);
		WSACleanup();
		return 1;
	}
	char* buffer = new char[LenMsg + 1];

	// Receive until the peer shuts down the connection
	do {
		char sendMsg[LenMsg];
	
		// Server turn
		cout << "Server: ";
		cin.getline(sendMsg, 1024);
		iSendResult = send(Client, sendMsg, iResult, 0);

		if (iSendResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(Client);
			WSACleanup();
			return 1;
		}

		iResult = recv(Client, buffer, LenMsg, 0);
		int len = strlen(buffer);
		buffer[len] = '\0';
		cout << " Client: " << buffer << endl;

		
	} while (recv(Client, buffer, LenMsg, 0) > 0);
			
	// cleanup
	closesocket(Client);
	closesocket(Server);
	WSACleanup();

	return 0;
}