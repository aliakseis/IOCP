#pragma once

#include "Log.h"
#include <windows.h>
#include <iostream>
#include <iomanip>

#include "CritSecLock.h"

using std::cout;
using std::endl;
using std::hex;
using std::dec;

namespace Log
{
	const int BUFFER_SIZE = 256;

	static bool s_Enable = true;

	HMODULE libModule;
	CRITICAL_SECTION logCS;

	void Error(const char * fileName, const char * funcName, int line, const char * msg, ...)
	{
        CritSecLock lock(logCS);

		static char buffer[BUFFER_SIZE] = {0,};
		va_list args;
		va_start(args, msg);
		vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE-1, msg, args);
		va_end(args);

		cout << "File: " << fileName << "\nFunction: " << funcName << "\nLine: " << line
			 << "\nError: " << buffer << endl;
	}

	void Error(const char * fileName, const char * funcName, int line, int code, const char * msg, ...)
	{
        CritSecLock lock(logCS);

		char* lpMessageBuffer;

		FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_FROM_HMODULE,
			libModule, 
			code,  
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR) &lpMessageBuffer,  
			0,  
			NULL );

		static char buffer[BUFFER_SIZE] = {0,};
		va_list args;
		va_start(args, msg);
		vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE-1, msg, args);
		va_end(args);		

		cout << "File: " << fileName << "\nFunction: " << funcName << "\nLine: " << line
			<< "\nError: " << buffer << "\nMsg: " << lpMessageBuffer << "Code: " << code << " 0x" << hex << code << endl;

		cout << dec;

		// Free the buffer allocated by the system.
		LocalFree( lpMessageBuffer ); 
	}

	void Trace(const char * msg, ...)
	{
		if( s_Enable )
		{
            CritSecLock lock(logCS);

			static char buffer[BUFFER_SIZE] = {0,};
			va_list args;
			va_start(args, msg);
			vsnprintf_s(buffer, BUFFER_SIZE, BUFFER_SIZE-1, msg, args);
			va_end(args);
			
			cout << buffer << endl;
		}
	}

	void Setup()
	{
		InitializeCriticalSection(&logCS);

		libModule = LoadLibraryA("NTDLL.DLL");
	}

	void Cleanup()
	{
		EnterCriticalSection(&logCS);

		if(!FreeLibrary(libModule))
		{
			ERROR_CODE(GetLastError(), "Log::CleanUp() - FreeLibrary() failed.");
		}

        LeaveCriticalSection(&logCS);

        DeleteCriticalSection(&logCS);
	}

	void EnableTrace(bool enable)
	{
		s_Enable = enable;
	}
}
