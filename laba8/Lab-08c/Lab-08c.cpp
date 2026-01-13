//для Windows
#include <windows.h>
#include <iostream>
using namespace std;

void HeapInfo(HANDLE hHeap) {
    PROCESS_HEAP_ENTRY entry;
    entry.lpData = NULL;
    SIZE_T totalSize = 0;

    cout << "--- Информация о куче ---" << endl;
    while (HeapWalk(hHeap, &entry)) {
        cout << "Адрес: " << entry.lpData
            << " | Размер: " << entry.cbData
            << " | Тип: ";

        if (entry.wFlags & PROCESS_HEAP_REGION) {
            cout << "REGION (Сегмент)";
        }
        else if (entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) {
            cout << "BUSY (Занят)";
        }
        else {
            cout << "FREE (Свободен)";
        }
        cout << endl;
        totalSize += entry.cbData;
    }
    cout << "Общий размер кучи: " << totalSize / 1024 << " КБ" << endl;
    cout << "-------------------------" << endl;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    HANDLE hCustomHeap = HeapCreate(0, 1024 * 1024, 8 * 1024 * 1024);
    HeapInfo(hCustomHeap);
    system("pause & cls");

    void* blocks[10];
    for (int i = 0; i < 10; ++i) {
        blocks[i] = HeapAlloc(hCustomHeap, HEAP_ZERO_MEMORY, 512 * 1024);

        if (blocks[i] != NULL) {
            int* data = static_cast<int*>(blocks[i]);
            DWORD elements = (512 * 1024) / sizeof(int);
            for (DWORD j = 0; j < elements; ++j) {
                data[j] = static_cast<int>(j);
            }
        }

        cout << "Итерация " << i + 1 << " (Выделено 512 КБ):" << endl;
        HeapInfo(hCustomHeap);
        system("pause & cls");
    }

    for (int i = 0; i < 10; ++i) {
        if (blocks[i] != NULL) {
            HeapFree(hCustomHeap, 0, blocks[i]);
        }
    }
    cout << "После освобождения всех блоков:" << endl;
    HeapInfo(hCustomHeap);
    system("pause");

    HeapDestroy(hCustomHeap);
    cout << "Куча уничтожена." << endl;

    return 0;
}

//для Linux
//#include <iostream>
//#include <malloc.h>
//#include <unistd.h>
//#include <vector>
//
//using namespace std;
//
//int main() {
//    const size_t numBlocks = 10;
//    const size_t blockSize = 512 * 1024;
//    const size_t elements = blockSize / sizeof(int);
//    void* blocks[numBlocks];
//
//    cout << "PID: " << getpid() << endl;
//
//    for (int i = 0; i < numBlocks; ++i) {
//        blocks[i] = malloc(blockSize);
//    }
//    cout << "1. Allocated 10 blocks of 512 KiB." << endl;
//    cin.get();
//
//    for (int i = 0; i < numBlocks; ++i) {
//        int* pInt = (int*)blocks[i];
//        for (size_t j = 0; j < elements; ++j) {
//            pInt[j] = (int)j;
//        }
//    }
//    cout << "2. Blocks filled with arrays of numbers." << endl;
//    cin.get();
//
//    for (int i = 0; i < numBlocks; ++i) {
//        free(blocks[i]);
//    }
//    cout << "3. Blocks memory freed (free)." << endl;
//    cin.get();
//
//    malloc_trim(0);
//    cout << "4. Heap destroyed (memory returned to OS)." << endl;
//    cin.get();
//
//    return 0;
//}
