//#define WIN32_LEAN_AND_MEAN

#pragma once
#include "client.h"
#include <iostream>
//#include <windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include <string>
//#include <thread>
//#include <mutex>
//#include <conio.h>
//#include <fstream>
//#include <vector>
//
//#pragma comment (lib, "Ws2_32.lib")

//std::mutex m;

using namespace std;

#define LenMsg 1024
#define FILE_LENGHT 1024*1024*5
#define PORT "8080"
//void LogIn(SOCKET Server);
//bool SignIn(SOCKET Server);
//bool SignUp(SOCKET Server);
//int checkUser(SOCKET Server, char Id[], char Pass[]);
//void receiving(SOCKET Server);
//void sending(SOCKET Server);
//void stringtochar(char*& result, string input);
//void recvUserList(SOCKET Server);
//struct User
//{
//	char Id[30] = { 0 };
//	char Password[30] = { 0 };
//};
//User user;
//vector<string> onlineUser;
//int main()
//{
//	int iResult;
//	WSADATA wsaData;
//	struct addrinfo* result = NULL, * ptr = NULL, hints;
//
//	//Initialize Winsock
//	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
//	if (iResult != 0)
//	{
//		printf("WSAStartup failed with error: %d\n", iResult);
//		return 1;
//	}
//
//	ZeroMemory(&hints, sizeof(hints));
//	hints.ai_family = AF_INET;
//	hints.ai_socktype = SOCK_STREAM;
//	hints.ai_protocol = IPPROTO_TCP;
//
//	SOCKET Server = INVALID_SOCKET;
//
//	// Resolve the server address and port
//	iResult = getaddrinfo(NULL, PORT, &hints, &result);
//	if (iResult != 0)
//	{
//		printf("getaddrinfo failed with error: %d\n", iResult);
//		WSACleanup();
//		return 1;
//	}
//	// Attempt to connect to the first address returned by
//	// the call to getaddrinfo
//	ptr = result;
//
//	// Create a SOCKET for connecting to server
//	Server = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
//	if (Server == INVALID_SOCKET)
//	{
//		printf("Error at socket(): %ld\n", WSAGetLastError());
//		freeaddrinfo(result);
//		WSACleanup();
//		return 1;
//	}
//
//	// Connect to server.
//	iResult = connect(Server, ptr->ai_addr, (int)ptr->ai_addrlen);
//
//	if (iResult == SOCKET_ERROR)
//	{
//		closesocket(Server);
//		Server = INVALID_SOCKET;
//		return 1;
//	}
//	freeaddrinfo(result);
//
//	if (Server == INVALID_SOCKET) {
//		printf("Unable to connect to server!\n");
//		WSACleanup();
//		return 1;
//	}
//
//	// shutdown the connection since no more data will be sent
//	//iResult = shutdown(Server, SD_SEND);
//	if (iResult == SOCKET_ERROR) {
//		printf("shutdown failed with error: %d\n", WSAGetLastError());
//		closesocket(Server);
//		WSACleanup();
//		return 1;
//	}
//
//	if (iResult == SOCKET_ERROR)
//	{
//		printf("send failed with error: %d\n", WSAGetLastError());
//		closesocket(Server);
//		WSACleanup();
//		return 1;
//	}
//
//	LogIn(Server);
//
//	thread send(sending, Server);
//	thread receive(receiving, Server);
//	if (send.joinable())
//		send.join();
//	if (receive.joinable())
//		receive.join();
//
//	// cleanup
//	closesocket(Server);
//	WSACleanup();
//	system("pause");
//	return 0;
//}

void Client::LogIn(SOCKET &Server)
{
	int choose;
	cout << "1: Sign Up | 2: Log In " << endl;
	cin >> choose;
	int flag;
	if (choose == 1) {
		if (SignUp(Server))
		{
			flag = 2; //gui flag = 2 de bao server dang ky thanh cong
			cout << endl << "Login successfully !" << endl;
		}
		else cout << endl << "Login failed !";
	}
	else {
		if (SignIn(Server))
		{
			flag = 1; //gui flag = 1 de server biet la dang nhap thanh cong
			cout << endl << "Login successfully !" << endl;
		}
		else cout << endl << "Login failed !";
	}

	int iResult = send(Server, (char*)&flag, sizeof(int), 0);
}

