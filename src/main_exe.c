#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

// The path to the legitimate host process
const char *hostProcessPath = "Tomb2.exe";

bool fileExists(const char *filePath)
{
    DWORD fileAttributes = GetFileAttributes(filePath);

    if (fileAttributes != INVALID_FILE_ATTRIBUTES
        && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }

    return false;
}

int main()
{
    char dll_path[MAX_PATH];
    GetModuleFileNameA(NULL, dll_path, MAX_PATH);
    strcpy(strstr(dll_path, ".exe"), ".dll");

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
            hostProcessPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL,
            NULL, &si, &pi)) {
        fprintf(stderr, "Failed to create the process.\n");
        return 1;
    }

    LPVOID loadLibraryAddr =
        (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    if (!fileExists(dll_path)) {
        fprintf(stderr, "DLL does not exist.\n");
        return 1;
    }

    LPVOID dllPathAddr = VirtualAllocEx(
        pi.hProcess, NULL, strlen(dll_path) + 1, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (!dllPathAddr) {
        fprintf(stderr, "Failed to allocate remote memory.\n");
        return 1;
    }

    if (!WriteProcessMemory(
            pi.hProcess, dllPathAddr, dll_path, strlen(dll_path) + 1, NULL)) {
        fprintf(stderr, "Failed to write remote memory.\n");
        return 1;
    }

    HANDLE hRemoteThread = CreateRemoteThread(
        pi.hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr,
        dllPathAddr, 0, NULL);
    if (hRemoteThread == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to create remote thread.\n");
        return 1;
    }
    WaitForSingleObject(hRemoteThread, INFINITE);

    VirtualFreeEx(pi.hProcess, dllPathAddr, strlen(dll_path) + 1, MEM_RELEASE);
    CloseHandle(hRemoteThread);

    if (ResumeThread(pi.hThread) == (DWORD)-1) {
        fprintf(stderr, "Failed to resume the execution of the process.\n");
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
