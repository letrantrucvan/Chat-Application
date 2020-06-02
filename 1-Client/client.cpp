#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <mutex>

#pragma comment (lib, "Ws2_32.lib")

#define LenMsg 1024
#define PORT "8080"
std::mutex m;

using namespace std;

struct User
{
	char Id[30] = { 0 };
	char Password[30] = { 0 };
};

int main()
{
	char* buffer = new char[LenMsg + 1];
	int iResult;
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;

	//Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	SOCKET Server = INVALID_SOCKET;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	// Attempt to connect to the first address returned by
// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	Server = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if (Server == INVALID_SOCKET)
	{
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Connect to server.
	iResult = connect(Server, ptr->ai_addr, (int)ptr->ai_addrlen);

	if (iResult == SOCKET_ERROR) 
	{
		closesocket(Server);
		Server = INVALID_SOCKET;
		return 1;
	}
	freeaddrinfo(result);

	if (Server == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	// shutdown the connection since no more data will be sent
	iResult = shutdown(Server, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(Server);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {
		iResult = recv(Server, buffer, LenMsg, 0);
		char sendMsg[LenMsg];

		if (iResult > 0)
		{
			int len = strlen(buffer);
			buffer[len] = '\0';
			cout << "Server: " << buffer << endl;
			// Client turn
			cout << "Client: ";
			cin.getline(sendMsg, 1024);
			int iSendResult = send(Server, sendMsg, iResult, 0);

			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(Server);
				WSACleanup();
				return 1;
			}
		}
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (recv(Server, buffer, LenMsg, 0) > 0);

	// cleanup
	closesocket(Server);
	WSACleanup();

	return 0;
}

