#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
extern WORD __stdcall GetMMedia16(DWORD,int);
extern DWORD __stdcall GetMMedia32(WORD);
extern void __stdcall FreeMMedia16(WORD);
CRITICAL_SECTION mmHandleCriticalSection;
static unsigned char fake_teb[0x38],fake_ptd[0x40];
void *__stdcall opennt_support_current_teb(void) { return fake_teb; }
void *__stdcall malloc_w(unsigned size) { return HeapAlloc(GetProcessHeap(),0,size); }
int main(void) {
 WORD a,b,c; DWORD before,after,reused;
 *(void **)(fake_teb+0x34)=fake_ptd; *(WORD *)(fake_ptd+0x3c)=0x1234;
 InitializeCriticalSection(&mmHandleCriticalSection);
 a=GetMMedia16(0x12345000,1); b=GetMMedia16(0x12345000,1);
 before=GetMMedia32(a); FreeMMedia16(a); after=GetMMedia32(a);
 c=GetMMedia16(0x12346000,1); reused=GetMMedia32(c);
 printf("a=%04x b=%04x before=%08lx after=%08lx c=%04x new=%08lx\n",a,b,before,after,c,reused);
 DeleteCriticalSection(&mmHandleCriticalSection);
 return a && a==b && before==0x12345000 && after==0 && c==a && reused==0x12346000 ? 0:2;
}
