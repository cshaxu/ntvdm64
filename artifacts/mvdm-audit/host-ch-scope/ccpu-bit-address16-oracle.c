#include <windows.h>
#include <stdio.h>
static ULONG_PTR where;
static DWORD code;
static int filter(EXCEPTION_POINTERS *p) { code=p->ExceptionRecord->ExceptionCode; where=p->ExceptionRecord->NumberParameters>1?p->ExceptionRecord->ExceptionInformation[1]:0; return EXCEPTION_EXECUTE_HANDLER; }
int main(void){ int i; for(i=0;i<4;i++){ code=0;where=0; __try { if(i==0) { __asm {
 xor ebx,ebx
 mov cx,-1
 _emit 0x67
 _emit 0x66
 _emit 0x0f
 _emit 0xa3
 _emit 0x0f
 } } else if(i==1) { __asm {
 mov bx,0xfffe
 mov cx,16
 _emit 0x67
 _emit 0x66
 _emit 0x0f
 _emit 0xa3
 _emit 0x0f
 } } else if(i==2) { __asm {
 xor ebx,ebx
 mov ecx,-1
 _emit 0x67
 _emit 0x0f
 _emit 0xa3
 _emit 0x0f
 } } else { __asm {
 mov bx,0xfffc
 mov ecx,32
 _emit 0x67
 _emit 0x0f
 _emit 0xa3
 _emit 0x0f
 } } } __except(filter(GetExceptionInformation())) {} printf("case=%d exception=%08lx address=%08lx\n",i,(unsigned long)code,(unsigned long)where); if(code!=0xc0000005 || where!=(i==0?0xfffeUL:i==2?0xfffcUL:0UL))return 1; }return 0; }
