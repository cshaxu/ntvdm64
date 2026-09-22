#include "wow_task_profile_bindings.h"
#include <stdio.h>

static unsigned errors;
typedef char full_class_is_one[(KeyValueFullInformation==1) ? 1 : -1];
typedef char partial_class_is_two[(KeyValuePartialInformation==2) ? 1 : -1];
typedef char partial_data_offset[(offsetof(KEY_VALUE_PARTIAL_INFORMATION,Data)==12) ? 1 : -1];
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError());} } while(0)
#ifdef WOW_PROFILE_EXISTING_KEY
/* Test-only opening boundary. Product binding still opens PMAP_COMPAT.
 * Existing OS metadata permits real successful queries without registry writes. */
HANDLE WINAPI wow_profile_open_key(UINT section,ACCESS_MASK access)
{
    HKEY key=NULL;
    CHECK(section==14 && access==KEY_READ);
    if(RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows NT\\CurrentVersion",0,
        access|KEY_WOW64_64KEY,&key)!=ERROR_SUCCESS)return NULL;
    return key;
}
int __cdecl main(void)
{
    WCHAR expected[256],output[256];
    struct { WCHAR before; WCHAR data[2]; WCHAR after; } guarded;
    DWORD bytes=sizeof(expected),type=0,before,after,result;
    HKEY key=(HKEY)wow_profile_open_key(14,KEY_READ);
    unsigned i;
    CHECK(key!=NULL);if(!key)return 99;
    result=RegQueryValueExW(key,L"ProductName",NULL,&type,(BYTE *)expected,&bytes);
    CHECK(result==ERROR_SUCCESS && type==REG_SZ && bytes>=4 && bytes<=sizeof(expected));
    if(result!=ERROR_SUCCESS || type!=REG_SZ || bytes<4 || bytes>sizeof(expected)) {
        RegCloseKey(key);return 99;
    }
    CHECK(expected[bytes/sizeof(WCHAR)-1]==0);
    bytes=sizeof(output);
    CHECK(RegQueryValueExW(key,L"S40_41865857_MISSING_VALUE",NULL,NULL,(BYTE *)output,&bytes)==ERROR_FILE_NOT_FOUND);
    RegCloseKey(key);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<32;++i) {
        result=FastGetProfileStringW(14,L"ProductName",L"",output,256);
        CHECK(result==wcslen(expected)+1 && !memcmp(output,expected,result*sizeof(WCHAR)));
        guarded.before=0x5a5a;guarded.after=0x6b6b;guarded.data[0]=guarded.data[1]=L'?';
        CHECK(FastGetProfileStringW(14,L"ProductName",L"",guarded.data,2)==result);
        CHECK(guarded.data[0]==expected[0] && guarded.data[1]==0 && guarded.before==0x5a5a && guarded.after==0x6b6b);
        guarded.data[0]=guarded.data[1]=L'?';
        CHECK(FastGetProfileStringW(14,L"ProductName",L"",guarded.data,1)==result);
        CHECK(!guarded.data[0] && guarded.data[1]==L'?' && guarded.before==0x5a5a && guarded.after==0x6b6b);
        CHECK(FastGetProfileStringW(14,L"S40_41865857_MISSING_VALUE",L"safe",output,256)==5);
        CHECK(!memcmp(output,L"safe",5*sizeof(WCHAR)));
        output[0]=L'?';
        CHECK(!FastGetProfileStringW(14,L"S40_41865857_MISSING_VALUE",NULL,output,256) && output[0]==L'?');
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
    printf("WOW_PROFILE_EXISTING errors=%u repeats=32 success=1 truncation=2 missing=2\n",errors);
    return errors!=0;
}
#else
int __cdecl main(void)
{
    WCHAR output[80];
    DWORD before,after;unsigned i;
    HKEY key=NULL;
    LONG status=RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Compatibility",
        0,KEY_READ|KEY_WOW64_64KEY,&key);
    /* Read-only native absence precondition, never delete a key to force it. */
    CHECK(status==ERROR_FILE_NOT_FOUND || status==ERROR_PATH_NOT_FOUND);
    if(key)RegCloseKey(key);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<32;++i) {
        output[0]=L'?';
        CHECK(wow_task_profile(NULL,14,L"S40_PROFILE_MISSING",L"",output,80)==1 && !output[0]);
        CHECK(wow_task_profile(NULL,14,L"S40_PROFILE_MISSING",L"safe",output,80)==5 && !memcmp(output,L"safe",5*sizeof(WCHAR)));
        output[0]=L'?';
        CHECK(wow_task_profile(NULL,14,L"S40_PROFILE_MISSING",NULL,output,80)==0 && output[0]==L'?');
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    printf("profile_handles before=%lu after=%lu\n",before,after);
    CHECK(after==before);
    CHECK(!wow_task_profile(NULL,13,L"X",L"",output,80));
    CHECK(!wow_task_profile(NULL,14,L"X",L"too long",output,2));
    CHECK(!wow_task_profile(NULL,14,L"X",L"",output,MAXDWORD));
    CHECK(!wow_task_profile(NULL,14,L"X",L"",output,0));
    printf("WOW_TASK_PROFILE errors=%u native_absence=1 repeats=32 invalid=4\n",errors);
    return errors!=0;
}
#endif
