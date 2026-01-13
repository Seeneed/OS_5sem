//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <stdio.h>

CRITICAL_SECTION cs;

struct ThreadArgs {
    int N;
    const wchar_t* name;
};

DWORD WINAPI ThreadFunc(LPVOID arg) {
    ThreadArgs* data = (ThreadArgs*)arg;
    int N = data->N;
    const wchar_t* threadName = data->name;

    wchar_t username[256];
    DWORD len = GetEnvironmentVariableW(L"USERNAME", username, 256);
    if (len == 0 || len >= 256) wcscpy(username, L"Unknown");
    int uname_len = wcslen(username);

    for (int i = 1; i <= N; i++) {

        if (i == 30) {
            EnterCriticalSection(&cs);
        }

        wprintf(L"Поток=%ls  Итерация=%d  Символ=%lc\n",
            threadName,
            i,
            username[(i - 1) % uname_len]);

        if (i == 60) {
            LeaveCriticalSection(&cs);
        }

        Sleep(100);
    }

    return 0;
}

int wmain() {
    setlocale(LC_ALL, "Rus");

    InitializeCriticalSection(&cs);

    HANDLE threads[2];

    ThreadArgs argA{ 90, L"A" };
    ThreadArgs argB{ 90, L"B" };
    ThreadArgs argMain{ 90, L"C" };

    threads[0] = CreateThread(NULL, 0, ThreadFunc, &argA, 0, NULL);
    threads[1] = CreateThread(NULL, 0, ThreadFunc, &argB, 0, NULL);

    if (!threads[0] || !threads[1]) {
        wprintf(L"Ошибка создания потоков!\n");
        return 1;
    }

    wprintf(L"Созданы потоки A и B.\n\n");

    ThreadFunc(&argMain);

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    CloseHandle(threads[0]);
    CloseHandle(threads[1]);

    DeleteCriticalSection(&cs);

    wprintf(L"\nВсе потоки завершены.\n");
    return 0;
}

//для Linux
//#include <pthread.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <pwd.h>
//#include <sys/syscall.h>
//#include <string>
//#include <iostream>
//
//pthread_mutex_t mutex;
//
//struct ThreadArgs {
//    int N;
//    const char* name;
//};
//
//void* ThreadFunc(void* arg) {
//    ThreadArgs* data = (ThreadArgs*)arg;
//    int N = data->N;
//    const char* threadName = data->name;
//
//    struct passwd* pw = getpwuid(getuid());
//    std::string username = pw ? pw->pw_name : "Unknown";
//    int uname_len = username.length();
//
//    pid_t tid = syscall(SYS_gettid);
//
//    for (int i = 1; i <= N; i++) {
//
//        if (i == 30) pthread_mutex_lock(&mutex);
//
//        std::cout << "Thread=" << threadName
//            << " | TID=" << tid
//            << " | Iteration=" << i
//            << " | Char=" << username[(i - 1) % uname_len]
//            << std::endl;
//
//        if (i == 60) pthread_mutex_unlock(&mutex);
//
//        usleep(100 * 1000);
//    }
//    return nullptr;
//}
//
//int main() {
//    if (pthread_mutex_init(&mutex, nullptr) != 0) {
//        std::cerr << "Error initializing mutex!" << std::endl;
//        return 1;
//    }
//
//    pthread_t threads[2];
//
//    ThreadArgs argA{ 90, "A" };
//    ThreadArgs argB{ 90, "B" };
//    ThreadArgs argM{ 90, "M" };
//
//    if (pthread_create(&threads[0], nullptr, ThreadFunc, &argA) != 0 ||
//        pthread_create(&threads[1], nullptr, ThreadFunc, &argB) != 0) {
//        std::cerr << "Error creating threads!" << std::endl;
//        pthread_mutex_destroy(&mutex);
//        return 1;
//    }
//    std::cout << "Threads A and B created.\n\n";
//
//    ThreadFunc(&argM);
//
//    pthread_join(threads[0], nullptr);
//    pthread_join(threads[1], nullptr);
//
//    std::cout << "\nAll threads finished.\n";
//
//    pthread_mutex_destroy(&mutex);
//    return 0;
//}