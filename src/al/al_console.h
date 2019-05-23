#ifndef AL_CONSOLE_H
#define AL_CONSOLE_H

#include <iostream>

#include "al_platform.h"

struct Console {

#ifdef AL_WIN
	HANDLE console_mutex;
	//HANDLE  hConsole;
#endif 
	
	Console() {
		printf("Console()\n");
#ifdef AL_WIN
		console_mutex = CreateMutex(
			NULL,              // default security attributes
			FALSE,             // initially not owned
			NULL);             // unnamed mutex
		
		//hConsole = GetStdHandle(STD_ERROR_HANDLE);
#endif
	}
	
	int log(const char *fmt, ...) {
		int ret;
#ifdef AL_WIN
		if (WAIT_OBJECT_0 == WaitForSingleObject(console_mutex, 10)) {  // 10ms timeout
#endif
			va_list myargs;
			va_start(myargs, fmt);
			ret = vfprintf(stdout, fmt, myargs);
			va_end(myargs);
			fputc('\n', stdout);
#ifdef AL_WIN
			ReleaseMutex(console_mutex);
		}
#endif
		return ret;
	}
	
	int error(const char *fmt, ...) {
		int ret;
#ifdef AL_WIN
		if (WAIT_OBJECT_0 == WaitForSingleObject(console_mutex, 10)) {  // 10ms timeout
			//fprintf(stderr, "\033[31m");
			//SetConsoleTextAttribute(hConsole, 7);
			
#endif
			va_list myargs;
			va_start(myargs, fmt);
			ret = vfprintf(stderr, fmt, myargs);
			va_end(myargs);
			fputc('\n', stderr);
#ifdef AL_WIN
			//fprintf(stderr, "\033[0m");
			//SetConsoleTextAttribute(hConsole, 15);
			ReleaseMutex(console_mutex);
		}
#endif
		return ret;
	}
	
};

Console console;

#endif // AL_CONSOLE_H
