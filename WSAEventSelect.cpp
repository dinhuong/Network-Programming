#include<stdio.h>
#include<WinSock2.h>

#define MAX_CLIENT 1024

WSAEVENT g_events[MAX_CLIENT];
SOCKET g_sockets[MAX_CLIENT];
int g_count = 0;

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);

	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	g_sockets[g_count] = s;
	g_events[g_count] = WSACreateEvent();
	WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_ACCEPT);
	g_count++;
	while (true) {
		int index = WSAWaitForMultipleEvents(g_count, g_events, FALSE, INFINITE, FALSE); //treo tai day
		index = index - WSA_WAIT_EVENT_0; //smallest index of the opened event
		//duyet de tim ra event nao duoc bao hieu
		for (int i = index;i < g_count;i++) {
			WSANETWORKEVENTS networkEvent;
			WSAEnumNetworkEvents(g_sockets[i], g_events[i], &networkEvent);
			if (networkEvent.lNetworkEvents & FD_ACCEPT) {
				if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {//no error
					SOCKADDR_IN caddr;
					int clen = sizeof(caddr);
					SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
					HANDLE e = WSACreateEvent();
					g_sockets[g_count] = c;
					g_events[g_count] = e;
					WSAEventSelect(g_sockets[g_count], g_events[g_count], FD_READ | FD_CLOSE);
					g_count++;
					printf("accept\n");
				}
			}

			if (networkEvent.lNetworkEvents & FD_READ) {
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(g_sockets[i], buffer, sizeof(buffer), 0);
					printf("%s\n", buffer);
				}
			}

			if (networkEvent.lNetworkEvents & FD_CLOSE) {
				if (networkEvent.iErrorCode[FD_CLOSE_BIT] == 10053) {
					printf("A client has disconnected.\n");
				}
			}
		}
	}
}