#include <windows.h>
#include <stdio.h>
extern DWORD __stdcall WK32ICallProc32MakeCall(DWORD, DWORD, void *);
static DWORD __cdecl add_c(DWORD a, DWORD b, DWORD c) { return a+2*b+3*c; }
static DWORD __stdcall add_s(DWORD a, DWORD b, DWORD c) { return a+2*b+3*c; }
static DWORD __cdecl zero(void) { return 73; }
static DWORD __cdecl nested(DWORD a, DWORD b, DWORD c) {
    DWORD args[3]={a,b,c};
    return WK32ICallProc32MakeCall((DWORD)add_c,3,args);
}
static DWORD __cdecl fail(void) { RaiseException(0xE4780002,0,0,NULL); return 0; }
static int check(DWORD target,DWORD count,DWORD *args,DWORD expected) {
    DWORD before,after,result,ebx_value;
    __asm {
        push ebx
        mov ebx,12345678h
        mov before,esp
        push args
        push count
        push target
        call WK32ICallProc32MakeCall
        mov result,eax
        mov after,esp
        mov ebx_value,ebx
        pop ebx
    }
    return result!=expected || before!=after || ebx_value!=0x12345678;
}
int main(void) {
    DWORD args[3]={1,2,3}; int errors=0,caught=0;
    errors+=check((DWORD)add_c,3,args,14);
    errors+=check((DWORD)add_s,3,args,14);
    errors+=check((DWORD)nested,3,args,14);
    errors+=check((DWORD)zero,0,NULL,73);
    __try { WK32ICallProc32MakeCall((DWORD)fail,0,NULL); }
    __except(GetExceptionCode()==0xE4780002?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH) { caught=1; }
    errors+=!caught;
    printf("helper checks: errors=%d exception=%d\n",errors,caught);
    return errors!=0;
}
