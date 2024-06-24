#pragma once

#include <windows.h>
#include <tchar.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream> 
#include <iostream> 
#include <sstream>
#include <unordered_map>
#include <cstdlib> 
#include <time.h>


using namespace std;

//Permitir que o mesmo c�digo possa funcionar para ASCII ou UNICODE
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

#define COMPANIES_FILE _T("Companies.txt")

#define CLIENTS_FILE _T("Clients.txt")

#define REGISTRY_KEY_PATH _T("TPSO2_2324\\Bolsa")

#define REGISTRY_VALUE_NAME _T("NCLIENTES")

#define MAX_CLIENTES 5

#define MAX_COMPANIES 30



//#define MAX_CONNECTED_CLIENTS 5

#define ARBITARRY_LENGTH 20

#define N_SALES 5

#define LOGIN_SEMAPHORE _T("Login_Semaphore")


#define SHARED_MEMORY_NAME _T("SharedMemory")
#define SHARED_MEMORY_MUTEX_NAME _T("SharedMemoryMutex")
#define SHARED_MEMORY_EVENT _T("SharedMmemoryEvent")
#define SHUT_DOWN_EVENT _T("ShutDownEvent")






typedef struct {
	tstring client_name;
	tstring company_name;
	double num_stocks;
}SellOrder;


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
	tstring name;
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

	HANDLE login_semaphore;

	int num_companies;
	vector<Company> companies_list;
	HANDLE hMutex_companies_list; //MUTEX for the list of companies

	int num_clients;
	vector<Client> clients_list;
	HANDLE hMutex_clients_list;

	int sales;// this variable is used for the price changes
	vector<SellOrder> sell_orders;
	HANDLE hMutex_sellOrders_list;

	unordered_map<tstring, unordered_map<tstring, double>> wallets;//HashMap of wallets
	HANDLE hMutex_mapOfWallets;


	SharedMemoryCapsule* capusle;

	HANDLE hSharedMemoryEvent;


	int seconds;//sleep for n seconds
	HANDLE hMutexSleep; //mutex for sleeping


	HANDLE hShut_down_event;//Event for shut down


}MarketInfo;



typedef struct {
	HANDLE hPipe;
	MarketInfo* market_info;
}ArgumentsForHandleClient;





int updateSharedMemory(MarketInfo& market_info, Transaction* transaction);

