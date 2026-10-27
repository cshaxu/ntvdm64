#include "precomp.h"
#include <stdio.h>
DWORD FASTCALL wow_original_WK32ICallProc32(PVDMFRAME);
DWORD __stdcall WK32ICallProc32MakeCall(DWORD,DWORD,void*);
PVOID FASTCALL GetPModeVDMPointerAssert(DWORD a) { return (PVOID)a; }
BOOL UpdateDosCurrentDirectory(UDCDFUNC direction) { (void)direction; return TRUE; }
PFNWOWHANDLERSOUT pfnOut;
__declspec(noinline) DWORD __cdecl probe_cdecl(DWORD a,DWORD b,DWORD c) { return a+2*b+3*c; }
__declspec(noinline) DWORD __stdcall probe_stdcall(DWORD a,DWORD b,DWORD c) { return a+2*b+3*c; }
int __cdecl main(int argc,char **argv) {
    union { DWORD alignment; BYTE bytes[sizeof(VDMFRAME)+sizeof(ICALLPROC32)+64]; } storage;
    PVDMFRAME frame=(PVDMFRAME)storage.bytes;
    PICALLPROC32 args=(PICALLPROC32)&frame->bArgs;
    DWORD result;
    ZeroMemory(&storage,sizeof(storage));
    args->cParams=3;
    args->lpProcAddress=argc>1 ? (DWORD)probe_cdecl : (DWORD)probe_stdcall;
    args->p1=1;args->p2=2;args->p3=3;
    if (argc>1 && strcmp(argv[1],"original")==0) { DWORD original_args[3]={3,2,1}; result=WK32ICallProc32MakeCall((DWORD)probe_cdecl,3,original_args); printf("original_helper result=%lu\n",result); return result==10 ? 0 : 1; }
    printf("before_thunk mode=%s\n", argc>1 ? "mismatch" : "normal");fflush(stdout);
    result=wow_original_WK32ICallProc32(frame);
    printf("after_thunk result=%lu\n",result);fflush(stdout);
    return result==10 ? 0 : 1;
}

LPVOID WINAPI WOWGetVDMPointer(DWORD vp,DWORD n,BOOL pm) { (void)vp;(void)n;(void)pm; ExitProcess(91); }
HMODULE WINAPI wow_extension_load_library_a(LPCSTR s,HANDLE h,DWORD d){(void)s;(void)h;(void)d;ExitProcess(92);}
BOOL WINAPI wow_extension_free_library(HMODULE h){(void)h;ExitProcess(93);}
FARPROC WINAPI wow_extension_get_proc_address(HMODULE h,LPCSTR s){(void)h;(void)s;ExitProcess(94);}
ULONG FASTCALL WK32Yield(PVDMFRAME f){(void)f;ExitProcess(95);}
VOID __cdecl BlockWOWIdle(BOOL b){(void)b;ExitProcess(96);}
