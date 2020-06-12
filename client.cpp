#define WIN32_LEAN_AND_MEAN

#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <conio.h>
#include <fstream>

#pragma comment (lib, "Ws2_32.lib")

#define LenMsg 1024
#define PORT "8080"
//std::mutex m;

using namespace std;


bool SignIn();
void SignUp();
int checkUser(char Id[], char Pass[]);
void receiving(SOCKET Server);
void sending(SOCKET Server);

struct User
{
	char Id[30] = { 0 };
	char Password[30] = { 0 };
};
User user;
int main()
{
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
	//iResult = shutdown(Server, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(Server);
		WSACleanup();
		return 1;
	}

	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(Server);
		WSACleanup();
		return 1;
	}

	int choose;
	cout << "1: Sign Up | 2: Log In " << endl;
	cin >> choose;

	if (choose == 1){
		SignUp();
		if (SignIn()) cout << endl << "Login successfully !" << endl;
		else cout << endl << "Login failed !";
	}
	else{
		if (SignIn()) cout << endl << "Login successfully !" << endl;
		else cout << endl << "Login failed !";
	}
	
	//string temp = user.Id;
	int len = strlen(user.Id);
	user.Id[len] = '\0';
	iResult = send(Server, user.Id, 30, 0);

	thread send(sending, Server);
	thread receive(receiving, Server);
	if (send.joinable())
		send.join();
	if (receive.joinable())
		receive.join();

	// cleanup
	closesocket(Server);
	WSACleanup();
	system("pause");
	return 0;
}

int checkUser(char Id[], char Pass[])
{
	ifstream fin("UserList.txt", ios::in);

	string id, pass;

	if (!fin.is_open())
	{
		cout << "Open file UserList.txt failed" << endl;
		return -1;
	}

	while (!fin.eof())
	{
		getline(fin, id);
		getline(fin, pass);
		if (Id == id && Pass != pass) // check if username is available
			return 0;
		else if (Id == id && Pass == pass) //  check if username and password are correct 
			return 1;
	}

	return 2;

	fin.close();
}

void SignUp()
{
	ofstream ofs("UserList.txt", ios::app);
	do
	{
		cout << "_____SIGN UP_____" << endl;
		cout << "Enter ID: ";
		cin >> user.Id;
		cout << "Enter Password: ";
		cin >> user.Password;
		
		if (checkUser(user.Id, user.Password) == 2 && ofs.is_open())
		{
			ofs << user.Id << endl;
			ofs << user.Password << endl;
			cout << "Sign Up successfully " << endl;
			break;
		}
		cout << "Username is already taken. Please try another " << endl;
		
	} while (checkUser(user.Id, user.Password) != 2);

	ofs.close();
}

bool SignIn()
{
	int flag = 0;
	do {
		char input = 0;
		cout << "_____LOGIN_____" << endl;
		//Nhap ID
		int i = 0;
		cout << "Enter ID:";
		fflush(stdin);
		cin >> user.Id;
		//cout << strlen(Id);
		user.Id[strlen(user.Id)] = '\0';
		//Nhap Pass
		i = 0;
		cout << "Enter password :";
		while (1) {
			input = _getch();
			if (input == 13) // dau Enter
				break;
			else if (input == 8) // dau Xoa
			{
				if (i > 0) {
					i--;
					user.Password[i] = '\0';
					cout << "\b" << " " << "\b";
				}
			}
			else
			{
				user.Password[i] = input;
				i++;
				cout << "*";
			}
		}
		user.Password[i] = '\0';
		//cout << endl << strlen(Pass);

		if (checkUser(user.Id, user.Password) == 1)
		{
			return true;
		}

		else
		{
			cout << "\n\n The username or password you entered is invalid ! \nYou have 3 times to try again. If incorrect, the program is exit. " << endl;
			flag++;
			Sleep(3000);
			system("cls");
			cout << "\t___Trial " << flag << endl;
		}

	} while (flag < 4);

	return false;

}

void receiving(SOCKET Server)
{
	char* buffer = new char[LenMsg];
	int iResult;
	while (true)
	{
		iResult = recv(Server, buffer, LenMsg, 0);
		if (iResult == SOCKET_ERROR)
		{
			//printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(Server);
			WSACleanup();
			return;
		}

		cout << buffer << endl;

		if (*buffer == '#')
		{
			cout << "Chat ended by server." << endl;
			break;
		}

	
	}
	delete[] buffer;
}
void sending(SOCKET Server)
{
	char sendMsg[LenMsg];
	*sendMsg = '0';
	int lenMsg;
	int iResult;
	while (true)
	{
		// client nhan tin nhan cho server 

		//cout << "Client: ";
		cin.getline(sendMsg, 1024);
		lenMsg = strlen(sendMsg);
		sendMsg[lenMsg] = '\0';
		iResult = send(Server, sendMsg, sizeof(sendMsg), 0);

		if (*sendMsg == '#')
		{
			cout << "Disconnected..." << endl;
			closesocket(Server);
			WSACleanup();
			return;
		}
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(Server);
			WSACleanup();
			return;
		}
	}

}
