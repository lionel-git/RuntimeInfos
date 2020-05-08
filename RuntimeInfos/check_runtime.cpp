#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <strsafe.h>


std::string MsgErrorCode(DWORD ec)
{
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		ec,
		MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	std::string msg((const char*)lpMsgBuf);

	LocalFree(lpMsgBuf);
	return msg;
}



void print_file_version(const TCHAR* pszFilePath)
{
	DWORD               dwSize = 0;
	BYTE* pbVersionInfo = NULL;
	VS_FIXEDFILEINFO* pFileInfo = NULL;
	UINT                puLenFileInfo = 0;

	// Get the version information for the file requested
	dwSize = GetFileVersionInfoSize(pszFilePath, NULL);
	if (dwSize == 0)
	{
		//1813 : no version inf file
		int ec = GetLastError();
		printf("Error in GetFileVersionInfoSize: %d %s\n", ec, MsgErrorCode(ec).c_str());
		return;
	}

	pbVersionInfo = new BYTE[dwSize];

	if (!GetFileVersionInfo(pszFilePath, 0, dwSize, pbVersionInfo))
	{
		printf("Error in GetFileVersionInfo: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return;
	}

	if (!VerQueryValue(pbVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &puLenFileInfo))
	{
		printf("Error in VerQueryValue: %d\n", GetLastError());
		delete[] pbVersionInfo;
		return;
	}
	int major, minor, revision, build;

	major = HIWORD(pFileInfo->dwProductVersionMS);
	minor = LOWORD(pFileInfo->dwProductVersionMS);
	revision = HIWORD(pFileInfo->dwProductVersionLS);
	build = LOWORD(pFileInfo->dwProductVersionLS);
	printf("Product Version: %d.%d.%d.%d\n", major, minor, revision, build);
}

void check_runtime()
{
    char* system_root = std::getenv("SystemRoot");
    std::string path = std::string(system_root) + "\\SYSTEM32\\VCRUNTIME140.dll";
    std::cout << path << std::endl;


    print_file_version(path.c_str());

}