//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <iostream>

void RunLoop(const char* procName, HANDLE hEvent) {
    char userBuf[256];
    DWORD nameLen = 256;
    GetUserNameA(userBuf, &nameLen);

    int uname_len = (int)strlen(userBuf);
    DWORD pid = GetCurrentProcessId();

    for (int i = 1; i <= 90; ++i) {

        if (i == 1 && strcmp(procName, "Main") != 0) {
            WaitForSingleObject(hEvent, INFINITE);
        }

        printf("PID=%lu | Процесс=%s | Итерация=%d | Символ=%c\n",
            (unsigned long)pid,
            procName,
            i,
            userBuf[(i - 1) % uname_len]);

        Sleep(300);

        if (i == 15 && strcmp(procName, "Main") == 0) {
            SetEvent(hEvent);
        }
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Rus");

    HANDLE hEvent = CreateEventA(NULL, TRUE, FALSE, "Lab06dEvent");

    if (argc > 1) {
        RunLoop(argv[1], hEvent);
        CloseHandle(hEvent);
        return 0;
    }

    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    STARTUPINFOA si;
    PROCESS_INFORMATION piA, piB;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    char cmdA[512];
    char cmdB[512];

    sprintf(cmdA, "\"%s\" A", exePath);
    sprintf(cmdB, "\"%s\" B", exePath);

    if (!CreateProcessA(NULL, cmdA, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piA)) {
        printf("Ошибка запуска процесса A\n");
        return 1;
    }

    if (!CreateProcessA(NULL, cmdB, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piB)) {
        printf("Ошибка запуска процесса B\n");
        return 1;
    }

    printf("Главный процесс PID=%lu\n", (unsigned long)GetCurrentProcessId());

    RunLoop("Main", hEvent);

    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piB.hProcess, INFINITE);

    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piB.hProcess);
    CloseHandle(piB.hThread);
    CloseHandle(hEvent);

    printf("\nВсе процессы завершены.\n");

    return 0;
}