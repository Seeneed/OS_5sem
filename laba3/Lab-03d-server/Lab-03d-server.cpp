//для Windows
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 4)
        return 1;
    int procCount = atoi(argv[1]);
    int low = atoi(argv[2]);
    int high = atoi(argv[3]);

    PROCESS_INFORMATION* pInfo = (PROCESS_INFORMATION*)calloc(procCount, sizeof(PROCESS_INFORMATION));
    HANDLE* hReadPipes = (HANDLE*)calloc(procCount, sizeof(HANDLE));
    int step = (high - low + 1) / procCount;
    for (int i = 0; i < procCount; i++) {
        HANDLE hRead, hWrite;
        SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE };
        CreatePipe(&hRead, &hWrite, &sa, 0);
        hReadPipes[i] = hRead;
        int start = low + i * step;
        int end = (i == procCount - 1) ? high : start + step - 1;
        STARTUPINFOA si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.hStdOutput = hWrite;
        si.hStdError = hWrite;
        si.dwFlags = STARTF_USESTDHANDLES;
        char cmd[128];
        sprintf_s(cmd, sizeof(cmd), "Lab-03d-client %d %d", start, end);
        ZeroMemory(&pInfo[i], sizeof(PROCESS_INFORMATION));
        CreateProcessA(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pInfo[i]);
        CloseHandle(hWrite);
    }
    for (int i = 0; i < procCount; i++) {
        char buffer[512];
        DWORD bytesRead;
        printf("Client[%lu]: ", pInfo[i].dwProcessId);
        while (ReadFile(hReadPipes[i], buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("%s", buffer);
        }
        printf("\n");
        CloseHandle(hReadPipes[i]);
    }
    for (int i = 0; i < procCount; i++) {
        WaitForSingleObject(pInfo[i].hProcess, INFINITE);
        CloseHandle(pInfo[i].hProcess);
        CloseHandle(pInfo[i].hThread);
    }

    free(pInfo);
    free(hReadPipes);
    return 0;
}

//для Linux
//#include <iostream>
//#include <vector>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <cstdlib>
//int main(int argc, char* argv[]) {
//    if (argc != 4) {
//        std::cerr << "Usage: Lab-03d-server <num_processes> <low> <high>\n";
//        return 1;
//    }    int num = std::atoi(argv[1]);
//    int low = std::atoi(argv[2]);
//    int high = std::atoi(argv[3]);
//    if (num <= 0 || low > high) {
//        std::cerr << "Invalid input.\n";
//        return 1;
//    }    int total = high - low + 1;
//    int chunk = total / num;
//    int remainder = total % num;
//    for (int i = 0; i < num; ++i) {
//        int start = low + i * chunk;
//        int end = start + chunk - 1;
//        if (i == num - 1) end += remainder;
//        int pipefd[2];
//        if (pipe(pipefd) == -1) {
//            std::cerr << "Pipe creation failed.\n";
//            return 1;
//        }        pid_t pid = fork();
//        if (pid < 0) {
//            std::cerr << "Fork failed.\n";
//            return 1;
//        }
//        if (pid == 0) {
//            // Child
//            close(pipefd[0]); // Close read end
//            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
//            close(pipefd[1]);
//            execl("./Lab-03d-client", "Lab-03d-client",
//                std::to_string(start).c_str(),
//                std::to_string(end).c_str(),
//                nullptr);
//            std::cerr << "Exec failed.\n";
//            exit(1);
//        }
//        else {
//            // Parent
//            close(pipefd[1]); // Close write end
//            char buffer[1024];
//            ssize_t bytes = read(pipefd[0], buffer, sizeof(buffer) - 1);
//            if (bytes > 0) {
//                buffer[bytes] = '\0';
//                std::cout << "Child " << pid << " primes: " << buffer;
//            }
//            close(pipefd[0]);
//        }
//    }
//    // Wait for all children
//    for (int i = 0; i < num; ++i)
//        wait(nullptr);
//    return 0;
//}

