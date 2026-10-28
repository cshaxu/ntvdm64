#include <windows.h>
#include <stdint.h>
#include <stdio.h>
static DWORD code; static ULONG_PTR fault; static ULONG_PTR kind;
static int filt(EXCEPTION_POINTERS *p){code=p->ExceptionRecord->ExceptionCode;kind=p->ExceptionRecord->NumberParameters>0?p->ExceptionRecord->ExceptionInformation[0]:99;fault=p->ExceptionRecord->NumberParameters>1?p->ExceptionRecord->ExceptionInformation[1]:0;return EXCEPTION_EXECUTE_HANDLER;}
int main(void){SYSTEM_INFO si;DWORD old;uint8_t *p;uint32_t *base;unsigned char carry=0;int fail=0;GetSystemInfo(&si);p=(uint8_t*)VirtualAlloc(NULL,si.dwPageSize*3,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);if(!p)return 2;base=(uint32_t*)(p+si.dwPageSize);base[-1]=0x80000000; *base=1;
 __asm {
 mov edi,base
 mov ecx,-1
 btc dword ptr [edi],ecx
 setc carry
 }
 printf("negative carry=%u near=%08lx base=%08lx\n",carry,(unsigned long)base[-1],(unsigned long)*base);if(carry!=1||base[-1]!=0||*base!=1)fail=1;
 base[-1]=0x80000000;if(!VirtualProtect(p,si.dwPageSize,PAGE_NOACCESS,&old))return 3;code=0;fault=0;
 __try { __asm {
 mov edi,base
 mov ecx,-1
 btc dword ptr [edi],ecx
 } } __except(filt(GetExceptionInformation())) {}
 printf("negative_guard code=%08lx access=%lu delta=%ld\n",(unsigned long)code,(unsigned long)kind,(long)(fault-(ULONG_PTR)base));if(code!=0xc0000005 || kind!=1 || fault!=(ULONG_PTR)(base-1))fail=1;
 if(!VirtualProtect(p,si.dwPageSize,PAGE_READWRITE,&old))return 4;if(base[-1]!=0x80000000||*base!=1)fail=1;
 if(!VirtualProtect(base,si.dwPageSize,PAGE_READONLY,&old))return 5;code=0;fault=0;carry=0;
 __try { __asm {
 mov edi,base
 xor ecx,ecx
 bt dword ptr [edi],ecx
 setc carry
 } } __except(filt(GetExceptionInformation())) {}
 printf("readonly_bt code=%08lx carry=%u value=%08lx\n",(unsigned long)code,carry,(unsigned long)*base);if(code!=0||carry!=1||*base!=1)fail=1;
 code=0;fault=0;__try { __asm {
 mov edi,base
 xor ecx,ecx
 btc dword ptr [edi],ecx
 } } __except(filt(GetExceptionInformation())) {}
 printf("readonly_btc code=%08lx access=%lu delta=%ld value=%08lx\n",(unsigned long)code,(unsigned long)kind,(long)(fault-(ULONG_PTR)base),(unsigned long)*base);if(code!=0xc0000005||kind!=1||fault!=(ULONG_PTR)base||*base!=1)fail=1;
 VirtualFree(p,0,MEM_RELEASE);return fail;}
