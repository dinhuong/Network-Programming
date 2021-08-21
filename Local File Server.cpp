#include<stdio.h>
#include<WinSock2.h>

void Concat(char** phtml, char* string) {
	char* html = *phtml;
	int oldlen = html == NULL ? 0 : strlen(html);
	int tmplen = strlen(string);

	//ko hieu tai sao html ko cap phat dc ma phai dung *phtml
	*phtml = (char*)realloc(html, oldlen + tmplen + 1);
	html = *phtml;
	memset(html + oldlen, 0, tmplen + 1);

	sprintf(html + oldlen, "%s", string);
}

char* ScanFolder(char* folder) {  // "/"
	char* html = (char*)calloc(1024,1);
	sprintf(html, "%s", "<html>");

	char findpath[1024]; // search pattern "C:\*.*"
	memset(findpath, 0, sizeof(findpath));

	if (folder[strlen(folder) - 1] == '/') {
		sprintf(findpath, "C:%s*.*", folder); 
	}
	else {
		sprintf(findpath, "C:%s/*.*", folder);

	} 
	
	Concat(&html, (char*)"<form action=\"\/\" method=\"POST\"><input type=\"file\"><input type=\"submit\"><\/form>");

	WIN32_FIND_DATAA FindData;
	HANDLE hFind = FindFirstFileA(findpath, &FindData);
	if (hFind != INVALID_HANDLE_VALUE) {
		Concat(&html, (char*)"<a href=\"");

		Concat(&html, folder);
		if (folder[strlen(folder) - 1] != '/') {
			Concat(&html, (char*)"\/");
		}

		Concat(&html, FindData.cFileName);
		Concat(&html, (char*)"\">");
		Concat(&html, FindData.cFileName);
		Concat(&html, (char*)"</a><br>");

		while (FindNextFileA(hFind, &FindData)) {
			Concat(&html, (char*)"<a href=\"");

			Concat(&html, folder);
			if (folder[strlen(folder) - 1] != '/') {
				Concat(&html, (char*)"\/");
			}

			Concat(&html, FindData.cFileName);
			Concat(&html, (char*)"\">");
			Concat(&html, FindData.cFileName);
			Concat(&html, (char*)"</a><br>");
		}
		CloseHandle(hFind);
	}
	Concat(&html, (char*)"</html>");
	return html;
}

