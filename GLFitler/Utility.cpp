#include "stdafx.h"
#include "Utility.h"
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

void GetExecuteDirectoryW(wchar_t * pszPath, int nMaxCount)
{
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);

	size_t len = wcslen(path);
	for (size_t pos = len - 1; pos >= 0; pos--)
	{
		if (path[pos] == '\\')
		{
			path[pos + 1] = '\0';
			break;
		}
	}
	
	wcscpy_s(pszPath, nMaxCount, path);
}

void GetExecuteDirectoryA(char * pszPath, int nMaxCount)
{
	char path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);
	size_t len = strlen(path);
	for (size_t pos = len - 1; pos >= 0; pos--)
	{
		if (path[pos] == '\\')
		{
			path[pos + 1] = '\0';
			break;
		}
	}

	strcpy_s(pszPath, nMaxCount, path);
}

char * ReadFile(const char* fileName, int & length)
{
	FILE * fp = NULL;
	fopen_s(&fp, fileName, "rb");
	if (fp == NULL)
	{
		length = -1;
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	size_t fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (fileSize == 0)
	{
		length = 0;
		return NULL;
	}

	char * buffer = (char*)malloc(fileSize + 1);
	if (buffer == NULL)
	{
		length = -2;
		fclose(fp);
		return NULL;
	}

	if (fread(buffer, 1, fileSize, fp) != fileSize)
	{
		length = -3;
		free(buffer);
		fclose(fp);
		return NULL;
	}

	fclose(fp);
	buffer[fileSize] = '\0';
	length = fileSize;
	return buffer;
}

void Release(char* & buffer)
{
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
}
