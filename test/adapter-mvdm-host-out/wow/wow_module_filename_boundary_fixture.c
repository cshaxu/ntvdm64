/* Exact extracted WOW thunk with real native GetModuleFileName and test-owned
 * guarded mappings. This verifies bounds, not a guest selector integration. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
typedef char *PSZ;
typedef struct {WORD f1;ULONG_PTR f2;WORD f3;} ARGS, *PGETMODULEFILENAME16, *PVDMFRAME;
static unsigned allocated,flushed;
#define FASTCALL
#define GETARGPTR(frame,size,p) ((p)=(frame))
#define FREEARGPTR(p) ((void)0)
#define ALLOCVDMPTR(vp,cb,p) ((p)=(PSZ)(vp),allocated=(unsigned)(cb))
#define FREEVDMPTR(p) ((void)0)
#define FLUSHVDMPTR(vp,cb,p) (flushed=(unsigned)(cb))
#define INT32(x) ((INT)(SHORT)(x))
#define GETINT16(x) ((SHORT)(x))
#define ISTASKALIAS(x) 0
#define HMODULE32(x) ((HMODULE)(ULONG_PTR)1)
#define RETURN(x) return (x)
static UINT GetHtaskAliasProcessName(WORD task,PSZ out,UINT count){(void)task;(void)out;(void)count;return 0;}
#include THUNK_SOURCE
static int run(const char *name,WORD module,WORD count,char *buffer) {
 ARGS args={module,(ULONG_PTR)buffer,count};ULONG result=0;DWORD exception=0;
 allocated=flushed=0;
 __try {result=WK32GetModuleFileName(&args);} __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
 printf("%s result=%lu exception=%08lx allocated=%u flushed=%u\n",name,result,exception,allocated,flushed);
 if(exception)return 1;
 if(result!=((module==0 && count==4)?4u:0u))return 1;
 if(allocated!=((SHORT)count>0?count:0u) || flushed!=allocated)return 1;
 return 0;
}
int main(void){SYSTEM_INFO si;char *memory,*edge;DWORD old;int faults=0;
 GetSystemInfo(&si);memory=VirtualAlloc(NULL,si.dwPageSize*2,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
 if(!memory||!VirtualProtect(memory+si.dwPageSize,si.dwPageSize,PAGE_NOACCESS,&old))return 3;
 edge=memory+si.dwPageSize-1;*edge='X';faults+=run("invalid-module-one-byte",1,1,edge);
 faults+=run("zero-capacity",0,0,edge+1);
 faults+=run("negative-capacity",1,0xffff,edge+1);
 memset(memory,'X',16);faults+=run("valid-module-short",0,4,memory);
 VirtualFree(memory,0,MEM_RELEASE);printf("faults=%d\n",faults);return faults?1:0;
}
