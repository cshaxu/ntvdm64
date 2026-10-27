#include "precomp.h"
#include <stdio.h>
DWORD FASTCALL wow_original_WK32ICallProc32(PVDMFRAME);
DWORD __stdcall WK32ICallProc32MakeCall(DWORD,DWORD,void*);
__declspec(naked) DWORD __stdcall wow_callproc32_guarded(DWORD target,DWORD count,void *args) {
 __asm {
  push ebx
  push dword ptr [esp+16]
  push dword ptr [esp+16]
  push dword ptr [esp+16]
  call WK32ICallProc32MakeCall
  pop ebx
  ret 12
 }
}
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD a){return (PVOID)a;}
BOOL UpdateDosCurrentDirectory(UDCDFUNC d){(void)d;return TRUE;}
PFNWOWHANDLERSOUT pfnOut;
LPVOID WINAPI WOWGetVDMPointer(DWORD a,DWORD b,BOOL c){(void)a;(void)b;(void)c;ExitProcess(91);}
HMODULE WINAPI wow_extension_load_library_a(LPCSTR s,HANDLE h,DWORD d){(void)s;(void)h;(void)d;ExitProcess(92);}
BOOL WINAPI wow_extension_free_library(HMODULE h){(void)h;ExitProcess(93);}
FARPROC WINAPI wow_extension_get_proc_address(HMODULE h,LPCSTR s){(void)h;(void)s;ExitProcess(94);}
ULONG FASTCALL WK32Yield(PVDMFRAME f){(void)f;ExitProcess(95);}
VOID __cdecl BlockWOWIdle(BOOL b){(void)b;ExitProcess(96);}
__declspec(noinline) DWORD __stdcall zero(void){return 0xAA;}
__declspec(noinline) DWORD __cdecl one(DWORD a){return a+1;}
__declspec(noinline) DWORD __stdcall three_std(DWORD a,DWORD b,DWORD c){return a+2*b+3*c;}
__declspec(noinline) DWORD __cdecl three_cdecl(DWORD a,DWORD b,DWORD c){return a+2*b+3*c;}
__declspec(noinline) DWORD __cdecl many(DWORD a0, DWORD a1, DWORD a2, DWORD a3, DWORD a4, DWORD a5, DWORD a6, DWORD a7, DWORD a8, DWORD a9, DWORD a10, DWORD a11, DWORD a12, DWORD a13, DWORD a14, DWORD a15, DWORD a16, DWORD a17, DWORD a18, DWORD a19, DWORD a20, DWORD a21, DWORD a22, DWORD a23, DWORD a24, DWORD a25, DWORD a26, DWORD a27, DWORD a28, DWORD a29, DWORD a30, DWORD a31){return a0+2*a15+3*a31;}
__declspec(noinline) DWORD __cdecl except_target(void){RaiseException(0xE4730001,0,0,NULL);return 0;}
static unsigned errors;
static DWORD invoke(DWORD target,DWORD n,DWORD flags,BOOL reverse) {
 union {DWORD align; BYTE bytes[sizeof(VDMFRAME)+sizeof(ICALLPROC32)+64];} storage;
 PVDMFRAME frame=(PVDMFRAME)storage.bytes; PICALLPROC32 a=(PICALLPROC32)&frame->bArgs; DWORD *v=(DWORD*)&a->p1; unsigned i;
 ZeroMemory(&storage,sizeof(storage));a->cParams=n|flags;a->lpProcAddress=target;
 for(i=0;i<n;i++)v[i]=reverse?n-i:i+1;
 return wow_original_WK32ICallProc32(frame);
}
static void run(const char*name,DWORD target,DWORD n,DWORD flags,BOOL reverse,DWORD expected){DWORD before,after,result;
 __asm mov before,esp
 result=invoke(target,n,flags,reverse);
 __asm mov after,esp
 if(result!=expected||before!=after){++errors;printf("FAIL %s result=%lu expected=%lu delta=%ld\n",name,result,expected,(long)after-(long)before);}else printf("PASS %s result=%lu\n",name,result);
}
int __cdecl main(void){int caught=0;DWORD flags[] = {0,0x40000000u,0x80000000u,0xC0000000u};unsigned i;
 run("zero",(DWORD)zero,0,0,TRUE,0xAA);
 run("one",(DWORD)one,1,0,TRUE,2);
 for(i=0;i<4;i++){BOOL reverse=(flags[i]&0x40000000u)==0;run("three-stdcall",(DWORD)three_std,3,flags[i],reverse,14);run("three-cdecl",(DWORD)three_cdecl,3,flags[i],reverse,14);}
 run("32-cdecl",(DWORD)many,32,0xC0000000u,FALSE,129);
 __try{(void)invoke((DWORD)except_target,0,0,FALSE);}__except(GetExceptionCode()==0xE4730001?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH){caught=1;}
 if(!caught){++errors;puts("FAIL exception");}else puts("PASS exception propagated");
 printf("errors=%u\n",errors);return errors?1:0;
}
