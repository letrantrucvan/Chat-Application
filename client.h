//#pragma once
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
#include <vector>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

struct User
{
	char Id[30] = { 0 };
	char Password[30] = { 0 };
};
//struct File
//{
//	string name;
//	char* filebuf;
//	int size;
//	string recipient;
//	string sender;
//};
class Client
{
public:
	void LogIn(SOCKET& Server);
	bool SignIn(SOCKET& Server);
	bool SignUp(SOCKET& Server);

	int checkUser(SOCKET& Server, char Id[], char Pass[]);
	void printOnlineUser();
	void recvUserList(SOCKET& Server);
	void updateUserlist(string buf);

	void receiving(SOCKET& Server);
	void sending(SOCKET& Server);

	void stringtochar(char*& result, string input);
	void showInstruction();

	// Client send request and get services from server
	bool getMsg(SOCKET Server, char sendMsg[], int lenMsg);
	bool recvMsg(SOCKET Server, char buffer[]);

	// FILE
	void printFileList();
	void recvFileList(SOCKET& Server);
	bool checkFileExist(string name);
	void downloadFile(SOCKET Server,string name);
	void sendFile(SOCKET Server, char sendMsg[]);
	void updateFilelist(string buf);

	int start();

	User user;
	vector <string> onlineUser;
	vector <string> fileNameList;
	//vector <File> fileList;
};