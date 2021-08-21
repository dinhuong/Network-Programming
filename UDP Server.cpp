#include<WinSock2.h>
#include<conio.h>
#include<stdio.h>

void main() {
	const int MAX_CLIENT = 1024;
	SOCKADDR_IN clients[MAX_CLIENT];
	int clientCount = 0;

	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	SOCKADDR_IN caddr;
	int clen = sizeof(caddr);
	int byterecv = recvfrom(s, buffer, sizeof(buffer), 0, (sockaddr*)&caddr, &clen);
	//printf("port %d\n", caddr.sin_port);
    //printf("addr %s\n", inet_ntoa(caddr.sin_addr));
	memcpy(&clients[clientCount], &caddr, sizeof(caddr));
	clientCount++;
	for (int i = 0; i < clientCount; i++) {
		SOCKET c = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		sendto(c, buffer, strlen(buffer), 0, (sockaddr*)&clients[clientCount], sizeof(SOCKADDR_IN));
		closesocket(c);
	}
	
	printf("%s\n", buffer);
	printf("Da nhan: %d", byterecv);
}