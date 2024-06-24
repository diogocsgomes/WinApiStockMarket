#include "Bolsa.h"


int GetNClientesFromRegistry() {
	HKEY hKey;
	DWORD nClientes = MAX_CLIENTES; // Default value if not found
	DWORD dataSize = sizeof(nClientes);
	DWORD regType = REG_DWORD;
	LONG result;

	// Open the registry key
	result = RegOpenKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY_PATH, 0, KEY_READ | KEY_WRITE, &hKey);
	if (result == ERROR_SUCCESS) {

		// Try to read the value
		result = RegQueryValueEx(hKey, REGISTRY_VALUE_NAME, NULL, &regType, (LPBYTE)&nClientes, &dataSize);

		if (result != ERROR_SUCCESS || regType != REG_DWORD) {
			// If the value does not exist or is of the wrong type, set it to the default value
			nClientes = MAX_CLIENTES;
			RegSetValueEx(hKey, REGISTRY_VALUE_NAME, 0, REG_DWORD, (const BYTE*)&nClientes, sizeof(nClientes));
		}
	}
	else {
		// If the key does not exist, create it and set the default value
		result = RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY_PATH, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL);

		if (result == ERROR_SUCCESS) {
			RegSetValueEx(hKey, REGISTRY_VALUE_NAME, 0, REG_DWORD, (const BYTE*)&nClientes, sizeof(nClientes));
		}
	}

	// Close the registry key
	RegCloseKey(hKey);

	return nClientes;
}

void copyClient(Client& dest, const Client& src) {
	//dest.name = src.name;
	_tcscpy_s(dest.name, src.name);
	//dest.password = src.password;
	_tcscpy_s(dest.password, src.password);
	dest.balance = src.balance;
	dest.connected = src.connected;
}

void copyCompany(Company& dest, const Company& src)
{
	dest.name = src.name;
	dest.num_stocks = src.num_stocks;
	dest.stock_price = src.stock_price;
}

int tstringToInt(const tstring& string)
{
	for (WCHAR i : string)
	{
		if (!isdigit(i))
		{
			throw invalid_argument("Insert only positive integer nyumbers (ex: 12)");
		}
	}

	return stoi(string);
}

double tstringToDouble(const tstring& string) {
	
	int dotCounter = 0;

	for (WCHAR i : string)
	{
		if (!isdigit(i))
		{
			if (i == '.')
			{
				dotCounter++;
				if (dotCounter > 1)
				{
					throw invalid_argument("Invalid Argument use only one . ");
					//return -1.0;
				}
				continue;
			}
			if(i == ',')
			{
				throw invalid_argument("Invalid Argument use only numbers and . to seprerate them");
				//return -1.0;
			}
			throw invalid_argument("Invalid Argument make sure you use only valid and positive numbers (ex: 2.45)");

		}
		
	}

	return stod(string);
	
}

int stringToArrayOfStrings(tstring &string, vector<tstring>& words) {
	if (string.empty())
		return 0;
	
	int length = string.size();
	int numWords = 0;
	
	
	

	for (int i = 0; i < length ; i++)
	{
		if (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
			continue;
		numWords++;
		tstring word;
		for (i; i < length ; i++)
		{
			if (string[i] == ' ' || string[i] == '\n' || string[i] == '\t')
				break;
			word += string[i];
			
		}
		
		words.push_back(word);
		
		
	}

	return numWords;
}

int addCompany(Company& company, MarketInfo& market_info)
{
	Company company_to_add = Company();
	
	copyCompany(company_to_add, company);

	WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

	if (market_info.num_companies = market_info.companies_list.size() >= MAX_COMPANIES)
	{
		ReleaseMutex(market_info.hMutex_companies_list);
		return -1;
	}


	for (const Company c : market_info.companies_list)
	{
		if (c.name == company.name)
		{
			ReleaseMutex(market_info.hMutex_companies_list);
			return -1;
		}
	}


	//market_info.companies_list.push_back(company_to_add);

	market_info.companies_list.insert(market_info.companies_list.begin(), company_to_add);
	/*market_info.num_companies = market_info.companies_list.size();

	if (market_info.num_companies > MAX_COMPANIES)
	{
		market_info.companies_list.erase(market_info.companies_list.begin() + MAX_COMPANIES, market_info.companies_list.end());
	}*/


	wofstream file(COMPANIES_FILE);

	if (!file.is_open()) {
		tcout << _T("ERROR opening companies file for writting - ") << GetLastError() << endl;
		ReleaseMutex(market_info.hMutex_companies_list);
		return -1;
	}

	for (const Company& c : market_info.companies_list)
	{
		file << c.name << ' ' << c.num_stocks << ' ' << c.stock_price << '\n';
	}

	

	file.close();




	ReleaseMutex(market_info.hMutex_companies_list);

}

int defineStock(vector<tstring> &words,MarketInfo& market_info) {
	//UPDATES THE STOCK PRICE
	 //1 company name, 2 price of stock
	

	WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

	

	for (Company& c : market_info.companies_list) {
		if (c.name == words.at(1))
		{
			wofstream file(COMPANIES_FILE);

			if (!file.is_open()) {
				tcout << _T("ERROR opening companies file for writting was not able to update stock price - ") << GetLastError() << endl;
				ReleaseMutex(market_info.hMutex_companies_list);
				return -1;
			}
			c.stock_price = tstringToDouble(words.at(2));

			for (const Company& c2 : market_info.companies_list)
			{
				file << c2.name << ' ' << c2.num_stocks << ' ' << c2.stock_price << '\n';
			}

			ReleaseMutex(market_info.hMutex_companies_list);

			file.close();
			return 1;

		}
	}

	ReleaseMutex(market_info.hMutex_companies_list);
	
	tcout << _T("Company not found") << endl;
	return 1;
}

void printClients(MarketInfo& market_info)
{
	tcout << _T("Clients: ") << endl;
	WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);

	for (const Client& c : market_info.clients_list) {
		tcout << c.name << _T(" balance: ") << c.balance;
		if (c.connected == TRUE)
		{
			tcout << _T(" Online") << endl;
		}
		else if(c.connected == FALSE)
		{
			tcout << _T(" Offline") << endl;
		}
	}


	ReleaseMutex(market_info.hMutex_clients_list);

}

