#include<stdio.h>
#include<WinSock2.h>

void main()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET c = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //noi den dia chi nao
		//inet_addr: chuyen tu string sang int
	int result = connect(c, (sockaddr*)&saddr, sizeof(saddr));
	if (result != SOCKET_ERROR) {
		char* hello = (char*)"Hello Huong";
		send(c, hello, sizeof(hello), 0);
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		recv(c, buffer, sizeof(buffer), 0);
		printf("%s\n", buffer);
		free(buffer);
	}
	closesocket(c);
	WSACleanup();
}