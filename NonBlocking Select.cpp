#include<stdio.h>
#include<WinSock2.h>

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN caddr;
	caddr.sin_family = AF_INET;
	caddr.sin_port = htons(8888);
	caddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	unsigned long argp = 1;
	int result = ioctlsocket(c, FIONBIO, &argp); //chuyen sang socket non-blocking
	int r = connect(c, (sockaddr*)&caddr, sizeof(caddr));

	argp = 0;
	result = ioctlsocket(c, FIONBIO, &argp); //chuyen lai socket blocking

	fd_set fwrite, ferr;
	FD_ZERO(&fwrite);
	FD_ZERO(&ferr);
	FD_SET(c, &fwrite);
	FD_SET(c, &ferr);

	timeval timeout;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	select(0, NULL, &fwrite, &ferr, &timeout);

	if (FD_ISSET(c, &fwrite)) {
		printf("Ket noi thanh cong\n");
		closesocket(c);
	}
	else {
		printf("Ket noi khong thanh cong\n");
	}
}