DWORD WINAPI pauseMarket(LPVOID arg) {
	MarketInfo* market_info = (MarketInfo*)arg;

	WaitForSingleObject(market_info->hMutexSleep, INFINITE);

	Sleep(market_info->seconds * 1000);

	ReleaseMutex(market_info->hMutexSleep);

	return 0;
}

int getCommand(MarketInfo &market_info) {

	tstring command;
	vector<tstring> words;
	int maxNumWordsForCommand = 4;

	tcout << _T("Insert command: ");
	getline(tcin, command);
	stringToArrayOfStrings(command,words);

	
	/*if (words.size() > maxNumWordsForCommand)
	{
		tcout << _T("Invalid command") << endl;
		return;
	}*/
	if (words.size() == 0)
	{
		return 1;
	}

	tstring mainCommand = words.at(0);
	int numOfArguments = words.size() - 1;

	if (mainCommand == _T("addc"))
	{
		if (numOfArguments != 3)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return 1;
		}

		//further filtration yet to be implemented

		double numStock;
		double stockPrice;

		try { 
			//numStock = stod(words.at(2));
			//stockPrice = stod(words.at(3));

			numStock = tstringToDouble(words.at(2));
			stockPrice = tstringToDouble(words.at(3));

			tcout << numStock << endl << stockPrice << endl;
		}
		catch (invalid_argument e)
		{
			tcout << e.what() << endl;
			return 1;
			}

		
		
		Company company;
		company.name = words.at(1);
		company.num_stocks = numStock;
		company.stock_price = stockPrice;

		
		if (addCompany(company, market_info) == -1)
		{
			tcout << _T("Could not add company");
			return 1;
		}
		updateSharedMemory(market_info, NULL);
		SetEvent(market_info.hSharedMemoryEvent);
		//tcout << _T("Valid command") << endl;



	}
	else if (mainCommand == _T("listc"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return 1;
		}

		//tcout << _T("Valid command") << endl;
		tcout << _T("Companies: ") << endl;
		WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

		for (const Company& c : market_info.companies_list) {
			tcout << c.name << _T(" ") << c.num_stocks << _T(" ") << c.stock_price << endl;
		}

		ReleaseMutex(market_info.hMutex_companies_list);



	}
	else if (mainCommand == _T("stock"))
	{
		if (numOfArguments != 2)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return 1;
		}


		double stockPrice;

		try {
			stockPrice = tstringToDouble(words.at(2));
			tcout << stockPrice << endl;
		}
		catch (invalid_argument e)
		{
			tcout << e.what() << endl;
			return 1;
		}

		
		defineStock(words, market_info);
		updateSharedMemory(market_info, NULL);
		SetEvent(market_info.hSharedMemoryEvent);
		//tcout << _T("Valid command") << endl;

	}
	else if (mainCommand == _T("users"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return 1;
		}

		//tcout << _T("Valid command") << endl;
		printClients(market_info);


	}
	else if (mainCommand == _T("pause"))
	{
		if (numOfArguments != 1)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return 1;
		}

		

		try {
			market_info.seconds = tstringToInt(words.at(1));
			//tcout << numSeconds << endl;
		}
		catch (invalid_argument e)
		{
			tcout << e.what() << endl;
			return 1;
		}

		tcout << _T("Market will freeze for ") << market_info.seconds << _T(" secodns") << endl;


		HANDLE h_sleepThread = CreateThread(NULL, 0, pauseMarket, &market_info, 0, NULL);

		if (h_sleepThread == INVALID_HANDLE_VALUE)
		{
			tcout << _T("ERROR creating sleep thread - ") << GetLastError() << endl;
		}

		CloseHandle(h_sleepThread);
		


	}
	else if (mainCommand == _T("close"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invaid number of arguments") << endl;
			return 1;
		}

		SetEvent(market_info.hShut_down_event);
		//Sleep(10000);
		return 0;

	}
	else {
		tcout << _T("Unknown command") << endl;
		return 1;
	}

	

	return 1;
	
}

int buyStocksFromMarket(tstring &company,double num_stocks_to_buy,Client &client, MarketInfo& market_info){
	/*
	RETURN CODES:
	1 bought successfully from market
	0 not enough balance
	-1 not enough stock
	-2 error
	*/

	double previous_amount = 0;
	tstring client_name = tstring(client.name);

	//Update company

	WaitForSingleObject(market_info.hMutex_companies_list,INFINITE);

	for (Company& c : market_info.companies_list)
	{
		if (c.name == company)
		{
			if (num_stocks_to_buy > c.num_stocks)
			{
				ReleaseMutex(market_info.hMutex_companies_list);
				return -1;//not enough stock
			}

			double price_to_pay = num_stocks_to_buy * c.stock_price;
			
			if (price_to_pay > client.balance)
			{
				ReleaseMutex(market_info.hMutex_companies_list);
				return 0;//not enough balance
			}
			previous_amount = c.num_stocks;
			c.num_stocks -= num_stocks_to_buy;

			c.stock_price += c.stock_price * (num_stocks_to_buy / previous_amount);

			client.balance -= price_to_pay;
			break;

		}
	}

	ReleaseMutex(market_info.hMutex_companies_list);


	//Update Wallet

	WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

	//market_info.wallets[client.name][company] = num_stocks_to_buy;
	market_info.wallets[client_name][company] += num_stocks_to_buy;
	//tcout << client_name << _T(" has ") << market_info.wallets[client_name][company] << _T("from ") << company << endl;


	ReleaseMutex(market_info.hMutex_mapOfWallets);


	//Update client in list
	
	WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);

	for (Client& c : market_info.clients_list)
	{
		tstring cliNameAux = tstring(c.name);
		if (cliNameAux == client_name)
		{
			copyClient(c, client);
			ReleaseMutex(market_info.hMutex_clients_list);
			return 1;
		}
	}

	ReleaseMutex(market_info.hMutex_clients_list);


	return 1;


}

