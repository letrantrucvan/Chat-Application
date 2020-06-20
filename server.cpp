//#define WIN32_LEAN_AND_MEAN
//
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <iostream>
//#include <string.h>
//#include <string>
//#include <cstring>
//#include <vector>
//#include <thread>
//#include <fstream>
//#include <conio.h>
//// Need to link with Ws2_32.lib
//#pragma comment (lib, "Ws2_32.lib")
//// #pragma comment (lib, "Mswsock.lib")
#include "server.h"

using namespace std;

#define LenMsg 1024
#define PORT "8080" // nè
#define NumClient 100 // để đi có ảnh huong dau
#define FILE_LENGHT 1024*1024*5
//
//struct User
//{
//	string Id;
//	string Password;
//};
//vector <User>  UserList;
//vector <bool> isExisted;
//
//SOCKET ClientList[NumClient];
//thread ClientThread[NumClient];
//int count = 0;
//
////---HAM HO TRO
//string FormatStr(char*& buffer, int& mode, string userName);
//bool checkUser(string name); //check trong vector UserList - luu nhung user dang online
//int checkUser(char Id[], char Pass[]); //check trong file UserList.txt
//void getUsername(int index);
//void sendUserList(int index);
//void run(int index);
//void start();

void server::getUsername(int index)
{
	count++;
	User newUser;
	UserList.push_back(newUser);
	char id[30], password[30];
	char buffname[50];
	isExisted.push_back(FALSE);
	//bool check = TRUE;
	// Save data in Server
	int flag = 0; 
	do {
		// receive name and password from client to check 
		int iResult = recv(ClientList[index], id, 30, 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ClientList[index]);
			WSACleanup();
			//return 1;
		}
		iResult = recv(ClientList[index], password, 30, 0);

		int Check = checkUser(id, password); // check if the username is available
		send(ClientList[index], (char*)&Check, sizeof(int), 0);
		// recv thong bao dang nhap thanh cong + mode dnhap / dky
		recv(ClientList[index], (char*)&flag, sizeof(int), 0);

	} while (flag == 0);

	if (flag == 2) //SignUP, nen mo file txt ra ghi thong tin User moi dang ky vo
	{
		ofstream ofs("UserList.txt", ios::app);
		do
		{
			if (checkUser(id, password) == 2 && ofs.is_open())
			{
				ofs << id << endl;
				ofs << password << endl;
				break;
			}
		} while (checkUser(id, password) != 2);
		ofs.close();
	}
	string temp = id;
	temp = temp + " logged in" + "\0";
	cout << temp << endl;

	UserList[index].Id = id;
	isExisted[index] = TRUE;

	for (int i = 0; i < temp.length(); i++)  //copy lai buff vao buffer
	{
		buffname[i] = temp[i];
	}
	buffname[temp.length()] = '\0';

	// Send username to the other clients
	for (int i = 0; i < count; i++)
	{
		if (i == index) continue;
		if (isExisted[i])
		{
			int iResult = send(ClientList[i], buffname, 50, 0);
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
bool server::checkUser(string name)
{
	for (int i = 0; i < UserList.size(); i++)
	{
		if (UserList[i].Id == name)
			return true;
	}
	return false;
}
void server::run(int index)
{
	index = index - 1;
	getUsername(index);
	//count++;
	printUserList();
	sendUserList(index);
	char* temp = new char[LenMsg];
	//receive(, msg);
	int itemp = recv(ClientList[index], temp, LenMsg, 0);
	while (1)
	{
		int mode;
		char* buffer = new char[LenMsg];
		//receive(, msg);
		int iResult = recv(ClientList[index], buffer, LenMsg, 0);

		if (iResult == SOCKET_ERROR)
		{
			*buffer = '#';
			string name = FormatStr(buffer, mode, UserList[index].Id);
			cout << buffer << endl; 
			for (int i = 0; i < count; i++)
			{
				if (ClientList[i] == ClientList[index]) continue;
				if (isExisted[i] == TRUE)
				{
					iResult = send(ClientList[i], buffer, LenMsg, 0);
				}
			}
			// close socket
			isExisted[index] = FALSE;
			printUserList();
			closesocket(ClientList[index]);
			return;
		}

		string name = FormatStr(buffer, mode, UserList[index].Id);
		cout << buffer << endl;
		// khi nhan # log out
		if (mode == 2)
		{
			//xoa khoi 
			//gui cho nhung client khac
			for (int i = 0; i < count; i++)
			{
				if (ClientList[i] == ClientList[index]) continue;
				if (isExisted[i] == TRUE)
				{
					iResult = send(ClientList[i], buffer, LenMsg, 0);
				}
			}
			// close socket
			isExisted[index] = FALSE;
			printUserList();
			closesocket(ClientList[index]);
			return;
		}

		// neu chat all thi gui cho may thang con lai
		else if (mode == 0)
		{
			for (int i = 0; i < count; i++)
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
			for (int i = 0; i < count; i++)
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
		else if (mode == 5) //nhan va gui lai FILE
		{
			int size;
			recv(ClientList[index], (char*)&size, sizeof(int), 0); //nhan file size
			char* filebuf = new char[size];
			recv(ClientList[index], filebuf, size, 0); //nhan noi dung file duoi dang binary

			for (int i = 0; i < count; i++)
			{
				if (ClientList[i] == ClientList[index]) continue;
				if (isExisted[i] == TRUE)
				{
					send(ClientList[i], buffer, LenMsg, 0); //gui ten file
					send(ClientList[i], (char*)&size, sizeof(int), 0); //gui file size
					send(ClientList[i], filebuf, size, 0); //gui noi dung file
					delete[] filebuf;
				}
			}
		}
		delete[] buffer;
	}
	delete[] temp;

}

string server::FormatStr(char*& buffer, int& mode, string userName)
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
			buff = userName + ": " + "/ " + name + buff;
			mode = 0;
			name = "ALL";
		}
	}
	else if (buff[0] == '#')	// log out		
	{
		buff = userName + " logged out";
		mode = 2;
	}
	else if (buff[0] == '+')  // +D:\\client\\test  //FILE
	{
		buff.erase(buff.begin());
		buff = "+" + userName + " sent file " + "[" + buff + "]";  //+client A send file [D:\\client\\test.txt]
		mode = 5;
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


int server::checkUser(char Id[], char Pass[])
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
	fin.close();
	return 2;
}

void server::sendUserList(int index)
{
	string user = "";
	for (int i = 0; i < UserList.size(); i++)
	{
		if (isExisted[i])
		{
			user += UserList[i].Id;
			user += " ";
		}
	}
	user.erase(user.end());
	user += '\0';
	char* result = new char[user.length() + 1];
	for (int i = 0; i < user.length(); i++)
	{
		result[i] = user[i];
	}
	result[user.length()] = '\0';
	send(ClientList[index], result, user.length() + 1, 0); //gui ten cac onlineUser
}
int server::start()
{
	WSADATA wsaData;
	int iResult;

	SOCKET Client = INVALID_SOCKET;
	SOCKET Server = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;


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
			//run(i);
			//ClientThread[i] = thread(&server::run, i);
			//std::thread newThread([&](server* client) { client->run(i); }, this);
			ClientThread[i]= thread([&](server* client) { client->run(i); }, this);

		}
	}

	// cleanup
	//closesocket(Client);
	closesocket(Server);
	WSACleanup();

	return 0;
}

void server::gotoxy(int x, int y)
{
	static HANDLE h = NULL;
	if (!h)
	{
		h = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	COORD c = { x,y };
	SetConsoleCursorPosition(h, c);
}
void server::printUserList()
{
	int a = wherex();
	int b = wherey();
	int x = 50, y = 3;
	gotoxy(x, y);
	cout << "Online Users:";
	for (int i = 0; i < UserList.size(); i++)
	{
		y++;
		gotoxy(x, y);
		cout << "                 ";
	}
	y = 4;
	for (int i = 0; i < UserList.size(); i++)
	{
		if (isExisted[i])
		{
			gotoxy(x, y);
			cout << UserList[i].Id;
			y++;
		}
	}
	gotoxy(a, b);
}

int server::wherex()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;
	return csbi.dwCursorPosition.X;
}

int server::wherey()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
		return -1;
	return csbi.dwCursorPosition.Y;
}