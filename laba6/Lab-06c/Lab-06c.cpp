//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <iostream>

void RunLoop(const char* procName) {
    char userBuf[256];
    DWORD nameLen = 256;
    GetUserNameA(userBuf, &nameLen);

    HANDLE hSem = OpenSemaphoreA(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, "LabSem");
    if (!hSem) {
        hSem = CreateSemaphoreA(NULL, 1, 1, "LabSem");
    }

    int uname_len = (int)strlen(userBuf);

    for (int i = 1; i <= 90; i++) {
        if (i == 30) WaitForSingleObject(hSem, INFINITE);

        printf("Процесс=%s | Итерация=%d | Символ=%c\n",
            procName,
            i,
            userBuf[(i - 1) % uname_len]);
        Sleep(100);
        if (i == 60) ReleaseSemaphore(hSem, 1, NULL);
    }
    CloseHandle(hSem);
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Rus");

    if (argc > 1) {
        RunLoop(argv[1]);
        return 0;
    }

    HANDLE hSem = CreateSemaphoreA(NULL, 1, 1, "LabSem");
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

    RunLoop("Main");

    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piB.hProcess, INFINITE);

    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piB.hProcess);
    CloseHandle(piB.hThread);
    CloseHandle(hSem);

    printf("\nВсе процессы завершены.\n");
    return 0;
}