int buyStockFromClients(tstring& company, double num_stocks_to_buy, double *stock_price, Client& client, MarketInfo& market_info) {

	/*
	Return Codes:
	1  sucess
	0 not enough stock
	*/
	double stocks_aux_counter = 0;
	int num_sales;
	//double current_stock_price = 0;
	unordered_map<tstring, double> sales = {}; //client who sold - amount sold 
	WaitForSingleObject(market_info.hMutex_sellOrders_list, INFINITE);

	market_info.sales++;
	num_sales = market_info.sales;

	if (market_info.sales == N_SALES)
	{
		market_info.sales = 0;
	}

	for (const SellOrder& s : market_info.sell_orders)
	{
		if (s.company_name == company)
		{
			stocks_aux_counter += s.num_stocks;


		}
	}

	ReleaseMutex(market_info.hMutex_sellOrders_list);


	if (num_stocks_to_buy > stocks_aux_counter)
	{
		return 0; 
	}



	WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

	for (Company& c : market_info.companies_list)
	{
		if (c.name == company)
		{
			*stock_price = c.stock_price;

			//Update the stock price
			c.stock_price -= c.stock_price * 0.05;
			srand(time(0));
			int random = rand() % 101;
			if (random > 50)
			{
				c.stock_price = c.stock_price + (c.stock_price * 0.1);
			}

			if (num_sales == N_SALES)
			{
				int random_index = rand() % market_info.companies_list.size();
				if (random > 50)
				{
					market_info.companies_list.at(random_index).stock_price += market_info.companies_list.at(random_index).stock_price * 0.7;
					break;
				}
				market_info.companies_list.at(random_index).stock_price -= market_info.companies_list.at(random_index).stock_price * 0.7;

			}

			break;
		}
	}

	ReleaseMutex(market_info.hMutex_companies_list);



	WaitForSingleObject(market_info.hMutex_sellOrders_list, INFINITE);


	auto it = market_info.sell_orders.begin();

	while (it != market_info.sell_orders.end() || num_stocks_to_buy != 0) {

		if (it->company_name == company) {

			if (it->num_stocks >= num_stocks_to_buy) {
				it->num_stocks -= num_stocks_to_buy;
				sales[it->client_name] += num_stocks_to_buy;
				num_stocks_to_buy = 0;

				
				if (it->num_stocks == 0) {
					
					it = market_info.sell_orders.erase(it);
					continue; 
				}
			}
			else {
				sales[it->client_name] += it->num_stocks;
				num_stocks_to_buy -= it->num_stocks;
				it->num_stocks = 0; 
			}
		}
		++it;
	}

	ReleaseMutex(market_info.hMutex_sellOrders_list);


	////update clients wallets
	//WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

	//for (auto c : sales)
	//{
	//	// the client               the company   amount sold
	//	market_info.wallets[c.first][company] -= c.second;
	//	tcout << c.first << _T(" has ") << market_info.wallets[c.first][company] << _T("from ") << company << endl;
	//}

	//ReleaseMutex(market_info.hMutex_mapOfWallets);



	//update clients balance
	WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);

	for (auto c : sales)
	{
		////               the client  the company   amount sold
		//market_info.wallets[c.first][company] -= c.second;

		for (Client &cli : market_info.clients_list)
		{
			tstring name_aux = tstring(cli.name);
			if (name_aux == c.first)
			{
				cli.balance += *stock_price * c.second;
			}
		}

	}

	ReleaseMutex(market_info.hMutex_clients_list);




}

void updateClientsWalletAndBalance(Client &client,tstring &company, double num_stock_bought, double price_of_stock,MarketInfo &market_info)
{
	tstring client_name = tstring(client.name);

	WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

	market_info.wallets[client_name][company] += num_stock_bought;
	//tcout << client_name << _T(" has ") << market_info.wallets[client_name][company] << _T("from ") << company << endl;


	ReleaseMutex(market_info.hMutex_mapOfWallets);


	WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);

	for (Client& c : market_info.clients_list)
	{
		tstring name_aux = tstring(c.name);
		if (client_name == name_aux)
		{
			c.balance -= num_stock_bought * price_of_stock;
			copyClient(client, c);
			
			break;
		}
	}

	ReleaseMutex(market_info.hMutex_clients_list);

}

int doesClientHaveStock(MarketInfo& market_info, tstring& client, tstring &empresa, double num_stock)
{
	/*
	RETURNS
	1 succes/ has stock
	-1 does not have stock of that company
	-2 not enough stock
	*/
	WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

	if (market_info.wallets[client].count(empresa) == 0) {
		ReleaseMutex(market_info.hMutex_mapOfWallets);
		return -1;
	}
	double user_stocks = market_info.wallets[client][empresa];

	if (user_stocks < num_stock)
	{
		ReleaseMutex(market_info.hMutex_mapOfWallets);
		return -2;
	}

	ReleaseMutex(market_info.hMutex_mapOfWallets);

	return 1;

}

