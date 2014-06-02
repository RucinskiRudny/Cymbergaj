#include "cKlient.h"
#include <conio.h>
#include <iostream>
#include <Windows.h>



char a;
int b;
sRamkaV ramka;

int main (int argc, char **argv)
{
	cKlient* klient = new cKlient("127.0.0.1",11000);
	klient->polacz();
	a=1;

	while(1)
	{
	Sleep(50);
	if(_kbhit())
	{
		a=_getch();
		if(a=='a'||a=='A')
		{
			ramka.xV=30;
			ramka.yV=0;
			send(klient->m_socket,(char*)&ramka,sizeof(ramka),0);
		}
		if(a=='d'||a=='D')
		{
			ramka.xV=-30;
			ramka.yV=0;
			send(klient->m_socket,(char*)&ramka,sizeof(ramka),0);
		}
		if(a=='w'||a=='W')
		{
			ramka.xV=0;
			ramka.yV=30;
			send(klient->m_socket,(char*)&ramka,sizeof(ramka),0);
		}
		if(a=='s'||a=='S')
		{
			ramka.xV=0;
			ramka.yV=-30;
			send(klient->m_socket,(char*)&ramka,sizeof(ramka),0);
		}
	}
	else
	{
			ramka.xV=0;
			ramka.yV=0;
			send(klient->m_socket,(char*)&ramka,b=sizeof(ramka),0);
			b=b;
	}

	if(a=='p'||a=='P')
	{
	delete klient;	
	break;
	}
	}


}