#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

// The path to the legitimate host process
const char *hostProcessPath = "Tomb2.exe";

static bool FileExists(const char *path)
{
    DWORD fileAttributes = GetFileAttributes(path);
    if (fileAttributes != INVALID_FILE_ATTRIBUTES
        && !(fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
        return true;
    }
    return false;
}

static bool InjectDLL(HANDLE process_handle, const char *dll_path)
{
    bool status = false;
    LPVOID load_library_addr =
        (LPVOID)GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    fprintf(stderr, "Injecting %s\n", dll_path);

    if (!FileExists(dll_path)) {
        fprintf(stderr, "DLL does not exist.\n");
        goto finish;
    }

    LPVOID dll_path_adr = VirtualAllocEx(
        process_handle, NULL, strlen(dll_path) + 1, MEM_RESERVE | MEM_COMMIT,
        PAGE_READWRITE);
    if (!dll_path_adr) {
        fprintf(stderr, "Failed to allocate remote memory.\n");
        goto finish;
    }

    if (!WriteProcessMemory(
            process_handle, dll_path_adr, dll_path, strlen(dll_path) + 1,
            NULL)) {
        fprintf(stderr, "Failed to write remote memory.\n");
        goto finish;
    }

    HANDLE remote_thread_handle = CreateRemoteThread(
        process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)load_library_addr,
        dll_path_adr, 0, NULL);
    if (remote_thread_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to create remote thread.\n");
        goto finish;
    }
    WaitForSingleObject(remote_thread_handle, INFINITE);

    VirtualFreeEx(
        process_handle, dll_path_adr, strlen(dll_path) + 1, MEM_RELEASE);
    CloseHandle(remote_thread_handle);

    status = true;
finish:
    return status;
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

    if (!InjectDLL(pi.hProcess, dll_path)) {
        return 1;
    }

    if (ResumeThread(pi.hThread) == (DWORD)-1) {
        fprintf(stderr, "Failed to resume the execution of the process.\n");
        return 1;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}
