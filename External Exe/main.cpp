#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>

using namespace std;

typedef ULONG_PTR SIZE_T;
typedef unsigned __int3264 ULONG_PTR;

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (unsigned int i = 0; i < offsets.size(); ++i) {
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
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

int main(int argc, char** argv) {
	HWND wesnoth_window = FindWindow(NULL, L"The Battle for Wesnoth - 1.16.8");

	DWORD process_id = 0;
	GetWindowThreadProcessId(wesnoth_window, &process_id);

	// exe name here
	uintptr_t moduleBase = GetModuleBaseAddress(process_id, L"wesnoth.exe");

	HANDLE wesnoth_process = OpenProcess(PROCESS_ALL_ACCESS, true, process_id);

	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x015D2040;
	//uintptr_t dynamicPtrBaseAddr = 0x2cb93ba6068;
	cout << "DynamicPtrBaseAddr = " << "0x" << hex << dynamicPtrBaseAddr << endl;
	
	vector<unsigned int> offsets = { 0x8, 0x8 };
	
	uintptr_t gold_addr = FindDMAAddy(wesnoth_process, dynamicPtrBaseAddr, offsets);

	cout << "Gold Addr = " << "0x" << hex << gold_addr << endl;

	DWORD new_gold_value = 555;
	SIZE_T bytes_written = 0;

	WriteProcessMemory(wesnoth_process, (void*)gold_addr, &new_gold_value, 4, &bytes_written);

	cout << "Gold value updated" << endl;
}
