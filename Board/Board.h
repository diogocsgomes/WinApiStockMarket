#pragma once


#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
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



#define BUFFERSIZE 1024

#define MAX_COMPANIES 30

#define MAX_COMPANIES_TO_SHOW 10

#define ARBITARRY_LENGTH 20

#define SHARED_MEMORY_NAME _T("SharedMemory")

#define SHARED_MEMORY_MUTEX_NAME _T("SharedMemoryMutex")

#define SHARED_MEMORY_EVENT _T("SharedMmemoryEvent")

#define LOOP_MUTEX _T("LoopMutex")

#define SHUT_DOWN_EVENT _T("ShutDownEvent")



typedef struct {
	TCHAR name[ARBITARRY_LENGTH];
	double num_stocks;
	double stock_price;
}Company;


typedef struct {
	TCHAR name[ARBITARRY_LENGTH];
	double num_stocks;
	double stock_price;
}CompanyForShM;

typedef struct {
	TCHAR company_name[ARBITARRY_LENGTH];
	TCHAR client_name[ARBITARRY_LENGTH];
	double num_stocks;
	double value;
}Transaction;


typedef struct {

	//TCHAR example[10];
	int num_companies;
	CompanyForShM companies[MAX_COMPANIES];
	Transaction last_transaction;

}SharedMemory;


typedef struct {

	HANDLE hSharedMemory;
	HANDLE hMutex;

	SharedMemory* shared_memory;

}SharedMemoryCapsule;


typedef struct {

	HANDLE hMutex;

	int loop;

	SharedMemoryCapsule *capsule;

	int num_companies;

}MainInfo;
