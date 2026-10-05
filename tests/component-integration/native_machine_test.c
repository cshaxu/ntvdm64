/* Real SEC_IMAGE metadata and real suspended process identity, both widths.
 * No broker/global endpoint, guest or input injection is used by this fixture. */
#include <windows.h>
#include <stdio.h>
#include "common/image_classification.h"
static unsigned checks;
static PROCESS_INFORMATION owned_child;
static int failed(int line,const char *expression)
{
    DWORD error=GetLastError();
    if(owned_child.hProcess) {
        TerminateProcess(owned_child.hProcess,ERROR_PROCESS_ABORTED);
        WaitForSingleObject(owned_child.hProcess,10000);
        CloseHandle(owned_child.hThread);CloseHandle(owned_child.hProcess);
        ZeroMemory(&owned_child,sizeof(owned_child));
    }
    fprintf(stderr,"line %d: %s (error %lu)\n",line,expression,error);return 1;
}
#define CHECK(x) do { ++checks;if(!(x))return failed(__LINE__,#x); } while(0)
int wmain(int argc,WCHAR **argv)
{
    DWORD before,after,machine=0,subsystem=0,i;
    const DWORD expected[2]={IMAGE_FILE_MACHINE_I386,IMAGE_FILE_MACHINE_AMD64};
    WCHAR path[MAX_PATH];
    CHECK(argc==3);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    CHECK(!common_process_machine(GetCurrentProcess(),&machine));
#ifdef _WIN64
    CHECK(machine==IMAGE_FILE_MACHINE_AMD64);
#else
    CHECK(machine==IMAGE_FILE_MACHINE_I386);
#endif
    /* Measure steady-state lifetime after the first host CreateProcess. Its
     * one-time host initialization changes the process handle baseline; each
     * subsequent complete create/query/terminate/close cycle must not grow it. */
    for(i=0;i<8;++i) {
        STARTUPINFOW start={sizeof(start)};PROCESS_INFORMATION child={0};
        CHECK(!common_classify_native_image(argv[i%2+1],&machine,&subsystem,path,ARRAYSIZE(path)));
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after) && before==after);
        CHECK(machine==expected[i%2] && subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI && path[0]);
        CHECK(CreateProcessW(argv[i%2+1],NULL,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&start,&child));
        owned_child=child;
        CHECK(!common_process_machine(child.hProcess,&machine) && machine==expected[i%2]);
        {
            WCHAR dos_path[MAX_PATH],native_path[MAX_PATH],exact_path[MAX_PATH+32];
            DWORD count=ARRAYSIZE(dos_path),file_machine=0,file_subsystem=0,error;
            CHECK(QueryFullProcessImageNameW(child.hProcess,0,dos_path,&count));
            error=common_classify_native_image(dos_path,&file_machine,&file_subsystem,NULL,0);
            printf("process-path actual=%04lx DOS-open error=%lu machine=%04lx\n",
                machine,error,file_machine);
            count=ARRAYSIZE(native_path);
            CHECK(QueryFullProcessImageNameW(child.hProcess,PROCESS_NAME_NATIVE,native_path,&count));
            CHECK(swprintf_s(exact_path,ARRAYSIZE(exact_path),L"\\\\?\\GLOBALROOT%ls",native_path)>0);
            CHECK(!common_classify_native_image(exact_path,&file_machine,&file_subsystem,NULL,0));
            CHECK(file_machine==machine && file_subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
            CHECK(!common_classify_native_process(child.hProcess,&file_machine,&file_subsystem));
            CHECK(file_machine==machine && file_subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
        }
        CHECK(TerminateProcess(child.hProcess,0));
        CHECK(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);
        CHECK(CloseHandle(child.hThread));CHECK(CloseHandle(child.hProcess));
        ZeroMemory(&owned_child,sizeof(owned_child));
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
        if(!i){printf("host first-create handle initialization=%ld\n",(long)after-(long)before);before=after;}
        else CHECK(before==after);
        CHECK(common_classify_native_image(argv[i%2+1],&machine,&subsystem,path,1)==ERROR_FILENAME_EXCED_RANGE);
        CHECK(!machine && !subsystem && !path[0]);
    }
    CHECK(common_classify_native_image(L"",&machine,&subsystem,NULL,0)==ERROR_INVALID_PARAMETER);
    CHECK(!machine && !subsystem);
    CHECK(common_classify_native_process(NULL,&machine,&subsystem)==ERROR_INVALID_PARAMETER);
    CHECK(!machine && !subsystem);
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    CHECK(before==after);
    printf("PASS native-machine checks=%u handle-delta=%ld\n",checks,(long)after-(long)before);
    return 0;
}
