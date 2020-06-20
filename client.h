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
struct File
{
	string name;
	char* filebuf;
	int size;
};
class Client
{
public:
	void LogIn(SOCKET &Server);
	bool SignIn(SOCKET &Server);
	bool SignUp(SOCKET &Server);
	int checkUser(SOCKET &Server, char Id[], char Pass[]);
	void receiving(SOCKET &Server);
	void sending(SOCKET &Server);
	void stringtochar(char*& result, string input);
	void recvUserList(SOCKET &Server);
	int start();

	User user;
	vector<string> onlineUser;
	vector<File> fileList;

	//SEND
	bool getMsg(SOCKET Server, char sendMsg[], int lenMsg);

	void showInstruction();
	void getOnlineUser();
	void getFilelist();
	void downloadFile(string option);
	void sendFile(SOCKET Server, char sendMsg[]);

	//RECIEVE
	bool recvMsg(SOCKET Server, char buffer[]);

	void updateUserlist(string buf);
	void recvFile(SOCKET Server, string buf);
};