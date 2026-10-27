#include <windows.h>
#include <stdio.h>
#include <stdint.h>
uintptr_t __security_cookie=0xdeadbeef;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x){(void)x;}
void __report_rangecheckfailure(void){ExitProcess(99);}
extern unsigned short demCreateLabel(unsigned char, char*);
static BYTE *guard;
static LONG CALLBACK on_exception(PEXCEPTION_POINTERS p){EXCEPTION_RECORD*r=p->ExceptionRecord;if(r->ExceptionCode==EXCEPTION_ACCESS_VIOLATION&&r->NumberParameters>=2&&r->ExceptionInformation[0]==0&&(BYTE*)r->ExceptionInformation[1]==guard&&(uintptr_t)p->ContextRecord->Eip-(uintptr_t)&demCreateLabel==0x40){puts("PASS bounded label: selected object read exactly one byte beyond NUL into guarded page, before volume call");fflush(stdout);ExitProcess(0);}return EXCEPTION_CONTINUE_SEARCH;}
int main(void){SYSTEM_INFO i;GetSystemInfo(&i);SIZE_T z=i.dwPageSize;BYTE*p=VirtualAlloc(0,2*z,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);DWORD old;if(!p||!VirtualProtect(p+z,z,PAGE_NOACCESS,&old))return 10;guard=p+z;char*s=(char*)(guard-5);s[0]='?';s[1]=':';s[2]='\\';s[3]='A';s[4]=0;SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);if(!AddVectoredExceptionHandler(1,on_exception))return 11;demCreateLabel((unsigned char)s[0],s+3);puts("FAIL: no guard read");return 1;}
