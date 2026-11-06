/* Exact original spool bodies with isolated native/Win16 boundaries.
 * ClosePrinter consumes the native handle even when it reports failure. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winspool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef DWORD VPVOID;
typedef WORD HAND16;
typedef DWORD *PDWORD16;
typedef struct { DWORD f1,f2; } ARGS;
typedef ARGS OPENJOB16,CLOSEJOB16,*POPENJOB16,*PCLOSEJOB16;
typedef struct { ARGS args; } VDMFRAME,*PVDMFRAME;
typedef struct { char *name; ULONG (*lpfn)(); } DLLENTRYPOINTS;
enum { WOW_OpenPrinterA=3,WOW_StartDocPrinterA=4,WOW_EndDocPrinter=7,
       WOW_ClosePrinter=8,WOW_SPOOLERAPI_COUNT=15 };
static DLLENTRYPOINTS spoolerapis[15];
static WORD gUser16hInstance=0x1234;
static unsigned checks,failures,native_live,carrier_live,locks,closes,starts,frees;
static int open_fail,start_fail,close_fail,end_fail,alloc_fail,lock_fail,map_fail,profile_missing;
static DWORD carrier;
static HANDLE local_handle=(HANDLE)0x12340022;
static void check(int ok,const char *label){++checks;if(!ok){++failures;printf("FAIL %s\n",label);}}
static HANDLE local_alloc(WORD flags,INT bytes,HANDLE instance){
    check(flags==LMEM_MOVEABLE&&bytes==4&&instance==(HANDLE)0x1234,"carrier owner/layout");
    if(alloc_fail)return NULL;carrier_live=1;carrier=0xa5a5a5a5;return local_handle;
}
static VPVOID local_lock(HANDLE h){
    if(!h||lock_fail)return 0;
    check(h==local_handle&&carrier_live,"lock owned carrier");++locks;return 0x12340100;
}
static BOOL local_unlock(HANDLE h){check(h==local_handle&&locks>0,"balanced unlock");--locks;return FALSE;}
static HANDLE local_free(HANDLE h){check(h==local_handle&&carrier_live&&!locks,"free unlocked carrier");carrier_live=0;++frees;return NULL;}
static ULONG open_printer(char *name,HANDLE *h,PRINTER_DEFAULTS *d){
    (void)name;(void)d;if(open_fail){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}
    *h=(HANDLE)19;native_live=1;return TRUE;
}
static ULONG start_doc(HANDLE h,DWORD level,LPBYTE info){
    (void)level;(void)info;check(h==(HANDLE)19&&native_live,"start owned printer");++starts;
    if(start_fail){SetLastError(ERROR_DISK_FULL);return 0;}return 13;
}
static ULONG end_doc(HANDLE h){check(h==(HANDLE)19&&native_live,"end owned printer");if(end_fail){SetLastError(ERROR_WRITE_FAULT);return 0;}return TRUE;}
static ULONG close_printer(HANDLE h){
    check(h==(HANDLE)19&&native_live,"close owned printer once");native_live=0;++closes;
    SetLastError(ERROR_INVALID_PRINTER_STATE);return !close_fail;
}
static BOOL driver_name(char *port,char *out){(void)port;strcpy(out,"fixture");return !profile_missing;}
static LPDEVMODE default_mode(char *name){(void)name;return NULL;}
static BOOL load_apis(char *name,DLLENTRYPOINTS *p,int n){(void)name;(void)p;(void)n;return FALSE;}
#define FASTCALL __fastcall
#define GETARGPTR(frame,size,arg) ((void)(size),(arg)=&(frame)->args)
#define FREEARGPTR(arg) ((void)(arg))
#define GETPSZPTR(v,p) ((p)=(char *)(v))
#define FREEPSZPTR(p) ((void)(p))
#define GETMISCPTR(v,p) ((void)(v),(p)=map_fail?NULL:(LPBYTE)&carrier)
#define FREEMISCPTR(p) ((void)(p))
#define RETURN(value) return (value)
#define LOGDEBUG(a,b) ((void)0)
#define LocalAlloc16 local_alloc
#define LocalLock16 local_lock
#define LocalUnlock16 local_unlock
#define LocalFree16 local_free
#define GetDriverName driver_name
#define GetDefaultDevMode32 default_mode
#define LoadLibraryAndGetProcAddresses load_apis
#define free_w free
WORD GetPrn16(HANDLE h);
HANDLE Prn32(WORD h);
VOID FreePrn(WORD h);
#include "spool_exact.h"
static void reset(void){
    native_live=carrier_live=locks=closes=starts=frees=0;
    open_fail=start_fail=close_fail=end_fail=alloc_fail=lock_fail=map_fail=profile_missing=0;
}
static ULONG open_job(void){VDMFRAME frame={{(DWORD)"port",(DWORD)"title"}};return WG32OpenJob(&frame);}
int main(void){
    int e,c;ULONG result;VDMFRAME frame={{0x22,0}};
    spoolerapis[WOW_OpenPrinterA].lpfn=(ULONG(*)())open_printer;
    spoolerapis[WOW_StartDocPrinterA].lpfn=(ULONG(*)())start_doc;
    spoolerapis[WOW_EndDocPrinter].lpfn=(ULONG(*)())end_doc;
    spoolerapis[WOW_ClosePrinter].lpfn=(ULONG(*)())close_printer;
    for(e=0;e<2;e++)for(c=0;c<2;c++){
        reset();check(open_job()==0x22,"normal open alias");
        check(native_live&&carrier_live&&!locks&&carrier==19,"normal carrier payload");
        end_fail=e;close_fail=c;result=WG32CloseJob(&frame);
        check(result==(c?ERROR_INVALID_PRINTER_STATE:TRUE),"original close result precedence");
        check(!native_live&&!carrier_live&&!locks&&closes==1&&frees==1,"close consumes handle on success and failure");
    }
    reset();profile_missing=1;check(!open_job()&&!native_live&&!carrier_live,"no-profile original zero");
    reset();open_fail=1;check(open_job()==ERROR_ACCESS_DENIED&&!native_live&&!starts,"open provider failure");
    reset();start_fail=1;check(open_job()==ERROR_DISK_FULL,"start error preserved across cleanup");
    check(!native_live&&closes==1&&!carrier_live,"failed start retires native handle");
    for(e=0;e<3;e++){
        reset();alloc_fail=e==0;lock_fail=e==1;map_fail=e==2;
        check(open_job()==0,"failed carrier returns zero");
        check(!native_live&&closes==1&&!carrier_live&&!locks,"failed carrier retires both owners");
    }
    reset();carrier_live=1;lock_fail=1;check(Prn32(0x22)==NULL,"failed carrier lock has defined null result");
    reset();carrier_live=1;map_fail=1;check(Prn32(0x22)==NULL&&!locks,"failed mapping has defined null result and unlock");
    printf("WOW_SPOOL_FAILURE checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