int updateSharedMemory(MarketInfo& market_info, Transaction* transaction)
{
	Company aux_companies[MAX_COMPANIES];
	int tam;
	WaitForSingleObject(market_info.capusle->hMutex, INFINITE);

	if (transaction != nullptr)
	{
		_tcscpy_s(market_info.capusle->shared_memory->last_transaction.client_name, transaction->client_name);
		_tcscpy_s(market_info.capusle->shared_memory->last_transaction.company_name, transaction->company_name);
		
		market_info.capusle->shared_memory->last_transaction.num_stocks = transaction->num_stocks;
		market_info.capusle->shared_memory->last_transaction.value = transaction->value;
	}

	//if (transaction == nullptr)
	//{

	//	
	//	//market_info.capusle->shared_memory->last_transaction.client_name = _T("-");
	//	_tcscpy_s(market_info.capusle->shared_memory->last_transaction.client_name, _T("-"));

	//	//market_info.capusle->shared_memory->last_transaction.company_name = _T("-");
	//	_tcscpy_s(market_info.capusle->shared_memory->last_transaction.company_name, _T("-"));
	//	
	//	market_info.capusle->shared_memory->last_transaction.num_stocks = 0;
	//	market_info.capusle->shared_memory->last_transaction.value = 0;


	//}
	//else {
	//	//market_info.capusle->shared_memory->last_transaction.client_name = transaction->client_name;
	//	_tcscpy_s(market_info.capusle->shared_memory->last_transaction.client_name, transaction->client_name);
	//	//market_info.capusle->shared_memory->last_transaction.company_name = transaction->company_name;
	//	_tcscpy_s(market_info.capusle->shared_memory->last_transaction.company_name, transaction->company_name);
	//	market_info.capusle->shared_memory->last_transaction.num_stocks = transaction->num_stocks;
	//	market_info.capusle->shared_memory->last_transaction.value = transaction->value;
	//}

	ReleaseMutex(market_info.capusle->hMutex);

	WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);
	tam = market_info.companies_list.size();
	for (int i = 0; i < market_info.companies_list.size(); i++)
	{
		aux_companies[i].name = market_info.companies_list.at(i).name;
		aux_companies[i].num_stocks = market_info.companies_list.at(i).num_stocks;
		aux_companies[i].stock_price = market_info.companies_list.at(i).stock_price;
	}

	ReleaseMutex(market_info.hMutex_companies_list);


	WaitForSingleObject(market_info.capusle->hMutex, INFINITE);

	market_info.capusle->shared_memory->num_companies = tam;

	for (int i = 0; i < tam; i++)
	{
		//market_info.capusle->shared_memory->companies[i].name = aux_companies[i].name;
		//_tcscpy_s(market_info.capusle->shared_memory->companies[i].name, aux_companies[i].name.c_str());
		_tcscpy_s(market_info.capusle->shared_memory->companies[i].name,
			sizeof(market_info.capusle->shared_memory->companies[i].name) / sizeof(TCHAR),
			aux_companies[i].name.c_str());
		market_info.capusle->shared_memory->companies[i].num_stocks = aux_companies[i].num_stocks;
		market_info.capusle->shared_memory->companies[i].stock_price = aux_companies[i].stock_price;
	}

	ReleaseMutex(market_info.capusle->hMutex);

	return 0;

}

