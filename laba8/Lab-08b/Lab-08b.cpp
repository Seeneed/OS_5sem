//для Windows
#include <windows.h>
#include <iostream>
using namespace std;

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    DWORD pageSize = si.dwPageSize;
    DWORD totalPages = 256;
    SIZE_T totalSize = static_cast<SIZE_T>(pageSize) * totalPages;

    LPVOID pVirtual = VirtualAlloc(NULL, totalSize, MEM_RESERVE, PAGE_NOACCESS);
    cout << "1. Память зарезервирована (256 страниц). Адрес: " << pVirtual << endl;
    system("pause");

    SIZE_T halfSize = static_cast<SIZE_T>(pageSize) * 128;
    LPVOID pHalf = static_cast<LPVOID>(static_cast<BYTE*>(pVirtual) + halfSize);
    VirtualAlloc(pHalf, halfSize, MEM_COMMIT, PAGE_READWRITE);
    cout << "2. Физическая память выделена для 128 страниц (вторая половина)." << endl;
    system("pause");

    int* pInt = static_cast<int*>(pHalf);
    DWORD numElements = static_cast<DWORD>(halfSize / sizeof(int));
    for (DWORD i = 0; i < numElements; ++i) {
        pInt[i] = static_cast<int>(i);
    }
    cout << "3. Память заполнена числами (наполнение физических страниц)." << endl;
    system("pause");

    DWORD oldProtect;
    VirtualProtect(pHalf, halfSize, PAGE_READONLY, &oldProtect);
    cout << "4. Атрибуты защиты изменены на PAGE_READONLY." << endl;
    system("pause");

    VirtualFree(pHalf, halfSize, MEM_DECOMMIT);
    cout << "5. Физическая память (Decommit) освобождена." << endl;
    system("pause");

    VirtualFree(pVirtual, 0, MEM_RELEASE);
    cout << "6. Виртуальная память полностью освобождена (Release)." << endl;
    system("pause");

    return 0;
}

//для Linux
//#include <iostream>
//#include <sys/mman.h>
//#include <unistd.h>
//
//using namespace std;
//
//int main() {
//    long pageSize = sysconf(_SC_PAGESIZE);
//    size_t totalPages = 256;
//    size_t halfPages = 128;
//    size_t totalSize = (size_t)pageSize * totalPages;
//    size_t halfSize = (size_t)pageSize * halfPages;
//
//    void* pVirtual = mmap(NULL, totalSize, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
//    cout << "PID: " << getpid() << endl;
//    cout << "1. Memory reserved (256 pages). Address: " << pVirtual << endl;
//    cin.get();
//
//    void* pHalf = (void*)((char*)pVirtual + halfSize);
//    mprotect(pHalf, halfSize, PROT_READ | PROT_WRITE);
//    cout << "2. Physical memory allocated for the second half (128 pages) (RW)." << endl;
//    cin.get();
//
//    int* pInt = (int*)pHalf;
//    size_t numElements = halfSize / sizeof(int);
//    for (size_t i = 0; i < numElements; ++i) {
//        pInt[i] = (int)i;
//    }
//    cout << "3. Memory filled with a sequence of numbers." << endl;
//    cin.get();
//
//    mprotect(pHalf, halfSize, PROT_READ);
//    cout << "4. Protection attributes changed to READ ONLY." << endl;
//    cin.get();
//
//    munmap(pHalf, halfSize);
//    cout << "5. 128 pages freed (second half)." << endl;
//    cin.get();
//
//    munmap(pVirtual, halfSize);
//    cout << "6. Remaining pages freed." << endl;
//    cin.get();
//
//    return 0;
//}

