//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <cmath>

std::string FileTimeToString(const FILETIME& ft) {
    SYSTEMTIME stUTC, stLocal;
    FileTimeToSystemTime(&ft, &stUTC);
    SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

    char buffer[256];
    sprintf(buffer, "%02d.%02d.%04d %02d:%02d:%02d",
        stLocal.wDay, stLocal.wMonth, stLocal.wYear,
        stLocal.wHour, stLocal.wMinute, stLocal.wSecond);
    return std::string(buffer);
}

bool IsBinaryFile(HANDLE hFile, bool& outIsPE) {
    char buffer[1024];
    DWORD bytesRead = 0;
    LARGE_INTEGER ptr;
    ptr.QuadPart = 0;

    LARGE_INTEGER currentPos;
    LARGE_INTEGER move;
    move.QuadPart = 0;
    SetFilePointerEx(hFile, move, &currentPos, FILE_CURRENT);

    SetFilePointerEx(hFile, ptr, NULL, FILE_BEGIN);

    if (!ReadFile(hFile, buffer, sizeof(buffer), &bytesRead, NULL) || bytesRead == 0) {
        SetFilePointerEx(hFile, currentPos, NULL, FILE_BEGIN);
        return false;
    }

    SetFilePointerEx(hFile, currentPos, NULL, FILE_BEGIN);

    outIsPE = (bytesRead >= 2 && buffer[0] == 'M' && buffer[1] == 'Z');

    for (DWORD i = 0; i < bytesRead; ++i) {
        if (buffer[i] == '\0') return true;
    }

    return false; 
}

void PrintInfo(LPSTR FileName) {
    std::cout << "--- Информация о файле ---\n";
    std::cout << "Имя файла: " << FileName << "\n";

    HANDLE hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка: Не удалось открыть файл для чтения информации. Код: " << GetLastError() << "\n";
        return;
    }

    LARGE_INTEGER fileSize;
    if (GetFileSizeEx(hFile, &fileSize)) {
        double sizeBytes = (double)fileSize.QuadPart;
        std::cout << "Размер: " << (long long)sizeBytes << " Б";
        std::cout << " (" << std::fixed << std::setprecision(2) << (sizeBytes / 1024.0) << " KiB, ";
        std::cout << (sizeBytes / (1024.0 * 1024.0)) << " MiB)\n";
    }

    DWORD fileType = GetFileType(hFile);
    std::cout << "Системный тип: ";
    switch (fileType) {
    case FILE_TYPE_DISK: std::cout << "Disk file (Дисковый файл)\n"; break;
    case FILE_TYPE_CHAR: std::cout << "Character file (Устройство, консоль, принтер)\n"; break;
    case FILE_TYPE_PIPE: std::cout << "Pipe (Канал)\n"; break;
    case FILE_TYPE_UNKNOWN: std::cout << "Unknown (Неизвестно)\n"; break;
    default: std::cout << "Other\n"; break;
    }

    FILETIME ftCreate, ftAccess, ftWrite;
    if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
        std::cout << "Создан:           " << FileTimeToString(ftCreate) << "\n";
        std::cout << "Последний доступ: " << FileTimeToString(ftAccess) << "\n";
        std::cout << "Изменен:          " << FileTimeToString(ftWrite) << "\n";
    }

    bool isPE = false;
    if (IsBinaryFile(hFile, isPE)) {
        std::cout << "Формат: Бинарный";
        if (isPE) std::cout << " (Portable Executable - .exe/.dll)";
        std::cout << "\n";
    }
    else {
        std::cout << "Формат: Текстовый\n";
    }

    CloseHandle(hFile);
    std::cout << "--------------------------\n";
}

void PrintText(LPSTR FileName) {
    std::cout << "\n--- Содержимое файла ---\n";
    HANDLE hFile = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Ошибка: Не удалось открыть файл. Код: " << GetLastError() << "\n";
        return;
    }

    bool isPE = false;
    if (IsBinaryFile(hFile, isPE)) {
        std::cout << "Файл определен как БИНАРНЫЙ. Вывод содержимого невозможен.\n";
        CloseHandle(hFile);
        return;
    }

    const DWORD BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE + 1];
    DWORD bytesRead = 0;

    while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        std::cout.write(buffer, bytesRead);
    }
    std::cout << "\n------------------------\n";

    CloseHandle(hFile);
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 2) {
        std::cout << "Использование: Lab-09a.exe <путь_к_файлу>\n";
        std::cout << "Пример: Lab-09a.exe test.txt\n";
        return 1;
    }

    LPSTR fileName = argv[1];

    PrintInfo(fileName);
    PrintText(fileName);

    return 0;
}