void processComandFromClient(tstring &command,Client &client_recieved,PipeMessage &return_message,MarketInfo &market_info) { //you can assume the command is interily valid as it is filtrated in the client program

	vector<tstring> command_as_array;
	

	stringToArrayOfStrings(command, command_as_array);
	TCHAR client_already_connected[] = _T("You are already connected");
	//tstring client_already_connected = tstring(_T("You are already connected"));
	TCHAR connected_sucessfully[] = _T("Sucessfully conected");
	TCHAR invalid_credentials[] = _T("Invalid credentials");
	TCHAR must_be_connected[] = _T("You must be connected ");
	TCHAR unavaliable_command[] = _T("That command is not avaliable at the moment");

	//ZeroMemory(return_message.message, BUFFERSIZE);


	int valid = 0;
	Transaction aux_transaction = Transaction();

	if (command_as_array.at(0) == _T("login"))//  login <username > <password>.
	{
		//_tcscpy_s(return_message.message, aux);


		if (client_recieved.connected == TRUE)
		{
			
			_tcscpy_s(return_message.message, client_already_connected);
			return;
		}


		WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);
		for (Client &c : market_info.clients_list)
		{
			tstring name = tstring(c.name);
			tstring password = tstring(c.password);
			if (name == command_as_array.at(1) && password == command_as_array.at(2))
			{
				/*client_recieved.name = c.name;
				client_recieved.password = c.password;
				client_recieved.balance = c.balance;
				client_recieved.connected = TRUE;*/

				if (c.connected == TRUE)
				{
					_tcscpy_s(return_message.message, client_already_connected);
					copyClient(return_message.client, client_recieved);
					ReleaseMutex(market_info.hMutex_clients_list);
					return;
				}

				c.connected = TRUE;

				_tcscpy_s(return_message.client.name, c.name);
				//return_message.client.name = c.name;
				_tcscpy_s(return_message.client.password, c.password);

				//return_message.client.password = c.password;
				return_message.client.balance = c.balance;
				return_message.client.connected = TRUE;
				
				valid = 1;
				_tcscpy_s(return_message.message, connected_sucessfully);
				ReleaseMutex(market_info.hMutex_clients_list);


				WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

				market_info.wallets[return_message.client.name];

				ReleaseMutex(market_info.hMutex_mapOfWallets);

				return;
				
			}
		}
		ReleaseMutex(market_info.hMutex_clients_list);
		
		ZeroMemory(&return_message.client,sizeof(Client));
		_tcscpy_s(return_message.message, invalid_credentials);
		return;
	}
	
	if (client_recieved.connected == FALSE)
	{
		copyClient(return_message.client, client_recieved);
		_tcscpy_s(return_message.message, must_be_connected);
		return;
	}

	if (command_as_array.at(0) == _T("listc"))//does not have more arguments
	{
		basic_ostringstream<TCHAR> oss;
		

		WaitForSingleObject(market_info.hMutex_companies_list,INFINITE);
		for (Company c : market_info.companies_list) {
		
			/*_tcscat_s(return_message.message, BUFFERSIZE, c.name.c_str());
			tstring aux = _T(" ");
			_tcscat_s(return_message.message, BUFFERSIZE, _T("\n"));*/
			
			oss  << c.name << _T(" ") << c.num_stocks << _T(" ") << c.stock_price << _T("\n");

			// Copiando a mensagem para o campo message de PipeMessage
			

		}
		basic_string<TCHAR> str = oss.str();
		_tcscpy_s(return_message.message, str.c_str());
		ReleaseMutex(market_info.hMutex_companies_list);
		return;
	}
	
	if (command_as_array.at(0) == _T("balance"))
	{
		tstring cli_name_aux = tstring(client_recieved.name);
		int client_found = 0;
		basic_ostringstream<TCHAR> oss;
		WaitForSingleObject(market_info.hMutex_clients_list, INFINITE);
		for (const Client& c : market_info.clients_list) {
			tstring cName = tstring(c.name);
			if (cName == cli_name_aux)
			{
				client_found = 1;
				oss << _T("\nYour balance is: ") << c.balance;
			}
		}
		basic_string<TCHAR> str = oss.str();
		
		ReleaseMutex(market_info.hMutex_clients_list);
		if (client_found == 1)
		{

			_tcscpy_s(return_message.message, str.c_str());
			return;
		}
		_tcscpy_s(return_message.message, _T("\nBalance Unavaliable"));
		return;
	}

	if (command_as_array.at(0) == _T("buy")) {

		tstring cli_name_aux = tstring(client_recieved.name);

		WaitForSingleObject(market_info.hMutexSleep,INFINITE);
		ReleaseMutex(market_info.hMutexSleep);

		WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);
		//verify if it is possible to buy
		if (market_info.wallets[cli_name_aux].size() >= 5 && market_info.wallets[cli_name_aux].count(command_as_array.at(1)) == 0)
		{
			_tcscpy_s(return_message.message, _T("You can no longer buy stock from companies outside of your wallet"));
			ReleaseMutex(market_info.hMutex_mapOfWallets);
			return;
		}
		
		ReleaseMutex(market_info.hMutex_mapOfWallets);

		WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

		int company_exists = 0;
		

		for (const Company& c : market_info.companies_list) {
			
			if (c.name == command_as_array.at(1))
			{
				company_exists = 1;
				
				break;
			}
		}

		ReleaseMutex(market_info.hMutex_companies_list);


		if (company_exists != 1)
		{
			_tcscpy_s(return_message.message, _T("That company does not exist"));
			return;
		}
		
		switch (buyStocksFromMarket(command_as_array.at(1), tstringToDouble(command_as_array.at(2)), client_recieved, market_info))
		{
		case 1:
			_tcscpy_s(return_message.message, _T("Stock bought successfully"));
			
			//aux_transaction.client_name = client_recieved.name;
			_tcscpy_s(aux_transaction.client_name, client_recieved.name);
			//aux_transaction.company_name = command_as_array.at(1);
			_tcscpy_s(aux_transaction.company_name, command_as_array.at(1).c_str());
			aux_transaction.num_stocks = tstringToDouble(command_as_array.at(2));
			//Here return_messageClient comes first because since it has not been updated yet it has the previous balance of the client
			//client_recieved comes second beacuse it has the clients real balance
			aux_transaction.value = return_message.client.balance - client_recieved.balance;

			updateSharedMemory(market_info,&aux_transaction);
			SetEvent(market_info.hSharedMemoryEvent);
			copyClient(return_message.client, client_recieved);
			

			return;
			break;
		case 0:
			_tcscpy_s(return_message.message, _T("Not enough balance"));
			return;
			break;
		case -1:

			double stock_price = 0;

			int aux = buyStockFromClients(command_as_array.at(1), tstringToDouble(command_as_array.at(2)),&stock_price, client_recieved, market_info);

			if (aux == 0)//Not enough stock in market
			{
				_tcscpy_s(return_message.message, _T("Not enough stock in market"));
				return;
				break;
			}
			_tcscpy_s(return_message.message, _T("Stock bought successfully"));

			_tcscpy_s(aux_transaction.client_name, client_recieved.name);
			_tcscpy_s(aux_transaction.company_name, command_as_array.at(1).c_str());
			aux_transaction.num_stocks = tstringToDouble(command_as_array.at(2));
			
			                                     // number of stocks to buy
			aux_transaction.value = stock_price * tstringToDouble(command_as_array.at(2));

			updateSharedMemory(market_info, &aux_transaction);
			SetEvent(market_info.hSharedMemoryEvent);
			updateClientsWalletAndBalance(client_recieved, command_as_array.at(1), tstringToDouble(command_as_array.at(2)), stock_price, market_info);
			
			
			copyClient(return_message.client, client_recieved);
			_tcscpy_s(return_message.message, _T("Stock bought successfully"));
			return;

			break;

		//default:
		//	_tcscpy_s(return_message.message, _T("An error ocurred unable to buy  stock"));
		//	//return;
		//	break;
		}

		//buyStocksFromMarket(command_as_array.at(1), tstringToDouble(command_as_array.at(2)), client_recieved, market_info);


		//WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);
		//if (company_exists == 1)
		//{
		//	//Name of client                           company                         value
		//market_info.wallets[client_recieved.name][command_as_array.at(1)] == tstringToDouble(command_as_array.at(2));
		//_tcscpy_s(return_message.message, _T("Stock added to you wallet"));
		//ReleaseMutex(market_info.hMutex_mapOfWallets);
		//return;
		//}

		//_tcscpy_s(return_message.message, _T("That company does not exist"));
		//ReleaseMutex(market_info.hMutex_mapOfWallets);
		return;
		



	}

	if (command_as_array.at(0) == _T("sell"))
	{
		WaitForSingleObject(market_info.hMutexSleep, INFINITE);
		ReleaseMutex(market_info.hMutexSleep);
		tstring company_name;
		int company_exists = 0;
		WaitForSingleObject(market_info.hMutex_companies_list, INFINITE);

		for (const Company &c : market_info.companies_list)
		{
			if (c.name == command_as_array.at(1))
			{
				company_exists = 1;
				company_name = c.name;
			}
		}

		ReleaseMutex(market_info.hMutex_companies_list);

		if (company_exists == 0)
		{
			_tcscpy_s(return_message.message, _T("That company does not exist"));
			return;
		}
		double num_stocks = tstringToDouble(command_as_array.at(2));

		tstring aux_argument = tstring(client_recieved.name);
		int has_stock = doesClientHaveStock(market_info,aux_argument , company_name, num_stocks);

		if (has_stock == 1)
		{
			SellOrder sell_order;
			sell_order.client_name = client_recieved.name;
			sell_order.company_name = company_name;
			sell_order.num_stocks = num_stocks;

			WaitForSingleObject(market_info.hMutex_sellOrders_list, INFINITE);

			market_info.sell_orders.push_back(sell_order);

			ReleaseMutex(market_info.hMutex_sellOrders_list);

			WaitForSingleObject(market_info.hMutex_mapOfWallets, INFINITE);

			//double previous_amount = market_info.wallets[client_recieved.name][company_name];

			market_info.wallets[client_recieved.name][company_name] -=  num_stocks;
			//tcout << client_recieved.name << _T(" has ") << market_info.wallets[client_recieved.name][company_name] << _T("from ") << company_name << endl;

			if (market_info.wallets[client_recieved.name][company_name] == 0)
			{
				market_info.wallets[client_recieved.name].erase(company_name);
			}


			ReleaseMutex(market_info.hMutex_mapOfWallets);

			_tcscpy_s(return_message.message, _T("Sell order created"));
			return;
		}
		if (has_stock == -1)
		{
			_tcscpy_s(return_message.message, _T("You dont have any stock of that company"));
			return;
		}
		if (has_stock == -2)
		{
			_tcscpy_s(return_message.message, _T("You dont have enough stocks "));
			return;
		}


	}

	_tcscpy_s(return_message.message, _T("That command is not avaliable at the moment"));


}

