#include "Cliente.h"

//#include <windows.h>
//#include <tchar.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <string>
//#include <vector>
//#include <stdexcept>
//using namespace std;
////Permitir que o mesmo código possa funcionar para ASCII ou UNICODE
//#ifdef UNICODE 
//#define tcout wcout
//#define tcin wcin
//#define tstring wstring
//#else
//#define tcout cout
//#define tcin cin
//#define tstring string
//#endif
//
//#define PIPE_NAME _T("\\\\.\\pipe\\TP_SO_PIPE")
//#define BUFFERSIZE 256

void copyClient(Client& dest, const Client& src) {
	

	//dest.name = src.name;
	_tcscpy_s(dest.name, src.name);

	//dest.password = src.password;
	_tcscpy_s(dest.password, src.password);
	dest.balance = src.balance;
	dest.connected = src.connected;
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
			if (i == ',')
			{
				throw invalid_argument("Invalid Argument use only numbers and . to seprerate them");
				//return -1.0;
			}
			throw invalid_argument("Invalid Argument make sure you use only valid and positive numbers (ex: 2.45)");

		}

	}

	return stod(string);

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

int getCommand(const tstring &command) {

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


	if (mainCommand == _T("login"))
	{
		if (numOfArguments != 2)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return -1;
		}
		//tcout << _T("Valid command") << endl;
	}

	else if (mainCommand == _T("listc"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return -1;
		}

		//tcout << _T("Valid command") << endl;

	}

	else if (mainCommand == _T("buy"))
	{
		if (numOfArguments != 2)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return -1;
		}

		try {
			double numStock = tstringToDouble(words.at(2));
		}
		catch (invalid_argument e)
		{
			tcout << e.what() << endl;
			return -1;
		}

		//tcout << _T("Valid command") << endl;
	}
	else if (mainCommand == _T("sell"))
	{
		if (numOfArguments != 2)
		{
			tcout << _T("Invalid number of arguments") << endl;
			return -1;
		}

		try {
			double numStock = tstringToDouble(words.at(2));
		}
		catch (invalid_argument e)
		{
			tcout << e.what() << endl;
			return -1;
		}

		//tcout << _T("Valid command") << endl;
	}
	else if (mainCommand == _T("balance"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invaid number of arguments") << endl;
			return -1;
		}


		//tcout << _T("Valid command") << endl;
		return 1;

	}
	
	else if (mainCommand == _T("exit"))
	{
		if (numOfArguments != 0)
		{
			tcout << _T("Invaid number of arguments") << endl;
			return -1;
		}

		return 1;

	}
	else {
		tcout << _T("Unknown command") << endl;
		return -1;
	}

	return 1;

}

