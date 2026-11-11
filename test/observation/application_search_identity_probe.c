/* Windowless native target: record the image Windows actually executed.
 * Test-only; output and compilation remain below repository build/. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE previous, PWSTR tail, int show)
{
    WCHAR destination[32768], image[32768];
    DWORD length, written, bytes;
    HANDLE file;
    (void)instance; (void)previous; (void)tail; (void)show;
    length=GetEnvironmentVariableW(L"SEARCH_IDENTITY_REPORT",destination,32768);
    if(!length || length>=32768) return 90;
    length=GetModuleFileNameW(NULL,image,32768);
    if(!length || length>=32768) return 91;
    file=CreateFileW(destination,GENERIC_WRITE,0,NULL,CREATE_NEW,0,NULL);
    if(file==INVALID_HANDLE_VALUE) return 92;
    bytes=length*sizeof(WCHAR);
    if(!WriteFile(file,image,bytes,&written,NULL) || written!=bytes) {
        CloseHandle(file); return 93;
    }
    CloseHandle(file);
    return 37;
}
