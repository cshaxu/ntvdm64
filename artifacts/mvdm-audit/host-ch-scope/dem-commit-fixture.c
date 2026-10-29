#include <windows.h>
#include <stdint.h>
#include <stdio.h>
static unsigned ax,bp,cf,calls,scenario; static uintptr_t observed;
unsigned getAX(void) { return ax; }
unsigned getBP(void) { return bp; }
void setCF(unsigned v) { cf=v; }
static BOOL WINAPI fake_flush(HANDLE h) { observed=(uintptr_t)h; calls++; if(scenario) { SetLastError(scenario==1?ERROR_ACCESS_DENIED:ERROR_INVALID_HANDLE); return FALSE; } return TRUE; }
void *hook_flush=(void*)fake_flush;
void demCommit(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s scenario=%u\n",__LINE__,#x,scenario); return 1; } } while(0)
int main(void) {
 for(scenario=0;scenario<3;scenario++) { ax=0x1234;bp=0xabcd;cf=99;calls=0;observed=0;demCommit();CHECK(calls==1 && observed==0x1234abcdu && cf==0); }
 puts("PASS commit handle reconstruction, success and two flush-failure cases (mocked host)"); return 0;
}
