//для Windows
#include <windows.h>
#include <iostream>

int main()
{
    unsigned long long counter = 0;

    DWORD startTime = GetTickCount();

    bool printed5 = false;
    bool printed10 = false;

    std::cout << "Start counting..." << std::endl;

    while (true) {
        counter++;

        DWORD elapsed = GetTickCount() - startTime;

        if (!printed5 && elapsed >= 5000) {
            std::cout << "5 seconds: " << counter << " iterations" << std::endl;
            printed5 = true;
        }
        else if (!printed10 && elapsed >= 10000) {
            std::cout << "10 seconds: " << counter << " iterations" << std::endl;
            printed10 = true;
        }
        else if (elapsed >= 15000) {
            std::cout << "15 seconds (Finish): " << counter << " total iterations" << std::endl;
            break;
        }
    }

    return 0;
}

//для Linux
//#include <iostream>
//#include <ctime>        
//#include <time.h>      
//#include <unistd.h>
//
//int main()
//{
//    unsigned long long counter = 0;
//
//    clock_t cpu_start = clock();
//
//    struct timespec real_start, real_current;
//    clock_gettime(CLOCK_MONOTONIC, &real_start);
//
//    bool printed5 = false;
//    bool printed10 = false;
//
//    std::cout << "Start counting (Tracking CPU Time)..." << std::endl;
//
//    while (true) {
//        counter++;
//
//        clock_t cpu_now = clock();
//        double cpu_elapsed_sec = (double)(cpu_now - cpu_start) / CLOCKS_PER_SEC;
//
//        if (!printed5 && cpu_elapsed_sec >= 5.0) {
//            std::cout << "CPU time 5 sec: " << counter << " iterations" << std::endl;
//            printed5 = true;
//        }
//
//        else if (!printed10 && cpu_elapsed_sec >= 10.0) {
//            std::cout << "CPU time 10 sec: " << counter << " iterations" << std::endl;
//            printed10 = true;
//        }
//
//        else if (cpu_elapsed_sec >= 15.0) {
//            std::cout << "CPU time 15 sec (Finish): " << counter << " total iterations" << std::endl;
//            break;
//        }
//    }
//
//    clock_gettime(CLOCK_MONOTONIC, &real_current);
//
//    double real_elapsed_sec = (real_current.tv_sec - real_start.tv_sec) +
//        (real_current.tv_nsec - real_start.tv_nsec) / 1e9;
//
//    std::cout << "Real (Wall-clock) time passed: " << real_elapsed_sec << " sec." << std::endl;
//
//    return 0;
//}