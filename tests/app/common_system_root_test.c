#include "common/system_root.h"
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#define CHECK(x) do { if(!(x)) { \
    fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);return 1; } } while(0)

int main(void)
{
    WCHAR image[32768],root[32768],path[32768],tiny_wide[2];
    CHAR ansi[32768],tiny[2];
    PWSTR slash;
    DWORD count;
    size_t length;
    static PCWSTR invalid[]={NULL,L"",L"..",L"a\\..\\b",L".\\a",
        L"\\a",L"C:\\a",L"a\\",L"a//b"};
    unsigned int index;
    count=GetModuleFileNameW(NULL,image,ARRAYSIZE(image));
    CHECK(count && count<ARRAYSIZE(image));
    slash=wcsrchr(image,L'\\');CHECK(slash);
    if(slash==image+2 && image[1]==L':')slash[1]=0;else *slash=0;
    CHECK(SetEnvironmentVariableW(L"NtvdmSystemRoot",L"C:\\not-the-product"));
    CHECK(SetCurrentDirectoryW(L"C:\\"));
    CHECK(common_system_root_w(root,ARRAYSIZE(root))==ERROR_SUCCESS);
    CHECK(!wcscmp(root,image));
    CHECK(common_product_path_w(L"system32\\krnl386",path,ARRAYSIZE(path))==0);
    length=wcslen(root);
    CHECK(!wcsncmp(path,root,length));
    CHECK(!wcscmp(path+length+(root[length-1]!=L'\\'),L"system32\\krnl386"));
    CHECK(common_system_root_w(tiny_wide,ARRAYSIZE(tiny_wide))==ERROR_INSUFFICIENT_BUFFER);
    CHECK(tiny_wide[0]==0);
    CHECK(common_product_path_w(L"ntvdm.exe",tiny_wide,ARRAYSIZE(tiny_wide))==ERROR_INSUFFICIENT_BUFFER);
    CHECK(tiny_wide[0]==0);
    CHECK(common_system_root_a(ansi,sizeof(ansi))==0);
    CHECK(common_product_path_a(L"ntvdm.exe",ansi,sizeof(ansi))==0);
    CHECK(strstr(ansi,"ntvdm.exe")!=NULL);
    CHECK(common_product_path_a(L"ntvdm.exe",tiny,sizeof(tiny))==ERROR_INSUFFICIENT_BUFFER);
    CHECK(tiny[0]==0);
    for(index=0;index<ARRAYSIZE(invalid);++index) {
        path[0]=L'x';ansi[0]='x';
        CHECK(common_product_path_w(invalid[index],path,ARRAYSIZE(path))==ERROR_INVALID_PARAMETER);
        CHECK(path[0]==0);
        CHECK(common_product_path_a(invalid[index],ansi,sizeof(ansi))==ERROR_INVALID_PARAMETER);
        CHECK(ansi[0]==0);
    }
    CHECK(common_system_root_w(NULL,1)==ERROR_INVALID_PARAMETER);
    CHECK(common_system_root_a(NULL,1)==ERROR_INVALID_PARAMETER);
    puts("COMMON-SYSTEM-ROOT-PASS");
    return 0;
}
