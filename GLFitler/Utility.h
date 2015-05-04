#ifndef __CORE_UTILITY_H__
#define __CORE_UTILITY_H__

#include <windows.h>

void GetExecuteDirectoryW(wchar_t * pszPath, int nMaxCount);
void GetExecuteDirectoryA(char * pszPath, int nMaxCount);

char* ReadFile(const char* fileName, int & length);
void Release(char* & buffer);

#define GetExeDir GetExecuteDirectoryA

#endif
