#include<stdio.h>
#include<WinSock2.h>
#include<WS2tcpip.h>

void main() {
	WSADATA wsData;
	int error = WSAStartup(MAKEWORD(2, 2), &wsData);
	if (error == SOCKET_ERROR) {
		printf("khoong khoi tao duoc WinSock\n");
		int ec = WSAGetLastError(); // tra ve ma loi, phai tra google moi biet dc :))
	}
	else {
		printf("Khoi tao thanh cong WinSock\n");
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //socket tcp
		if (s == INVALID_SOCKET) { //invalid_socket = -1
			printf("khong tao duoc socket\n");
		}
		else {
			printf("gia tri socket vua tao: %d\n", s);
			SOCKADDR_IN saddr;
			saddr.sin_family = AF_INET;
			//saddr.sin_port = 80; //dia chi cong http //SAI vi la dang little endian
			//saddr.sin_port = 20480; //80 la 0x00 0x50 dao nguoc byte -> 0x50 0x00 -> 20480
			saddr.sin_port = htons(80);
			saddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

			//phan giai ten mien
			PADDRINFOA result = NULL;
			getaddrinfo("vnexpress.net","http",NULL,&result);
			PADDRINFOA root = result;

			if (result == NULL) printf("khong phan giai duoc\n");
			while (result != NULL) {
				printf("dia chi tim duoc: \n");
				SOCKADDR_IN addr; //cau truc 16bytes: af(2), port(2), ip(4), zero(8)
				if (result->ai_family == AF_INET) {
					memcpy(&addr, result->ai_addr, result->ai_addrlen);
					printf("%s", inet_ntoa(addr.sin_addr));
				}
				result = result->ai_next;
			}
			freeaddrinfo(root);
		}
		printf("close socket.");
		WSACleanup(); //dong khoi tao va thu hoi bo nho
	}
}