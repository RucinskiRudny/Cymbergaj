#include "cSerwer.h"
#include <stdio.h>
#include <iostream>
#include <winsock2.h>



cSerwer::cSerwer(void)
{
ilu=0;

	WSADATA wsaData ;
int iResult=WSAStartup(MAKEWORD( 2 , 2 ) , &wsaData
) ;
if(iResult!= NO_ERROR )
printf("Blad w funkcji WSAStartup( ) \n" ) ;

m_socket=socket( AF_INET, SOCK_STREAM,IPPROTO_TCP);
if( m_socket == INVALID_SOCKET )
{
printf( " blad wynkcji socket( ): %ld \n" ,
WSAGetLastError ( ) ) ;
WSACleanup ( ) ;
exit(-1);
}

sockaddr_in service;
service.sin_family = AF_INET;
service.sin_addr.s_addr = ADDR_ANY; //wybierany domy±lny adres IP serwera
service.sin_port = htons ( 10000 ) ; //nr portu
if( bind ( m_socket , (SOCKADDR*) &service,sizeof(service) ) == SOCKET_ERROR )
{
printf( "blad funkcji bind ( ) . \ n" ) ;
closesocket(m_socket ) ;
exit(-1) ;
}
}

void cSerwer::Rejestracja(int i)
{
if(i==0)
{
	int n=1; // 
	if(listen( m_socket , n ) == SOCKET_ERROR )
	{
	printf( "blad tworzenia socketa. \ n"
	) ;
	exit(-1) ;
	}
}
SOCKET AcceptSocket ;
printf( " serwer oczekuje na polaczenie klienta \ n" ) ;

AcceptSocket = SOCKET_ERROR;
while ( AcceptSocket == SOCKET_ERROR )
{
AcceptSocket = accept ( m_socket , NULL, NULL ) ;
}
printf( "Klient polaczony. \ n" ) ;

tablica[i]=AcceptSocket;
ilu=i+1;
}

void cSerwer::PobierzV(sRamkaV* Predkosci)
{
	sRamkaV V;
	int buforIN,buforOUT=5;
	fd_set wejscie;
	timeval czas;
	czas.tv_sec=3;
	czas.tv_usec=0;
	int pom,ilosc=0;
	bool przyszla=true;

		for(int i=0;i<ilu;i++)
		wejscie.fd_array[i]=tablica[i];
		wejscie.fd_count=ilu;
		pom=select(ilu,&wejscie,NULL,NULL,&czas);
		for(int i=0;i<pom;i++)
			{
				recv(wejscie.fd_array[i],(char*)&V,sizeof(V),0);
				if(wejscie.fd_array[i]==tablica[0])
					Predkosci[0]=V;
				else
					Predkosci[1]=V;
			}

}


cSerwer::~cSerwer(void)
{

}