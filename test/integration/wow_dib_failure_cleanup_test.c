#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
static int checks,failures;
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
static BOOL handle_live(HANDLE h) {
    DWORD flags;
    __try { return GetHandleInformation(h,&flags); }
    __except(GetExceptionCode()==0xc0000008u ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { return FALSE; }
}
static HDC cleanup(BOOL bRet,HDC hdcMem,HANDLE hfile,HANDLE hsec,HBITMAP hbm) {
#include "dib_failure_cleanup.h"
    return hdcMem;
}
int main(void) {
    int prefix,success;BITMAPINFO bmi={0};
    bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bmi.bmiHeader.biWidth=4;
    bmi.bmiHeader.biHeight=-4;bmi.bmiHeader.biPlanes=1;bmi.bmiHeader.biBitCount=32;
    for(success=0;success<=1;success++)for(prefix=0;prefix<=4;prefix++){
        DWORD before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);
        HDC dc=NULL,result=NULL;BOOL invalid_handle=FALSE;HANDLE file=NULL,section=NULL;HBITMAP bitmap=NULL;void *bits=NULL;
        char name[80];
        if(prefix>=1){dc=CreateCompatibleDC(NULL);CHECK(dc!=NULL);}
        if(prefix>=2){sprintf(name,"dib-owner-%lu-%d-%d.tmp",GetCurrentProcessId(),success,prefix);
            file=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_FLAG_DELETE_ON_CLOSE,NULL);
            CHECK(file!=INVALID_HANDLE_VALUE);if(file==INVALID_HANDLE_VALUE)return 90;}
        if(prefix>=3){section=CreateFileMappingA(file,NULL,PAGE_READWRITE,0,65536,NULL);CHECK(section!=NULL);}
        if(prefix>=4){bitmap=CreateDIBSection(dc,&bmi,DIB_RGB_COLORS,&bits,section,0);CHECK(bitmap&&bits);
            CHECK(SelectObject(dc,bitmap)!=NULL);}
        __try { result=cleanup(success,dc,file,section,bitmap); }
        __except(GetExceptionCode()==0xc0000008u ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH) { invalid_handle=TRUE; }
        CHECK(!invalid_handle);
        CHECK(result==(success?dc:NULL));
        CHECK(GdiFlush());
        CHECK(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)==before+(success?((dc!=NULL)+(bitmap!=NULL)):0));
        if(file)CHECK((handle_live(file)!=0)==success);
        if(section)CHECK((handle_live(section)!=0)==success);

        /* Explicit fixture-owned retirement also contains the baseline leak. */
        if(success&&dc)CHECK(DeleteDC(dc));
        if(bitmap&&(success||GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)>before))CHECK(DeleteObject(bitmap));
        if(section&&handle_live(section))CHECK(CloseHandle(section));
        if(file&&handle_live(file))CHECK(CloseHandle(file));
    }
    printf("DIB_FAILURE_CLEANUP checks=%d failures=%d\n",checks,failures);
    return failures?1:0;
}
