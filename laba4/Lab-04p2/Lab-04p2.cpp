//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_GLOBAL_PRIMES 220000 

typedef struct {
    int start, end;
} Range;

int globalPrimes[MAX_GLOBAL_PRIMES];
int globalIndex = 0;
CRITICAL_SECTION g_cs;

__declspec(thread) int localCount;

int isPrime(int n) {
    if (n < 2) return 0;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return 0;
    }
    return 1;
}

DWORD WINAPI L4Primes(LPVOID arg) {
    Range* range = (Range*)arg;
    int* localPrimes = (int*)malloc(sizeof(int) * (range->end - range->start + 1));
    if (localPrimes == NULL) return 1;
    localCount = 0; 
    for (int i = range->start; i <= range->end; i++) {
        if (isPrime(i)) {
            localPrimes[localCount++] = i;
        }
    }
    EnterCriticalSection(&g_cs);
    for (int i = 0; i < localCount; i++) {
        if (globalIndex < MAX_GLOBAL_PRIMES) {
            globalPrimes[globalIndex++] = localPrimes[i];
        }
    }
    LeaveCriticalSection(&g_cs);
    free(localPrimes);
    return 0;
}

int cmp(const void* a, const void* b) {
    return (*(int*)a) - (*(int*)b);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("Usage: Lab-04p2 <threads> <low> <high>\n");
        return 1;
    }
    int nThreads = atoi(argv[1]);
    int low = atoi(argv[2]);
    int high = atoi(argv[3]);
    InitializeCriticalSection(&g_cs);
    HANDLE* threads = (HANDLE*)malloc(sizeof(HANDLE) * nThreads);
    Range* ranges = (Range*)malloc(sizeof(Range) * nThreads);
    int rangeSize = (high - low + 1) / nThreads;
    for (int i = 0; i < nThreads; i++) {
        ranges[i].start = low + i * rangeSize;
        ranges[i].end = (i == nThreads - 1) ? high : ranges[i].start + rangeSize - 1;
        threads[i] = CreateThread(NULL, 0, L4Primes, &ranges[i], 0, NULL);
    }
    LARGE_INTEGER t1, t2, freq;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t1);
    WaitForMultipleObjects(nThreads, threads, TRUE, INFINITE);
    QueryPerformanceCounter(&t2);
    double elapsed = (double)(t2.QuadPart - t1.QuadPart) / freq.QuadPart * 1000;
    qsort(globalPrimes, globalIndex, sizeof(int), cmp);
    printf("Primes:\n");
    for (int i = 0; i < globalIndex; i++) {
        printf("%d ", globalPrimes[i]);
    }
    printf("\n");
    printf("Lab-04p2: %d threads, time = %.3f ms, primes found = %d\n", nThreads, elapsed, globalIndex);
    for (int i = 0; i < nThreads; i++) CloseHandle(threads[i]);
    free(threads);
    free(ranges);
    DeleteCriticalSection(&g_cs);
    return 0;
}

//для Linux
//#include <stdio.h>
//#include <stdlib.h>
//#include <pthread.h>
//#include <unistd.h>
//#include <time.h>
//
//#define MAX_GLOBAL 2000000
//#define MAX_LOCAL 1000000
//
//int globalPrimes[MAX_GLOBAL];
//int globalIndex = 0;
//
//__thread int localPrimes[MAX_LOCAL];
//__thread int localCount;
//
//int isPrime(int n) {
//    if (n < 2) return 0;
//    for (int i = 2; i * i <= n; i++)
//        if (n % i == 0) return 0;
//    return 1;
//}
//
//void* L4Primes(void* arg) {
//    int* range = (int*)arg;
//    int start = range[0];
//    int end = range[1];
//
//    localCount = 0;
//    for (int i = start; i <= end; i++)
//        if (isPrime(i)) localPrimes[localCount++] = i;
//
//    for (int i = 0; i < localCount; i++)
//        globalPrimes[globalIndex++] = localPrimes[i];
//
//    return NULL;
//}
//
//int cmp(const void* a, const void* b) {
//    return (*(int*)a) - (*(int*)b);
//}
//
//int main(int argc, char* argv[]) {
//    if (argc != 4) {
//        printf("Usage: %s <threads> <low> <high>\n", argv[0]);
//        return 1;
//    }
//
//    int nThreads = atoi(argv[1]);
//    int low = atoi(argv[2]);
//    int high = atoi(argv[3]);
//
//    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * nThreads);
//    int (*ranges)[2] = (int(*)[2])malloc(sizeof(int[2]) * nThreads);
//
//    int rangeSize = (high - low + 1) / nThreads;
//
//    struct timespec startTime, endTime;
//    clock_gettime(CLOCK_MONOTONIC, &startTime);
//
//    for (int i = 0; i < nThreads; i++) {
//        ranges[i][0] = low + i * rangeSize;
//        ranges[i][1] = (i == nThreads - 1) ? high : ranges[i][0] + rangeSize - 1;
//        pthread_create(&threads[i], NULL, L4Primes, ranges[i]);
//    }
//
//    for (int i = 0; i < nThreads; i++)
//        pthread_join(threads[i], NULL);
//
//    qsort(globalPrimes, globalIndex, sizeof(int), cmp);
//
//    clock_gettime(CLOCK_MONOTONIC, &endTime);
//    double elapsed = (endTime.tv_sec - startTime.tv_sec) * 1000.0 +
//        (endTime.tv_nsec - startTime.tv_nsec) / 1000000.0;
//
//    printf("Primes found: %d\n", globalIndex);
//    for (int i = 0; i < globalIndex; i++)
//        printf("%d ", globalPrimes[i]);
//    printf("\nElapsed time: %.3f ms\n", elapsed);
//
//    free(threads);
//    free(ranges);
//
//    return 0;
//}
