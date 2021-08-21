#include<stdio.h>
#include<WinSock2.h>

#define MAX_CLIENT 1024

SOCKET g_sockets[MAX_CLIENT];
WSAEVENT g_events[MAX_CLIENT];
char g_wd[MAX_CLIENT][1000];
unsigned char g_ip1[MAX_CLIENT];
unsigned char g_ip2[MAX_CLIENT];
unsigned char g_ip3[MAX_CLIENT];
unsigned char g_ip4[MAX_CLIENT];
unsigned char g_p1[MAX_CLIENT];
unsigned char g_p2[MAX_CLIENT];

int count = 0;
SOCKET g_datasocket;

char* Month(int month) {
	char* result = (char*)calloc(10, 1);
	switch (month) {
	case 1:
		strcpy(result, "Jan");
		break;
	case 2:
		strcpy(result, "Feb");
		break;
	case 3:
		strcpy(result, "Mar");
		break;
	case 4:
		strcpy(result, "Apr");
		break;
	case 5:
		strcpy(result, "May");
		break;
	case 6:
		strcpy(result, "Jun");
		break;
	case 7:
		strcpy(result, "Jul");
		break;
	case 8:
		strcpy(result, "Aug");
		break;
	case 9:
		strcpy(result, "Sep");
		break;
	case 10:
		strcpy(result, "Oct");
		break;
	case 11:
		strcpy(result, "Nov");
		break;
	case 12:
		strcpy(result, "Dec");
		break;
	}
	return result;
}

