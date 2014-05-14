#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <iostream>

struct sRamkaV
{
float xV;
float yV;
float zV;
};


class cKlient
{
public:
char IP[16];
int port;
SOCKET m_socket;
bool polaczony;

	cKlient(void);
	cKlient(char *IP, int port);
	int polacz(); ///< nawi¹zuje po³¹czenie z serwerem
};