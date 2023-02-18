#include<Windows.h>
#include<vector>
#include <TlHelp32.h>
#include<iostream>

using namespace std;

uintptr_t FindDMAAddy(uintptr_t ptr, vector<unsigned int> offsets) {
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i) {
        addr = *(unsigned long long*)(addr);
        addr += offsets[i];
    }
    return addr;
}
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!_wcsicmp(modEntry.szModule, modName)) {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}
void injected_thread() {
    while (true) {
        if (GetAsyncKeyState('L')) {
            uintptr_t moduleAddr = GetModuleBaseAddress(0, L"wesnoth.exe");
            uintptr_t dynamicPtrBaseAddr=moduleAddr+0x015D2040;
            vector<unsigned int> offsets = { 0x8, 0x8 };
            uintptr_t gold_addr = FindDMAAddy(dynamicPtrBaseAddr, offsets);
            *(DWORD*)gold_addr = 555;
        }
        Sleep(1);
    }
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)injected_thread, NULL, 0, NULL);
    }

    return true;
}