char* ScanFolder(const char* folder) {
	char* result = NULL;
	char line[1024];

	char path[1024];
	memset(path, 0, sizeof(path));
	if (folder[strlen(folder) - 1] == '/') {
		sprintf(path, "C:%s*.*", folder);
	}
	else {
		sprintf(path, "C:%s/*.*", folder);
	}

	WIN32_FIND_DATAA FindData;
	HANDLE hFind = FindFirstFileA(path, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		//check if folder
		SYSTEMTIME st;
		FileTimeToSystemTime(&FindData.ftLastWriteTime, &st);
		memset(line, 0, sizeof(line));

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			sprintf(line, "type=dir;modify=%4d%02d%02d%02d%02d%02d; %s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
		}
		else {
			sprintf(line, "type=file;modify=%4d%02d%02d%02d%02d%02d; %s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
		}
		int oldlen = result == NULL ? 0 : strlen(result);
		result = (char*)realloc(result, oldlen + sizeof(line) + 1);
		strcpy(result + oldlen, line);

		while (FindNextFileA(hFind, &FindData)) {
			SYSTEMTIME st;
			FileTimeToSystemTime(&FindData.ftLastWriteTime, &st);
			memset(line, 0, sizeof(line));

			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				sprintf(line, "type=dir;modify=%4d%02d%02d%02d%02d%02d; %s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
			}
			else {
				sprintf(line, "type=file;modify=%4d%02d%02d%02d%02d%02d; %s\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, FindData.cFileName);
			}
			int oldlen = result == NULL ? 0 : strlen(result);
			result = (char*)realloc(result, oldlen + sizeof(line) + 1);
			sprintf(result + oldlen, "%s", line);
		}
	}

	return result;
}

char* ScanFolder1(const char* folder) {
	char* result = NULL;
	char line[1024];

	char path[1024];
	memset(path, 0, sizeof(path));
	if (folder[strlen(folder) - 1] == '/') {
		sprintf(path, "C:%s*.*", folder);
	}
	else {
		sprintf(path, "C:%s/*.*", folder);
	}

	WIN32_FIND_DATAA FindData;
	HANDLE hFind = FindFirstFileA(path, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		//check if folder
		SYSTEMTIME st;
		FileTimeToSystemTime(&FindData.ftLastWriteTime, &st);

		memset(line, 0, sizeof(line));

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			char* month = Month(st.wMonth);
			sprintf(line, "drwx------ 1 ftp ftp 0 %s %02d %04d %s\r\n", month, st.wDay, st.wYear, FindData.cFileName);
			free(month);
		}
		else {
			char* month = Month(st.wMonth);
			__int64 size = (__int64)(FindData.nFileSizeHigh);
			size = size << 32;
			size = size | FindData.nFileSizeLow;
			sprintf(line, "-rwx------ 1 ftp ftp %lld %s %02d %04d %s\r\n", size, month, st.wDay, st.wYear, FindData.cFileName);
			free(month);
		}
		int oldlen = result == NULL ? 0 : strlen(result);
		result = (char*)realloc(result, oldlen + sizeof(line) + 1);
		strcpy(result + oldlen, line);

		while (FindNextFileA(hFind, &FindData)) {
			SYSTEMTIME st;
			FileTimeToSystemTime(&FindData.ftLastWriteTime, &st);
			memset(line, 0, sizeof(line));

			if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				char* month = Month(st.wMonth);
				sprintf(line, "drwx------ 1 ftp ftp 0 %s %02d %04d %s\r\n", month, st.wDay, st.wYear, FindData.cFileName);
				free(month);
			}
			else {
				char* month = Month(st.wMonth);
				__int64 size = (__int64)(FindData.nFileSizeHigh);
				size = size << 32;
				size = size | FindData.nFileSizeLow;
				sprintf(line, "-rwx------ 1 ftp ftp %lld %s %02d %04d %s\r\n", size, month, st.wDay, st.wYear, FindData.cFileName);
				free(month);
			}
			int oldlen = result == NULL ? 0 : strlen(result);
			result = (char*)realloc(result, oldlen + sizeof(line) + 1);
			sprintf(result + oldlen, "%s", line);
		}
	}
	return result;
}

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
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
					if (g_sockets[i] == s) { //co ket noi den tren command port
						SOCKADDR_IN caddr;
						int clen = sizeof(caddr);
						SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
						HANDLE e = WSACreateEvent();
						g_sockets[count] = c;
						g_events[count] = e;
						WSAEventSelect(g_sockets[count], g_events[count], FD_READ | FD_CLOSE);

						//gui phan hoi server ready cho client
						char* response = (char*)"This is a sample FTP server\r\n220 OK\r\n";
						send(g_sockets[count], response, strlen(response), 0);
						count++;
					}
					else { //co ket noi den tren data port
						SOCKADDR_IN daddr;
						int dlen = sizeof(daddr);
						g_datasocket = accept(g_sockets[i], (sockaddr*)&daddr, &dlen); //g_datasocket se dung de truyen du lieu
					}
				}
			}

			if (networkEvent.lNetworkEvents & FD_READ) {
				if (networkEvent.iErrorCode[FD_READ_BIT] == 0) {
					char buffer[1024];
					memset(buffer, 0, sizeof(buffer));
					recv(g_sockets[i], buffer, sizeof(buffer), 0);
					//printf("%s", strlen(buffer));
					if (strncmp(buffer, "USER", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "331 Please input password\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PASS", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "230 Logged on\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);

						memset(g_wd[i], 0, sizeof(g_wd[i]));
						strcpy_s(g_wd[i], "/");
					}
					else if (strncmp(buffer, "SYST", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "215 UNIX emulated by FileZilla\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "FEAT", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "Features:\n MDTM\n REST STREAM \n MLST type*;size*;modify*;\n MLSD\n UTF8\n CLNT\n MFMT\n EPSV\n EPRT\n211 End\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "CLNT", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PWD", 3) == 0) {
						//thu muc hien tai (working directory) la thu muc nao
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "257 \"%s\" is current working directory.\r\n", g_wd[i]);
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "TYPE", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "noop", 4) == 0) {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PASV", 4) == 0) {
						//PASV
						//nghe o cong 9999
						g_datasocket = INVALID_SOCKET;
						SOCKADDR_IN daddr;
						daddr.sin_family = AF_INET;
						daddr.sin_port = htons(9999);
						daddr.sin_addr.S_un.S_addr = INADDR_ANY;
						SOCKET datasocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						bind(datasocket, (sockaddr*)&daddr, sizeof(daddr));
						listen(datasocket, 10);
						g_sockets[count] = datasocket;
						g_events[count] = WSACreateEvent();
						WSAEventSelect(g_sockets[count], g_events[count], FD_ACCEPT);
						count++;

						//phan hoi client la server da listen o port 9999
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "227 Entering Passive Mode (127,0,0,1,39,15)\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "LIST", 4) == 0) {
						//phan hoi ma 150 cho client
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);

						char* ftpdata = ScanFolder1(g_wd[i]);
						//gui du lieu ftpdata o tren socket duoc accept tu cong 9999
						if (g_datasocket != INVALID_SOCKET) {
							send(g_datasocket, ftpdata, strlen(ftpdata), 0);
							closesocket(g_datasocket);
							g_datasocket = INVALID_SOCKET;
						}

						free(ftpdata);
						ftpdata = NULL;

						//phan hoi ma 226 cho client
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "226 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "PORT", 4) == 0) {
						//PORT 127,0,0,1,215,52
						//PORT client ip, data port
						//tach port tu buffer va tao kenh truyen du lieu

						for (int j = 0; j < strlen(buffer); j++) {
							if (buffer[j] == ',') buffer[j] = ' ';
						}
						char port[8];
						sscanf(buffer, "%s%d%d%d%d%d%d", port, &g_ip1[i], &g_ip2[i], &g_ip3[i], &g_ip4[i], &g_p1[i], &g_p2[i]);
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "MLSD", 4) == 0) {
						short dataport = g_p1[i] * 256 + g_p2[i];
						SOCKET datasocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN dsadd;
						dsadd.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						dsadd.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						dsadd.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						dsadd.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						dsadd.sin_family = AF_INET;
						dsadd.sin_port = htons(dataport);

						if (connect(datasocket, (sockaddr*)&dsadd, sizeof(dsadd)) == 0) {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);

							char* ftpdata = ScanFolder(g_wd[i]);
							send(datasocket, ftpdata, strlen(ftpdata), 0);
							closesocket(datasocket);

							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 Successfully transferred\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);

						}
						else {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "425 Cant open data connection\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "CWD", 3) == 0) {
						char wd[1024];
						memset(wd, 0, sizeof(wd));
						while (buffer[strlen(buffer) - 1] == '\r' || buffer[strlen(buffer) - 1] == '\n') {
							buffer[strlen(buffer) - 1] = 0;
						}
						strcpy(wd, buffer + 4);

						if (wd[0] == '/') { //absolute path
							strcpy(g_wd[i], wd);
						}
						else { //relative path
							if (strcmp(g_wd[i], "/") == 0) {
								sprintf(g_wd[i] + strlen(g_wd[i]), "%s", wd);
							}
							else {
								sprintf(g_wd[i] + strlen(g_wd[i]), "/%s", wd);
							}
						}
						
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "250 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "CDUP", 4) == 0){
						for (int j = strlen(g_wd[i]) - 1;j >= 0; j--) {
							if (g_wd[i][j] == '/') {
								g_wd[i][j] = 0;
								break;
							}
						}
						if (strlen(g_wd[i]) == 0) strcpy(g_wd[i],"/");
						printf("%s", g_wd[i]);
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "250 OK\r\n");
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "RETR", 4) == 0) {
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\r' || fname[strlen(fname) - 1] == '\n') {
							fname[strlen(fname) - 1] = 0;
						}

						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "%s/%s", g_wd[i], fname);

						short dataport = g_p1[i] * 256 + g_p2[i];
						SOCKET datasocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN dsadd;
						dsadd.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						dsadd.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						dsadd.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						dsadd.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						dsadd.sin_family = AF_INET;
						dsadd.sin_port = htons(dataport);

						if (connect(datasocket, (sockaddr*)&dsadd, sizeof(dsadd)) == 0) {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);

							char fdata[1024];
							FILE* f = fopen(fullpath, "rb");
							while (!feof(f)) {
								int r = fread(fdata, 1, sizeof(fdata), f);
								if (r > 0) {
									send(datasocket, fdata, r, 0); //luu y: fdata la du lieu dang binary
								}
							}
							fclose(f);
							closesocket(datasocket);

							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 Successfully transferred\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
						else {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "425 Cant open data connection\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "STOR", 4) == 0) {
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\r' || fname[strlen(fname) - 1] == '\n') {
							fname[strlen(fname) - 1] = 0;
						}

						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "%s/%s", g_wd[i], fname);

						short dataport = g_p1[i] * 256 + g_p2[i];
						SOCKET datasocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						SOCKADDR_IN dsadd;
						dsadd.sin_addr.S_un.S_un_b.s_b1 = g_ip1[i];
						dsadd.sin_addr.S_un.S_un_b.s_b2 = g_ip2[i];
						dsadd.sin_addr.S_un.S_un_b.s_b3 = g_ip3[i];
						dsadd.sin_addr.S_un.S_un_b.s_b4 = g_ip4[i];
						dsadd.sin_family = AF_INET;
						dsadd.sin_port = htons(dataport);

						if (connect(datasocket, (sockaddr*)&dsadd, sizeof(dsadd)) == 0) {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "150 OK\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);

							char fdata[1024];
							FILE* f = fopen(fullpath, "wb");
							while (!feof(f)) {
								int r = recv(datasocket, fdata, sizeof(fdata), 0);
								if (r > 0) {
									fwrite(fdata, 1, r, f);
								}
								else {
									break;
								}
							}
							fclose(f);
							closesocket(datasocket);

							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "226 Successfully transferred\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
						else {
							memset(buffer, 0, sizeof(buffer));
							sprintf_s(buffer, sizeof(buffer), "425 Cant open data connection\r\n");
							send(g_sockets[i], buffer, strlen(buffer), 0);
						}
					}
					else if (strncmp(buffer, "SIZE", 4) == 0) {
						char* fname = buffer + 5;
						while (fname[strlen(fname) - 1] == '\r' || fname[strlen(fname) - 1] == '\n') {
							fname[strlen(fname) - 1] = 0;
						}

						char fullpath[1024];
						memset(fullpath, 0, sizeof(fullpath));
						sprintf(fullpath, "%s/%s", g_wd[i], fname);

						FILE* f = fopen(fullpath, "rb");
						fseek(f, 0, SEEK_END);
						long fsize = ftell(f);
						fclose(f);

						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "213 %d\r\n", fsize);
						send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else if (strncmp(buffer, "DELE", 4) == 0) {
					char* fname = buffer + 5;
					while (fname[strlen(fname) - 1] == '\r' || fname[strlen(fname) - 1] == '\n') {
						fname[strlen(fname) - 1] = 0;
					}

					char fullpath[1024];
					memset(fullpath, 0, sizeof(fullpath));
					sprintf(fullpath, "%s/%s", g_wd[i], fname);

					remove(fullpath);

					memset(buffer, 0, sizeof(buffer));
					sprintf_s(buffer, sizeof(buffer), "200 OK\r\n");
					send(g_sockets[i], buffer, strlen(buffer), 0);
					}
					else {
						memset(buffer, 0, sizeof(buffer));
						sprintf_s(buffer, sizeof(buffer), "202 Command not implemented.\r\n");
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