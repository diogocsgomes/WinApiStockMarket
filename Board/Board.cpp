
#include "Board.h"


bool compareByStockPrice(const Company& a, const Company& b) {
	return a.stock_price > b.stock_price;
}

void sortCompaniesByStockPrice(Company* companies, int size) {
										//Descending order
	sort(companies, companies + size, compareByStockPrice);
}

void printFirstTime(MainInfo &main_info)
{
	Company companies[MAX_COMPANIES];
	Transaction last_transaction;
	int num_companies;

	system("cls");
	WaitForSingleObject(main_info.capsule->hMutex, INFINITE);

	num_companies = main_info.capsule->shared_memory->num_companies;

	for (int i = 0; i < num_companies; i++)
	{
		_tcscpy_s(companies[i].name, main_info.capsule->shared_memory->companies[i].name);


		companies[i].num_stocks = main_info.capsule->shared_memory->companies[i].num_stocks;
		companies[i].stock_price = main_info.capsule->shared_memory->companies[i].stock_price;
	}

	_tcscpy_s(last_transaction.client_name, main_info.capsule->shared_memory->last_transaction.client_name);


	_tcscpy_s(last_transaction.company_name, main_info.capsule->shared_memory->last_transaction.company_name);


	last_transaction.num_stocks = main_info.capsule->shared_memory->last_transaction.num_stocks;
	last_transaction.value = main_info.capsule->shared_memory->last_transaction.value;

	ReleaseMutex(main_info.capsule->hMutex);


	sortCompaniesByStockPrice(companies, num_companies);

	tcout << _T("Last transaction made by: ") << last_transaction.client_name << _T("\n ") << last_transaction.company_name << _T(" ") << last_transaction.num_stocks << _T(" stocks at ") << last_transaction.value << _T("€") << endl;


	tcout << _T("-------- -------- --------") << endl;
	for (int i = 0; i < num_companies && i < main_info.num_companies; i++)
	{
		tcout << companies[i].name << _T(" Num stock: ") << companies[i].num_stocks << _T(" Stock price: ") << companies[i].stock_price << endl;
	}
	tcout << _T("-------- -------- --------") << endl;

}

DWORD WINAPI printBoard(LPVOID args)
{
	//SharedMemoryCapsule* capsule = (SharedMemoryCapsule*)args;
	MainInfo* main_info = (MainInfo*)args;
	SharedMemoryCapsule* capsule = main_info->capsule;
	HANDLE hEvent;
	Company companies[MAX_COMPANIES];
	Transaction last_transaction;
	int num_companies;
	int loop = 1;


	hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SHARED_MEMORY_EVENT);
	if (hEvent == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR opening event - ") << GetLastError() << endl;
		return -1;
	}

	printFirstTime(*main_info);

	while (true)
	{

		/*WaitForSingleObject(main_info->hMutex, INFINITE);
		if (main_info->loop == 0)
			break;

		ReleaseMutex(main_info->hMutex);*/

		WaitForSingleObject(hEvent, INFINITE);
		
		WaitForSingleObject(capsule->hMutex, INFINITE);
		system("cls");
		num_companies = capsule->shared_memory->num_companies;

		for (int i = 0; i < num_companies; i++)
		{
			//companies[i].name = tstring(capsule->shared_memory->companies[i].name);
			_tcscpy_s(companies[i].name, capsule->shared_memory->companies[i].name);
			

			companies[i].num_stocks = capsule->shared_memory->companies[i].num_stocks;
			companies[i].stock_price = capsule->shared_memory->companies[i].stock_price;
		}

		//last_transaction.client_name = capsule->shared_memory->last_transaction.client_name;
		_tcscpy_s(last_transaction.client_name, capsule->shared_memory->last_transaction.client_name);

		//last_transaction.company_name = capsule->shared_memory->last_transaction.company_name;

		_tcscpy_s(last_transaction.company_name, capsule->shared_memory->last_transaction.company_name);
		
		
		last_transaction.num_stocks = capsule->shared_memory->last_transaction.num_stocks;
		last_transaction.value = capsule->shared_memory->last_transaction.value;

		ReleaseMutex(capsule->hMutex);


		sortCompaniesByStockPrice(companies, num_companies);

		tcout << _T("Last transaction made by: ") << last_transaction.client_name << _T("\n ") << last_transaction.company_name << _T(" ") << last_transaction.num_stocks << _T(" stocks at ") << last_transaction.value << _T("€") << endl;


		tcout << _T("-------- -------- --------") << endl;
		for (int i = 0; i < num_companies && i < main_info->num_companies; i++)
		{
			tcout << companies[i].name << _T(" Num stock: ") << companies[i].num_stocks << _T(" Stock price: ") << companies[i].stock_price << endl;
		}
		tcout << _T("-------- -------- --------") << endl;

		ResetEvent(hEvent);

	}

	return 0;
}


