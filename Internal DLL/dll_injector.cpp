#include <Windows.h>
#include<stdio.h>

char dll[] = "C:\\Users\\shivanshu mishra\\Documents\\projects\\wesnoth_gold_hacker\\Wesnoth_gold_hacker_Dll\\x64\\Debug\\Wesnoth_gold_hacker_Dll.dll";
unsigned int len = sizeof(dll) + 1;

int main(int argc, char* argv[]) {
    HANDLE ph; // process handle
    HANDLE rt; // remote thread
    LPVOID rb; // remote buffer

    // handle to kernel32 and pass it to GetProcAddress
    HMODULE hKernel32 = GetModuleHandle(L"Kernel32");
    VOID* lb = GetProcAddress(hKernel32, "LoadLibraryA");

    HWND wesnoth_window = FindWindow(NULL, L"The Battle for Wesnoth - 1.16.8");

    DWORD process_id = 0;
    GetWindowThreadProcessId(wesnoth_window, &process_id);

    ph = OpenProcess(PROCESS_ALL_ACCESS, FALSE, process_id);

    // allocate memory buffer for remote process
    rb = VirtualAllocEx(ph, NULL, len, (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

    // "copy" evil DLL between processes
    WriteProcessMemory(ph, rb, dll, len, NULL);

    // our process start new thread
    rt = CreateRemoteThread(ph, NULL, 0, (LPTHREAD_START_ROUTINE)lb, rb, 0, NULL);
    CloseHandle(ph);
    return 0;
}
