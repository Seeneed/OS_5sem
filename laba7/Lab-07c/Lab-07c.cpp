//для Windows
#include <windows.h>
#include <iostream>

int main()
{
    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    if (!hTimer) return 1;

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -30000000LL;

    if (!SetWaitableTimer(hTimer, &liDueTime, 3000, NULL, NULL, FALSE)) {
        return 1;
    }

    unsigned long long counter = 0;
    int triggerCount = 0;

    std::cout << "Start counting with Waitable Timer..." << std::endl;

    while (true) {
        counter++;

        if (WaitForSingleObject(hTimer, 0) == WAIT_OBJECT_0) {
            triggerCount++;
            std::cout << "Timer triggered (" << triggerCount * 3 << " sec): "
                << counter << " iterations" << std::endl;

            if (triggerCount >= 5) {
                break;
            }
        }
    }

    std::cout << "Final result: " << counter << " total iterations" << std::endl;

    CloseHandle(hTimer);
    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>
//#include <sys/timerfd.h> 
//#include <time.h>
//#include <cstdint>       
//
//int main()
//{
//    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
//    if (tfd == -1) {
//        return 1;
//    }
//
//    struct itimerspec new_value {};
//
//    new_value.it_interval.tv_sec = 3;
//    new_value.it_interval.tv_nsec = 0;
//
//    new_value.it_value.tv_sec = 3;
//    new_value.it_value.tv_nsec = 0;
//
//    if (timerfd_settime(tfd, 0, &new_value, NULL) == -1) {
//        return 1;
//    }
//
//    unsigned long long counter = 0;
//    int triggerCount = 0;
//    uint64_t exp;
//
//    std::cout << "Start counting with timerfd (Linux)..." << std::endl;
//
//    while (true) {
//        counter++;
//
//        ssize_t s = read(tfd, &exp, sizeof(uint64_t));
//
//        if (s == sizeof(uint64_t)) {
//            triggerCount++;
//            std::cout << "Timer triggered (" << triggerCount * 3 << " sec): "
//                << counter << " iterations" << std::endl;
//
//            if (triggerCount >= 5) {
//                break;
//            }
//        }
//    }
//
//    std::cout << "Final result: " << counter << " total iterations" << std::endl;
//
//    close(tfd);
//    return 0;
//}