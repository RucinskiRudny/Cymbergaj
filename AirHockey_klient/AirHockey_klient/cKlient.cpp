#include "cKlient.h"
#include <winsock2.h>

cKlient::cKlient(void)
{
WSADATA wsaData ;
int iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
strcpy(IP,"127.0.0.1");
port=10000;
}

cKlient::cKlient(char *IP, int port)
{
WSADATA wsaData;
int iResult=WSAStartup(MAKEWORD(2,2),&wsaData);
strcpy(this->IP,IP);
this->port=port;
}

int cKlient::polacz() 
{
m_socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(m_socket==INVALID_SOCKET)
	{
	printf("Blad w funkcji socket( ): %ld \n" , WSAGetLastError( ));
	WSACleanup( );
	exit(-1);
	}

sockaddr_in clientService;
clientService.sin_family=AF_INET;
clientService.sin_addr.s_addr=inet_addr(IP);
clientService.sin_port=htons(port); //adres portu
if(connect(m_socket,(SOCKADDR*)&clientService, sizeof(clientService))==SOCKET_ERROR)
{
printf("Blad polaczenia do serwera.\n");
WSACleanup();
return 0;
exit(-1);
}

return 1;

}


