#include "run16-exe/image_classification.h"
#include <stdio.h>

static unsigned checks,failures;
#define CHECK(x) do { ++checks; if(!(x)) { ++failures; printf("FAIL %d: %s\n",__LINE__,#x); } } while(0)
int wmain(int argc,WCHAR **argv)
{
    DWORD subsystem,error,before,after,i;
    if(argc!=4)return ERROR_INVALID_PARAMETER;
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
    printf("image-classification checks=%u failures=%u handle-delta=%ld\n",
        checks,failures,(long)after-(long)before);
    return failures ? ERROR_INVALID_DATA : ERROR_SUCCESS;
}
