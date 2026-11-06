#include "lib-win32/process-image/image.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"IMAGE TEST FAIL line=%d error=%lu\n",__LINE__,GetLastError());return 1;}}while(0)
int main(void)
{
    WCHAR self[MAX_PATH];WORD machine=0,subsystem=0;
    WORD expected=sizeof(void *)==8?IMAGE_FILE_MACHINE_AMD64:IMAGE_FILE_MACHINE_I386;
    WORD other=sizeof(void *)==8?IMAGE_FILE_MACHINE_I386:IMAGE_FILE_MACHINE_AMD64;
    CHECK(GetModuleFileNameW(NULL,self,MAX_PATH));
    CHECK(win32_process_image_machine(NULL,&machine)==ERROR_INVALID_PARAMETER);
    CHECK(win32_process_image_machine(GetCurrentProcess(),NULL)==ERROR_INVALID_PARAMETER);
    CHECK(!win32_process_image_machine(GetCurrentProcess(),&machine) && machine==expected);
    CHECK(!win32_process_image_validate_ex(self,expected,FALSE,&subsystem));
    CHECK(subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
    CHECK(win32_process_image_validate(self,other,FALSE)==ERROR_BAD_EXE_FORMAT);
    CHECK(win32_process_image_validate(self,expected,TRUE)==ERROR_BAD_EXE_FORMAT);
    CHECK(win32_process_image_validate(L"?:\\invalid\\missing.exe",expected,FALSE)!=ERROR_SUCCESS);
    HANDLE file=CreateFileW(self,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,0,NULL);
    CHECK(file!=INVALID_HANDLE_VALUE);
    HANDLE mapping=CreateFileMappingW(file,NULL,PAGE_READONLY|SEC_IMAGE,0,0,NULL);
    CHECK(mapping!=NULL);CloseHandle(mapping);CloseHandle(file);
    printf("IMAGE_COMPARE_SELF bits=%u expected=0 wrong_machine=%u sec_image=0\n",(unsigned)(sizeof(void *)*8),(unsigned)ERROR_BAD_EXE_FORMAT);
    printf("PROCESS_IMAGE_PASS bits=%u subsystem=%u\n",(unsigned)(sizeof(void *)*8),subsystem);
    return 0;
}
