#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <map>
#include "injector.h"

// Get a list of processus
void injector::setProcList() {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);

        if (Process32First(hSnap, &procEntry)) {
            do {
                injector:procList[procEntry.szExeFile] = procEntry.th32ProcessID;
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
}

// Inject given dll into processus by its ProcessusID
int injector::injectDll(DWORD ProcID)
{
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, ProcID);
    if (hProc and hProc != INVALID_HANDLE_VALUE) {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (loc) {
            WriteProcessMemory(hProc, loc, injector::dllPath, strlen(injector::dllPath) + 1, 0);
        }
        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);
        if (hThread) {
            CloseHandle(hThread);
        }
    }
    if (hProc) {
        CloseHandle(hProc);
    }
    return 0;
}

// Get dll path from explorer
void injector::setDllFile(void)
{
    OPENFILENAME ofn;
    CHAR szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "DLL Files\0*.dll\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileName(&ofn) == TRUE) {
        char* filePath = new char[MAX_PATH];
        strcpy_s(filePath, MAX_PATH, ofn.lpstrFile);
		if (injector::dllPath) {
			delete[] injector::dllPath;
		}
		injector::dllPath = filePath;
    } else {
        injector::dllPath = nullptr;
    }
}
