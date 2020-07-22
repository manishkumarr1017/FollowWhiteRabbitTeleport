#include<iostream>
#include<windows.h>
#include<tlhelp32.h>
#include<vector>

using namespace std;

DWORD getProcId(const wchar_t* procName);
uintptr_t getModuleBaseAddress(DWORD procid, const wchar_t* modName);
uintptr_t findDMAAddy(HANDLE hProcess,uintptr_t ptr, vector<unsigned int> offsets);

int main() {
    DWORD procId = 0;
    procId = getProcId(L"FollowWhiteRabbit.exe");
    if (procId) {
        uintptr_t modBaseAddress = getModuleBaseAddress(procId, L"UnityPlayer.dll");
        cout << "My First External Hack" << endl;
        if (modBaseAddress) {
            uintptr_t staticptr = modBaseAddress + 0x017DEF10;
            HANDLE hProcess = 0;
            hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
            DWORD dwExit = 0;
            double xPosValue = 0, yPosValue = 0, zPosValue = 0;
            while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
                vector<unsigned int> PositionOffsets = { 0x58,0x10,0x68,0x0,0x1f0 };
                uintptr_t xPosAddr = findDMAAddy(hProcess, staticptr, PositionOffsets);
                uintptr_t yPosAddr = (uintptr_t)((double*)xPosAddr + 1);
                uintptr_t zPosAddr = (uintptr_t)((double*)yPosAddr + 1);
                if (GetAsyncKeyState(VK_NUMPAD8) & 1) {
                    ReadProcessMemory(hProcess, (BYTE*)xPosAddr, &xPosValue, sizeof(xPosValue), 0);
                    ReadProcessMemory(hProcess, (BYTE*)yPosAddr, &yPosValue, sizeof(xPosValue), 0);
                    ReadProcessMemory(hProcess, (BYTE*)zPosAddr, &zPosValue, sizeof(xPosValue), 0);
                    cout << xPosValue << endl;
                    cout << yPosValue << endl;
                    cout << zPosValue << endl;
                    cout << "Saved The Co-ordinates" << endl;
                }
                if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
                    WriteProcessMemory(hProcess, (BYTE*)xPosAddr, &xPosValue, sizeof(xPosValue), 0);
                    WriteProcessMemory(hProcess, (BYTE*)yPosAddr, &yPosValue, sizeof(yPosValue), 0);
                    WriteProcessMemory(hProcess, (BYTE*)zPosAddr, &zPosValue, sizeof(zPosValue), 0);
                    cout << xPosValue << endl;
                    cout << yPosValue << endl;
                    cout << zPosValue << endl;
                    cout << "Loded The Saved Co-ordinates" << endl;
                }
            }
            return 0;
        }
        else {
            cout << "Module Not Found" << endl;
            return 0;
        }
    }
    else {
        cout << "Process Not Found" << endl;
        return 0;
    }
}

DWORD getProcId(const wchar_t* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry;
        procEntry.dwSize = sizeof(procEntry);
        if (Process32First(hSnap, &procEntry)) {
            do {
                if (!_wcsicmp(procEntry.szExeFile, procName)) {
                    procId = procEntry.th32ProcessID;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
        CloseHandle(hSnap);
        return procId;
    }
    else {
        cout << "Returned an invalid handle " << endl;
        return procId;
    }
}

uintptr_t getModuleBaseAddress(DWORD procId,const wchar_t* modName) {

    uintptr_t modBaseAddress = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseAddress = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
        CloseHandle(hSnap);
        return modBaseAddress;
    }
    else {
        cout << "Returned an invalid handle" << endl;
        return modBaseAddress;
    }
}
uintptr_t findDMAAddy(HANDLE hProcess,uintptr_t ptr, vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (int i = 0; i < offsets.size(); i++) {
        ReadProcessMemory(hProcess, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}