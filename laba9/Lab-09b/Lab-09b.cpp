//для Windows
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <locale>
#include <codecvt>
static HANDLE g_hFile = INVALID_HANDLE_VALUE;
static char* g_filePath = nullptr;
static char* g_buf = nullptr;
static DWORD  g_bufSize = 0;
static DWORD  g_bufCap = 0;
static size_t* g_lineStart = nullptr;
static size_t* g_lineLen = nullptr;
static size_t  g_lineCount = 0;
static char* g_workBuf = nullptr;
static DWORD g_workCap = 0;
enum APP_ERR : DWORD {
    APP_OK = 0,
    APP_ERR_INVALID_HANDLE = 1,
    APP_ERR_NULL_ROW = 2,
    APP_ERR_BAD_POS = 3,
    APP_ERR_EMPTY_FILE = 4,
    APP_ERR_TOO_LARGE = 5,
    APP_ERR_NO_SCENARIO = 6,
    APP_ERR_ALLOC = 7
};
static DWORD g_appErr = APP_OK;
static DWORD g_winErr = 0;
static void ResetErrors()
{
    g_appErr = APP_OK;
    g_winErr = 0;
}
static void SetAppErr(DWORD e)
{
    g_appErr = e;
    g_winErr = 0;
}
static void CaptureWinErr()
{
    g_winErr = GetLastError();
    g_appErr = APP_OK;
}
static void PrintLastError(const char* where)
{
    if (g_winErr != 0) {
        std::cerr << where << " ошибка. Код ошибки Win32: " << g_winErr << "\n";
        return;
    }
    if (g_appErr != APP_OK) {
        std::cerr << where << " ошибка. Код ошибки приложения: " << g_appErr << "\n";
        return;
    }
    std::cerr << where << " ошибка.\n";
}
static void FreeGlobals()
{
    if (g_filePath) { delete[] g_filePath; g_filePath = nullptr; }
    if (g_buf) { delete[] g_buf; g_buf = nullptr; }
    g_bufSize = 0;
    g_bufCap = 0;
    if (g_lineStart) { delete[] g_lineStart; g_lineStart = nullptr; }
    if (g_lineLen) { delete[] g_lineLen; g_lineLen = nullptr; }
    g_lineCount = 0;
    if (g_workBuf) { delete[] g_workBuf; g_workBuf = nullptr; }
    g_workCap = 0;
    ResetErrors();
}
static bool EnsureBufCap(DWORD needBytes)
{
    if (needBytes <= g_bufCap) return true;
    DWORD newCap = g_bufCap ? g_bufCap : 1024;
    while (newCap < needBytes) {
        if (newCap > 0x7fffffff / 2) { newCap = needBytes; break; }
        newCap *= 2;
    }
    char* nb = new (std::nothrow) char[newCap];
    if (!nb) { SetAppErr(APP_ERR_ALLOC); return false; }
    if (g_buf && g_bufSize > 0) {
        std::memcpy(nb, g_buf, g_bufSize);
    }
    delete[] g_buf;
    g_buf = nb;
    g_bufCap = newCap;
    return true;
}
static bool EnsureWorkCap(DWORD needBytes)
{
    if (needBytes <= g_workCap) return true;
    DWORD newCap = g_workCap ? g_workCap : 1024;
    while (newCap < needBytes) {
        if (newCap > 0x7fffffff / 2) { newCap = needBytes; break; }
        newCap *= 2;
    }
    char* nb = new (std::nothrow) char[newCap];
    if (!nb) { SetAppErr(APP_ERR_ALLOC); return false; }
    delete[] g_workBuf;
    g_workBuf = nb;
    g_workCap = newCap;
    return true;
}
static bool SeekFileBegin(HANDLE hFile)
{
    LARGE_INTEGER zero{};
    if (!SetFilePointerEx(hFile, zero, nullptr, FILE_BEGIN)) {
        CaptureWinErr();
        return false;
    }
    return true;
}
static bool LoadFileToBuffer(HANDLE hFile)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return false;
    }
    LARGE_INTEGER li{};
    if (!GetFileSizeEx(hFile, &li)) {
        CaptureWinErr();
        return false;
    }
    if (li.QuadPart < 0 || li.QuadPart > 0x7fffffff) {
        SetAppErr(APP_ERR_TOO_LARGE);
        return false;
    }
    DWORD size = (DWORD)li.QuadPart;
    if (!EnsureBufCap(size + 1)) {
        return false;
    }
    if (!SeekFileBegin(hFile)) {
        return false;
    }
    DWORD total = 0;
    while (total < size) {
        DWORD chunk = 0;
        DWORD toRead = size - total;
        if (!ReadFile(hFile, g_buf + total, toRead, &chunk, nullptr)) {
            CaptureWinErr();
            return false;
        }
        if (chunk == 0) break;
        total += chunk;
    }
    g_bufSize = total;
    g_buf[g_bufSize] = '\0';
    return true;
}
static bool RebuildLineIndex()
{
    ResetErrors();
    if (g_lineStart) { delete[] g_lineStart; g_lineStart = nullptr; }
    if (g_lineLen) { delete[] g_lineLen;   g_lineLen = nullptr; }
    g_lineCount = 0;
    if (!g_buf || g_bufSize == 0) {
        g_lineCount = 0;
        return true;
    }
    size_t count = 0;
    size_t i = 0;
    while (i < g_bufSize) {
        size_t start = i;
        while (i < g_bufSize && g_buf[i] != '\r' && g_buf[i] != '\n') {
            i++;
        }
        (void)start;
        count++;
        if (i < g_bufSize) {
            if (g_buf[i] == '\r' && (i + 1) < g_bufSize && g_buf[i + 1] == '\n') i += 2;
            else i += 1;
        }
    }
    g_lineStart = new (std::nothrow) size_t[count];
    g_lineLen = new (std::nothrow) size_t[count];
    if (!g_lineStart || !g_lineLen) {
        SetAppErr(APP_ERR_ALLOC);
        if (g_lineStart) { delete[] g_lineStart; g_lineStart = nullptr; }
        if (g_lineLen) { delete[] g_lineLen;   g_lineLen = nullptr; }
        g_lineCount = 0;
        return false;
    }
    i = 0;
    size_t idx = 0;
    while (i < g_bufSize && idx < count) {
        size_t start = i;
        while (i < g_bufSize && g_buf[i] != '\r' && g_buf[i] != '\n') {
            i++;
        }
        size_t len = i - start;
        g_lineStart[idx] = start;
        g_lineLen[idx] = len;
        idx++;
        if (i < g_bufSize) {
            if (g_buf[i] == '\r' && (i + 1) < g_bufSize && g_buf[i + 1] == '\n') i += 2;
            else i += 1;
        }
    }
    g_lineCount = idx;
    return true;
}
static size_t NormalizePosForInsert(int pos)
{
    if (pos > 0) {
        size_t p = (size_t)pos;
        if (p > g_lineCount + 1) return (size_t)-1;
        return p - 1;
    }
    if (pos == 0) return 0;
    if (pos == -1) return g_lineCount;
    return (size_t)-1;
}
static size_t NormalizePosForExistingLine(int pos)
{
    if (g_lineCount == 0) return (size_t)-1;
    if (pos > 0) {
        size_t p = (size_t)pos;
        if (p == 0 || p > g_lineCount) return (size_t)-1;
        return p - 1;
    }
    if (pos == 0) return 0;
    if (pos == -1) return g_lineCount - 1;
    return (size_t)-1;
}
static void TrimRowEndCRLF(char* s)
{
    if (!s) return;
    size_t n = std::strlen(s);
    while (n > 0 && (s[n - 1] == '\r' || s[n - 1] == '\n')) {
        s[n - 1] = '\0';
        n--;
    }
}
static bool RewriteFileFromWorkBuf(HANDLE hFile, DWORD bytesToWrite)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return false;
    }
    if (!SeekFileBegin(hFile)) return false;
    DWORD total = 0;
    while (total < bytesToWrite) {
        DWORD written = 0;
        DWORD toWrite = bytesToWrite - total;
        if (!WriteFile(hFile, g_workBuf + total, toWrite, &written, nullptr)) {
            CaptureWinErr();
            return false;
        }
        if (written == 0) {
            SetAppErr(APP_ERR_NO_SCENARIO);
            return false;
        }
        total += written;
    }
    if (!SetEndOfFile(hFile)) {
        CaptureWinErr();
        return false;
    }
    if (!FlushFileBuffers(hFile)) {
        CaptureWinErr();
        return false;
    }
    return true;
}
static std::string ConvertToConsoleEncoding(const char* data, size_t len)
{
    if (len == 0) return "";

    std::string result;
    result.reserve(len);

    for (size_t i = 0; i < len; ++i) {
        unsigned char c = static_cast<unsigned char>(data[i]);

        if (c >= 128) {
            if (c >= 0xD0 && c <= 0xDF && i + 1 < len) {
                i++;
                result += '?';
            }
            else if (c >= 0x80 && c <= 0xFF) {
                result += '?';
            }
            else {
                result += static_cast<char>(c);
            }
        }
        else {
            result += static_cast<char>(c);
        }
    }
    return result;
}
BOOL OpenFile(LPSTR filePath)
{
    ResetErrors();
    if (!filePath) {
        SetAppErr(APP_ERR_NO_SCENARIO);
        return FALSE;
    }
    if (g_hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(g_hFile);
        g_hFile = INVALID_HANDLE_VALUE;
    }
    FreeGlobals();
    size_t len = std::strlen(filePath);
    g_filePath = new (std::nothrow) char[len + 1];
    if (!g_filePath) {
        SetAppErr(APP_ERR_ALLOC);
        return FALSE;
    }
    std::memcpy(g_filePath, filePath, len + 1);
    g_hFile = CreateFileA(
        g_filePath,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );
    if (g_hFile == INVALID_HANDLE_VALUE) {
        CaptureWinErr();
        FreeGlobals();
        return FALSE;
    }
    if (!LoadFileToBuffer(g_hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    return TRUE;
}
BOOL AddRow(HANDLE hFile, LPSTR row, INT pos)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return FALSE;
    }
    if (row == nullptr) {
        SetAppErr(APP_ERR_NULL_ROW);
        return FALSE;
    }
    if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    TrimRowEndCRLF(row);
    size_t ins = NormalizePosForInsert((int)pos);
    if (ins == (size_t)-1) {
        SetAppErr(APP_ERR_BAD_POS);
        return FALSE;
    }
    const size_t newRowLen = std::strlen(row);
    const size_t newLineCount = g_lineCount + 1;
    size_t outSize64 = 0;
    for (size_t i = 0; i < g_lineCount; ++i) {
        outSize64 += g_lineLen[i] + 2;
    }
    outSize64 += newRowLen + 2;
    if (outSize64 > 0x7fffffff) {
        SetAppErr(APP_ERR_TOO_LARGE);
        return FALSE;
    }
    DWORD outSize = (DWORD)outSize64;
    if (!EnsureWorkCap(outSize)) return FALSE;
    size_t w = 0;
    for (size_t i = 0; i < newLineCount; ++i) {
        bool isInserted = (i == ins);
        if (isInserted) {
            if (newRowLen > 0) {
                std::memcpy(g_workBuf + w, row, newRowLen);
                w += newRowLen;
            }
        }
        else {
            size_t src = (i < ins) ? i : (i - 1);
            size_t lenL = g_lineLen[src];
            if (lenL > 0) {
                std::memcpy(g_workBuf + w, g_buf + g_lineStart[src], lenL);
                w += lenL;
            }
        }
        g_workBuf[w++] = '\r';
        g_workBuf[w++] = '\n';
    }
    if (w != outSize) {
        SetAppErr(APP_ERR_NO_SCENARIO);
        return FALSE;
    }
    if (!RewriteFileFromWorkBuf(hFile, outSize)) return FALSE;
    if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    return TRUE;
}
BOOL RemRow(HANDLE hFile, INT pos)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return FALSE;
    }
    if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    if (g_lineCount == 0) {
        SetAppErr(APP_ERR_EMPTY_FILE);
        return FALSE;
    }
    size_t rem = NormalizePosForExistingLine((int)pos);
    if (rem == (size_t)-1) {
        SetAppErr(APP_ERR_BAD_POS);
        return FALSE;
    }
    const size_t newLineCount = g_lineCount - 1;
    size_t outSize64 = 0;
    for (size_t i = 0; i < g_lineCount; ++i) {
        if (i == rem) continue;
        outSize64 += g_lineLen[i] + 2;
    }
    if (outSize64 > 0x7fffffff) {
        SetAppErr(APP_ERR_TOO_LARGE);
        return FALSE;
    }
    DWORD outSize = (DWORD)outSize64;
    if (outSize > 0 && !EnsureWorkCap(outSize)) return FALSE;
    size_t w = 0;
    for (size_t i = 0, kept = 0; i < g_lineCount; ++i) {
        if (i == rem) continue;
        size_t lenL = g_lineLen[i];
        if (lenL > 0) {
            std::memcpy(g_workBuf + w, g_buf + g_lineStart[i], lenL);
            w += lenL;
        }
        g_workBuf[w++] = '\r';
        g_workBuf[w++] = '\n';
        kept++;
        (void)kept;
    }
    if (w != outSize) {
        SetAppErr(APP_ERR_NO_SCENARIO);
        return FALSE;
    }
    if (!RewriteFileFromWorkBuf(hFile, outSize)) return FALSE;
    if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    return TRUE;
}
BOOL PrintRow(HANDLE hFile, INT pos)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return FALSE;
    }
    if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    if (g_lineCount == 0) {
        SetAppErr(APP_ERR_EMPTY_FILE);
        return FALSE;
    }
    size_t idx = NormalizePosForExistingLine((int)pos);
    if (idx == (size_t)-1) {
        SetAppErr(APP_ERR_BAD_POS);
        return FALSE;
    }
    std::cout << (idx + 1) << ") ";
    std::string line(g_buf + g_lineStart[idx], g_lineLen[idx]);
    std::cout << line << "\n";
    return TRUE;
}
BOOL PrintRows(HANDLE hFile)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return FALSE;
    }if (!LoadFileToBuffer(hFile)) return FALSE;
    if (!RebuildLineIndex()) return FALSE;
    if (g_lineCount == 0) {
        std::cout << "(Файл пуст)\n";
        return TRUE;
    }
    for (size_t i = 0; i < g_lineCount; ++i) {
        std::cout << (i + 1) << ") ";
        std::string line(g_buf + g_lineStart[i], g_lineLen[i]);
        std::cout << line << "\n";
    }
    return TRUE;
}
BOOL CloseFile(HANDLE hFile)
{
    ResetErrors();
    if (hFile == INVALID_HANDLE_VALUE || hFile != g_hFile) {
        SetAppErr(APP_ERR_INVALID_HANDLE);
        return FALSE;
    }
    if (!CloseHandle(g_hFile)) {
        CaptureWinErr();
        return FALSE;
    }
    g_hFile = INVALID_HANDLE_VALUE;
    FreeGlobals();
    return TRUE;
}
static bool ReadIntSafe(const char* prompt, int& out)
{
    while (true) {
        std::cout << prompt;
        std::string s;
        if (!std::getline(std::cin, s)) return false;
        size_t b = 0;
        while (b < s.size() && std::isspace((unsigned char)s[b])) b++;
        size_t e = s.size();
        while (e > b && std::isspace((unsigned char)s[e - 1])) e--;
        std::string t = s.substr(b, e - b);
        if (t.empty()) {
            std::cout << "Ошибка ввода. Введите целое число.\n";
            continue;
        }
        char* endp = nullptr;
        long v = std::strtol(t.c_str(), &endp, 10);
        if (endp == t.c_str() || *endp != '\0') {
            std::cout << "Ошибка ввода. Введите целое число.\n";
            continue;
        }
        out = (int)v;
        return true;
    }
}
static std::string ReadLineSafe(const char* prompt)
{
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}
static std::string UnquotePath(std::string p)
{
    if (p.size() >= 2 && p.front() == '"' && p.back() == '"') {
        return p.substr(1, p.size() - 2);
    }
    return p;
}
int main()
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    while (true) {
        std::cout << "\nВыберите выполняемую операцию:\n"
            << "1. Открыть файл.\n"
            << "2. Вставить строку.\n"
            << "3. Удалить строку.\n"
            << "4. Вывести строку.\n"
            << "5. Вывести файл.\n" << "6. Закрыть файл.\n" << "0. Выход.\n";
        int cmd = 0;
        if (!ReadIntSafe("Выбор: ", cmd)) break;

        if (cmd == 0) {
            if (g_hFile != INVALID_HANDLE_VALUE) {
                CloseFile(g_hFile);
            }
            break;
        }
        switch (cmd) {
        case 1: {
            std::string path = UnquotePath(ReadLineSafe("Введите путь к файлу: "));
            if (path.empty()) {
                std::cout << "Путь пуст.\n";
                break;
            }
            std::string tmp = path;
            if (!OpenFile((LPSTR)tmp.c_str())) {
                PrintLastError("OpenFile");
            }
            else {
                std::cout << "Файл открыт.\n";
            }
            break;
        }
        case 2: {
            if (g_hFile == INVALID_HANDLE_VALUE) {
                std::cout << "Сначала откройте файл (OpenFile).\n";
                break;
            }
            int pos = 0;
            ReadIntSafe("Введите pos (>0/0/-1): ", pos);
            std::string row = ReadLineSafe("Введите строку: ");
            std::string tmp = row;
            if (!AddRow(g_hFile, (LPSTR)tmp.c_str(), (INT)pos)) {
                PrintLastError("AddRow");
            }
            else {
                std::cout << "Строка добавлена.\n";
            }
            break;
        }
        case 3: {
            if (g_hFile == INVALID_HANDLE_VALUE) {
                std::cout << "Сначала откройте файл (OpenFile).\n";
                break;
            }
            int pos = 0;
            ReadIntSafe("Введите pos (>0/0/-1): ", pos);
            if (!RemRow(g_hFile, (INT)pos)) {
                PrintLastError("RemRow");
            }
            else {
                std::cout << "Строка удалена.\n";
            }
            break;
        }
        case 4: {
            if (g_hFile == INVALID_HANDLE_VALUE) {
                std::cout << "Сначала откройте файл (OpenFile).\n";
                break;
            }
            int pos = 0;
            ReadIntSafe("Введите pos (>0/0/-1): ", pos);
            if (!PrintRow(g_hFile, (INT)pos)) {
                PrintLastError("PrintRow");
            }
            break;
        }
        case 5: {
            if (g_hFile == INVALID_HANDLE_VALUE) {
                std::cout << "Сначала откройте файл (OpenFile).\n";
                break;
            }
            if (!PrintRows(g_hFile)) {
                PrintLastError("PrintRows");
            }
            break;
        }
        case 6: {
            if (g_hFile == INVALID_HANDLE_VALUE) {
                std::cout << "Файл не открыт.\n";
                break;
            }
            if (!CloseFile(g_hFile)) {
                PrintLastError("CloseFile");
            }
            else {
                std::cout << "Файл закрыт.\n";
            }
            break;
        }
        default:
            std::cout << "Неизвестная команда.\n";
            break;
        }
    }
    return 0;
}

