#pragma once
#include "client.h"
#include <iostream>

using namespace std;

#define LenMsg 1024
#define FILE_LENGHT 1024*1024*5
#define PORT "8080"

int main()
{
	Client c;
	c.start();

	system("pause");
	return 0;
}