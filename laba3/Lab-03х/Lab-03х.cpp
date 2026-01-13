//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "russian");
    int iterations = 0;

    if (argc > 1) {
        iterations = atoi(argv[1]);
    }
    else {
        char* envValue = getenv("ITER_NUM");
        if (envValue != NULL) {
            iterations = atoi(envValue);
        }
    }
    if (iterations <= 0) {
        fprintf(stderr, "Ошибка: не задано количество итераций (через аргумент или переменную среды ITER_NUM)\n");
        ExitProcess(1);
    }
    DWORD pid = GetCurrentProcessId();
    printf("Количество итераций: %d\n", iterations);
    for (int i = 1; i <= iterations; i++) {
        printf("PID: %lu — итерация %d\n", pid, i);
        Sleep(500);  // задержка 500 мс
    }
    return 0;
}

//для Linux
//#include <iostream>
//#include <cstdlib>
//#include <unistd.h>
//#include <chrono>
//#include <thread>
//int main(int argc, char* argv[]) {
//    int iterations = 0;
//
//    if (argc >= 2) {
//        iterations = std::atoi(argv[1]);
//    }
//    else {
//        const char* env = std::getenv("ITER_NUM");
//        if (env) {
//            iterations = std::atoi(env);
//        }
//        else {
//            std::cerr << "Error: number of iterations not specified (argument or ITER_NUM environment variable)" << std::endl;
//            return 1;
//        }
//    }
//    std::cout << "Number of iterations: " << iterations << std::endl;
//
//    pid_t pid = getpid();
//    for (int i = 1; i <= iterations; ++i) {
//        std::cout << "PID: " << pid << " - iteration " << i << std::endl;
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//    }
//    return 0;
//}
