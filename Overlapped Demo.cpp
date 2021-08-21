#include<stdio.h>
#include<WinSock2.h>

WSABUF wsabuf;
char buffer[1024];
DWORD byteRevc = 0;
DWORD flag = 0;
OVERLAPPED ov;
SOCKET s;

void CALLBACK RecvCompleted(
	IN DWORD dwError,
	IN DWORD cbTransferred,
	IN LPWSAOVERLAPPED lpOverlapped,
	IN DWORD dwFlags) 
{
	if (dwError == 0) {
		printf("Byte received: %d\n", cbTransferred);
		printf("Data: %s\n", wsabuf.buf);

		memset(lpOverlapped, 0, sizeof(LPOVERLAPPED));
		WSARecv(s, &wsabuf, 1, &byteRevc, &flag, lpOverlapped, RecvCompleted);
	}
}

int main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	connect(s, (sockaddr*)&saddr, sizeof(saddr));

	wsabuf.buf = buffer;
	wsabuf.len = sizeof(buffer);
	memset(&ov, 0, sizeof(ov));
	WSARecv(s, &wsabuf, 1, &byteRevc, &flag, &ov, RecvCompleted);

	while (true) {
		SleepEx(1000, TRUE);
	}
}