//для Linux
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <sstream>
//#include <iomanip>
//#include <ctime>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <unistd.h>
//#include <pwd.h>
//#include <grp.h>
//#include <cstring>
//
//using namespace std;
//
//void PrintInfo(const string& FileName);
//void PrintText(const string& FileName);
//string FormatFileSize(long long size);
//string GetFileType(const string& FileName, mode_t mode);
//void PrintError(const string& message, int errnum = 0);
//string GetFileNameFromPath(const string& fullPath);
//
//int main(int argc, char* argv[]) {
//    setlocale(LC_ALL, "en_US.UTF-8");
//
//    if (argc < 2) {
//        cerr << "Usage: " << argv[0] << " <filename>" << endl;
//        return 1;
//    }
//
//    string filename = argv[1];
//    struct stat fileStat;
//
//    if (stat(filename.c_str(), &fileStat) != 0) {
//        PrintError("File not found or inaccessible", errno);
//        return 1;
//    }
//
//    if (S_ISDIR(fileStat.st_mode)) {
//        cerr << "Error: specified path is a directory, not a file." << endl;
//        return 1;
//    }
//
//    cout << "=== File Information ===" << endl;
//    PrintInfo(filename);
//
//    cout << "\n=== File Content ===" << endl;
//    PrintText(filename);
//
//    return 0;
//}
//
//string GetFileNameFromPath(const string& fullPath) {
//    size_t lastSlash = fullPath.find_last_of('/');
//    if (lastSlash != string::npos) {
//        return fullPath.substr(lastSlash + 1);
//    }
//    return fullPath;
//}
//
//string FormatFileSize(long long size) {
//    ostringstream oss;
//    oss << size << " B";
//    if (size >= 1024) {
//        oss << " (" << fixed << setprecision(2) << size / 1024.0 << " KiB)";
//    }
//    if (size >= 1024 * 1024) {
//        oss << " (" << fixed << setprecision(2) << size / (1024.0 * 1024.0) << " MiB)";
//    }
//    return oss.str();
//}
//
//string GetFileType(const string& FileName, mode_t mode) {
//    if (S_ISREG(mode)) {
//        ifstream file(FileName.c_str(), ios::binary);
//        if (file) {
//            char buffer[4096];
//            file.read(buffer, sizeof(buffer));
//            streamsize bytesRead = file.gcount();
//
//            bool isLikelyText = true;
//            for (int i = 0; i < bytesRead; i++) {
//                unsigned char c = buffer[i];
//                if (c == 0 || (c < 32 && c != '\n' && c != '\r' && c != '\t' && c != '\b' && c != '\f')) {
//                    isLikelyText = false;
//                    break;
//                }
//            }
//            file.close();
//
//            if (isLikelyText) {
//                string fileNameOnly = GetFileNameFromPath(FileName);
//                size_t dotPos = fileNameOnly.find_last_of('.');
//                if (dotPos != string::npos) {
//                    string ext = fileNameOnly.substr(dotPos);
//                    for (char& c : ext) c = tolower(c);
//                    if (ext == ".txt" || ext == ".log" || ext == ".ini" || ext == ".cfg" ||
//                        ext == ".conf" || ext == ".sh" || ext == ".bash" || ext == ".c" ||
//                        ext == ".cpp" || ext == ".h" || ext == ".py" || ext == ".java" ||
//                        ext == ".js" || ext == ".html" || ext == ".css" || ext == ".xml" ||
//                        ext == ".json" || ext == ".csv") {
//                        return "Text file";
//                    }
//                }
//                return "Text file";
//            }
//            else {
//                return "Binary file";
//            }
//        }
//        return "Regular file";
//    }
//    else if (S_ISDIR(mode)) return "Directory";
//    else if (S_ISCHR(mode)) return "Character device";
//    else if (S_ISBLK(mode)) return "Block device";
//    else if (S_ISFIFO(mode)) return "FIFO/named pipe";
//    else if (S_ISLNK(mode)) return "Symbolic link";
//    else if (S_ISSOCK(mode)) return "Socket";
//
//    return "Unknown type";
//}
//
//void PrintInfo(const string& FileName) {
//    struct stat fileStat;
//    if (stat(FileName.c_str(), &fileStat) != 0) {
//        PrintError("Error getting file info", errno);
//        return;
//    }
//
//    string fileNameOnly = GetFileNameFromPath(FileName);
//
//    cout << "1. File name: " << fileNameOnly << endl;
//    cout << "2. File size: " << FormatFileSize(fileStat.st_size) << endl;
//    cout << "3. File type: " << GetFileType(FileName, fileStat.st_mode) << endl;
//
//    struct tm* timeinfo;
//    char timeBuffer[64];
//
//    timeinfo = localtime(&fileStat.st_ctime);
//    strftime(timeBuffer, sizeof(timeBuffer), "%d.%m.%Y %H:%M:%S", timeinfo);
//    cout << "4. Creation/Change time: " << timeBuffer << endl;
//
//    timeinfo = localtime(&fileStat.st_atime);
//    strftime(timeBuffer, sizeof(timeBuffer), "%d.%m.%Y %H:%M:%S", timeinfo);
//    cout << "5. Last access time: " << timeBuffer << endl;
//
//    timeinfo = localtime(&fileStat.st_mtime);
//    strftime(timeBuffer, sizeof(timeBuffer), "%d.%m.%Y %H:%M:%S", timeinfo);
//    cout << "6. Last modification time: " << timeBuffer << endl;
//}
//
//void PrintText(const string& FileName) {
//    struct stat fileStat;
//    if (stat(FileName.c_str(), &fileStat) != 0) {
//        PrintError("Error getting file info", errno);
//        return;
//    }
//
//    string fileType = GetFileType(FileName, fileStat.st_mode);
//    if (fileType != "Text file") {
//        cout << "File is not text. Content output is impossible." << endl;
//        return;
//    }
//
//    ifstream file(FileName.c_str());
//    if (!file) {
//        PrintError("Error opening file for reading", errno);
//        return;
//    }
//
//    string line;
//    int lineNumber = 1;
//    while (getline(file, line)) {
//        cout << setw(4) << lineNumber++ << ": " << line << endl;
//    }
//
//    file.close();
//
//    if (lineNumber == 1) {
//        cout << "(File is empty)" << endl;
//    }
//}
//
//void PrintError(const string& message, int errnum) {
//    cerr << "ERROR: " << message << endl;
//    if (errnum != 0) {
//        cerr << "Error code: " << errnum << endl;
//        cerr << "Description: " << strerror(errnum) << endl;
//    }
//}