//DWORD WINAPI handleClient(LPVOID arg) {
//
//	ArgumentsForHandleClient *args = (ArgumentsForHandleClient*)arg;
//	BOOL success;
//	PipeMessage message_recieved;
//	PipeMessage return_message;
//	DWORD bytes_read = 0, bytes_written = 0;
//	TCHAR exit[BUFFERSIZE] = _T("exit\n");
//	
//	
//
//
//	if (args->hPipe == INVALID_HANDLE_VALUE)
//	{
//		tcout << _T("Something went wrong with the pipe handle, error: ") << GetLastError() << endl;
//		return -1;
//	}
//
//	while (true)
//	{
//		success = ReadFile(
//			args->hPipe,        // handle to pipe 
//			&message_recieved,    // buffer to receive data 
//			sizeof(PipeMessage), // size of buffer 
//			&bytes_read, // number of bytes read 
//			NULL);        // not overlapped I/O 
//
//		if (!success || bytes_read == 0)
//		{
//			if (GetLastError() == ERROR_BROKEN_PIPE)
//			{
//				tcout << _T("Client disconnected unexpectedly") << endl;
//			}
//			else
//			{
//				tcout << _T("ReadFile failed, error: ") << GetLastError() << endl;
//			}
//			break;
//		}
//
//		tcout << _T("I recieved ") << message_recieved.message << endl;
//		tstring aux = tstring(message_recieved.message);
//		processComandFromClient(aux, message_recieved.client, return_message, *((ArgumentsForHandleClient*)arg)->market_info);
//		
//
//		if (aux == _T("exit\n"))
//		{
//			tcout << _T("Client disconectd with intent") << endl;
//			FlushFileBuffers(args->hPipe);
//			DisconnectNamedPipe(args->hPipe);
//			//CloseHandle(args->hPipe);
//			//delete args;
//			//break;
//			return 1;
//		}
//
//
//		success = WriteFile(
//			args->hPipe,        // handle to pipe 
//			&return_message,     // buffer to write from 
//			sizeof(PipeMessage), // number of bytes to write 
//			&bytes_written,   // number of bytes written 
//			NULL);        // not overlapped I/O 
//
//		if (!success || sizeof(PipeMessage) != bytes_written)
//		{
//			tcout << _T("WriteFile failed, error: ") << GetLastError() << endl;
//			break;
//		}
//
//	}
//	FlushFileBuffers(args->hPipe);
//	DisconnectNamedPipe(args->hPipe);
//	CloseHandle(args->hPipe);
//	delete args;
//	//return 1;
//
//}


DWORD WINAPI handleClient(LPVOID arg) {
	ArgumentsForHandleClient* args = (ArgumentsForHandleClient*)arg;
	BOOL success;
	PipeMessage  message_received ;
	PipeMessage return_message;
	DWORD bytes_read = 0, bytes_written = 0;
	tstring exit_message = _T("exit\n");

	if (args->hPipe == INVALID_HANDLE_VALUE) {
		tcout << _T("Something went wrong with the pipe handle, error: ") << GetLastError() << endl;
		return -1;
	}

	while (true) {
		success = ReadFile(
			args->hPipe,        // handle to pipe 
			&message_received,    // buffer to receive data 
			sizeof(PipeMessage), // size of buffer 
			&bytes_read, // number of bytes read 
			NULL);        // not overlapped I/O 

		if (!success || bytes_read == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				tcout << _T("Client disconnected unexpectedly") << endl;

				WaitForSingleObject(args->market_info->hMutex_clients_list, INFINITE);
				tstring cName = tstring(return_message.client.name);
				for (Client& c : args->market_info->clients_list)
				{
					tstring cName2 = tstring(c.name);
					if (cName == cName2)
					{
						c.connected = FALSE;
						break;
					}
				}

				ReleaseMutex(args->market_info->hMutex_clients_list);

				ReleaseSemaphore(args->market_info->login_semaphore, 1, NULL);
			}
			else {
				tcout << _T("ReadFile failed, error: ") << GetLastError() << endl;
			}
			break;
		}

		//tcout << _T("I received ") << message_received.message << endl;
		tstring aux = tstring(message_received.message);
		processComandFromClient(aux, message_received.client, return_message, *((ArgumentsForHandleClient*)arg)->market_info);

		if (aux == exit_message) {
			tcout << _T("Client disconnected with intent") << endl;

			WaitForSingleObject(args->market_info->hMutex_clients_list, INFINITE);
			tstring cName = tstring(message_received.client.name);
			for (Client& c : args->market_info->clients_list)
			{
				tstring cName2 = tstring(c.name);
				if (cName == cName2)
				{
					c.connected = FALSE;
					break;
				}
			}

			ReleaseMutex(args->market_info->hMutex_clients_list);

			break;
		}

		success = WriteFile(
			args->hPipe,        // handle to pipe 
			&return_message,     // buffer to write from 
			sizeof(PipeMessage), // number of bytes to write 
			&bytes_written,   // number of bytes written 
			NULL);        // not overlapped I/O 

		if (!success || sizeof(PipeMessage) != bytes_written) {
			tcout << _T("WriteFile failed, error: ") << GetLastError() << endl;
			break;
		}
	}

	// Cleanup on loop exit
	FlushFileBuffers(args->hPipe);
	DisconnectNamedPipe(args->hPipe);
	CloseHandle(args->hPipe);

	delete args;

	return 0;
}




