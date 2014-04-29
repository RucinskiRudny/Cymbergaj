#pragma once
#include <winsock2.h>
#include <Windows.h>

struct sRamkaV
{
int xV;
int yV;
int zV;
};


class cSerwer
{
	public:
SOCKET m_socket;
SOCKET tablica[4];
int ilu;
	cSerwer(void);
	~cSerwer(void);
	void PobierzV(sRamkaV* Predkosci);
	void Rejestracja(int i); //!Przeprowadza rejestracje u¿ytkownika o numerze "i"

};

