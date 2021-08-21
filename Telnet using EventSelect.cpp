//slide 136, ko can dang nhap vao server

#include<stdio.h>
#include<WinSock2.h>

#define MAX_SOCKET 64

SOCKET sockets[MAX_SOCKET];
WSAEVENT events[MAX_SOCKET];
int count = 0;
CRITICAL_SECTION cs;

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(80);
	saddr.sin_addr.S_un.S_addr = INADDR_ANY;
	bind(s, (sockaddr*)&saddr, sizeof(saddr));

	printf("%d", saddr.sin_port);

	listen(s, 10);

	SOCKET client = accept(s, 0, 0);
	char buffer[256] = "";
	long n = 1234;
	send(client, itoa(n,buffer,10), 4, 0);

	/*sockets[count] = s;
	events[count] = WSACreateEvent();
	WSAEventSelect(sockets[count], events[count], FD_ACCEPT);
	count++;*/
	

	//while (true) {
	//	int i = WSAWaitForMultipleEvents(count, events, FALSE, INFINITE, FALSE);
	//	i = i - WSA_WAIT_EVENT_0;

	//	WSANETWORKEVENTS networkEvent;
	//	WSAEnumNetworkEvents(sockets[i], events[i], &networkEvent);

	//	if (networkEvent.lNetworkEvents & FD_ACCEPT) {
	//		if (networkEvent.iErrorCode[FD_ACCEPT_BIT] == 0) {
	//			SOCKADDR_IN caddr;
	//			int clen = sizeof(caddr);
	//			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
	//			HANDLE e = WSACreateEvent();
	//			sockets[count] = c;
	//			events[count] = e;
	//			WSAEventSelect(sockets[count], events[count], FD_READ | FD_CLOSE);
	//			count++;
	//		}
	//	}

	//	if (networkEvent.lNetworkEvents & FD_READ) {
	//		if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
	//			char command[1024];
	//			memset(command, 0, sizeof(command));
	//			recv(sockets[i], command, sizeof(command), 0);
	//			while (command[strlen(command) - 1] == '\r' || command[strlen(command) - 1] == '\n') {
	//				command[strlen(command) - 1] = 0;
	//			}
	//			sprintf(command + strlen(command), " > result.txt");

	//			EnterCriticalSection(&cs);
	//			system(command);

	//			FILE* f = fopen("result.txt", "rb");
	//			fseek(f, 0, SEEK_END); //dua con tro ve cuoi file
	//			int flen = ftell(f); //lay vi tri cua con tro
	//			fseek(f, 0, SEEK_SET); //dua con tro ve dau file
	//			char* data = (char*)calloc(flen, 1);
	//			fread(data, 1, flen, f);
	//			fclose(f);
	//			LeaveCriticalSection(&cs);

	//			send(sockets[i], data, flen, 0); //ko dung strlen(data)
	//			free(data);
	//		}
	//	}

	//	if (networkEvent.lNetworkEvents & FD_CLOSE) {
	//		if (networkEvent.iErrorCode[FD_CLOSE_BIT] == 0) {
	//			//
	//		}
	//	}
	//}
}