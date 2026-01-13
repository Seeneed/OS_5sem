//для Windows
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <ctime> 

using namespace std;

void threadFunc(const string& name)
{
    clock_t start = clock();

    for (int i = 0; i < 1000000; i++) {
        for (volatile int k = 0; k < 10000; ++k) {}
        if (i % 1000 == 0) {
            cout << name << " iteration: " << i
                << " | ProcID: " << GetCurrentProcessId()
                << " | ThreadID: " << GetCurrentThreadId()
                << " | Proc#: " << GetCurrentProcessorNumber() << endl;
            Sleep(200);
        }
    }

    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC;

    cout << name << " finished. Total time: " << elapsed << " seconds" << endl;
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");

    if (argc < 5) {
        cout << "Usage: Lab05c <P1 mask> <P2 proc_priority> <P3 thread1_priority> <P4 thread2_priority>\n";
        return 0;
    }

    DWORD_PTR mask = stoi(argv[1]);
    int procPriority = stoi(argv[2]);
    int thread1Priority = stoi(argv[3]);
    int thread2Priority = stoi(argv[4]);

    cout << "=== Parameters ===" << endl;
    cout << "Affinity mask (P1): " << mask << endl;
    cout << "Process priority (P2): " << procPriority << endl;
    cout << "Thread 1 priority (P3): " << thread1Priority << endl;
    cout << "Thread 2 priority (P4): " << thread2Priority << endl;

    HANDLE hProcess = GetCurrentProcess();
    SetProcessAffinityMask(hProcess, mask);
    SetPriorityClass(hProcess, procPriority);

    HANDLE t1 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)
        [](LPVOID)->DWORD { threadFunc("Thread 1"); return 0; }, nullptr, 0, nullptr);
    HANDLE t2 = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)
        [](LPVOID)->DWORD { threadFunc("Thread 2"); return 0; }, nullptr, 0, nullptr);

    SetThreadPriority(t1, thread1Priority);
    SetThreadPriority(t2, thread2Priority);

    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);

    CloseHandle(t1);
    CloseHandle(t2);

    cout << "All threads completed." << endl;
    system("pause");
    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>
//#include <sys/syscall.h>
//#include <sys/resource.h>
//#include <sched.h>
//#include <cstdlib>
//#include <ctime>       
//#include <pthread.h>   
//
//using namespace std;
//
//struct ThreadArgs {
//    int priority;
//};
//
//void* thread_function(void* arg) {
//    int priority = ((ThreadArgs*)arg)->priority;
//
//    pid_t pid = getpid();
//    long tid = syscall(SYS_gettid);
//
//    setpriority(PRIO_PROCESS, tid, priority);
//
//    clock_t start = clock();
//
//    for (int i = 1; i <= 1000000; i++) {
//        if (i % 1000 == 0) {
//            int nice_level = getpriority(PRIO_PROCESS, tid);
//            int cpu = sched_getcpu();
//
//            cout << "Iteration: " << i << "\n";
//            cout << "Process ID: " << pid << "\n";
//            cout << "Thread ID: " << tid << "\n";
//            cout << "Niceness: " << nice_level << "\n";
//            cout << "Current CPU: " << cpu << "\n\n";
//
//            usleep(200000);
//        }
//    }
//
//    clock_t end = clock();
//    double elapsed = double(end - start) / CLOCKS_PER_SEC;
//
//    cout << ">>> TID " << tid << " Time elapsed: " << elapsed << " sec\n";
//
//    return NULL;
//}
//
//int main(int argc, char* argv[]) {
//    if (argc != 4) return 1;
//
//    int P1 = atoi(argv[1]);
//    int P2 = atoi(argv[2]);
//    int P3 = atoi(argv[3]);
//
//    cpu_set_t set;
//    CPU_ZERO(&set);
//
//    if (P1 == 1) {
//        CPU_SET(0, &set);
//    }
//    else {
//        long cpus = sysconf(_SC_NPROCESSORS_ONLN);
//        for (int i = 0; i < cpus; i++) CPU_SET(i, &set);
//    }
//
//    sched_setaffinity(0, sizeof(set), &set);
//
//    pthread_t t1, t2;
//
//    ThreadArgs args1 = { P2 };
//    ThreadArgs args2 = { P3 };
//
//    pthread_create(&t1, NULL, thread_function, &args1);
//    pthread_create(&t2, NULL, thread_function, &args2);
//
//    pthread_join(t1, NULL);
//    pthread_join(t2, NULL);
//
//    return 0;
//}