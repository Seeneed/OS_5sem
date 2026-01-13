//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>

DWORD WINAPI Lab_04x(LPVOID arg) {
    int N = *(int*)arg;
    wchar_t username[256];
    DWORD len = GetEnvironmentVariableW(L"USERNAME", username, 256);
    if (len == 0 || len >= 256) wcscpy(username, L"Unknown");
    int uname_len = wcslen(username);
    for (int i = 0; i < N; i++) {
        wprintf(L"PID=%lu - TID=%lu - Iteration=%d - %lc\n",
            GetCurrentProcessId(),
            GetCurrentThreadId(),
            i + 1,
            username[i % uname_len]);
        Sleep(300 + rand() % 101); // 300-400 ms
    }
    return 0;
}

int wmain() {
    SetConsoleOutputCP(65001);
    HANDLE threads[2];
    int iters1 = 50, iters2 = 125, itersMain = 100;
    threads[0] = CreateThread(NULL, 0, Lab_04x, &iters1, 0, NULL);
    threads[1] = CreateThread(NULL, 0, Lab_04x, &iters2, 0, NULL);
    if (!threads[0] || !threads[1]) {
        wprintf(L"Thread creation error!\n");
        return 1;
    }
    wprintf(L"Thread 1 TID=%lu\n", GetThreadId(threads[0]));
    wprintf(L"Thread 2 TID=%lu\n", GetThreadId(threads[1]));
    wprintf(L"Main thread TID=%lu\n\n", GetCurrentThreadId());
    Lab_04x(&itersMain);
    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    wprintf(L"\nAll threads finished.\n");
    return 0;
}

//для Linux (приложение Lab-04px: функция + main)
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h>
//#include <pthread.h>
//#include <sys/types.h>
//#include <stdint.h>
//
//void* Lab_04x(void* arg) {
//    int N = ((int*)arg)[0];
//    int num = ((int*)arg)[1];
//    const char* username = getenv("USER");
//    if (!username) username = "Unknown";
//    int uname_len = strlen(username);
//
//    for (int i = 0; i < N; i++) {
//        printf("Thread#%d PID=%d – TID=%lu – Iteration=%d – %c\n",
//            num,
//            getpid(),
//            (unsigned long)pthread_self(),
//            i + 1,
//            username[i % uname_len]);
//        fflush(stdout);
//        usleep((300 + rand() % 101) * 1000);
//    }
//    return NULL;
//}
//
//int main() {
//    pthread_t t1, t2;
//    int args1[2] = { 50, 1 };
//    int args2[2] = { 125, 2 };
//    int argsMain[2] = { 100, 0 };
//
//    pthread_create(&t1, NULL, Lab_04x, args1);
//    pthread_create(&t2, NULL, Lab_04x, args2);
//
//    Lab_04x(argsMain);
//
//    pthread_join(t1, NULL);
//    pthread_join(t2, NULL);
//
//    printf("\nAll threads finished.\n");
//    return 0;
//}


