//для Windows
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
    char cmdLine1[256];
    sprintf_s(cmdLine1, "Lab-03х.exe 50");

    BOOL success[3] = { FALSE, FALSE, FALSE };

    success[0] = CreateProcessA(
        cmdLine1,
        NULL,
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si[0], &pi[0]);

    success[1] = CreateProcessA(
        NULL,
        cmdLine1,
        NULL, NULL, FALSE,
        0, NULL, NULL,
        &si[1], &pi[1]);

    success[2] = CreateProcessA(
        appPath,
        (LPSTR)"Lab-03х.exe 50",
        NULL, NULL, FALSE,
        0, NULL, NULL,
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

//для Linux
//#include <iostream>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <cstdlib>
//
//int main() {
//    pid_t pid1, pid2;
//    const char* iterations_arg = "5";   // for first child
//    const char* iterations_env = "10";  // for second child
//    pid1 = fork();
//    if (pid1 < 0) {
//        perror("fork failed for first child");
//        return 1;
//    }
//    if (pid1 == 0) {
//        execl("./Lab-03x.exe", "Lab-03x.exe", iterations_arg, nullptr);
//        perror("execl failed for first child");
//        return 1;
//    }
//    pid2 = fork();
//    if (pid2 < 0) {
//        perror("fork failed for second child");
//        return 1;
//    }
//    if (pid2 == 0) {
//        setenv("ITER_NUM", iterations_env, 1);
//        execl("./Lab-03x.exe", "Lab-03x.exe", nullptr);
//        perror("execl failed for second child");
//        return 1;
//    }
//    int status;
//    waitpid(pid1, &status, 0);
//    waitpid(pid2, &status, 0);
//
//    std::cout << "Both child processes have finished." << std::endl;
//    return 0;
//}

