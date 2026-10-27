#include <windows.h>
#include <stdio.h>
typedef DWORD (__stdcall *TARGET)(DWORD,DWORD,DWORD);
extern DWORD __stdcall WK32ICallProc32MakeCall(DWORD,DWORD,void*);
__declspec(noinline) DWORD __cdecl actual_cdecl(DWORD a,DWORD b,DWORD c) { return a+2*b+3*c; }
__declspec(noinline) static int compare(void) {
 DWORD before, after, result, helper_before,helper_after,helper_result;
 DWORD args[3]={1,2,3}; TARGET incorrectly_declared=(TARGET)actual_cdecl;
 __asm mov before,esp
 result=incorrectly_declared(1,2,3);
 __asm mov after,esp
 __asm mov esp,before
 __asm mov helper_before,esp
 helper_result=WK32ICallProc32MakeCall((DWORD)actual_cdecl,3,args);
 __asm mov helper_after,esp
 printf("portable_result=%lu portable_esp_delta=%ld original_result=%lu original_esp_delta=%ld\n",result,(long)after-(long)before,helper_result,(long)helper_after-(long)helper_before);
 return result==14 && helper_result==14 && (long)after-(long)before==-12 && helper_after==helper_before ? 0:1;
}
int main(void){return compare();}
