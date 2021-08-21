#include <WinSock2.h>
#include <stdio.h>

void main() {
	//step 1: khoi tao winsock
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	//step 2: tao socket
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) {
		printf("khong tao duoc socket, ma loi: %d\n", WSAGetLastError());
	}
	else {
	//step3: gan socket vao 1 addr
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));

	//step 4: listen
	listen(s, 10);

	//step 5: accept du lieu va tao 1 socket moi
	SOCKADDR_IN caddr;
	int addrlen = sizeof(saddr);
	SOCKET c = accept(s, (sockaddr*)&caddr, &addrlen);

	//step 6: xu li du lieu, gui, nhan tren socket moi
	FILE *f1 = fopen("chao.txt", "r");
	if (f1 == NULL) {
		printf("can't open chao.txt");
	}
	else {
		char chao[1024];
		memset(chao, 0, sizeof(chao));
		fgets(chao, sizeof(chao), f1);
		send(c, chao, strlen(chao), 0);
		fclose(f1);
	}
	

	FILE *f2 = fopen("client.txt", "w");
	if (f2 == NULL) {
		printf("can't open client.txt");
	}
	else {
		char data[1024];
		memset(data, 0, sizeof(data));
		int byteRecv = recv(c, data, 1024, 0);
		fprintf(f2, "%s\n", data);
		fclose(f2);
	}

	//step 7: dong socket
	closesocket(c);
	closesocket(s);

	//step 8: giai phong winsock
	WSACleanup();
	}
}