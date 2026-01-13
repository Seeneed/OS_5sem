//для Windows
#include <windows.h>
#include <iostream>
using namespace std;

int globalInit = 10;
int globalUninit;

static int globalStaticInit = 20;
static int globalStaticUninit;

void func() {
    return;
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int localInit = 30;
    int localUninit;

    static int localStaticInit = 40;
    static int localStaticUninit;

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    cout << "PID процесса: " << GetCurrentProcessId() << endl;
    cout << "------------------------------------------" << endl;

    cout << "Функция (func):    " << reinterpret_cast<void*>(func) << endl;
    cout << "Функция (main):             " << reinterpret_cast<void*>(main) << endl;

    cout << "Глобальная (init):          " << &globalInit << endl;
    cout << "Глобальная (uninit):        " << &globalUninit << endl;

    cout << "Глобальная статич. (init):  " << &globalStaticInit << endl;
    cout << "Глобальная статич. (uninit):" << &globalStaticUninit << endl;

    cout << "Локальная статич. (init):   " << &localStaticInit << endl;
    cout << "Локальная статич. (uninit): " << &localStaticUninit << endl;

    cout << "Локальная (init):           " << &localInit << endl;
    cout << "Локальная (uninit):         " << &localUninit << endl;

    cout << "Аргумент (argc):            " << &argc << endl;
    cout << "Аргумент (argv):            " << &argv << endl;

    cout << "------------------------------------------" << endl;
    system("pause");

    return 0;
}

//для Linux
//#include <iostream>
//#include <unistd.h>
//
//using namespace std;
//
//int globalInit = 10;
//int globalUninit;
//
//static int globalStaticInit = 20;
//static int globalStaticUninit;
//
//void func() {
//    return;
//}
//
//int main(int argc, char* argv[]) {
//    int localInit = 30;
//    int localUninit;
//
//    static int localStaticInit = 40;
//    static int localStaticUninit;
//
//    cout << "Process PID: " << getpid() << endl;
//    cout << "------------------------------------------" << endl;
//
//    cout << "Function (func):            " << reinterpret_cast<void*>(func) << endl;
//    cout << "Function (main):            " << reinterpret_cast<void*>(main) << endl;
//
//    cout << "Global (init):              " << &globalInit << endl;
//    cout << "Global (uninit):            " << &globalUninit << endl;
//
//    cout << "Global static (init):       " << &globalStaticInit << endl;
//    cout << "Global static (uninit):     " << &globalStaticUninit << endl;
//
//    cout << "Local static (init):        " << &localStaticInit << endl;
//    cout << "Local static (uninit):      " << &localStaticUninit << endl;
//
//    cout << "Local (init):               " << &localInit << endl;
//    cout << "Local (uninit):             " << &localUninit << endl;
//
//    cout << "Argument (argc):            " << &argc << endl;
//    cout << "Argument (argv):            " << &argv << endl;
//
//    cout << "------------------------------------------" << endl;
//    cin.get();
//
//    return 0;
//}
