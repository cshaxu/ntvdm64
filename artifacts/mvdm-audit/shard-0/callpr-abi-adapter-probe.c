#include <windows.h>
#include <stdio.h>
extern DWORD __stdcall WK32ICallProc32MakeCall(DWORD,DWORD,void*);
__declspec(noinline) DWORD __cdecl callee(DWORD a,DWORD b,DWORD c){return a+2*b+3*c;}
__declspec(naked) DWORD __stdcall thunk_abi(DWORD pfn,DWORD count,void *args) {
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
int __cdecl main(void){DWORD args[3]={1,2,3},observed=0,result=0;
 __asm {
  push ebx
  mov ebx, 12345678h
  lea eax,args
  push eax
  push 3
  push offset callee
  call thunk_abi
  mov result,eax
  mov observed,ebx
  pop ebx
 }
 printf("result=%lu ebx_after=%08lx\n",result,observed); return result==14 && observed==0x12345678u?0:1;
}