int Client::checkUser(SOCKET &Server, char Id[], char Pass[])
{
	int check = 0;
	int iResult = send(Server, user.Id, 30, 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		//closesocket(Server);
		WSACleanup();
		return 1;
	}

	iResult = send(Server, user.Password, 30, 0);
	iResult = recv(Server, (char*)&check, sizeof(int), 0);
	return check;
}
void Client::stringtochar(char*& result, string input)
{
	result = new char[input.length() + 1];
	for (int i = 0; i < input.length(); i++)
	{
		result[i] = input[i];
	}
	result[input.length()] = '\0';
}

bool Client::SignUp(SOCKET &Server)
{
	int check = 0;
	do
	{
		cout << "_____SIGN UP_____" << endl;
		cout << "Enter ID: ";
		fflush(stdin);
		cin >> user.Id;
		user.Id[strlen(user.Id)] = '\0';

		cout << "Enter Password: ";
		fflush(stdin);
		cin >> user.Password;
		user.Password[strlen(user.Password)] = '\0';

		check = checkUser(Server, user.Id, user.Password);

		if (check == 2)
		{
			cout << "Sign up successfully !" << endl;
			return true;
		}

		else cout << "Username is already taken. Please try another." << endl;
	} while (true);

	return false;
}

bool Client::SignIn(SOCKET &Server)
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

		if (checkUser(Server, user.Id, user.Password) == 1)
		{
			return true;
		}

		else
		{
			int fflag = 0;
			int iResult = send(Server, (char*)&fflag, sizeof(int), 0);
			cout << "\n\n The username or password you entered is invalid ! \nYou have 3 times to try again. If incorrect, the program is exit. " << endl;
			flag++;
			Sleep(3000);
			system("cls");
			cout << "\t___Trial " << flag << endl;
		}

	} while (flag < 4);

	return false;
}
void Client::recvUserList(SOCKET &Server)
{
	char* buffer = new char[LenMsg];
	int iResult = recv(Server, buffer, LenMsg, 0);
	string buf = buffer;
	string name;
	while (buf.length() != 0)
	{
		name = buf.substr(0, buf.find_first_of(' '));
		onlineUser.push_back(name);
		buf.erase(0, buf.find_first_of(' ') + 1);
	}
}

void Client::receiving(SOCKET &Server)
{
	char* buffer = new char[LenMsg];
	int iResult;
	recvUserList(Server);
	while (true)
	{
		iResult = recv(Server, buffer, LenMsg, 0);

		if (iResult == SOCKET_ERROR)
		{
			cout << "Recv failed with error: " << WSAGetLastError() << endl;
			cout << "Your connection is terminated." << endl;
			closesocket(Server);
			WSACleanup();
			return;
		}
		cout << buffer << endl;

		recvMsg(Server, buffer);
	}
	delete[] buffer;
}
void Client::sending(SOCKET &Server)
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

		if (getMsg(Server, sendMsg, lenMsg)) continue;

		else
		{
			iResult = send(Server, sendMsg, sizeof(sendMsg), 0);
			if (iResult == SOCKET_ERROR)
			{
				cout << "Send failed with error: " << WSAGetLastError() << endl;
				cout << "Your connection is terminated." << endl;
				closesocket(Server);
				WSACleanup();
				return;
			}
		}
	}

}

int Client::start()
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

	LogIn(Server);

	//thread send (&Client::sending, std::ref(Server),this);
	std::thread send([&](Client* client) { client->sending(Server); },this);
	//thread receive (&Client::receiving, std::ref(Server),this);
	std::thread receive([&](Client* client) { client->receiving(Server); }, this);
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



/// SEND

void Client::showInstruction()
{
	cout << "\tINSTRUCTION :" << endl;
	cout << "1. /1             | Display the list of online users." << endl;
	cout << "2. /2             | Display the list of files ." << endl;
	cout << "3. /[Username]    | Chat privately." << endl;
	cout << "4. /[File's no]   | Download the file's [no]." << endl;
	cout << "5. +[pathFileName]| Send file by inputing path File ." << endl;
	cout << "6. #              | End connection." << endl;
}

