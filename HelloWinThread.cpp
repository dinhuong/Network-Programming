#include<stdio.h>
#include<Windows.h>
#include<conio.h>

DWORD WINAPI myThread(LPVOID param) {
	printf("hello\n", GetCurrentThreadId);
	printf("hello\n");
	return 0;
}

void main() {
	HANDLE hThread = CreateThread(NULL, 0, myThread, NULL, 0,NULL);
	printf();
	getch();
}