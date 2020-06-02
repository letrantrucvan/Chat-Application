#include "pch.h"
#include "framework.h"
#include "server.h"
#include "afxsock.h"
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#define portNum 8080
#endif

// The one and only application object

CWinApp theApp;

using namespace std;

struct User
{
	char m_Name[30] = { 0 };
	char m_Password = { 0 };
};

int main()
{
	int nRetCode = 0;

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
			//Init Socket
			if (AfxSocketInit(NULL) == NULL)
			{
				cout << "Socket init function failed with errors:  " << GetLastError() << endl;
				exit(1);
			}

			int n;
			cout << "Input the number of clients: ";
			cin >> n;

			//Socket
			CSocket m_Server;
			CSocket* m_Client = new CSocket[n];
			User* user = new User[n];
			int isExisted = n;

			if (m_Server.Create(portNum, SOCK_STREAM, NULL) == 0)
			{
				cout << "Socket function failed with errors: " << m_Server.GetLastError() << endl;
				exit(1);
			}
			else
			{
				cout << "Server inited successfully " << endl;
				cout << "Looking for clients... " << endl;

				//listen
				if (m_Server.Listen(5) == FALSE)
				{
					cout << "Socket listen function failed with errors: " << m_Server.GetLastError() << endl;
					m_Server.Close();
					exit(1);
				}

				// accept 
				for (int i = 0; i < n; i++)
				{
					if (m_Server.Accept(m_Client[i]))
					{
						char* buffer;
						int lenName = strlen(user[i].m_Name);

						m_Client[i].Receive((char*)&lenName, sizeof(int), 0);
						buffer = new char[lenName + 1];
						m_Client[i].Receive((char*)buffer, lenName, 0);
						buffer[lenName] = '\0';
						for (int j = 0; j < lenName; j++)
						{
							user[i].m_Name[j] = buffer[j];
						}
						cout << user[i].m_Name << " is connected to server " << endl;
					}
				}

				//Start sending and receiving msg
				char sendMsg[1024];
				int lenMsg;
				char* buffer = "0";

				cout << "Enter # to end the connection" << endl;
				do {
					do
					{
						for (int i = 0; i < n; i++)
						{
							if (1)
							{
								/*do
								{*/
								m_Client[i].Receive((char*)&lenMsg, sizeof(int), 0);
								buffer = new char[lenMsg + 1];
								m_Client[i].Receive((char*)buffer, lenMsg, 0);
								buffer[lenMsg] = '\0';
								cout << user[i].m_Name << " : " << buffer << endl;

								if (*buffer == '#') //client[i] logged out
								{
									//Hien tren server :client[i] logged out
									cout << user[i].m_Name << " logged out. " << endl;
									m_Client[i].Close();
									isExisted--;
									char* temp = new char[100];
									temp = user[i].m_Name + 'logg' + 'ed' + ' out';
									int lentemp = strlen(temp);

									//Gui cho cac client khac client[i] logged out
									for (int j = 0; j < n; j++)
									{
										if (j == i && j != n - 1)
											j++;
										else if (j == i && j == n - 1)
											break;
										m_Client[j].Send(&lentemp, sizeof(lentemp), 0);
										m_Client[j].Send(temp, lentemp, 0);
									}
								}
								for (int j = 0; j < n; j++) //client[i] tiep tuc chat
								{
									if (j == i) j++;
									m_Client[j].Send(&lenMsg, sizeof(lenMsg), 0);
									m_Client[j].Send(buffer, lenMsg, 0);

								}
								/*} while (buffer[lenMsg - 1] != '*');*/
							}
						}

					} while (isExisted);

					//do {
					//	cout << "Server: ";
					//	cin.getline(sendMsg, 1024);
					//	lenMsg = strlen(sendMsg);
					//	m_Client.Send(&lenMsg, sizeof(lenMsg), 0);
					//	m_Client.Send(sendMsg, lenMsg, 0);

					//	if (sendMsg[lenMsg - 1] == '*')
					//		break;
					//	if (*sendMsg == '#')
					//	{
					//		cout << "Server connection is terminated " << endl;
					//		isExisted = TRUE;
					//	}
					//} while (*sendMsg != '#');
				} while (!isExisted);
			}
			for (int i = 0; i < n; i++)
			{
				m_Client[i].Close();
			}
			m_Server.Close();
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
