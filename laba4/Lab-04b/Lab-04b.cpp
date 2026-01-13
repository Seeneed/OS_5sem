#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>

DWORD WINAPI Lab_04x(LPVOID arg) {
    int N = *(int*)arg;
    wchar_t username[256];
    DWORD len = GetEnvironmentVariableW(L"USERNAME", username, 256);
    if (len == 0 || len >= 256) wcscpy(username, L"Unknown");
    int uname_len = wcslen(username);
    for (int i = 0; i < N; i++) {
        wprintf(L"PID=%lu - TID=%lu - Iteration=%d - %lc\n",
            GetCurrentProcessId(),
            GetCurrentThreadId(),
            i + 1,
            username[i % uname_len]);
        Sleep(300 + rand() % 101);
    }
    return 0;
}

int wmain() {
    SetConsoleOutputCP(65001);
    HANDLE threads[2];
    int iters1 = 50, iters2 = 125, itersMain = 100;
    threads[0] = CreateThread(NULL, 0, Lab_04x, &iters1, 0, NULL);
    threads[1] = CreateThread(NULL, 0, Lab_04x, &iters2, 0, NULL);
    if (!threads[0] || !threads[1]) {
        wprintf(L"Thread creation error!\n");
        return 1;
    }
    wprintf(L"Thread 1 TID=%lu\n", GetThreadId(threads[0]));
    wprintf(L"Thread 2 TID=%lu\n", GetThreadId(threads[1]));
    wprintf(L"Main thread TID=%lu\n\n", GetCurrentThreadId());
    wchar_t username[256];
    DWORD len = GetEnvironmentVariableW(L"USERNAME", username, 256);
    if (len == 0 || len >= 256) wcscpy(username, L"Unknown");
    int uname_len = wcslen(username);
    for (int i = 0; i < itersMain; i++) {
        wprintf(L"PID=%lu - TID=%lu - Iteration=%d - %lc\n",
            GetCurrentProcessId(),
            GetCurrentThreadId(),
            i + 1,
            username[i % uname_len]);
        Sleep(300 + rand() % 101);
        if (i == 19) {
            SuspendThread(threads[0]);
            wprintf(L"--- Thread 1 suspended at main iteration 20 ---\n");
        }
        if (i == 59) {
            ResumeThread(threads[0]);
            wprintf(L"--- Thread 1 resumed at main iteration 60 ---\n");
        }
        if (i == 39) {
            SuspendThread(threads[1]);
            wprintf(L"--- Thread 2 suspended at main iteration 40 ---\n");
        }
    }
    ResumeThread(threads[1]);
    wprintf(L"--- Thread 2 resumed after main loop ---\n");
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    wprintf(L"\nAll threads finished.\n");
    return 0;
}
