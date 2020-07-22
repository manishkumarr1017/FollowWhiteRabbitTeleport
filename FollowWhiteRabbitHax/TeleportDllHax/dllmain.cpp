// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include<iostream>
#include<vector>
#include<TlHelp32.h>

using namespace std;

uintptr_t findDMAAddy(uintptr_t ptr, vector <unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); i++) {
        addr = *((uintptr_t*)addr);
        addr += offsets[i];
    }
    return addr;
}

DWORD WINAPI HackThread(HMODULE hModule) {
    AllocConsole();
    FILE *f;
    freopen_s(&f,"CONOUT$","w",stdout);
    cout << "First Internal Hack" << endl;
    uintptr_t modBaseAddress = (uintptr_t)GetModuleHandle(L"UnityPlayer.dll");
    uintptr_t staticPtr = (modBaseAddress + 0x017DEF10);
    vector<unsigned int> PositionOffsets = { 0x58,0x10,0x68,0x0,0x1f0 };
    uintptr_t xPosValue = 0, yPosValue = 0, zPosValue = 0;
    while (true) {
        uintptr_t xPosAddr = findDMAAddy(staticPtr, PositionOffsets);
        uintptr_t yPosAddr = (uintptr_t)((double*)xPosAddr + 1);
        uintptr_t zPosAddr = (uintptr_t)((double*)yPosAddr + 1);
        if (GetAsyncKeyState(VK_NUMPAD8) & 1) {
            xPosValue = *((uintptr_t*)xPosAddr);
            yPosValue = *((uintptr_t*)yPosAddr);
            zPosValue = *((uintptr_t*)zPosAddr);
            cout << xPosValue << endl;
            cout << yPosValue << endl;
            cout << zPosValue << endl;
            cout << "Saved the coordinates" << endl;
        }
        if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
            *((uintptr_t*)xPosAddr) = xPosValue;
            *((uintptr_t*)yPosAddr) = yPosValue;
            *((uintptr_t*)zPosAddr) = zPosValue;
            cout << "Loaded the saved coordinates" << endl;
        }
        Sleep(5);
    }
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: {
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

