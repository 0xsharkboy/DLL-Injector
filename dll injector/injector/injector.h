#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <map>

namespace injector
{

    inline char *dllPath;
    inline std::map<std::string, DWORD> procList;

    void setProcList();

    int injectDll(DWORD ProcID);

    void setDllFile(void);
}