int openSharedMemory(SharedMemoryCapsule & capsule) {


	// Create the shared memory object
	capsule.hSharedMemory = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		SHARED_MEMORY_NAME
	);

	if (capsule.hSharedMemory == NULL) {
		tcout << _T("ERROR opening shared memory object - ") << GetLastError() << endl;

		return -1;
	}

	// Mapp the shared memory to the address space of the process
	capsule.shared_memory = (SharedMemory*)MapViewOfFile(
		capsule.hSharedMemory,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SharedMemory)
	);

	if (capsule.shared_memory == NULL) {
		tcout << _T("ERROR mapping shared memory to current adress space - ") << GetLastError() << endl;
		CloseHandle(capsule.hSharedMemory);
		return -1;
	}

	// Create Mutex
	capsule.hMutex = OpenMutex(SYNCHRONIZE, FALSE, SHARED_MEMORY_MUTEX_NAME);;

	if (capsule.hMutex == INVALID_HANDLE_VALUE) {
		tcout << _T("ERROR opening mutex - ") << GetLastError();
		UnmapViewOfFile(capsule.shared_memory);
		CloseHandle(capsule.hSharedMemory);
		return -1;
	}

	return 1;
}

void closeSharedMemory(SharedMemoryCapsule* capsule)
{
	if (capsule == NULL)
		return;

	CloseHandle(capsule->hMutex);
	UnmapViewOfFile(capsule->shared_memory);
	CloseHandle(capsule->hSharedMemory);
	delete capsule;
	return;
}

int processCommand(tstring string, vector<tstring>& words) {
	if (string.empty())
		return 0;

	int length = string.size();
	int numWords = 0;




	for (int i = 0; i < length; i++)
	{
		if (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
			continue;
		numWords++;
		tstring word;
		for (i; i < length; i++)
		{
			if (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
				break;
			word += string[i];

		}

		words.push_back(word);


	}

	return numWords;
}

int getCommand(const tstring& command,MainInfo &main_info) {

	//tstring command;
	vector<tstring> words;
	int maxNumWordsForCommand = 3; //including the command word

	//tcout << _T("Insert command: ");
	//getline(tcin, command);
	processCommand(command, words);


	/*if (words.size() > maxNumWordsForCommand)
	{
		tcout << _T("Invalid command") << endl;
		return;
	}*/
	if (words.size() == 0)
	{
		return -1;
	}

	tstring mainCommand = words.at(0);
	int numOfArguments = words.size() - 1;


	if (mainCommand == _T("exit"))
	{
		if (numOfArguments != 0)
		{
			
			return 1;
		}

		WaitForSingleObject(main_info.hMutex, INFINITE);

		main_info.loop = 0;

		ReleaseMutex(main_info.hMutex);


		return 0;


		
	}

	
	else {
		
		return 1;
	}

	return 1;

}

DWORD WINAPI waitForShutDown(LPVOID arg) {
	MainInfo* info = (MainInfo*)arg;
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE, SHUT_DOWN_EVENT);

	if (hEvent == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR opemimg event for shutdown - ") << GetLastError() << endl;
	}

	WaitForSingleObject(hEvent, INFINITE);

	closeSharedMemory(info->capsule);
	CloseHandle(hEvent);
	exit(0);


}


int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	MainInfo main_info;
	main_info.loop = 1;
	main_info.capsule = new SharedMemoryCapsule();


	//SharedMemoryCapsule* capsule = new SharedMemoryCapsule();
	HANDLE showInfoThread,hShutDownThread;




	if (argc != 2)
	{
		main_info.num_companies = MAX_COMPANIES_TO_SHOW;
	}
	else {
		try 
		{
			main_info.num_companies = _ttoi(argv[1]);
			if (main_info.num_companies > MAX_COMPANIES_TO_SHOW)
				main_info.num_companies = MAX_COMPANIES_TO_SHOW;
		}
		catch (exception e)
		{
			tcout << e.what() << endl;
			main_info.num_companies = MAX_COMPANIES_TO_SHOW;
		}
	}

	main_info.hMutex = CreateMutex(NULL, FALSE, NULL);

	if (main_info.hMutex == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating mutex for loop - ") << GetLastError() << endl;
		return -1;
	}

	if (openSharedMemory(*main_info.capsule) == -1)
	{
		CloseHandle(main_info.hMutex);
		delete main_info.capsule;
		return -1;
	}

	showInfoThread = CreateThread(NULL, 0, printBoard, &main_info, 0, NULL);
	if (showInfoThread == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating thread to show board - ") << GetLastError() << endl;
		CloseHandle(main_info.hMutex);
		closeSharedMemory(main_info.capsule);
		return -1;
	}


	hShutDownThread = CreateThread(NULL, 0, waitForShutDown, &main_info, 0, NULL);
	if (hShutDownThread == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating thread for shut down - ") << GetLastError() << endl;
		CloseHandle(showInfoThread);
		CloseHandle(main_info.hMutex);
		closeSharedMemory(main_info.capsule);
		return -1;
	}




	tcout << _T("Welcome type exit to leave") << endl;

	int a = 1;
	TCHAR aux[ARBITARRY_LENGTH];
	do
	{
		_fgetts(aux, ARBITARRY_LENGTH, stdin);
		a = getCommand(tstring(aux),main_info);
	} while (a == 1);



	//WaitForSingleObject(showInfoThread, INFINITE);

	CloseHandle(showInfoThread);
	
	closeSharedMemory(main_info.capsule);
	CloseHandle(main_info.hMutex);

	/*int a = 1;
	do
	{
		a = getCommand();
	} while (a == 1);*/

	

	return 0;
}