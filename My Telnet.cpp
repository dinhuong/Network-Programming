#include <stdio.h>
#include<WinSock2.h>

DWORD WINAPI ClientThread(LPVOID param);

CRITICAL_SECTION cs;

void main(int argc, char** argv) {
	if (argc < 2) {
		return;
	}
	else {
		short port = atoi(argv[1]);
		WSADATA data;
		WSAStartup(MAKEWORD(2, 2), &data);
		SOCKADDR_IN saddr;
		SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		saddr.sin_family = AF_INET;
		saddr.sin_addr.S_un.S_addr = INADDR_ANY;
		saddr.sin_port = htons(port);
		bind(s, (sockaddr*)&saddr, sizeof(saddr));
		listen(s, 10);
		InitializeCriticalSection(&cs);
		while (true) {
			SOCKADDR_IN caddr;
			int clen = sizeof(caddr);
			SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
			CreateThread(NULL, 0, ClientThread, (LPVOID)c, 0, NULL);
		}
		DeleteCriticalSection(&cs);
	}
}

DWORD WINAPI ClientThread(LPVOID param) {
	SOCKET c = (SOCKET)param;
	char* welcome = (char*)"Send user/password in format [user password]\n";
	send(c, welcome, strlen(welcome), 0);
	
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	recv(c, buffer, sizeof(buffer), 0);

	//cut duoi \n
	while (buffer[strlen(buffer) - 1] == '\r' ||
		buffer[strlen(buffer) - 1] == '\n') {
		buffer[strlen(buffer) - 1] = 0;
	}

	//tach [user password]
	char user[1024];
	memset(user, 0, sizeof(user));
	char password[1024];
	memset(password, 0, sizeof(password));
	sscanf(buffer, "%s%s", user, password);  //tach xau theo dinh dang

	//so sanh voi user co san trong file
	FILE* f = fopen("users.txt", "r");

	int matched = 0;
	while (!feof(f)) {
		char line[1024];
		memset(line, 0, sizeof(line));
		fgets(line, sizeof(line), f); //doc theo dong

		char _u[1024];
		memset(_u, 0, sizeof(_u));
		char _p[1024];
		memset(_p, 0, sizeof(_p));

		sscanf(line, "%s%s", _u, _p);
		if (strcmp(user, _u) == 0 && strcmp(password, _p) == 0) { //tim thay trong file
			matched = 1;
			break;
		}
	}
	fclose(f);

	if (matched == 0) {
		char failed[] = "Failed to login\n";  // char* failed = (char*)"Failed to login\n";
		send(c, failed, sizeof(failed), 0);
		closesocket(c);
	}
	else {
		char succeeded[] = "Please send command to execute\n";  // char* succeeded = (char*)"Please send command to execute\n";
		int sze = sizeof(succeeded);
		send(c, succeeded, sizeof(succeeded), 0);

		memset(buffer, 0, sizeof(buffer));
		recv(c, buffer, sizeof(buffer), 0);
		while (buffer[strlen(buffer) - 1] == '\r' ||
			buffer[strlen(buffer) - 1] == '\n') {
			buffer[strlen(buffer) - 1] = 0;
		}
		sprintf(buffer + strlen(buffer), "%s", "> result.txt"); //noi xau theo dinh dang
		
		//prevent conflict 
		EnterCriticalSection(&cs);

		system(buffer); //thuc hien lenh trong buffer roi redirect ket qua vao result.txt

		FILE* f = fopen("result.txt", "rb"); //doc dang binary
		fseek(f, 0, SEEK_END); //dua con tro ve cuoi file
		int flen = ftell(f); //lay vi tri con tro = so byte
		fseek(f, 0, SEEK_SET); //dua con tro ve lai dau file
		char* fdata = (char*)calloc(flen, 1); //moi ptu la 1 byte
		fread(fdata, 1, flen, f); //doc tu file f flen ptu, moi ptu 1 byte, luu kq vao fdata
		fclose(f);

		LeaveCriticalSection(&cs);

		send(c, fdata, flen, 0); //ko dung strlen(fdata) vi co the co byte 0 o giua
		free(fdata);

		//doc dang text thi doc theo tung dong va gui tung dong

		closesocket(c);
	}
	return 0;
}