/* Exact entry prefixes through the DLL-load decision only; not a full
 * thunk/driver emulator. Native API calls after this boundary never run. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef DWORD VPVOID,VPDEVMODE31;
typedef struct {DWORD f1,f2,f3,f4,f5,f6,f7,f8;} ARGS;
typedef ARGS DEVICEMODE16,*PDEVICEMODE16,EXTDEVICEMODE16,*PEXTDEVICEMODE16;
typedef ARGS DEVICECAPABILITIES16,*PDEVICECAPABILITIES16,GETENVIRONMENT16,*PGETENVIRONMENT16;
typedef struct {ARGS args;} VDMFRAME,*PVDMFRAME;
typedef struct {char *name;ULONG (*lpfn)();} DLLENTRYPOINTS;
static DLLENTRYPOINTS spoolerapis[15];
enum {WOW_EXTDEVICEMODE,WOW_DEVICEMODE,WOW_DEVICECAPABILITIES,WOW_SPOOLERAPI_COUNT=15};
static unsigned checks,failures,allocs,live,fail_at,loads;
static void *blocks[8];
static void check(int ok,const char *name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static void *allocate(size_t n){unsigned i;void *p;if(++allocs==fail_at)return NULL;p=malloc(n);if(!p)abort();for(i=0;i<8;i++)if(!blocks[i]){blocks[i]=p;++live;return p;}abort();}
static void release(void *p){unsigned i;if(!p)return;for(i=0;i<8;i++)if(blocks[i]==p){free(p);blocks[i]=NULL;--live;return;}check(0,"owned free");}
static BOOL load_apis(char *name,DLLENTRYPOINTS *table,int count){
    unsigned i;check(!strcmp(name,"WINSPOOL.DRV")&&table==spoolerapis&&count==15,"original loader boundary");
    for(i=0;i<8;i++)if(blocks[i])check(!strcmp(blocks[i],"fixture"),"copied guest string before callback");
    ++loads;return FALSE;
}
#define FASTCALL __fastcall
#define GETARGPTR(frame,size,arg) ((void)(size),(arg)=&(frame)->args)
#define FREEARGPTR(arg) ((void)(arg))
#define GETPSZPTR(v,p) ((p)=(PSZ)(v))
#define FREEPSZPTR(p) ((void)(p))
#define FETCHDWORD(v) (v)
#define FETCHWORD(v) ((WORD)(v))
#define HWND32(v) ((HWND)(v))
#define malloc_w allocate
#define free_w release
#define LoadLibraryAndGetProcAddresses load_apis
#define RETURN(v) return (v)
#include "loader_prefixes.h"
int main(void){
    ULONG (__fastcall *entry[])(PVDMFRAME)={WG32DeviceMode,WG32ExtDeviceMode,WG32DeviceCapabilities,WG32GetEnvironment};
    unsigned n,k,i;VDMFRAME frame;
    for(n=0;n<4;n++)for(k=0;k<4;k++){
        for(i=0;i<8;i++)((DWORD*)&frame.args)[i]=(DWORD)"fixture";
        allocs=loads=0;fail_at=k;
        check(entry[n](&frame)==0,"loader failure keeps original zero return");
        check(loads==1&&!live,"failed load releases copied parameters");
        for(i=0;i<8;i++)if(blocks[i])release(blocks[i]);
    }
    printf("WOW_SPOOL_LOADER_FAILURE checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
