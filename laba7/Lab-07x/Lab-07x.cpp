//для Windows
#include <windows.h>
#include <iostream>

bool IsPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; ++i) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    DWORD durationLimit = 0;

    if (argc > 1) {
        durationLimit = atoi(argv[1]);
    }

    DWORD startTime = GetTickCount();

    int number = 0;
    int count = 0;

    if (durationLimit == 0) {
        std::cout << "Mode: Infinite run (no time limit set)" << std::endl;
    }
    else {
        std::cout << "Mode: Run for " << durationLimit << " ms" << std::endl;
    }

    while (true) {
        number++;

        if (IsPrime(number)) {
            count++;
            std::cout << count << ": " << number << std::endl;
        }

        if (durationLimit > 0) {
            DWORD current = GetTickCount();
            if ((current - startTime) >= durationLimit) {
                break;
            }
        }
    }

    DWORD totalTime = GetTickCount() - startTime;
    std::cout << "\nProcess finished.\n";
    std::cout << "Total work time: " << totalTime << " ms." << std::endl;

    system("pause");

    return 0;
}