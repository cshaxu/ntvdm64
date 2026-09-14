/* Execute the original classifier. Only process fields/suffix initialization
 * are test bindings; file/path/section services are native operations. */
#include <nt.h>
#include <base_classifier.h>
#include <stdio.h>
#include <wchar.h>
UNICODE_STRING BaseDotComSuffixName, BaseDotPifSuffixName, BaseDotExeSuffixName;
static OPENNT_SUPPORT_PEB peb;
static OPENNT_SUPPORT_TEB teb;
POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID) { return &peb; }
POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID) { return &teb; }
PVOID NTAPI RtlProcessHeap(VOID) { return GetProcessHeap(); }
int main(void)
{
    LPCWSTR names[]={L"O:\\ntvdm64\\MEM.EXE",L"O:\\ntvdm64\\COMMAND.COM",
        L"O:\\ntvdm64\\EDIT.COM",L"O:\\ntvdm64\\system32\\WRITE.EXE"};
    DWORD expected[]={SCS_DOS_BINARY,SCS_DOS_BINARY,SCS_DOS_BINARY,SCS_WOW_BINARY};
    WCHAR self[MAX_PATH];
    DWORD type,i,error;
    BOOL ok;
    peb.ImageBaseAddress=GetModuleHandleW(NULL);
    /* Original base/win32/client/baseinit.c initialization values. */
    RtlInitUnicodeString(&BaseDotComSuffixName,L".com");
    RtlInitUnicodeString(&BaseDotPifSuffixName,L".pif");
    RtlInitUnicodeString(&BaseDotExeSuffixName,L".exe");
    if(sizeof(RTL_RELATIVE_NAME)!=12 || sizeof(SECTION_IMAGE_INFORMATION)!=48) return 2;
    for(i=0;i<4;i++) {
        type=0xffffffff;
        if(!GetBinaryTypeW(names[i],&type) || type!=expected[i]) {
            printf("FAIL image=%ls type=%lu error=%lu\n",names[i],type,GetLastError()); return 3;
        }
        printf("PASS image=%ls type=%lu\n",names[i],type);
    }
    if(!GetModuleFileNameW(NULL,self,MAX_PATH)) return 4;
    if(!GetBinaryTypeW(self,&type) || type!=SCS_32BIT_BINARY) return 5;
    if(!GetBinaryTypeW(wcsrchr(self,L'\\')+1,&type) || type!=SCS_32BIT_BINARY) return 7;
    if(!GetModuleFileNameW(GetModuleHandleW(L"kernel32.dll"),self,MAX_PATH)) return 8;
    type=0xffffffff;
    if(GetBinaryTypeW(self,&type) || GetLastError()!=ERROR_BAD_EXE_FORMAT || type!=0xffffffff) return 9;
    if(!GetBinaryTypeW(L"suffix.COM",&type) || type!=SCS_DOS_BINARY) return 10;
    if(!GetBinaryTypeW(L"suffix.pif",&type) || type!=SCS_PIF_BINARY) return 11;
    if(!GetBinaryTypeW(L"suffix.exe",&type) || type!=SCS_DOS_BINARY) return 12;
    type=0xffffffff;
    if(GetBinaryTypeW(L"suffix.bin",&type) || GetLastError()!=ERROR_BAD_EXE_FORMAT || type!=0xffffffff) return 13;
    type=0xffffffff;
    ok=GetBinaryTypeW(L"missing-original-classifier-input.exe",&type);
    error=GetLastError();
    printf("missing result=%u error=%lu type=%lu native=%08lx\n",ok,error,type,(ULONG)teb.LastStatusValue);
    if(ok || error!=ERROR_FILE_NOT_FOUND || type!=0xffffffff) return 6;
    puts("PASS: original classifier guest formats, absolute/relative PE, DLL rejection, suffix policy, missing-file failure");
    return 0;
}
