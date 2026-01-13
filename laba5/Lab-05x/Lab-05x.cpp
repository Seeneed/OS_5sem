//для Windows
#include <windows.h>
#include <iostream>
#include <ctime>

using namespace std;

int main()
{
    DWORD processId = GetCurrentProcessId();
    DWORD threadId = GetCurrentThreadId();
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    clock_t start = clock();

    for (int i = 1; i <= 1000000; i++)
    {
        if (i % 1000 == 0)
        {
            cout << "Iteration: " << i << endl;
            cout << "Process ID: " << processId << endl;
            cout << "Thread ID: " << threadId << endl;
            cout << "Process priority class: " << GetPriorityClass(hProcess) << endl;
            cout << "Thread priority: " << GetThreadPriority(hThread) << endl;
            cout << "Processor number: " << GetCurrentProcessorNumber() << endl;
            cout << "-----------------------------" << endl;
            Sleep(200);
        }
    }

    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    cout << "Total time: " << elapsed << " seconds" << endl;

    system("pause");
    return 0;
}

//для Linux
//#include <iostream>
//#include <sys/syscall.h>
//#include <unistd.h>
//#include <sys/resource.h>
//#include <sched.h>
//#include <ctime>
//
//int main() {
//    clock_t start = clock();
//
//    pid_t pid = getpid();
//    long tid = syscall(SYS_gettid);
//
//    for (int i = 1; i <= 1000000; i++) {
//        if (i % 1000 == 0) {
//            int nice_level = getpriority(PRIO_PROCESS, pid);
//            int cpu = sched_getcpu();
//
//            std::cout << "Iteration: " << i << "\n";
//            std::cout << "Process ID: " << pid << "\n";
//            std::cout << "Thread ID: " << tid << "\n";
//            std::cout << "Niceness: " << nice_level << "\n";
//            std::cout << "Current CPU: " << cpu << "\n\n";
//
//            usleep(200000);
//        }
//    }
//
//    clock_t end = clock();
//
//    double elapsed = double(end - start) / CLOCKS_PER_SEC;
//
//    std::cout << "Time elapsed: " << elapsed << " sec\n";
//
//    return 0;
//}