int IsFolder(char* path) {
	if (path[strlen(path) - 1] == '/') {
		path[strlen(path) - 1] = 0; //bo gau "/" o cuoi path
	}
	char findpath[1024]; // search pattern "C:\*.*"
	memset(findpath, 0, sizeof(findpath));
	sprintf(findpath, "C:\\%s", path); //noi xau theo dinh dang minh muon, thay cho cach dung strcat

	if (findpath[strlen(findpath) - 1] == '/') {
		findpath[strlen(findpath) - 1] = 0;
	}

	WIN32_FIND_DATAA FindData;
	HANDLE hFind = FindFirstFileA(findpath, &FindData);

	if (hFind != INVALID_HANDLE_VALUE) {
		CloseHandle(hFind);
		if ( (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (!(FindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) ){
			return 1; //FOLDER
		}
		else if ( (FindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) &&(!(FindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)) ) {
			return 0; //FILE
		}
	}
	return -1;
}

void ProcessSpace(char* string) { // "/Temp/FTP%20Server"
	char* space = NULL;
	do {
		space = strstr(string, "%20"); //return "%20Server"
		if (space != NULL) {
			space[0] = 32; // " 20Server"
			strcpy(space + 1, space + 3); // " Server"
		}

	} while (space != NULL);
}

int FindBoundary(char* content, int len, char* boundary) {
	int start = strlen(boundary + 1);
	for (int i = start; i < len; i++) {
		if (content[i] == boundary[0]) {
			int found = 1;
			for (int j = 0; j < strlen(boundary); j++) {
				if (content[i + j] != boundary[j]) {
					found = 0;
					break;
				}
			}
			if (found == 1) {
				return i; //tim thay boundary o vi tri i trong mang du lieu nhi phan
			}
		}
	}
	return 0;
}

DWORD WINAPI ClientThread(LPVOID param) {
	SOCKET c = (SOCKET)param;
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	recv(c, buffer, sizeof(buffer), 0);

	char method[1024];
	memset(method, 0, sizeof(method));
	char version[1024];
	memset(version, 0, sizeof(version)); 
	char path[1024];
	memset(path, 0, sizeof(path));
	sscanf(buffer, "%s%s%s", method, path, version);  //tach xau 

	ProcessSpace(path);  //thay "%20" thanh " "

	if (strcmp(method, "GET") == 0)
	{
		if (strcmp(path, "/") == 0)
		{
			char* html = ScanFolder(path);
			char* response = (char*)calloc(strlen(html) + 1024, 1);
			sprintf(response, "HTTP/1.1 200 OK\r\nServer: MYLOCAl\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s", strlen(html), html);
			send(c, response, strlen(response), 0);
			closesocket(c);
			free(response);
			response = NULL;
			free(html);
			html = NULL;
		}
		else {
			if (IsFolder(path) == -1) {
				char html[1024];
				memset(html, 0, sizeof(html));
				strcpy(html, "<html>404 Not Found</html>");

				char* response = (char*)calloc(strlen(html) + 1024, 1);
				sprintf(response, "HTTP/1.1 404 Not found\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s", strlen(html), html);
				send(c, response, strlen(response), 0);
				closesocket(c);
				free(response);
			}
			else if (IsFolder(path) == 0) {
				char type[1024];
				memset(type, 0, sizeof(type));
				if (strlen(path) > 4) {
					if (strcmp(path + strlen(path) - 4, ".mp4") == 0) {
						strcpy(type, "video/mp4");
					}
					else if (strcmp(path + strlen(path) - 4, ".mp3") == 0) {
						strcpy(type, "audio/mp3");
					}
					else if (strcmp(path + strlen(path) - 4, ".jpg") == 0) {
						strcpy(type, "image/jpg");
					}
					else if (strcmp(path + strlen(path) - 4, ".txt") == 0) {
						strcpy(type, "text/html");
					}
					else {
						strcpy(type, "application/octet-stream");
					}
				}
				else {
					strcpy(type, "application/octet-stream");
				}

				char fullpath[1024];
				memset(fullpath, 0, sizeof(fullpath));
				sprintf(fullpath, "C:\\%s", path);
				FILE* f = fopen(fullpath, "rb");
				fseek(f, 0, SEEK_END); //con tro nhay toi cuoi file
				int flen = ftell(f); //lay vi tri con tro file dang o cuoi file -> do dai file
				fseek(f, 0, SEEK_SET);
				char* data = (char*)calloc(flen, 1);
				if (data != NULL) {
					fread(data, 1, flen, f);
					char* response = (char*)calloc(flen + 1024, 1);
					sprintf(response, "HTTP/1.1 200 OK\r\nServer: MYLOCAl\r\nContent-Type: %s\r\nContent-Length:%d\r\n\r\n", type, flen);
					send(c, response, strlen(response), 0);

					int byteSent = 0;
					while (byteSent < flen) {
						byteSent += send(c, data + byteSent, flen - byteSent, 0);
					}

					free(response);
					free(data);
					closesocket(c); //ko can doi socket timeout
				}
				fclose(f);
			}
			else {
				char* html = ScanFolder(path);
				char* response = (char*)calloc(strlen(html) + 1024, 1);
				sprintf(response, "HTTP/1.1 200 OK\r\nServer: MYLOCAl\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s", strlen(html), html);
				send(c, response, strlen(response), 0);
				closesocket(c);
				free(response);
				response = NULL;
				free(html);
				html = NULL;
			}
		}
	}
	else if (strcmp(method, "POST") == 0) {

		if (strstr(buffer, "Content-Length:")) {
			
			char tmp[1024];
			memset(tmp, 0, sizeof(tmp));
			int len = 0;
			sscanf(strstr(buffer, "Content-Length:"), "%s%d", tmp);
			char* content = (char*)calloc(len, 1);
			int r = 0;
			while (r < len) {
				r += recv(c, content+r, len-r, 0);
			}

			//tach filename
			char* fnamestart = strstr(content, "filename=\"") + strlen("filename=\"");
			char* fnamestop = strstr(fnamestart, "\"")-1; 
			char fname[1024];
			memset(fname, 0, sizeof(fname));
			strcpy(fname, "d:\\LTM\\");
			memcpy(fname+strlen(fname), fnamestart, fnamestop - fnamestart + 1);

			//boc du lieu
			sscanf(content, "%s", tmp); //tmp = boundary
			int start = strstr(content, "\r\n\r\n") + 4 - content; 
			int end = FindBoundary(content, len, tmp) - 3; //tu nhien bi thua 3 bytes o cuoi?
			FILE* f = fopen(fname, "wb");
			fwrite(content + start, 1, end - start + 1, f);
			fclose(f);

			//send reponse 200 OK
			char html[1024];
			memset(html, 0, sizeof(html));
			strcpy(html, "<html>File Uploaded<\/html>");
			char* response = (char*)calloc(strlen(html) + 1024, 1);
			sprintf(response, 
				"HTTP/1.1 200 OK\r\nServer: MYLOCAl\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n%s",
				strlen(html), html);
			send(c, response, strlen(response), 0);

			closesocket(c);
		}
	}
	else {

	}
	return 0;
}

void main() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
	SOCKADDR_IN saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8888);
	saddr.sin_addr.S_un.S_addr = 0;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	bind(s, (sockaddr*)&saddr, sizeof(saddr));
	listen(s, 10);

	while (true) {
		SOCKADDR_IN caddr;
		int clen = sizeof(caddr);
		SOCKET c = accept(s, (sockaddr*)&caddr, &clen);
		CreateThread(NULL, 0, ClientThread, (LPVOID) c, 0, NULL);
	}
}