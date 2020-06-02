#include <iostream>
#include <conio.h>
#include<string>
#include <string.h>
#include <windows.h>
#include <fstream>
using namespace std;

struct User
{
	char Id[30] = { 0 };
	char Password[30] = { 0 };
};

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
	User user;
	ofstream ofs("UserList.txt",ios::app);

	cout << "_____SIGN UP_____" << endl;
	cout << "Enter ID: ";
	cin >> user.Id;
	cout << "Enter Password: ";
	cin >> user.Password;

	do
	{
		if (checkUser(user.Id, user.Password) == 2 && ofs.is_open())
		{
			ofs << user.Id << endl;
			ofs << user.Password << endl;
			cout << "Sign Up successfully " << endl;
			break;
		}
		cout << "Username is already taken. Please try another " << endl;
		SignUp();
	} while (checkUser(user.Id, user.Password) != 2);

	ofs.close();
}

bool SignIn()
{
	int flag = 0;
	do {
		char input = 0, Id[20], Pass[20];

		cout << "_____LOGIN_____" << endl;

		//Nhap ID
		int i = 0;
		
		cout << "Enter ID:";
		fflush(stdin);
		cin >> Id;
		//cout << strlen(Id);
		Id[strlen(Id)] = '\0';

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
					Pass[i] = '\0';
					cout << "\b" << " " << "\b";
				}
			}
			else {
				Pass[i] = input;
				i++;
				cout << "*";
			}
		}
		Pass[i] = '\0';
		//cout << endl << strlen(Pass);
		if (checkUser(Id, Pass) == 1)
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



int main()
{
	int choose;
	cout << "1: Sign Up | 2: Log In " << endl;
	cin >> choose;
	
	if (choose == 1)
	{
		SignUp();
		if (SignIn())
		{
			cout << endl << "Login successfully !";
		}
		else
		{
			cout << endl << "Login failed !";
		}
	}
	else
	{
		if (SignIn())
		{
			cout << endl << "Login successfully !";
		}
		else
		{
			cout << endl << "Login failed !";
		}
	}
	
	return 0;
}