#include <windows.h>
#include <iostream>

int main() {
    STARTUPINFOA si[3];
    PROCESS_INFORMATION pi[3];
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));

    for (int i = 0; i < 3; i++)
        si[i].cb = sizeof(STARTUPINFOA);

    LPCSTR appPath = ".\\Lab-03х.exe";
    char cmdLine2[256];
    sprintf_s(cmdLine2, "Lab-03х.exe 50");

    BOOL success[3] = { FALSE, FALSE, FALSE };

    success[0] = CreateProcessA(
        appPath,
        NULL,
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si[0], &pi[0]);

    success[1] = CreateProcessA(
        NULL,
        (LPSTR)cmdLine2,
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si[1], &pi[1]);

    SetEnvironmentVariableA("ITER_NUM", "25");

    success[2] = CreateProcessA(
        appPath,
        NULL,
        NULL, NULL, FALSE,
        CREATE_UNICODE_ENVIRONMENT, NULL, NULL,
        &si[2], &pi[2]);

    HANDLE handles[3];
    int count = 0;
    for (int i = 0; i < 3; i++)
        if (success[i])
            handles[count++] = pi[i].hProcess;

    if (count > 0)
        WaitForMultipleObjects(count, handles, TRUE, INFINITE);

    for (int i = 0; i < 3; i++)
        if (success[i]) {
            CloseHandle(pi[i].hProcess);
            CloseHandle(pi[i].hThread);
        }

    return 0;
}
