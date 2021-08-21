#include<stdio.h>
#include<WinSock2.h>

int count = 0; //so luong ket noi
SOCKET clients[64];

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
		FD_ZERO(&fread); //clean fread
		FD_SET(s, &fread); //add socket into fread
		for (int i = 0;i < count;i++) {
			FD_SET(clients[i], &fread); //add sockets into fread
		}
		select(0, &fread, NULL, NULL, NULL); //treo o day va ko co timeout
		//select return so luong event xay ra

		if (FD_ISSET(s, &fread)) { //check if event comes to socket s
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			clients[count++] = c;
			continue;
		}

		for (int i = 0; i < count;i++) {
			if (FD_ISSET(clients[i], &fread)) { //check if client send data to socket clients[i]
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

//problem: as client disconnect, a event sends to client socket but no data is sent
//=> treo tai ham recv