//для Windows
#include <iostream>
#include <windows.h>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 4)
    {
        cout << "Usage: Lab05b <AffinityMask> <Priority1> <Priority2>" << endl;
        return 1;
    }

    DWORD_PTR affinityMask = atoi(argv[1]);
    int priority1 = atoi(argv[2]);
    int priority2 = atoi(argv[3]);

    cout << "=== Lab-05b Parameters ===" << endl;
    cout << "Affinity mask: " << affinityMask << endl;
    cout << "Priority class for child 1: " << priority1 << endl;
    cout << "Priority class for child 2: " << priority2 << endl;

    SetProcessAffinityMask(GetCurrentProcess(), affinityMask);

    STARTUPINFO si1 = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi1;

    STARTUPINFO si2 = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi2;

    if (CreateProcess(
        L"Lab-05x.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si1, &pi1))
    {
        SetPriorityClass(pi1.hProcess, priority1);
        cout << "Child process 1 started (PID: " << pi1.dwProcessId << ")" << endl;
    }
    else
    {
        cout << "Error starting child process 1" << endl;
    }

    if (CreateProcess(
        L"Lab-05x.exe",
        NULL, NULL, NULL, FALSE,
        CREATE_NEW_CONSOLE, NULL, NULL, &si2, &pi2))
    {
        SetPriorityClass(pi2.hProcess, priority2);
        cout << "Child process 2 started (PID: " << pi2.dwProcessId << ")" << endl;
    }
    else
    {
        cout << "Error starting child process 2" << endl;
    }

    HANDLE processes[2] = { pi1.hProcess, pi2.hProcess };
    WaitForMultipleObjects(2, processes, TRUE, INFINITE);

    cout << "Both child processes finished." << endl;

    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    system("pause");
    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>
//#include <sys/wait.h>
//#include <cstdlib>
//#include <sched.h>
//
//int main(int argc, char* argv[]) {
//    if (argc != 4) return 1;
//
//    int P1 = atoi(argv[1]);
//    int P2 = atoi(argv[2]);
//    int P3 = atoi(argv[3]);
//
//    std::cout << "P1: " << P1 << "\n";
//    std::cout << "P2: " << P2 << "\n";
//    std::cout << "P3: " << P3 << "\n";
//
//    cpu_set_t set;
//
//    CPU_ZERO(&set);
//    if (P1 == 1) CPU_SET(0, &set);
//    else {
//        long cpus = sysconf(_SC_NPROCESSORS_ONLN);
//
//        for (int i = 0; i < cpus; i++) CPU_SET(i, &set);
//    }
//
//    pid_t c1 = fork();
//
//    if (c1 == 0) {
//        sched_setaffinity(0, sizeof(set), &set);
//        nice(P2);
//        execl("./Lab-05x", "./Lab-05x", NULL);
//        exit(0);
//    }
//
//    pid_t c2 = fork();
//
//    if (c2 == 0) {
//        sched_setaffinity(0, sizeof(set), &set);
//        nice(P3);
//        execl("./Lab-05x", "./Lab-05x", NULL);
//        exit(0);
//    }
//
//    waitpid(c1, NULL, 0);
//    waitpid(c2, NULL, 0);
//
//    return 0;
//}

