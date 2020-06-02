// client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "framework.h"
#include "client.h"
#include "afxsock.h"
#include <string.h>
#include <thread>
#include <mutex>

#ifdef _DEBUG
#define new DEBUG_NEW
#define portNum 8080
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

struct User
{
	char m_Name[30];
	char m_Password[30];
};

void Sending(CSocket &m_Client, User user)
{
	char sendMsg[1024] = { 0 };
	int lenMsg;
	while (*sendMsg != '#')
	{
		//fflush(stdin);
		cin.getline(sendMsg, 1024);
		cout << user.m_Name << ": ";
		cin.getline(sendMsg, 1024);
		lenMsg = strlen(sendMsg);
		m_Client.Send(&lenMsg, sizeof(lenMsg), 0);
		m_Client.Send(sendMsg, lenMsg, 0);

		if (*sendMsg == '#')
		{
			cout << "\nLogged out." << endl;
			m_Client.Close();
			break;
			//isExit = TRUE;
		}
	}
}

void Receiving(CSocket &m_Client)
{
	char* buffer = "0";
	int lenMsg;
	do
	{
		m_Client.Receive((char*)& lenMsg, sizeof(int), 0);
		buffer = new char[lenMsg + 1];
		m_Client.Receive((char*)buffer, lenMsg, 0);

		cout << buffer << endl;
	} while (*buffer != '#');
}
thread sendMsg(CSocket& m_Client, User user)
{
	Sending(m_Client,user);
}

thread receiveMsg(CSocket& m_Client)
{
	Receiving(m_Client);
}

int main()
{
	int nRetCode = 0;
	bool isExit = FALSE;
	HMODULE hModule = ::GetModuleHandle(nullptr);

	if (hModule != nullptr)
	{
		// initialize MFC and print and error on failure
		if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
		{
			// TODO: code your application's behavior here.
			wprintf(L"Fatal Error: MFC initialization failed\n");
			nRetCode = 1;
		}
		else
		{
			// TODO: code your application's behavior here.
			if (AfxSocketInit(NULL) == NULL)
			{
				cout << "Socket init function failed with errors:  " << GetLastError() << endl;
				exit(1);
			}

			CSocket m_Client;
			m_Client.Create();

			//Input IP
			char add[100];
			cout << "Input Server's IP Address' : ";
			cin.getline(add, 100);

			User user;
			//do {
				cout << "SIGN IN" << endl;
				cout << "ID: ";
				cin >> user.m_Name;
				cout << "Password ";
				cin >> user.m_Password;
			//} while (!checkUser(user.m_Name, user.m_Password));

			//connect 
			if (m_Client.Connect(CA2W(add), portNum) != 0)
			{
				cout << "Connecting to server..." << endl;

				//sending ID's info
				int lenName = strlen(user.m_Name);
				//ID
				m_Client.Send(&lenName, sizeof(lenName), 0);
				m_Client.Send(user.m_Name, lenName, 0);
	
				//start sending and receiving msg
				cout << "Enter # to end the connection " << endl;
				
					/*thread sendingMsg(Sending, m_Client, user);
					thread receivingMsg(Receiving, m_Client);*/
				/*	Sending( m_Client, user);
					Receiving(m_Client);*/
				thread SendMsg = new thread;
				SendMsg = sendMsg(m_Client, user);
					receiveMsg(m_Client);
					if (sendMsg.joinable())
						sendMsg.join();
					if (receiveMsg.joinable())
						receiveMsg.join();
					/*do
					{
						cout << user.m_Name << ": ";
						cin.getline(sendMsg, 1024);
						lenMsg = strlen(sendMsg);
						m_Client.Send(&lenMsg, sizeof(lenMsg), 0);
						m_Client.Send(sendMsg, lenMsg, 0);
						if (sendMsg[lenMsg - 1] == '*') break;
						if (*sendMsg == '#')
						{
							cout << "\nLogged out." << endl;
							isExit = TRUE;
						}
					} while (*sendMsg != '#');*/


					/*do
					{
						m_Client.Receive((char*)& lenMsg, sizeof(int), 0);
						buffer = new char[lenMsg + 1];
	 					m_Client.Receive((char*)buffer, lenMsg, 0);
						buffer[lenMsg] = '\0';
						cout << "Server: " << buffer << endl;
						if (buffer[lenMsg - 1] == '*') break;
						if (*buffer == '#')
						{
							cout << "Server connection is terminated.\n" << endl;
							isExit = TRUE;
						}
					} while (*buffer != '#');*/

				
			}
			m_Client.Close();
		}
	}
	else
	{
		// TODO: change error code to suit your needs
		wprintf(L"Fatal Error: GetModuleHandle failed\n");
		nRetCode = 1;
	}

	return nRetCode;
}
