#include "run16-exe/image_classification.h"
#include "common/native_image.h"
#include <stdio.h>

static unsigned checks,failures;
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; printf("FAIL %d: %s\n",__LINE__,#x); } } while(0)
static void process_image(PCWSTR application,DWORD expected_machine)
{
    STARTUPINFOW startup={sizeof(startup)};PROCESS_INFORMATION child={0};
    WCHAR path[MAX_PATH+32];DWORD machine=0,subsystem=0,error,before,after,i;
    BOOL created=CreateProcessW(application,NULL,NULL,NULL,FALSE,
        CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,NULL,&startup,&child);
    CHECK(created);if(!created)return;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    error=common_native_process_image(child.hProcess,&machine,&subsystem);
    CHECK(!error && machine==expected_machine && subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
    CHECK(!common_native_process_path(child.hProcess,path,MAX_PATH+32));
    machine=subsystem=0;
    CHECK(!common_native_image(path,&machine,&subsystem) && machine==expected_machine);
    CHECK(common_native_process_path(child.hProcess,path,2)==ERROR_INSUFFICIENT_BUFFER && !path[0]);
    for(i=0;i<32;++i) {
        machine=subsystem=0;
        CHECK(!common_native_process_image(child.hProcess,&machine,&subsystem) &&
            machine==expected_machine && subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    CHECK(after==before);
    CHECK(WaitForSingleObject(child.hProcess,0)==WAIT_TIMEOUT);
    /* Only this fixture's newly created suspended child is terminated. */
    CHECK(TerminateProcess(child.hProcess,0));
    CHECK(WaitForSingleObject(child.hProcess,10000)==WAIT_OBJECT_0);
    CloseHandle(child.hThread);CloseHandle(child.hProcess);
}
int wmain(int argc,WCHAR **argv)
{
    DWORD subsystem,error,before,after,i;
    if(argc!=4 && argc!=5)return ERROR_INVALID_PARAMETER;
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&before));
    for(i=0;i<32;++i) {
        subsystem=MAXDWORD;
        error=run16_classify_native_image(argv[1],&subsystem);
        CHECK(error==ERROR_SUCCESS && subsystem==IMAGE_SUBSYSTEM_WINDOWS_GUI);
        subsystem=MAXDWORD;
        error=run16_classify_native_image(argv[2],&subsystem);
        CHECK(error==ERROR_SUCCESS && subsystem==IMAGE_SUBSYSTEM_WINDOWS_CUI);
        subsystem=MAXDWORD;
        error=run16_classify_native_image(argv[3],&subsystem);
        CHECK(error==ERROR_BAD_EXE_FORMAT && subsystem==0);
        subsystem=MAXDWORD;
        CHECK(run16_classify_native_image(L"",&subsystem)==ERROR_INVALID_PARAMETER && !subsystem);
        CHECK(run16_classify_native_image(NULL,&subsystem)==ERROR_INVALID_PARAMETER && !subsystem);
        CHECK(run16_classify_native_image(argv[1],NULL)==ERROR_INVALID_PARAMETER);
        subsystem=MAXDWORD;
        CHECK(run16_classify_native_image(L"?:\\invalid-image",&subsystem)!=ERROR_SUCCESS && !subsystem);
    }
    CHECK(GetProcessHandleCount(GetCurrentProcess(),&after));
    CHECK(after==before);
    if(argc==5) {
        process_image(argv[2],IMAGE_FILE_MACHINE_I386);
        process_image(argv[4],IMAGE_FILE_MACHINE_AMD64);
    }
    if(argc==5) {
        DWORD first,second;
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&first));
        process_image(argv[2],IMAGE_FILE_MACHINE_I386);
        process_image(argv[4],IMAGE_FILE_MACHINE_AMD64);
        CHECK(GetProcessHandleCount(GetCurrentProcess(),&second));
        printf("process bootstrap handles=%lu first=%lu second=%lu\n",before,first,second);
        CHECK(second==first);
    }
    printf("image-classification checks=%u failures=%u handle-delta=%ld\n",
        checks,failures,(long)after-(long)before);
    return failures ? ERROR_INVALID_DATA : ERROR_SUCCESS;
}