int connectToServer(ProgramInfo &info) {
	//HANDLE hPipe;
	BOOL success;
	DWORD bytes_read = 0, bytes_written = 0;
	//TCHAR message[BUFFERSIZE], message_recieved[BUFFERSIZE];
	//HANDLE login_semaphore
	info.login_semaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, LOGIN_SEMAPHORE);
	//TCHAR aux[BUFFERSIZE];
	tstring aux = _T(" ");
	TCHAR exit[BUFFERSIZE] = _T("exit\n");
	Client current_client;
	PipeMessage message_to_send,message_recieved;
	

	_tcscpy_s(current_client.name, aux.c_str());
	//current_client.name = _T("");
	_tcscpy_s(current_client.password, aux.c_str());
	//current_client.password = _T("");
	current_client.balance = 0.0;
	current_client.connected = FALSE;


	if (info.login_semaphore == NULL)
	{
		tcout << _T("ERROR opening login semaphore - ") << GetLastError() << endl;
		return -1;
	}

	tcout << _T("Waiting in line for connection...") << endl;
	DWORD result = WaitForSingleObject(info.login_semaphore, INFINITE);

	//while (true)
	//{
		info.hPipe = CreateFile(
			PIPE_NAME,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 

		if (info.hPipe != INVALID_HANDLE_VALUE)
		{
			tcout << _T("Sucessfully connected  ")  << endl;
			//return -1;
			//break;
		}
		else {
			tcout << _T("ERROR connecting to named pipe - ") << GetLastError() << endl;
			return -1;
		}


		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		/*if (GetLastError() != ERROR_PIPE_BUSY)
		{
			tcout << _T("Could not open pipe, error: ") << GetLastError() << endl;
			CloseHandle(hPipe);
			return -1;
		}*/


		if (!WaitNamedPipe(PIPE_NAME, INFINITE))
		{
			tcout << _T("Could not open pipe: wait timed out.") << endl;
			CloseHandle(info.hPipe);
			return -1;
		}
	//}

		
		//do {
		//int loop = 1;
		while (true){
			/*WaitForSingleObject(info.hMutex, INFINITE);

			loop = info.loop;

			ReleaseMutex(info.hMutex);*/


			copyClient(message_to_send.client, current_client);


			do
			{
				
				tcout << _T("Insert command: ");
				_fgetts(message_to_send.message, BUFFERSIZE, stdin);
			} while ( getCommand(tstring(message_to_send.message)) != 1);
			//tcout << _T("Will send that message") << endl;
			
			success = WriteFile(
				info.hPipe,                  // pipe handle 
				&message_to_send,             // message 
				sizeof(PipeMessage), // message length 
				&bytes_written,             // bytes written 
				NULL);                  // not overlapped 

			if (!success)
			{
				tcout << _T("WriteFile to pipe failed, error: ") << GetLastError() << endl;
				CloseHandle(info.hPipe);
				return -1;
			}

			if (tstring(message_to_send.message) == _T("exit\n"))
				break;

			success = ReadFile(
				info.hPipe,    // pipe handle 
				&message_recieved,    // buffer to receive reply 
				sizeof(PipeMessage),  // size of buffer 
				&bytes_read,  // number of bytes read 
				NULL);    // not overlapped 

			if (!success)
			{
				tcout << _T("Error reading from pipe ") << GetLastError() << endl;
				break;
			}
			//tcout << _T("Sou o ") << message_recieved.client.name << endl;
			tcout << _T("Message received: ") << message_recieved.message << endl;
			aux = tstring(message_recieved.message);

			//current_client = message_recieved.client;
			copyClient(current_client, message_recieved.client);
			//_tcscpy_s(aux, message_recieved.message);

		}// while (aux != _T("exit\n"));
			

		
	ReleaseSemaphore(info.login_semaphore,1,NULL);
	CloseHandle(info.login_semaphore);
	CloseHandle(info.hPipe);
	
}

DWORD WINAPI waitForShutDown(LPVOID arg) {
	ProgramInfo* info = (ProgramInfo*)arg;

	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, SHUT_DOWN_EVENT);

	if (hEvent == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR opening event for shut down") << endl;
		return -1;
	}


	WaitForSingleObject(hEvent, INFINITE);

	WaitForSingleObject(info->hMutex, INFINITE);

	//info->loop = 0;

	CloseHandle(info->login_semaphore);
	CloseHandle(info->hPipe);

	ReleaseMutex(info->hMutex);
	tcout << _T("Terminating under server order") << endl;

	CloseHandle(hEvent);
	exit(0);

	return 1;

}

int _tmain(int argc, LPTSTR argv[]) {

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
#endif
	/*int a = 1;
	do
	{
		a = getCommand();
	} while (a == 1);*/

	ProgramInfo info;
	HANDLE hWaitForShutDownThread;
	info.loop = 1;
	
	info.hMutex = CreateMutex(NULL, FALSE, NULL);

	if (info.hMutex == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating mutex - ") << GetLastError() << endl;
		return -1;
	}

	hWaitForShutDownThread = CreateThread(NULL, 0, waitForShutDown, &info, 0, NULL);
	if (hWaitForShutDownThread == INVALID_HANDLE_VALUE)
	{
		tcout << _T("ERROR creating thread - ") << GetLastError() << endl;
		CloseHandle(info.hMutex);
		return -1;
	}
	CloseHandle(hWaitForShutDownThread);

	connectToServer(info);
	//CloseHandle(info.hMutex); // has to be here otherwise the handle gets closed when the shutDownThread finishes

	//WaitForSingleObject(hWaitForShutDownThread, INFINITE);
	tcout << _T("Bye see you netx time") << endl;


	
	//CloseHandle(info.hMutex);





	return 0;
}