//для Linux
//#include <iostream>
//#include <string>
//#include <fstream>
//#include <vector>
//#include <algorithm>
//#include <cstdlib>
//#include <cstring>
//#include <unistd.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/types.h>
//#include <cctype>
//#include <cstdio>
//
//using namespace std;
//
//static int g_fileDesc = -1;
//static string g_filePath;
//static vector<string> g_lines;
//
//static bool LoadFileToLines() {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    lseek(g_fileDesc, 0, SEEK_SET);
//    struct stat fileStat;
//    if (fstat(g_fileDesc, &fileStat) != 0) {
//        return false;
//    }
//    if (fileStat.st_size > 100000000) {
//        return false;
//    }
//    vector<char> buffer(fileStat.st_size + 1);
//    ssize_t bytesRead = read(g_fileDesc, buffer.data(), fileStat.st_size);
//    if (bytesRead != fileStat.st_size) {
//        return false;
//    }
//    buffer[bytesRead] = '\0';
//    g_lines.clear();
//    string content(buffer.data(), bytesRead);
//    size_t start = 0;
//    while (start < content.length()) {
//        size_t end = content.find_first_of("\n", start);
//        if (end == string::npos) {
//            string line = content.substr(start);
//            if (!line.empty() && line.back() == '\r') {
//                line.pop_back();
//            }
//            g_lines.push_back(line);
//            break;
//        }
//        string line = content.substr(start, end - start);
//        if (!line.empty() && line.back() == '\r') {
//            line.pop_back();
//        }
//        g_lines.push_back(line);
//        start = end + 1;
//    }
//    return true;
//}
//
//static bool WriteLinesToFile() {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    string content;
//    for (size_t i = 0; i < g_lines.size(); ++i) {
//        content += g_lines[i];
//        if (i != g_lines.size() - 1) {
//            content += "\n";
//        }
//    }
//    lseek(g_fileDesc, 0, SEEK_SET);
//    ssize_t bytesWritten = write(g_fileDesc, content.c_str(), content.length());
//    if (bytesWritten != (ssize_t)content.length()) {
//        return false;
//    }
//    if (ftruncate(g_fileDesc, content.length()) != 0) {
//        return false;
//    }
//    fsync(g_fileDesc);
//    return true;
//}
//
//static size_t NormalizePosForInsert(int pos) {
//    if (pos > 0) {
//        size_t p = (size_t)pos;
//        if (p > g_lines.size() + 1) return (size_t)-1;
//        return p - 1;
//    }
//    if (pos == 0) return 0;
//    if (pos == -1) return g_lines.size();
//    return (size_t)-1;
//}
//
//static size_t NormalizePosForExistingLine(int pos) {
//    if (g_lines.empty()) return (size_t)-1;
//    if (pos > 0) {
//        size_t p = (size_t)pos;
//        if (p == 0 || p > g_lines.size()) return (size_t)-1;
//        return p - 1;
//    }
//    if (pos == 0) return 0;
//    if (pos == -1) return g_lines.size() - 1;
//    return (size_t)-1;
//}
//
//bool OpenFile(const string& filePath) {
//    if (g_fileDesc != -1) {
//        close(g_fileDesc);
//        g_fileDesc = -1;
//    }
//    g_lines.clear();
//    g_filePath.clear();
//
//    g_fileDesc = open(filePath.c_str(), O_RDWR);
//    if (g_fileDesc == -1) {
//        return false;
//    }
//
//    g_filePath = filePath;
//    return LoadFileToLines();
//}
//
//bool AddRow(const string& row, int pos) {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    if (!LoadFileToLines()) {
//        return false;
//    }
//    size_t ins = NormalizePosForInsert(pos);
//    if (ins == (size_t)-1) {
//        return false;
//    }
//    string newRow = row;
//    while (!newRow.empty() && (newRow.back() == '\n' || newRow.back() == '\r')) {
//        newRow.pop_back();
//    }
//    if (ins <= g_lines.size()) {
//        g_lines.insert(g_lines.begin() + ins, newRow);
//    }
//    else {
//        g_lines.push_back(newRow);
//    }
//
//    return WriteLinesToFile();
//}
//
//bool RemRow(int pos) {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    if (!LoadFileToLines()) {
//        return false;
//    }
//    if (g_lines.empty()) {
//        return false;
//    }
//    size_t rem = NormalizePosForExistingLine(pos);
//    if (rem == (size_t)-1) {
//        return false;
//    }
//    g_lines.erase(g_lines.begin() + rem);
//    return WriteLinesToFile();
//}
//
//bool PrintRow(int pos) {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    if (!LoadFileToLines()) {
//        return false;
//    }
//    if (g_lines.empty()) {
//        return false;
//    }
//    size_t idx = NormalizePosForExistingLine(pos);
//    if (idx == (size_t)-1) {
//        return false;
//    }
//    fprintf(stdout, "%zu) %s\n", idx + 1, g_lines[idx].c_str());
//    return true;
//}
//
//bool PrintRows() {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    if (!LoadFileToLines()) {
//        return false;
//    }
//    if (g_lines.empty()) {
//        fprintf(stdout, "(File is empty)\n");
//        return true;
//    }
//    for (size_t i = 0; i < g_lines.size(); ++i) {
//        fprintf(stdout, "%zu) %s\n", i + 1, g_lines[i].c_str());
//    }
//    return true;
//}
//
//bool CloseFile() {
//    if (g_fileDesc == -1) {
//        return false;
//    }
//    if (close(g_fileDesc) != 0) {
//        return false;
//    }
//    g_fileDesc = -1;
//    g_filePath.clear();
//    g_lines.clear();
//    return true;
//}
//
//static bool ReadIntSafe(const string& prompt, int& out) {
//    while (true) {
//        fprintf(stdout, "%s", prompt.c_str());
//        string s;
//        if (!getline(cin, s)) return false;
//
//        char* endp = nullptr;
//        long v = strtol(s.c_str(), &endp, 10);
//        if (endp == s.c_str() || *endp != '\0') {
//            fprintf(stdout, "Input error. Please enter an integer.\n");
//            continue;
//        }
//        out = (int)v;
//        return true;
//    }
//}
//
//int main() {
//    setlocale(LC_ALL, "en_US.UTF-8");
//
//    while (true) {
//        fprintf(stdout, "\nSelect an operation:\n");
//        fprintf(stdout, "1. Open file.\n");
//        fprintf(stdout, "2. Insert row.\n");
//        fprintf(stdout, "3. Remove row.\n");
//        fprintf(stdout, "4. Print row.\n");
//        fprintf(stdout, "5. Print file content.\n");
//        fprintf(stdout, "6. Close file.\n");
//        fprintf(stdout, "0. Exit.\n");
//        int cmd = 0;
//        if (!ReadIntSafe("Choice: ", cmd)) break;
//
//        if (cmd == 0) {
//            if (g_fileDesc != -1) {
//                CloseFile();
//            }
//            break;
//        }
//        switch (cmd) {
//        case 1: {
//            fprintf(stdout, "Enter file path: ");
//            string path;
//            getline(cin, path);
//            if (path.empty()) {
//                fprintf(stdout, "Path is empty.\n");
//                break;
//            }
//            if (!OpenFile(path)) {
//                fprintf(stdout, "Error opening file.\n");
//            }
//            else {
//                fprintf(stdout, "File opened.\n");
//            }
//            break;
//        }
//        case 2: {
//            if (g_fileDesc == -1) {
//                fprintf(stdout, "First open a file.\n");
//                break;
//            }
//            int pos = 0;
//            ReadIntSafe("Enter pos (>0/0/-1): ", pos);
//            fprintf(stdout, "Enter string: ");
//            string row;
//            getline(cin, row);
//            if (!AddRow(row, pos)) {
//                fprintf(stdout, "Error adding row.\n");
//            }
//            else {
//                fprintf(stdout, "Row added.\n");
//            }
//            break;
//        }
//        case 3: {
//            if (g_fileDesc == -1) {
//                fprintf(stdout, "First open a file.\n");
//                break;
//            }
//            int pos = 0;
//            ReadIntSafe("Enter pos (>0/0/-1): ", pos);
//
//            if (!RemRow(pos)) {
//                fprintf(stdout, "Error removing row.\n");
//            }
//            else {
//                fprintf(stdout, "Row removed.\n");
//            }
//            break;
//        }
//        case 4: {
//            if (g_fileDesc == -1) {
//                fprintf(stdout, "First open a file.\n");
//                break;
//            }
//            int pos = 0;
//            ReadIntSafe("Enter pos (>0/0/-1): ", pos);
//
//            if (!PrintRow(pos)) {
//                fprintf(stdout, "Error printing row.\n");
//            }
//            break;
//        }
//        case 5: {
//            if (g_fileDesc == -1) {
//                fprintf(stdout, "First open a file.\n");
//                break;
//            }
//
//            if (!PrintRows()) {
//                fprintf(stdout, "Error printing file.\n");
//            }
//            break;
//        }
//        case 6: {
//            if (g_fileDesc == -1) {
//                fprintf(stdout, "File is not open.\n");
//                break;
//            }
//            if (!CloseFile()) {
//                fprintf(stdout, "Error closing file.\n");
//            }
//            else {
//                fprintf(stdout, "File closed.\n");
//            }
//            break;
//        }
//        default:
//            fprintf(stdout, "Unknown command.\n");
//            break;
//        }
//    }
//    return 0;
//}