DWORD WINAPI mainPipeHandler(LPVOID arg) {
	MarketInfo* market_info = (MarketInfo*)arg;
	BOOL   client_conected = FALSE;
	DWORD  dwThreadId = 0;
	HANDLE hThread/*,hPipe*/;
	//ArgumentsForHandleClient *args = new ArgumentsForHandleClient();
	

	while (true)
	{
		ArgumentsForHandleClient* arguments = new ArgumentsForHandleClient;
		arguments->market_info = market_info;
		//args->market_info = market_info;

		arguments->hPipe = CreateNamedPipe(
			PIPE_NAME,             // pipe name 
			PIPE_ACCESS_DUPLEX,       // read/write access 
			PIPE_TYPE_MESSAGE |       // message type pipe 
			PIPE_READMODE_MESSAGE |   // message-read mode 
			PIPE_WAIT,                // blocking mode 
			PIPE_UNLIMITED_INSTANCES, // max. instances  
			sizeof(PipeMessage),                  // output buffer size 
			sizeof(PipeMessage),                  // input buffer size 
			0,                        // client time-out 
			NULL);                    // default security attribute 

		if (arguments->hPipe == INVALID_HANDLE_VALUE)
		{
			tcout << _T("CreateNamedPipe failed error: ") << GetLastError() << endl;
			return -1;
		}

		// Wait for the client to connect; 
		//it is possible for a client to connect between  CreateNamedPipe and ConnectNamedPipe functions. 
		// If this happens, ConnectNamedPipe returns zero, and GetLastError returns ERROR_PIPE_CONNECTED.


		client_conected = ConnectNamedPipe(arguments->hPipe, NULL);

		if (!client_conected && GetLastError() == ERROR_PIPE_CONNECTED)
		{
			client_conected = TRUE;
		}



		if (client_conected)
		{
			tcout << _T("Client connected") << endl;

			// Create a thread for this client. 
			hThread = CreateThread(
				NULL,              // no security attribute 
				0,                 // default stack size 
				handleClient,    // thread proc
				arguments,    // thread parameter 
				0,                 // not suspended 
				NULL);      // returns thread ID 

			if (hThread == NULL)
			{
				tcout << _T("CreateThread failed error: ") << GetLastError() << endl;
				return -1;
			}
			CloseHandle(hThread);
		}
		else
		{
			// The client could not connect, so close the pipe. 
			CloseHandle(arguments->hPipe);
		}
	
	}
	return 0;
}



int getCompaniesFromList(MarketInfo& market_info)
{
	vector<tstring> company_info;// holds the info of a company read from the file temporarily
	tstring company_info_as_string;
	int num_companies = 0;
	Company company;
	unordered_map<tstring, double> aux_hashMap = {};//Hash map to evaluata if a company exists

	wifstream  inputFile(COMPANIES_FILE);


	if (!inputFile.is_open())
		return -1;

	while (getline(inputFile,company_info_as_string))
	{
		num_companies++;
		stringToArrayOfStrings(company_info_as_string, company_info);
		company.name = company_info.at(0);
		company.num_stocks = stod(company_info.at(1));
		company.stock_price = stod(company_info.at(2));
		if(aux_hashMap.count(company.name) == 0)
		{
			aux_hashMap[company.name] = company.num_stocks;
			market_info.companies_list.push_back(company);
		}

		company_info.clear();
		if (market_info.companies_list.size() >= MAX_COMPANIES)
		{
			break;
		}
	}

	market_info.num_companies = num_companies;

	/*tcout << _T("Companies:") << endl;
	for (Company company : market_info.companies_list)
	{
		tcout << company.name << _T(' ') << company.num_stocks << _T(' ') << company.stock_price << endl;
	}*/
	inputFile.close();
	return 1;

}


int getClientsFromList(MarketInfo& market_info,tstring &clients_file_name) {
	vector<tstring> client_info;// holds the info of a company read from the file temporarily
	tstring client_info_as_string;
	int num_clients = 0;
	Client client;

	wifstream  inputFile(clients_file_name);


	if (!inputFile.is_open())
		return -1;

	while (getline(inputFile, client_info_as_string))
	{
		num_clients++;
		stringToArrayOfStrings(client_info_as_string, client_info);
		
		_tcscpy_s(client.name, client_info.at(0).c_str());
		//client.name = client_info.at(0);
		//client.password = client_info.at(1);
		_tcscpy_s(client.password, client_info.at(1).c_str());
		client.balance = stod(client_info.at(2));
		client.connected = FALSE;

		market_info.clients_list.push_back(client);

		client_info.clear();
	}

	market_info.num_clients = num_clients;

	/*tcout << _T("Clients:") << endl;
	for (Client client : market_info.clients_list)
	{
		tcout << client.name << _T(' ') << client.password << _T(' ') << client.balance << _T(' ') << client.connected << endl;
	}*/

	inputFile.close();

	return 1;

}


int createSharedMemory(SharedMemoryCapsule &capsule) {
	

	// Create the shared memory object
	capsule.hSharedMemory = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(SharedMemory),
		SHARED_MEMORY_NAME
	);

	if (capsule.hSharedMemory == NULL) {
		tcout << _T("ERROR creating shared memory object - ") << GetLastError() << endl;
		
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
	capsule.hMutex = CreateMutex(NULL, FALSE, SHARED_MEMORY_MUTEX_NAME);

	if (capsule.hMutex == INVALID_HANDLE_VALUE) {
		tcout << _T("ERROR creating mutex - ") << GetLastError();
		UnmapViewOfFile(capsule.shared_memory);
		CloseHandle(capsule.hSharedMemory);
		return -1;
	}

	_tcscpy_s(capsule.shared_memory->last_transaction.client_name, _T("-"));

	//market_info.capusle->shared_memory->last_transaction.company_name = _T("-");
	_tcscpy_s(capsule.shared_memory->last_transaction.company_name, _T("-"));

	capsule.shared_memory->last_transaction.num_stocks = 0;
	capsule.shared_memory->last_transaction.value = 0;

	return 1;
}

