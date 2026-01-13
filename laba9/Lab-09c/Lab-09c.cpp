//для Windows
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <iostream>
#include <string>
#include <clocale>
#include <vector>
#include <algorithm>
static void PrintWinErrorCode(const wchar_t* where)
{
    DWORD e = GetLastError();
    LPWSTR messageBuffer = nullptr;
    DWORD size = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        e,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&messageBuffer,
        0,
        NULL);
    if (size > 0) {
        std::wcerr << where << L" ошибка: " << messageBuffer;
        LocalFree(messageBuffer);
    }
    else {
        std::wcerr << where << L" ошибка. Код ошибки=" << e << L"\n";
    }
}
static bool IsDotOrDotDot(const wchar_t* name)
{
    return (name[0] == L'.' && name[1] == L'\0') ||
        (name[0] == L'.' && name[1] == L'.' && name[2] == L'\0');
}
static std::wstring JoinPath(const std::wstring& dir, const std::wstring& tail)
{
    if (dir.empty()) return tail;
    wchar_t lastChar = dir.back();
    if (lastChar == L'\\' || lastChar == L'/') return dir + tail;
    return dir + L"\\" + tail;
}
static void PrintDirectoryContentsFirstLevel(const std::wstring& dir)
{
    std::wstring mask = JoinPath(dir, L"*");
    WIN32_FIND_DATAW fdata{};
    HANDLE hFind = FindFirstFileW(mask.c_str(), &fdata);
    if (hFind == INVALID_HANDLE_VALUE) {
        PrintWinErrorCode(L"FindFirstFileW");
        return;
    }
    std::wcout << L"Содержимое каталога (первый уровень): " << dir << L"\n";
    std::wcout << L"------------------------------------" << std::endl;
    std::vector<std::wstring> directories;
    std::vector<std::wstring> files;
    do {
        if (IsDotOrDotDot(fdata.cFileName)) continue;

        bool isDir = (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (isDir) {
            directories.push_back(fdata.cFileName);
        }
        else {
            files.push_back(fdata.cFileName);
        }

    } while (FindNextFileW(hFind, &fdata));
    FindClose(hFind);
    std::sort(directories.begin(), directories.end());
    for (const auto& dirName : directories) {
        std::wcout << L"[Папка]    " << dirName << std::endl;
    }
    std::sort(files.begin(), files.end());
    for (const auto& fileName : files) {
        std::wcout << L"[Файл]     " << fileName << std::endl;
    }
    std::wcout << L"------------------------------------" << std::endl;
    std::wcout << L"Всего: " << (directories.size() + files.size()) << L" объектов" << std::endl;
}
static const wchar_t* ActionToString(DWORD action)
{
    switch (action) {
    case FILE_ACTION_ADDED:            return L"Файл/Папка добавлен(а)";
    case FILE_ACTION_REMOVED:          return L"Файл/Папка удален(а)";
    case FILE_ACTION_MODIFIED:         return L"Файл/Папка изменен(а)";
    case FILE_ACTION_RENAMED_OLD_NAME: return L"переименование (старое имя)";
    case FILE_ACTION_RENAMED_NEW_NAME: return L"переименование (новое имя)";
    default:                           return L"неизвестное событие";
    }
}
static void PrintEventDetails(DWORD action, const std::wstring& fileName,
    DWORD attributes = 0, const std::wstring& newFileName = L"")
{
    std::wcout << L"Событие: " << ActionToString(action) << std::endl;

    if (action == FILE_ACTION_RENAMED_OLD_NAME) {
        std::wcout << L"Старое имя: " << fileName << std::endl;
        if (!newFileName.empty()) {
            std::wcout << L"Новое имя: " << newFileName << std::endl;
        }
    }
    else if (action == FILE_ACTION_RENAMED_NEW_NAME) {
        std::wcout << L"Новое имя: " << fileName << std::endl;
    }
    else {
        std::wcout << L"Объект: " << fileName << std::endl;
    }
    if (attributes != 0) {
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
            std::wcout << L"Тип: ПАПКА" << std::endl;
        }
        else {
            std::wcout << L"Тип: ФАЙЛ" << std::endl;
        }
    }
}
int wmain(int argc, wchar_t* argv[])
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    _wsetlocale(LC_ALL, L"Russian");
    if (argc < 2) {
        std::wcerr << L"Использование:\n"
            << L"  Lab_09c.exe \"C:\\Путь\\К\\Каталогу\"\n"
            << L"\nПример:\n"
            << L"  Lab_09c.exe \"C:\\Temp\"\n"
            << L"  Lab_09c.exe \"D:\\Documents\"\n";
        return 1;
    }
    std::wstring dir = argv[1];
    DWORD attr = GetFileAttributesW(dir.c_str());
    if (attr == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"Ошибка: указанный каталог не существует или недоступен.\n";
        PrintWinErrorCode(L"GetFileAttributesW");
        return 2;
    }
    if ((attr & FILE_ATTRIBUTE_DIRECTORY) == 0) {
        std::wcerr << L"Ошибка: указанный путь не является каталогом.\n";
        return 3;
    }
    std::wcout << L"------------------------------------" << std::endl;
    std::wcout << L"Начальное состояние каталога:\n" << std::endl;
    PrintDirectoryContentsFirstLevel(dir);
    HANDLE hDir = CreateFileW(
        dir.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        nullptr
    );
    if (hDir == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Ошибка: не удалось открыть каталог для отслеживания изменений.\n";
        PrintWinErrorCode(L"CreateFileW");
        return 4;
    }
    const DWORD notifyFilter =
        FILE_NOTIFY_CHANGE_FILE_NAME |      // Изменение имени файла
        FILE_NOTIFY_CHANGE_DIR_NAME |       // Изменение имени каталога
        FILE_NOTIFY_CHANGE_ATTRIBUTES |     // Изменение атрибутов
        FILE_NOTIFY_CHANGE_SIZE |           // Изменение размера
        FILE_NOTIFY_CHANGE_LAST_WRITE |     // Изменение времени последней записи
        FILE_NOTIFY_CHANGE_LAST_ACCESS |    // Изменение времени последнего доступа
        FILE_NOTIFY_CHANGE_CREATION |       // Изменение времени создания
        FILE_NOTIFY_CHANGE_SECURITY;        // Изменение безопасности
    std::wcout << L"\nНачинается отслеживание изменений в каталоге: " << dir << std::endl;
    std::wcout << L"Для выхода нажмите Ctrl+C\n" << std::endl;
    HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (hEvent == NULL) {
        std::wcerr << L"Ошибка создания события\n";
        CloseHandle(hDir);
        return 5;
    }
        OVERLAPPED overlapped = {};
        overlapped.hEvent = hEvent;
        BYTE buffer[64 * 1024] = {};
        BOOL ok = ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            notifyFilter,
            NULL,
            &overlapped,
            NULL
        );
        if (!ok) {
            std::wcerr << L"Ошибка при запуске отслеживания изменений.\n";
            PrintWinErrorCode(L"ReadDirectoryChangesW");
            CloseHandle(hEvent);
            CloseHandle(hDir);
            return 6;
        }
        std::wcout << L"Отслеживание запущено успешно.\n" << std::endl;
        while (true) {
            DWORD waitResult = WaitForSingleObject(hEvent, 1000); // Таймаут 1 секунда
            if (waitResult == WAIT_OBJECT_0) {
                DWORD bytesTransferred = 0;
                if (!GetOverlappedResult(hDir, &overlapped, &bytesTransferred, FALSE)) {
                    DWORD error = GetLastError();
                    if (error == ERROR_OPERATION_ABORTED) {
                        std::wcout << L"Отслеживание прервано." << std::endl;
                        break;
                    }
                    std::wcerr << L"Ошибка получения результатов отслеживания.\n";
                    PrintWinErrorCode(L"GetOverlappedResult");
                    break;
                }
                if (bytesTransferred > 0) {
                    DWORD offset = 0;
                    bool hasEvents = false;
                    while (offset < bytesTransferred) {
                        FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer + offset);
                        std::wstring fileName(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
                        static std::wstring oldFileName;
                        if (fni->Action == FILE_ACTION_RENAMED_OLD_NAME) {
                            oldFileName = fileName;
                        }
                        else if (fni->Action == FILE_ACTION_RENAMED_NEW_NAME) {
                            PrintEventDetails(FILE_ACTION_RENAMED_OLD_NAME, oldFileName, 0, fileName);
                            oldFileName.clear();
                        }
                        else {
                            std::wstring fullPath = JoinPath(dir, fileName);
                            DWORD fileAttr = GetFileAttributesW(fullPath.c_str());
                            PrintEventDetails(fni->Action, fileName, fileAttr);
                        }
                        hasEvents = true;
                        if (fni->NextEntryOffset == 0) break;
                        offset += fni->NextEntryOffset;
                    }
                    if (hasEvents) {
                        std::wcout << std::endl;
                    }
                    ResetEvent(hEvent);
                    ZeroMemory(&overlapped, sizeof(overlapped));
                    overlapped.hEvent = hEvent;
                    ZeroMemory(buffer, sizeof(buffer));
                    ok = ReadDirectoryChangesW(
                        hDir,
                        buffer,
                        sizeof(buffer),
                        FALSE,
                        notifyFilter,
                        NULL,
                        &overlapped,
                        NULL
                    );
                    if (!ok) {
                        std::wcerr << L"Ошибка перезапуска отслеживания.\n";
                        PrintWinErrorCode(L"ReadDirectoryChangesW");
                        break;
                    }
                }
            }
            else if (waitResult == WAIT_TIMEOUT) {
                continue;
            }
            else {
                std::wcerr << L"Ошибка ожидания событий.\n";
                break;
            }
        }
        CancelIo(hDir);
        CloseHandle(hEvent);
        CloseHandle(hDir);
        std::wcout << L"\nОтслеживание завершено.\n";
        return 0;
}

