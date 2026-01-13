//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <iostream>

void RunLoop(const char* procName) {
    char userBuf[256];
    DWORD nameLen = 256;
    GetUserNameA(userBuf, &nameLen);

    HANDLE hMutex = OpenMutexA(SYNCHRONIZE, FALSE, "LabMutex");
    if (hMutex == NULL) {
        hMutex = CreateMutexA(NULL, FALSE, "LabMutex");
    }

    int uname_len = strlen(userBuf);

    for (int i = 0; i < 90; ++i)
    {
        if (i == 30) WaitForSingleObject(hMutex, INFINITE);

        printf("Процесс: %s | Итерация: %d | Символ: %c\n",
            procName, i, userBuf[i % uname_len]);

        Sleep(100);

        if (i == 60) ReleaseMutex(hMutex);
    }

    CloseHandle(hMutex);
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Rus");

    if (argc > 1) {
        RunLoop(argv[1]);
        return 0;
    }

    HANDLE hMutex = CreateMutexA(NULL, FALSE, "LabMutex");
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

    CreateProcessA(NULL, cmdA, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piA);
    CreateProcessA(NULL, cmdB, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si, &piB);

    RunLoop("Main");
    WaitForSingleObject(piA.hProcess, INFINITE);
    WaitForSingleObject(piB.hProcess, INFINITE);
    CloseHandle(piA.hProcess);
    CloseHandle(piA.hThread);
    CloseHandle(piB.hProcess);
    CloseHandle(piB.hThread);
    CloseHandle(hMutex);

    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>
//#include <sys/types.h>
//#include <pwd.h>
//#include <string>
//#include <semaphore.h>
//#include <fcntl.h>
//#include <sys/wait.h>
//
//void RunLoop(const std::string& procName, sem_t* sem) {
//    struct passwd* pw = getpwuid(getuid());
//    std::string username = pw ? pw->pw_name : "Unknown";
//    int uname_len = username.length();
//    pid_t pid = getpid();
//
//    for (int i = 1; i <= 90; ++i) {
//        if (i == 30) sem_wait(sem);
//
//        std::cout << "PID=" << pid
//            << " | Process=" << procName
//            << " | Iteration=" << i
//            << " | Char=" << username[(i - 1) % uname_len]
//            << std::endl;
//
//        if (i == 60) sem_post(sem);
//
//        usleep(100 * 1000);
//    }
//}
//
//int main() {
//    sem_t* sem = sem_open("/lab06b_sem", O_CREAT, 0644, 1);
//    if (sem == SEM_FAILED) {
//        perror("sem_open");
//        return 1;
//    }
//
//    pid_t pidA = fork();
//    if (pidA == 0) {
//        RunLoop("A", sem);
//        return 0;
//    }
//
//    pid_t pidB = fork();
//    if (pidB == 0) {
//        RunLoop("B", sem);
//        return 0;
//    }
//
//    std::cout << "Main process PID=" << getpid() << " | Process=Main\n";
//    RunLoop("Main", sem);
//
//    waitpid(pidA, nullptr, 0);
//    waitpid(pidB, nullptr, 0);
//
//    sem_close(sem);
//    sem_unlink("/lab06b_sem");
//
//    std::cout << "\nAll processes finished.\n";
//    return 0;
//}

