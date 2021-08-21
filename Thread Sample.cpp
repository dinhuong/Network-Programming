#include<Windows.h>
#include<stdio.h>

#define MAX_CLIENT 1024
SOCKET client[MAX_CLIENT];
int clientCount = 0;

DWORD WINAPI MyThread(LPVOID param) {
	
	SOCKET currentSocket = (SOCKET)param;
	printf("%d\n", GetCurrentThreadId());
	//sum++;
	//Sleep(1000); //neu 2 thread xung dot

	char buffer[1024];
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		recv(currentSocket, buffer, sizeof(buffer), 0);
		for (int i = 0;i < clientCount;i++) {
			if (client[i] != currentSocket) {
				send(client[i], buffer, strlen(buffer), 0);
			}
		}
	}
	return 0;
}

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	PSOCKADDR_IN saddr;
	saddr->sin_family = AF_INET;
	saddr->sin_addr.S_un.S_addr = 0;
	saddr->sin_port = htons(8888);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&s, sizeof(s));
	listen(s, 10);

	while (true) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		client[clientCount] = accept(s, (sockaddr*)&caddr, &clen);
		CreateThread(NULL, 0, MyThread, (LPVOID)client[clientCount], 0, NULL);
		clientCount++;
	}

	/*printf("%d\n", GetCurrentThreadId());
	const int MAX_THREAD = 10;
	HANDLE hThread[MAX_THREAD];

	for (int i = 0; i < MAX_THREAD; i++) {
		hThread[i] = CreateThread(NULL, 0, MyThread, NULL, 0, NULL);
	}
	WaitForMultipleObjects(MAX_THREAD, hThread, TRUE, INFINITE);
	for (int i = 0; i < MAX_THREAD; i++) {
		CloseHandle(hThread[i]);
	}
	printf("Tong: %d", sum);*/
}