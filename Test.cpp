#include<stdio.h>
#include<WinSock2.h>

#define MAX_CLIENT 1024

SOCKET g_sockets[MAX_CLIENT];
WSAEVENT g_events[MAX_CLIENT];
int g_room[MAX_CLIENT];

int count = 0;
SOCKET g_datasocket;

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(5555);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	g_sockets[count] = s;
	g_events[count] = WSACreateEvent();
	WSAEventSelect(g_sockets[count], g_events[count], FD_ACCEPT);
	count++;

	while (true) {
		int index = WSAWaitForMultipleEvents(count, g_events, FALSE, INFINITE, FALSE);
		index = index - WSA_WAIT_EVENT_0;

		for (int i = index; i < count;i++) {
			WSANETWORKEVENTS networkEvent;
			WSAEnumNetworkEvents(g_sockets[i], g_events[i], &networkEvent);
			if (networkEvent.lNetworkEvents & FD_ACCEPT) {
				if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
					SOCKADDR_IN caddr;
					int clen = sizeof(caddr);
					SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
					HANDLE e = WSACreateEvent();
					g_sockets[count] = c;
					g_events[count] = e;
					WSAEventSelect(g_sockets[count], g_events[count], FD_READ | FD_CLOSE);
					g_room[count] = 0;
					count++;
				}
			}

			if (networkEvent.lNetworkEvents & FD_READ) {
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(g_sockets[i], buffer, sizeof(buffer), 0);
					printf("%s", buffer);

					if (strncmp(buffer, "ROOM", 4) == 0) {
						int room = 0;
						char ROOM[8];
						sscanf(buffer, "%s%d", ROOM, &room);
						if ((room > 0) && (room <= 100)) {
							g_room[i] = room;
						}
						else {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "Invalid room number, please try again.\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "MSG", 3) == 0) {
						if (g_room[i] == 0) {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "Please use ROOM command to join first.\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
						else {
							char message[1024];
							memset(message, 0, sizeof(message));
							strcpy(message, buffer + 4);

							for (int j = 1; j < count;j++) {
								if ((g_room[j]==g_room[i]) && (j != i)) {
									send(g_sockets[j], message, strlen(message), 0);
								}
							}
						}
					}
					else if (strncmp(buffer, "QUIT", 4) == 0) {
						g_room[i] = 0;
						closesocket(g_sockets[i]);
					}
					else {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "Invalid command!\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
				}
			}

			if (networkEvent.lNetworkEvents & FD_CLOSE) {
				if (networkEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
					closesocket(g_sockets[i]);
				}
			}
		}
	}
}