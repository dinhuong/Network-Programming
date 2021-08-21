#include<WinSock2.h>
#include<stdio.h>

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	char* welcome = (char*)"Hello UDP Programming!";

	SOCKADDR_IN toaddr;
	toaddr.sin_family = AF_INET;
	toaddr.sin_port = htons(8888);
	toaddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	int bytesent = sendto(s, welcome, strlen(welcome), 0, (sockaddr*)&toaddr, sizeof(toaddr));
	printf("Da gui: %d bytes\n", bytesent);

}