bool Client::getMsg(SOCKET Server, char sendMsg[], int lenMsg)
{
	string option = sendMsg;
	if (option == "/0")
	{
		showInstruction();
		return true;
	}
	else if (option == "/1")
	{
		getOnlineUser();
		return true;
	}
	else if (option == "/2")
	{
		getFilelist();
		return true;
	}
	else if (option == "#")
	{
		int iResult = send(Server, sendMsg, LenMsg, 0);
		cout << "Disconnected..." << endl;
		closesocket(Server);
		WSACleanup();
		return true;
	}
	else if (option[0] == '+' && option[2] == ':' && option[3] == '\\')
	{
		sendFile(Server, sendMsg);
		return true;
	}
	else if (option.find("/download") == 0)
	{
		downloadFile(option);
		return true;
	}
	return false;
}
void Client::getOnlineUser()
{
	cout << "Online User: ";
	for (int i = 0; i < onlineUser.size(); i++)
	{
		cout << onlineUser[i] << ' ';
	}
	cout << endl;
}
void Client::getFilelist()
{
	cout << "List file: " << endl;
	if (fileList.size() == 0)
	{
		return;
	}
	for (int i = 0; i < fileList.size(); i++)
	{
		cout << i + 1 << ". " << fileList[i].name << endl;
	}
}
void Client::downloadFile(string option)
{
	//option = /download 5
	string i = option.substr(option.find_first_of(' ') + 1, option.length()); // i = 5
	int pos = stoi(i);
	if (pos < fileList.size()) 
	{
		int b = fileList[pos - 1].name.find_last_of('\\');
		string cname = user.Id;
		cname += "_";
		string fileeeee = fileList[pos - 1].name.insert(b + 1, cname); //fileName = D:\clientB_test.txt

		ofstream save(fileeeee, ios::binary);
		save.write(fileList[pos - 1].filebuf, fileList[pos - 1].size);
		save.close();
	}
	else cout << "File does not exist." << endl;
}

void Client::sendFile(SOCKET Server, char sendMsg[])
{
	string fileName = sendMsg;
	fileName.erase(fileName.begin()); //xoa dau +

	ifstream source(fileName, ios::binary);
	if (source.is_open())
	{
		//get file size
		source.seekg(0, ios::end);
		int size = source.tellg();
		source.seekg(0);
		char* buffer = new char[size];

		source.read(buffer, size);
		send(Server, sendMsg, sizeof(sendMsg), 0); //gui ten file: +D:\test.txt
		send(Server, (char*)& size, sizeof(int), 0); //gui file size
		send(Server, buffer, size, 0); //gui noi dung file duoi dang binary
		delete[] buffer;
		source.close();
	}
	else
	{
		cout << "File does not exist." << endl;
	}
}

/// RECIEVE
bool Client::recvMsg(SOCKET Server, char buffer[])
{
	string buf = buffer;
	if (buf.find(':') == -1) //logged out/in
	{
		updateUserlist(buf);
		return true;
	}
	else if (buf[0] == '+')  //nhan file 
	{
		recvFile(Server, buf);
		return true;
	}
}
void Client::updateUserlist(string buf)
{
	string name = buf.substr(0, buf.find_first_of(' '));
	buf.erase(0, buf.find_first_of(' ') + 1);
	if (buf == "logged out")
	{
		for (int i = 0; i < onlineUser.size(); i++)
		{
			if (onlineUser[i] == name)
			{
				onlineUser.erase(onlineUser.begin() + i);
				return;
			}
		}
	}
	else if (buf == "logged in")
	{
		onlineUser.push_back(name);
		return;
	}
}
void Client::recvFile(SOCKET Server, string buf)
{
	int size;
	recv(Server, (char*)& size, sizeof(int), 0); //nhan file size
	char* filebuf = new char[size];
	recv(Server, filebuf, size, 0); //nhan noi dung file duoi dang binary

	string fileName = buf; //fileName = +client A gui file [D:\test.txt]
	int a = fileName.find('[');
	int b = fileName.find(']');
	fileName = fileName.substr(a + 1, b - a - 1); //fileName = D:\test.txt

	File newfile;
	newfile.name = fileName;
	newfile.size = size;
	newfile.filebuf = filebuf;
	fileList.push_back(newfile);

	delete[] filebuf;
}