//для Windows
#include <windows.h>
#include <iostream>
#include <bitset>

using namespace std;

int main()
{
    HANDLE hProcess = GetCurrentProcess();
    HANDLE hThread = GetCurrentThread();

    DWORD_PTR processAffinityMask = 0;
    DWORD_PTR systemAffinityMask = 0;

    GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask);

    bitset<sizeof(DWORD_PTR) * 8> pMask(processAffinityMask);
    bitset<sizeof(DWORD_PTR) * 8> sMask(systemAffinityMask);

    cout << "=== Process and Thread Info ===" << endl;
    cout << "Process ID: " << GetCurrentProcessId() << endl;
    cout << "Thread ID: " << GetCurrentThreadId() << endl;
    cout << "Process priority class: " << GetPriorityClass(hProcess) << endl;
    cout << "Thread priority: " << GetThreadPriority(hThread) << endl;
    cout << "Process affinity mask (bin): " << pMask << endl;
    cout << "System affinity mask (bin):  " << sMask << endl;
    cout << "Processors available to process: " << pMask.count() << endl;
    cout << "Current processor number: " << GetCurrentProcessorNumber() << endl;

    system("pause");
    return 0;
}

//для Linux
//#include <cstdio>       
//#include <unistd.h>     
//#include <sys/syscall.h>
//#include <pthread.h>    
//#include <sched.h>      
//#include <sys/resource.h>
//
//int main()
//{
//    pid_t pid = getpid();
//
//    long tid = syscall(SYS_gettid);
//
//    int prio_process = getpriority(PRIO_PROCESS, pid);
//
//    sched_param sp;
//    int policy;
//    pthread_getschedparam(pthread_self(), &policy, &sp);
//    int prio_thread = sp.sched_priority;
//
//    cpu_set_t mask;
//    CPU_ZERO(&mask);
//    if (sched_getaffinity(0, sizeof(mask), &mask) == -1) {
//        perror("sched_getaffinity failed");
//    }
//
//    long cpus = sysconf(_SC_NPROCESSORS_ONLN);
//
//    int current_cpu = sched_getcpu();
//
//    printf("Process ID: %d\n", pid);
//    printf("Thread ID: %ld\n", tid);
//    printf("Process priority (nice): %d\n", prio_process);
//    printf("Thread priority: %d\n", prio_thread);
//
//    printf("Affinity mask (binary): ");
//    for (int i = cpus - 1; i >= 0; i--) {
//        printf("%d", CPU_ISSET(i, &mask) ? 1 : 0);
//    }
//    printf("\n");
//
//    printf("Available processors: %ld\n", cpus);
//    printf("Current CPU: %d\n", current_cpu);
//
//    return 0;
//}

