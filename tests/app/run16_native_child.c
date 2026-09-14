#include <windows.h>
#include <wchar.h>
int wmain(void)
{
    WCHAR value[64], expected[MAX_PATH], actual[MAX_PATH];
    CHAR input[32];
    DWORD count,written;
    LPCWSTR command=GetCommandLineW();
    if (!GetEnvironmentVariableW(L"RUN16_TEST_VALUE",value,64) ||
        wcscmp(value,L"value spaces 123")) return 41;
    if (!GetEnvironmentVariableW(L"RUN16_TEST_CWD",expected,MAX_PATH) ||
        !GetCurrentDirectoryW(MAX_PATH,actual) || wcscmp(expected,actual)) return 42;
    if (!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE),command,(DWORD)(wcslen(command)*sizeof(WCHAR)),&written,NULL)) return 43;
    if (!ReadFile(GetStdHandle(STD_INPUT_HANDLE),input,sizeof(input),&count,NULL) ||
        !WriteFile(GetStdHandle(STD_ERROR_HANDLE),input,count,&written,NULL) || written!=count) return 44;
    return 23;
}
