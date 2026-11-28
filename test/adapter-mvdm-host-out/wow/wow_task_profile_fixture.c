#include "non-mvdm/public/sdk/inc/nt.h"
#include "wow_task_profile_bindings.h"
#include <stdio.h>

static unsigned errors;
typedef char full_class_is_one[(KeyValueFullInformation==1) ? 1 : -1];
typedef char partial_class_is_two[(KeyValuePartialInformation==2) ? 1 : -1];
typedef char full_data_offset[(offsetof(KEY_VALUE_FULL_INFORMATION,Name)==20) ? 1 : -1];
typedef char partial_data_offset[(offsetof(KEY_VALUE_PARTIAL_INFORMATION,Data)==12) ? 1 : -1];
#define CHECK(x) do { if(!(x)){++errors;printf("FAIL line=%d error=%lu\n",__LINE__,GetLastError());} } while(0)
static DWORD profile_dword(UINT section,LPCWSTR key,DWORD fallback)
{
    DWORD value=0;
    CHECK(FastGetProfileDwordW(NULL,section,key,fallback,&value,0));
    return value;
}
static void policy_only(void)
{
    DWORD value=0x12345678;
    UINT number=321;
    WCHAR output[2]={L'?',0};
    CHECK(!FastGetProfileDwordW(NULL,10,L"Missing",7,&value,0x0010));
    CHECK(value==0x12345678);
    CHECK(!FastGetProfileIntW(NULL,10,L"Missing",7,&number,0x0010));
    CHECK(number==321);
    CHECK(!FastGetProfileStringW(NULL,10,L"Missing",L"x",output,2,0x0010));
    CHECK(output[0]==L'?');
}
#ifdef WOW_PROFILE_EXISTING_KEY
/* Test-only opening boundary. Product binding still opens PMAP_COMPAT.
 * Existing OS metadata permits real successful queries without registry writes. */
