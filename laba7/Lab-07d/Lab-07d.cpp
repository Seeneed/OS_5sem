//для Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <cstdio>

void RunChild(int durationMs, PROCESS_INFORMATION* pi) {
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    ZeroMemory(pi, sizeof(*pi));

    char cmdLine[MAX_PATH];

    wsprintfA(cmdLine, "Lab-07x.exe %d", durationMs);

    if (!CreateProcessA(
        NULL,           
        cmdLine,        
        NULL, NULL,     
        FALSE,          
        CREATE_NEW_CONSOLE,
        NULL,         
        NULL,         
        &si,          
        pi            
    )) {
        std::cerr << "Failed to launch: " << cmdLine << std::endl;
    }
    else {
        std::cout << "Launched: " << cmdLine << std::endl;
    }
}

int main()
{
    std::cout << "Parent process started." << std::endl;

    PROCESS_INFORMATION pi1{}, pi2{};

    RunChild(60000, &pi1);

    RunChild(120000, &pi2);

    HANDLE hProcesses[] = { pi1.hProcess, pi2.hProcess };

    std::cout << "Waiting for child processes to finish..." << std::endl;

    WaitForMultipleObjects(2, hProcesses, TRUE, INFINITE);

    std::cout << "All child processes finished. Parent exiting." << std::endl;

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>    
//#include <sys/wait.h>  
//#include <time.h>      
//#include <cmath>
//
//bool IsPrime(int n) {
//    if (n <= 1) return false;
//    for (int i = 2; i * i <= n; ++i) {
//        if (n % i == 0) return false;
//    }
//    return true;
//}
//
//void WorkerTask(int durationMs) {
//    pid_t myPid = getpid();
//    std::cout << "[Child " << myPid << "] Started. Duration: " << durationMs << " ms." << std::endl;
//
//    struct timespec start, now;
//    clock_gettime(CLOCK_MONOTONIC, &start);
//
//    int number = 0;
//    int count = 0;
//    long long elapsedMs = 0;
//
//    while (true) {
//        number++;
//
//        if (IsPrime(number)) {
//            count++;
//        }
//
//        if (number % 1000 == 0) {
//            clock_gettime(CLOCK_MONOTONIC, &now);
//
//            elapsedMs = (now.tv_sec - start.tv_sec) * 1000 +
//                (now.tv_nsec - start.tv_nsec) / 1000000;
//
//            if (elapsedMs >= durationMs) {
//                break;
//            }
//        }
//    }
//
//    std::cout << "[Child " << myPid << "] Finished. Found " << count << " primes. Time: " << elapsedMs << " ms." << std::endl;
//}
//
//int main() {
//    std::cout << "[Parent] Process started." << std::endl;
//
//    pid_t pid1 = fork();
//
//    if (pid1 == -1) {
//        std::cerr << "Failed to fork process 1" << std::endl;
//        return 1;
//    }
//
//    if (pid1 == 0) {
//        WorkerTask(60000); 
//        return 0;          
//    }
//
//    std::cout << "[Parent] Launched child 1 (PID: " << pid1 << ")" << std::endl;
//
//    pid_t pid2 = fork();
//
//    if (pid2 == -1) {
//        std::cerr << "Failed to fork process 2" << std::endl;
//    }
//
//    if (pid2 == 0) {
//        WorkerTask(120000);
//        return 0;          
//    }
//
//    std::cout << "[Parent] Launched child 2 (PID: " << pid2 << ")" << std::endl;
//
//    std::cout << "[Parent] Waiting for children..." << std::endl;
//
//    wait(NULL); 
//    std::cout << "[Parent] One child finished." << std::endl;
//
//    wait(NULL);
//    std::cout << "[Parent] All children finished. Exiting." << std::endl;
//
//    return 0;
//}