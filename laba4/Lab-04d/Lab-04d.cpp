#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>

int main() {
    DWORD pid = GetCurrentProcessId();
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        printf("Failed to create snapshot.\n");
        return 1;
    }

    THREADENTRY32 te;
    te.dwSize = sizeof(THREADENTRY32);

    if (Thread32First(hSnapshot, &te)) {
        printf("Threads in process %lu:\n", pid);
        do {
            if (te.th32OwnerProcessID == pid) {
                printf("Thread ID: %lu\n", te.th32ThreadID);
            }
        } while (Thread32Next(hSnapshot, &te));
    }
    else {
        printf("Failed to enumerate threads.\n");
    }

    CloseHandle(hSnapshot);
    return 0;
}