void CloseSharedMemory(SharedMemoryCapsule* capsule) {
	if (capsule == NULL)
		return;

	CloseHandle(capsule->hMutex);
	UnmapViewOfFile(capsule->shared_memory);
	CloseHandle(capsule->hSharedMemory);
	delete capsule;
}




//Initializes all the HANDLES and invoques shared memory creation
int initializeMemory(MarketInfo& market_info,tstring &clients_file_name) {
	market_info.hMutex_companies_list = CreateMutex(NULL, FALSE, NULL);
	if (market_info.hMutex_companies_list == NULL) {
		tcout << _T("Error creating mutex for the list of companies - ") << GetLastError() << endl;
		return -1;
	}

	market_info.hMutex_clients_list = CreateMutex(NULL, FALSE, NULL);
	if (market_info.hMutex_clients_list == NULL) {
		tcout << _T("Error creating mutex for the list of clients - ") << GetLastError() << endl;

		CloseHandle(market_info.hMutex_companies_list);
		return -1;
	}

	int max_clients = GetNClientesFromRegistry();

	market_info.login_semaphore = CreateSemaphore(NULL, max_clients, max_clients, LOGIN_SEMAPHORE);
	if (market_info.login_semaphore == NULL) {
		tcout << _T("Error creating the login semaphore - ") << GetLastError() << endl;

		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		return -1;
	}

	market_info.hMutex_mapOfWallets = CreateMutex(NULL, FALSE, NULL);
	if (market_info.hMutex_clients_list == NULL) {
		tcout << _T("Error creating mutex for the map of wallets - ") << GetLastError() << endl;

		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.login_semaphore);
		return -1;
	}

	market_info.hMutex_sellOrders_list = CreateMutex(NULL, FALSE, NULL);
	if (market_info.hMutex_clients_list == NULL) {
		tcout << _T("Error creating mutex for sell orders list - ") << GetLastError() << endl;

		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		return -1;
	}

	
	market_info.hSharedMemoryEvent = CreateEvent(NULL, TRUE, FALSE, SHARED_MEMORY_EVENT);
	if (market_info.hSharedMemoryEvent == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating event for shared memory - ") << GetLastError() << endl;
		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		CloseHandle(market_info.hMutex_sellOrders_list);
		return -1;
	}

	market_info.hMutexSleep = CreateMutex(NULL, FALSE, NULL);
	if (market_info.hMutexSleep == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating mutex for sleeping - ") << GetLastError() << endl;
		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		CloseHandle(market_info.hMutex_sellOrders_list);
		CloseHandle(market_info.hSharedMemoryEvent);
	}

	market_info.hShut_down_event = CreateEvent(NULL, TRUE, FALSE, SHUT_DOWN_EVENT);
	if (market_info.hMutexSleep == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating event for shut down - ") << GetLastError() << endl;
		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		CloseHandle(market_info.hMutex_sellOrders_list);
		CloseHandle(market_info.hSharedMemoryEvent);
		CloseHandle(market_info.hMutexSleep);
	}




	market_info.hMutex_mapOfWallets = {};
	market_info.sales = 0;

	market_info.capusle = new SharedMemoryCapsule();

	if (createSharedMemory(*market_info.capusle) == -1)
	{
		delete market_info.capusle;
		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		CloseHandle(market_info.hMutex_sellOrders_list);
		CloseHandle(market_info.hSharedMemoryEvent);
		CloseHandle(market_info.hMutexSleep);
		CloseHandle(market_info.hShut_down_event);
		return -1;
	}
	
	//_tcscpy_s(market_info.capusle->shared_memory->example, _T("Um Teste"));
	

	int a = getCompaniesFromList(market_info);
	int b = getClientsFromList(market_info, clients_file_name);


	if (a == -1 || b == -1)
	{
		CloseHandle(market_info.hMutex_companies_list);
		CloseHandle(market_info.hMutex_clients_list);
		CloseHandle(market_info.hMutex_mapOfWallets);
		CloseHandle(market_info.login_semaphore);
		CloseHandle(market_info.hMutex_sellOrders_list);
		CloseHandle(market_info.hSharedMemoryEvent);
		CloseHandle(market_info.hMutexSleep);
		CloseHandle(market_info.hShut_down_event);

		CloseSharedMemory(market_info.capusle);
		return -1;
	}
	return 1;
}


void closeMemory(MarketInfo& market_info) {

	CloseHandle(market_info.hMutex_companies_list);
	CloseHandle(market_info.hMutex_clients_list);
	CloseHandle(market_info.login_semaphore);
}






int _tmain(int argc, LPTSTR argv[]) {
		
#ifdef UNICODE 
		_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif

	MarketInfo market_info;
	
	
	HANDLE hHandleMainPipe;

	tstring clients_file_name;

	if (argc != 2)
	{
		clients_file_name = tstring(CLIENTS_FILE);
	}
	else {
		try
		{
			clients_file_name = tstring(argv[1]);

		}
		catch (exception e)
		{
			tcout << e.what() << endl;
			clients_file_name = tstring(CLIENTS_FILE);
		}
		
	}


	if (initializeMemory(market_info,clients_file_name) == -1)
	{
		tcout << _T("ERROR initializing memory - ") << GetLastError() << endl;
		return -1;
	}


	updateSharedMemory(market_info, nullptr);


	hHandleMainPipe = CreateThread(NULL, 0, mainPipeHandler, &market_info, 0, NULL);

	if (hHandleMainPipe == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating thread for main pipe - ") << GetLastError() << endl;
		closeMemory(market_info);
		return -1;
	}

	SetEvent(market_info.hSharedMemoryEvent);
	tcout << _T("Bolsa is running smoothly") << endl;

	int a = 1;
	do
	{
		a = getCommand(market_info);
	} while (a == 1);

	//WaitForSingleObject(hHandleMainPipe, INFINITE);
	CloseHandle(hHandleMainPipe);
	



	CloseSharedMemory(market_info.capusle);
	closeMemory(market_info);
	

	



	return 0;
}