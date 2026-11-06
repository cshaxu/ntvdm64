#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define FASTCALL __stdcall
#define LOGDEBUG(...)
typedef WORD HAND16;
#include "dib_retirement_types.h"
typedef FINDANDRELEASEDIB16 VDMFRAME,*PVDMFRAME;
#define GETARGPTR(frame,size,out) ((out)=(PFINDANDRELEASEDIB16)(frame))
static PDIBINFO pDibInfoHead;
static int checks,failures,restores,removes;
static BOOL restore_ok=TRUE;
#define CHECK(x) do{++checks;if(!(x)){++failures;printf("FAIL line=%d %s\n",__LINE__,#x);}}while(0)
static ULONG W32RestoreOldDib(PDIBINFO p){CHECK(p==pDibInfoHead);++restores;return restore_ok;}
static LONG VdmRemoveVirtualMemory(ULONG a){CHECK(a==0x120000);++removes;return 0;}
static void free_w(void *p){CHECK(HeapFree(GetProcessHeap(),0,p));}
VOID W32FreeDibInfo(PDIBINFO,PDIBINFO);
BOOL W32FreeDibInfoHandle(PDIBINFO,PDIBINFO);
#include "dib_retirement_exact.h"
static PDIBINFO create_owner(void) {
    BITMAPINFO bmi={0};void *bits;char name[80];static int serial;
    PDIBINFO p=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*p));
    CHECK(p!=NULL);if(!p)ExitProcess(90);
    p->di_hdc=CreateCompatibleDC(NULL);CHECK(p->di_hdc!=NULL);
    sprintf(name,"dib-retire-%lu-%d.tmp",GetCurrentProcessId(),++serial);
    p->di_hfile=CreateFileA(name,GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_FLAG_DELETE_ON_CLOSE,NULL);
    CHECK(p->di_hfile!=INVALID_HANDLE_VALUE);
    p->di_hsec=CreateFileMappingA(p->di_hfile,NULL,PAGE_READWRITE,0,65536,NULL);CHECK(p->di_hsec!=NULL);
    bmi.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);bmi.bmiHeader.biWidth=4;bmi.bmiHeader.biHeight=-4;
    bmi.bmiHeader.biPlanes=1;bmi.bmiHeader.biBitCount=32;
    p->di_hbm=CreateDIBSection(p->di_hdc,&bmi,DIB_RGB_COLORS,&bits,p->di_hsec,0);CHECK(p->di_hbm&&bits);
    CHECK(SelectObject(p->di_hdc,p->di_hbm)!=NULL);
    p->di_newdib=MapViewOfFile(p->di_hsec,FILE_MAP_WRITE,0,0,65536);CHECK(p->di_newdib!=NULL);
    p->di_newIntelDib=(PVOID)0x120000;p->di_originaldibsel=0x1237;p->di_lockcount=1;
    pDibInfoHead=p;restores=removes=0;restore_ok=TRUE;return p;
}
int main(void) {
    PDIBINFO p;HDC dc;HBITMAP bitmap;DWORD before;VDMFRAME args={0};
    before=GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS);p=create_owner();dc=p->di_hdc;bitmap=p->di_hbm;
    CHECK(W32FindAndLockDibInfo(0x1237)==dc&&p->di_lockcount==2);
    CHECK(W32CheckAndFreeDibInfo(dc)&&p->di_lockcount==1&&!restores&&!removes);
    CHECK(W32CheckAndFreeDibInfo(dc)&&!pDibInfoHead&&restores==1&&removes==1);
    CHECK(GdiFlush());CHECK(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)==before);
    if(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)>before)DeleteObject(bitmap);
    p=create_owner();bitmap=p->di_hbm;args.hdib=0x1236;args.wFunId=1;
    CHECK(WK32FindAndReleaseDib(&args)&&!pDibInfoHead&&restores==1&&removes==1);
    CHECK(GdiFlush());CHECK(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)==before);
    if(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)>before)DeleteObject(bitmap);
    p=create_owner();bitmap=p->di_hbm;restore_ok=FALSE;
    CHECK(!W32CheckAndFreeDibInfo(p->di_hdc)&&pDibInfoHead==p&&p->di_lockcount==0&&restores==1&&!removes);
    printf("ORIGINAL failed-restore lockcount=%lu node-retained=1\n",p->di_lockcount);
    restore_ok=TRUE;CHECK(W32FreeDibInfoHandle(p,NULL)); /* fixture retirement, not production retry */
    if(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)>before)DeleteObject(bitmap);
    p=create_owner();bitmap=p->di_hbm;CHECK(W32FindAndLockDibInfo(0x1237)==p->di_hdc);
    CHECK(!WK32FindAndReleaseDib(&args)&&p->di_lockcount==1&&!restores&&!removes);
    printf("ORIGINAL nested-find-release lockcount=%lu result=0\n",p->di_lockcount);
    CHECK(W32FreeDibInfoHandle(p,NULL));
    if(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)>before)DeleteObject(bitmap);
    CHECK(!pDibInfoHead);CHECK(GdiFlush());CHECK(GetGuiResources(GetCurrentProcess(),GR_GDIOBJECTS)==before);
    printf("DIB_RETIREMENT checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
