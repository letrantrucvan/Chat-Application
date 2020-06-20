#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <fstream>
#include <conio.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

using namespace std;
#define NumClient 100

struct User
{
	string Id;
	string Password;
};
class server 
{
public:
	vector <User>  UserList;
	vector <bool> isExisted;

	SOCKET ClientList[NumClient];
	thread ClientThread[NumClient];
	int count = 0;

	//---HAM HO TRO
	string FormatStr(char*& buffer, int& mode, string userName);
	bool checkUser(string name); //check trong vector UserList - luu nhung user dang online
	int checkUser(char Id[], char Pass[]); //check trong file UserList.txt
	void getUsername(int index);
	void sendUserList(int index);
	void run(int index);
	int start();
};
