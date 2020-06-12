#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <cstring>
#include <vector>
#include <thread>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define LenMsg 1024
#define PORT "8080"
#define NumClient 100
using namespace std;

struct User
{
	string Id;
	string Password;
};
vector <User>  UserList;
vector <bool> isExisted;

SOCKET ClientList[NumClient];
thread ClientThread[NumClient];
int count = 0;
//---HAM HO TRO
string FormatStr(char*& buffer, int& mode, string userName);
bool checkUser(string name);
void getUsername(int index);
void run(int index);

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

	cout << "Server init successfully" << endl;
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
	cout << "Looking for clients ... " << endl;
	//int count = 0;
	// Accept a client socket
	for (int i = 0; i < NumClient; i++)
	{
		SOCKET Client;
		Client = accept(Server, NULL, NULL);  //ham accept tra ve 1 SOCKET moi, ket noi client va server
		if (Client == INVALID_SOCKET)
		{
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(Server);
			WSACleanup();
			return 1;
		}
		else
		{
		
			ClientList[i] = Client;
			
			
			ClientThread[i] = thread(run, i);
			
		}
	}
	
	// cleanup
	//closesocket(Client);
	closesocket(Server);
	WSACleanup();

	return 0;
}

void getUsername(int index)
{
	User newUser;
	char name[30];
	char buffname[50];
	bool check = TRUE;
	// Save data in Server
	int iResult = recv(ClientList[index], name, 30, 0);
	string temp = name;
	temp = temp + " logged in" + "\0";
	cout << temp << endl;
	newUser.Id = name;

	UserList.push_back(newUser);
	isExisted.push_back(check);

	for (int i = 0; i < temp.length(); i++)  //copy lai buff vao buffer
	{
		buffname[i] = temp[i];
	}
	buffname[temp.length()] = '\0';

	// Send username to the other clients
	for (int i = 0; i < ::count; i++)
	{
		if (i == index) continue;
		if (isExisted[i])
		{
			iResult = send(ClientList[i], buffname, 50, 0);
			if (iResult == SOCKET_ERROR)
			{
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientList[index]);
				WSACleanup();
				return;
			}
		}
	}

}
bool checkUser(string name)
{
	for (int i = 0; i < UserList.size(); i++)
	{
		if (UserList[i].Id == name)
			return true;
	}
	return false;
}
void run(int index)
{
	
	getUsername(index);
	::count++;

	char* temp = new char[LenMsg];
	int iResult = recv(ClientList[index], temp, LenMsg, 0);
	while (1)
	{
		int mode;
		char* buffer = new char[LenMsg];
		//receive(, msg);
		int iResult = recv(ClientList[index], buffer, LenMsg, 0);

		if (iResult == SOCKET_ERROR)
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientList[index]);
			WSACleanup();
			return;
		}

		string name = FormatStr(buffer, mode, UserList[index].Id);
		cout << buffer << endl;
		// khi nhan # log out
		if (mode == 2)
		{
			//gui cho nhung client khac
			for (int i = 0; i < ::count; i++)
			{
				if (ClientList[i] == ClientList[index]) continue;
				if (isExisted[i] == TRUE)
				{
					iResult = send(ClientList[i], buffer, LenMsg, 0);
				}
			}
			// close socket
			isExisted[index] = FALSE;
			closesocket(ClientList[index]);
			return;
		}

		// neu chat all thi gui cho may thang con lai
		else if (mode == 0)
		{
			for (int i = 0; i < ::count; i++)
			{
				if (ClientList[i] == ClientList[index]) continue;
				if (isExisted[i] == TRUE)
				{
					iResult = send(ClientList[i], buffer, LenMsg, 0);
				}
			}
		}

		//chat privately thì gui cho thang nhan
		else if (mode == 1)
		{
			for (int i = 0; i < ::count; i++)
			{
				if (UserList[i].Id == name)
				{
					if (isExisted[i] == TRUE)
					{
						iResult = send(ClientList[i], buffer, LenMsg, 0);
					}
				}
			}
		}
		delete[] buffer;
	}
	delete[] temp;
	
}

string FormatStr(char*& buffer, int& mode, string userName)
{
	string name;
	string buff = buffer;

	if (buff[0] == '/')	// Chat private						
	{
		// buff = "/hoangminh hello"
		buff.erase(buff.begin());  // buff = "hoangminh hello"
		for (int i = 0; i < buff.length(); i++)
		{
			if (buff[i] == ' ')
			{
				name = buff.substr(0, i); // name = "hoangminh"
				buff.erase(0, i + 1);		  // buff = "hello"
				break;
			}
		}
		if (checkUser(name))
		{
			buff = userName + " to " + name + " (privately): " + buff;
			mode = 1;
		}
		else
		{
			buff = userName + ": " + "/" + name + buff;
			mode = 0;
			name = "ALL";
		}
	}
	else if (buff[0] == '#')	// log out		
	{
		buff = userName + " logged out.";
		mode = 2;
	}
	else  //chat chung ca dam
	{
		// buff = "Xin chao cac ban!"
		name = "ALL";
		buff = userName + ": " + buff;
		mode = 0;
	}

	for (int i = 0; i < buff.length(); i++)  //copy lai buff vao buffer
	{
		buffer[i] = buff[i];
	}
	buffer[buff.length()] = '\0';
	return name; // neu la chat private (A chat cho B) thi tra ve ten thang B, chat all thi tra ve "ALL"
}
