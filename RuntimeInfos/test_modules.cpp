#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <psapi.h>
#include <winver.h>

// To ensure correct resolution of symbols, add Psapi.lib to TARGETLIBS
// and compile with -DPSAPI_VERSION=1

void print_file_version(TCHAR* pszFilePath);

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
LPFN_ISWOW64PROCESS fnIsWow64Process;

BOOL IsWow64()
{
    BOOL bIsWow64 = FALSE;

    //IsWow64Process is not available on all supported versions of Windows.
    //Use GetModuleHandle to get a handle to the DLL that contains the function
    //and GetProcAddress to get a pointer to the function if available.

    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if (NULL != fnIsWow64Process)
    {
        if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
        {
            // Handle error...
        }
    }
    return bIsWow64;
}


int PrintModules(DWORD processID)
{
    HMODULE hMods[1024];
    HANDLE hProcess;
    DWORD cbNeeded;
    unsigned int i;

    // Print the process identifier.

    printf("\nProcess ID: %u\n", processID);

    // Get a handle to the process.

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
        PROCESS_VM_READ,
        FALSE, processID);
    if (NULL == hProcess)
        return 1;

    // Get a list of all the modules in this process.

    if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded))
    {
        for (i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
        {
            TCHAR szModName[MAX_PATH];

            // Get the full path to the module's file.

            if (GetModuleFileNameEx(hProcess, hMods[i], szModName,
                sizeof(szModName) / sizeof(TCHAR)))
            {
                // Print the module name and handle value.

                _tprintf(TEXT("\t%s (0x%08X)\n"), szModName, hMods[i]);
                print_file_version(szModName);
            }
        }
    }

    // Release the handle to the process.

    CloseHandle(hProcess);

    return 0;
}

int tester(void)
{

    DWORD aProcesses[1024];
    DWORD cbNeeded;
    DWORD cProcesses;
    unsigned int i;

    // Get the list of process identifiers.

    if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
        return 1;

    // Calculate how many process identifiers were returned.

    cProcesses = cbNeeded / sizeof(DWORD);

    // Print the names of the modules for each process.

    for (i = 0; i < cProcesses; i++)
    {
        PrintModules(aProcesses[i]);
    }

    return 0;
}


int tester2(void)
{
    int pid = GetCurrentProcessId();
    PrintModules(pid);
    return 0;
}

void print_file_version(TCHAR* pszFilePath)
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
        printf("Error in GetFileVersionInfoSize: %d\n", ec);
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

    // pFileInfo->dwFileVersionMS is usually zero. However, you should check
    // this if your version numbers seem to be wrong

    /*
    printf("File Version: %d.%d.%d.%d\n",
        (pFileInfo->dwFileVersionLS >> 24) & 0xff,
        (pFileInfo->dwFileVersionLS >> 16) & 0xff,
        (pFileInfo->dwFileVersionLS >> 8) & 0xff,
        (pFileInfo->dwFileVersionLS >> 0) & 0xff
    );

    // pFileInfo->dwProductVersionMS is usually zero. However, you should check
    // this if your version numbers seem to be wrong.

    printf("Product Version: %d.%d.%d.%d\n",
        (pFileInfo->dwProductVersionLS >> 24) & 0xff,
        (pFileInfo->dwProductVersionLS >> 16) & 0xff,
        (pFileInfo->dwProductVersionLS >> 8) & 0xff,
        (pFileInfo->dwProductVersionLS >> 0) & 0xff
    );
    */

    int major, minor, revision, build;
   
        major = HIWORD(pFileInfo->dwProductVersionMS);
        minor = LOWORD(pFileInfo->dwProductVersionMS);
        revision = HIWORD(pFileInfo->dwProductVersionLS);
        build = LOWORD(pFileInfo->dwProductVersionLS);

    printf("Product Version: %d.%d.%d.%d\n", major, minor, revision, build);

}