//для Linux
//#include <iostream>
//#include <string>
//#include <vector>
//#include <dirent.h>
//#include <sys/stat.h>
//#include <sys/inotify.h>
//#include <unistd.h>
//#include <cstring>
//#include <cstdlib>
//#include <cstdio>
//
//using namespace std;
//
//static void PrintDirectoryContentsFirstLevel(const string& dir) {
//    DIR* dp = opendir(dir.c_str());
//    if (!dp) {
//        fprintf(stderr, "[!] Failed to open directory: %s\n", strerror(errno));
//        return;
//    }
//
//    fprintf(stdout, "=== Scanning directory: %s ===\n", dir.c_str());
//
//    struct dirent* entry;
//    size_t count = 0;
//
//    while ((entry = readdir(dp)) != nullptr) {
//        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
//            continue;
//        }
//
//        string fullPath = dir + "/" + entry->d_name;
//        struct stat statbuf;
//
//        if (stat(fullPath.c_str(), &statbuf) == 0) {
//            bool isDir = S_ISDIR(statbuf.st_mode);
//            fprintf(stdout, "  %s  %s\n", (isDir ? "<DIR> " : "<FILE>"), entry->d_name);
//            count++;
//        }
//    }
//    closedir(dp);
//    fprintf(stdout, "=== Total objects: %zu ===\n\n", count);
//}
//
//static string EventToString(uint32_t mask) {
//    string events = "";
//
//    if (mask & IN_CREATE)        return "CREATE";
//    if (mask & IN_DELETE)        return "DELETE";
//    if (mask & IN_DELETE_SELF)   return "DELETE SELF (WATCHED DIR)";
//    if (mask & IN_MODIFY)        return "MODIFY CONTENT";
//    if (mask & IN_ATTRIB)        return "MODIFY ATTRIB";
//    if (mask & IN_MOVED_FROM)    return "MOVED FROM";
//    if (mask & IN_MOVED_TO)      return "MOVED TO";
//    if (mask & IN_MOVE_SELF)     return "MOVE SELF";
//    if (mask & IN_CLOSE_WRITE)   return "CLOSE WRITE";
//    if (mask & IN_CLOSE_NOWRITE) return "CLOSE NOWRITE";
//    if (mask & IN_OPEN)          return "OPEN";
//    if (mask & IN_ACCESS)        return "ACCESS (READ)";
//    if (mask & IN_UNMOUNT)       return "UNMOUNT";
//    if (mask & IN_Q_OVERFLOW)    return "QUEUE OVERFLOW";
//    if (mask & IN_IGNORED)       return "IGNORED";
//
//    return "UNKNOWN EVENT";
//}
//
//int main(int argc, char* argv[]) {
//    setlocale(LC_ALL, "en_US.UTF-8");
//
//    if (argc < 2) {
//        fprintf(stderr, "Usage:\n  %s <directory_path>\n", argv[0]);
//        return 1;
//    }
//
//    string dir = argv[1];
//    struct stat statbuf;
//
//    if (stat(dir.c_str(), &statbuf) != 0) {
//        fprintf(stderr, "[Error] Directory not found or inaccessible: %s\n", strerror(errno));
//        return 2;
//    }
//
//    if (!S_ISDIR(statbuf.st_mode)) {
//        fprintf(stderr, "[Error] Specified path is not a directory.\n");
//        return 3;
//    }
//
//    PrintDirectoryContentsFirstLevel(dir);
//
//    int inotifyFd = inotify_init();
//    if (inotifyFd < 0) {
//        fprintf(stderr, "[Error] inotify initialization failed: %s\n", strerror(errno));
//        return 4;
//    }
//
//    uint32_t mask = IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO | IN_ATTRIB;
//
//    int watchFd = inotify_add_watch(inotifyFd, dir.c_str(), mask);
//    if (watchFd < 0) {
//        fprintf(stderr, "[Error] Failed to add watch: %s\n", strerror(errno));
//        close(inotifyFd);
//        return 5;
//    }
//
//    fprintf(stdout, ">>> MONITORING STARTED: %s\n", dir.c_str());
//    fprintf(stdout, ">>> (Press Ctrl+C to exit)\n\n");
//
//    const size_t EVENT_SIZE = sizeof(struct inotify_event);
//    const size_t BUF_LEN = 1024 * (EVENT_SIZE + 16);
//    char buffer[BUF_LEN];
//
//    while (true) {
//        ssize_t length = read(inotifyFd, buffer, BUF_LEN);
//        if (length < 0) {
//            fprintf(stderr, "[Error] Event reading interrupted: %s\n", strerror(errno));
//            break;
//        }
//
//        ssize_t i = 0;
//        while (i < length) {
//            struct inotify_event* event = (struct inotify_event*)&buffer[i];
//
//            if (event->len > 0) {
//                string filename = event->name;
//                string eventDesc = EventToString(event->mask);
//                string typeObj = (event->mask & IN_ISDIR) ? "[Dir] " : "[File]";
//
//                fprintf(stdout, "%-25s | %s | %s\n",
//                    eventDesc.c_str(),
//                    typeObj.c_str(),
//                    filename.c_str()
//                );
//            }
//            else if ((event->mask & IN_DELETE_SELF) || (event->mask & IN_MOVE_SELF) || (event->mask & IN_UNMOUNT)) {
//                string eventDesc = EventToString(event->mask);
//                fprintf(stdout, "!!! CRITICAL EVENT: %s\n", eventDesc.c_str());
//            }
//
//            i += EVENT_SIZE + event->len;
//        }
//        fflush(stdout);
//    }
//
//    inotify_rm_watch(inotifyFd, watchFd);
//    close(inotifyFd);
//    fprintf(stdout, "\n>>> Monitoring finished.\n");
//    return 0;
//}
