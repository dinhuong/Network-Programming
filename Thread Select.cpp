#include<stdio.h>
#include<WinSock2.h>
#define N 2
int count = 0;
SOCKET* clients = NULL;  //unlimited client connect

DWORD WINAPI ClientThread(LPVOID param) {
	int startIndex = (int)param;
	fd_set fread;
	printf("Thread %d, startIndex %d\n", GetCurrentThreadId(),startIndex);

	while (true) {
		FD_ZERO(&fread);
		for (int i = startIndex;i < startIndex + N && i < count; i++) {
			FD_SET(clients[i], &fread);
		}

		timeval timeout;
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		select(0, &fread, NULL, NULL, &timeout);

		for (int i = startIndex; i < startIndex + N; i++) {
			if (FD_ISSET(clients[i], &fread)) {
				char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(clients[i], buffer, sizeof(buffer), 0);
					for (int j = 0; j < count;j++) {
						if (j != i) {
							send(clients[j], buffer, strlen(buffer), 0);
						}
					}
			}
		}
	}
	return 0;
}

void main()
{
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	saddr.sin_port = htons(8888);
	saddr.sin_family = AF_INET;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	fd_set fread;

	while (true) {
		FD_ZERO(&fread);
		FD_SET(s, &fread); 
		 
		for (int i = 0;i < N;i++) {
			FD_SET(clients[i], &fread); 
		}

		select(0, &fread, NULL, NULL, NULL);
		if (FD_ISSET(s, &fread)) { //check if event comes to socket s
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			clients = (SOCKET*)realloc(clients, (count + 1)*sizeof(SOCKET));
			clients[count++] = c;
			if (count % N == 0) {
				CreateThread(0, NULL, ClientThread, (LPVOID)count, NULL, NULL);
			}
		}

		for (int i = 0; i < N;i++) {
			if (FD_ISSET(clients[i], &fread)) {
				char buffer[1024];
				memset(buffer, 0, sizeof(buffer));
				recv(clients[i], buffer, sizeof(buffer), 0);
				for (int j = 0; j < count;j++) {
					if (j != i) {
						send(clients[j], buffer, strlen(buffer), 0);
					}
				}
			}
		}
	}
}