HANDLE WINAPI wow_profile_open_key(PUNICODE_STRING user,UINT section,ACCESS_MASK access,PDWORD policy)
{
    HKEY key=NULL;
    CHECK(!user && section==10 && access==KEY_READ);
    if(policy){DWORD requested=*policy;*policy=0;if(requested)return NULL;}
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
    DWORD expected_dword, dword_bytes=sizeof(expected_dword), dword_type;
    HKEY key=(HKEY)wow_profile_open_key(NULL,10,KEY_READ,NULL);
    unsigned i;
    UNICODE_STRING native_name;
    HANDLE native_key=NULL;
    union { KEY_VALUE_FULL_INFORMATION alignment; BYTE bytes[65536]; } native;
    WCHAR enum_name[16384]; BYTE enum_data[65536];
    DWORD enum_chars=16384,enum_bytes=sizeof(enum_data),enum_type;
    ULONG native_bytes=0;
    CHECK(key!=NULL);if(!key)return 99;
    RtlInitUnicodeString(&native_name,L"\\Registry\\Machine\\Software\\Microsoft\\Windows NT\\CurrentVersion");
    {
        OBJECT_ATTRIBUTES attributes;
        InitializeObjectAttributes(&attributes,&native_name,OBJ_CASE_INSENSITIVE,NULL,NULL);
        CHECK(NtOpenKey(&native_key,KEY_READ|KEY_WOW64_64KEY,&attributes)>=0);
    }
    if(!native_key){RegCloseKey(key);return 99;}
    CHECK(RegEnumValueW(key,0,enum_name,&enum_chars,NULL,&enum_type,enum_data,&enum_bytes)==ERROR_SUCCESS);
    CHECK(NtEnumerateValueKey(native_key,0,KeyValueFullInformation,&native,sizeof(native),&native_bytes)>=0);
    if(errors){NtClose(native_key);RegCloseKey(key);return 1;}
    CHECK(native.alignment.Type==enum_type && native.alignment.NameLength==enum_chars*sizeof(WCHAR));
    CHECK(native.alignment.NameLength<=sizeof(enum_name) && !memcmp(native.alignment.Name,enum_name,native.alignment.NameLength));
    CHECK(native.alignment.DataLength==enum_bytes && native.alignment.DataOffset<=sizeof(native)-enum_bytes);
    if(native.alignment.DataOffset<=sizeof(native)-enum_bytes)
        CHECK(!memcmp(native.bytes+native.alignment.DataOffset,enum_data,enum_bytes));
    CHECK(NtClose(native_key)>=0);
    result=RegQueryValueExW(key,L"ProductName",NULL,&type,(BYTE *)expected,&bytes);
    CHECK(result==ERROR_SUCCESS && type==REG_SZ && bytes>=4 && bytes<=sizeof(expected));
    if(result!=ERROR_SUCCESS || type!=REG_SZ || bytes<4 || bytes>sizeof(expected)) {
        RegCloseKey(key);return 99;
    }
    CHECK(expected[bytes/sizeof(WCHAR)-1]==0);
    CHECK(RegQueryValueExW(key,L"InstallDate",NULL,&dword_type,
        (BYTE *)&expected_dword,&dword_bytes)==ERROR_SUCCESS);
    CHECK(dword_type==REG_DWORD && dword_bytes==sizeof(expected_dword));
    if(errors){RegCloseKey(key);return 1;}
    bytes=sizeof(output);
    CHECK(RegQueryValueExW(key,L"S40_41865857_MISSING_VALUE",NULL,NULL,(BYTE *)output,&bytes)==ERROR_FILE_NOT_FOUND);
    RegCloseKey(key);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    policy_only();
    for(i=0;i<32;++i) {
        CHECK(profile_dword(10,L"InstallDate",0)==expected_dword);
        CHECK(profile_dword(10,L"S40_41865857_MISSING_VALUE",0x12345678)==0x12345678);
        result=FastGetProfileStringW(NULL,10,L"ProductName",L"",output,256,0);
        CHECK(result==wcslen(expected)+1 && !memcmp(output,expected,result*sizeof(WCHAR)));
        guarded.before=0x5a5a;guarded.after=0x6b6b;guarded.data[0]=guarded.data[1]=L'?';
        CHECK(FastGetProfileStringW(NULL,10,L"ProductName",L"",guarded.data,2,0)==result);
        CHECK(guarded.data[0]==expected[0] && guarded.data[1]==0 && guarded.before==0x5a5a && guarded.after==0x6b6b);
        guarded.data[0]=guarded.data[1]=L'?';
        CHECK(FastGetProfileStringW(NULL,10,L"ProductName",L"",guarded.data,1,0)==result);
        CHECK(!guarded.data[0] && guarded.data[1]==L'?' && guarded.before==0x5a5a && guarded.after==0x6b6b);
        CHECK(FastGetProfileStringW(NULL,10,L"S40_41865857_MISSING_VALUE",L"safe",output,256,0)==5);
        CHECK(!memcmp(output,L"safe",5*sizeof(WCHAR)));
        output[0]=L'?';
        CHECK(!FastGetProfileStringW(NULL,10,L"S40_41865857_MISSING_VALUE",NULL,output,256,0) && output[0]==L'?');
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && after==before);
    printf("WOW_PROFILE_EXISTING errors=%u repeats=32 success=1 truncation=2 missing=2 dword_success=1 dword_missing=1\n",errors);
    return errors!=0;
}
#else
UINT WINAPI wow_original_task_profile(UINT, LPCWSTR, LPCWSTR, LPWSTR, DWORD);
int __cdecl main(void)
{
    WCHAR output[80];
    DWORD before,after;unsigned i;
    HKEY key=NULL;
    LONG status;
    CHECK(!wow_profile_bind(NULL, wow_original_profile_int));
    CHECK(!wow_profile_bind(wow_original_task_profile, NULL));
    CHECK(wow_profile_bind(wow_original_task_profile, wow_original_profile_int));
    CHECK(wow_profile_bind(wow_original_task_profile, wow_original_profile_int));
    CHECK(wow_profile_int(10, L"S40_PROFILE_MISSING", 123) == 123);
    status=RegOpenKeyExW(HKEY_LOCAL_MACHINE,
        L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Compatibility",
        0,KEY_READ|KEY_WOW64_64KEY,&key);
    /* Read-only native absence precondition, never delete a key to force it. */
    CHECK(status==ERROR_FILE_NOT_FOUND || status==ERROR_PATH_NOT_FOUND);
    if(key)RegCloseKey(key);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    policy_only();
    for(i=0;i<32;++i) {
        CHECK(profile_dword(10,L"S40_PROFILE_MISSING",0x12345678)==0x12345678);
        output[0]=L'?';
        CHECK(wow_task_profile(NULL,10,L"S40_PROFILE_MISSING",L"",output,80)==1 && !output[0]);
        CHECK(wow_task_profile(NULL,10,L"S40_PROFILE_MISSING",L"safe",output,80)==5 && !memcmp(output,L"safe",5*sizeof(WCHAR)));
        output[0]=L'?';
        CHECK(wow_task_profile(NULL,10,L"S40_PROFILE_MISSING",NULL,output,80)==0 && output[0]==L'?');
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    printf("profile_handles before=%lu after=%lu\n",before,after);
    CHECK(after==before);
    CHECK(!wow_task_profile(NULL,13,L"X",L"",output,80));
    CHECK(!wow_task_profile(NULL,10,L"X",L"too long",output,2));
    CHECK(!wow_task_profile(NULL,10,L"X",L"",output,MAXDWORD));
    CHECK(!wow_task_profile(NULL,10,L"X",L"",output,0));
    printf("WOW_TASK_PROFILE errors=%u native_absence=1 repeats=32 invalid=4\n",errors);
    return errors!=0;
}
#endif
