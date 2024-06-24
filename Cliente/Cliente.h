#pragma once


#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
using namespace std;
//Permitir que o mesmo código possa funcionar para ASCII ou UNICODE
#ifdef UNICODE 
#define tcout wcout
#define tcin wcin
#define tstring wstring
#else
#define tcout cout
#define tcin cin
#define tstring string
#endif

#define PIPE_NAME _T("\\\\.\\pipe\\TP_SO_PIPE")

#define BUFFERSIZE 1024

#define LOGIN_SEMAPHORE _T("Login_Semaphore")

#define SHUT_DOWN_EVENT _T("ShutDownEvent")

#define ARBITARRY_LENGTH 20



typedef struct {
	TCHAR name[ARBITARRY_LENGTH];
	TCHAR password[ARBITARRY_LENGTH];
	double balance;
	BOOL connected;
}Client;



typedef struct {
	TCHAR message[BUFFERSIZE];
	Client client;
}PipeMessage;


typedef struct {

	HANDLE login_semaphore;

	HANDLE hPipe;

	HANDLE hMutex;
	int loop;

}ProgramInfo;


