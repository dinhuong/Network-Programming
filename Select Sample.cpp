#include<stdio.h>
#include<WinSock2.h>

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&saddr, sizeof(s));
	listen(s, 10);

	fd_set fread;  //tap read can tham do

	while (true) {
		FD_ZERO(&fread); //clean array fread
		FD_SET(s, &fread); //them socket s vao tap fread
		select(0, &fread, NULL, NULL, NULL);
		if (FD_ISSET(s, &fread)) {
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			printf("Client: %d\n", c);
		}
	}
}