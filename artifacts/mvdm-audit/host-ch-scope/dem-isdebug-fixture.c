#include <windows.h>
#include <stdint.h>
#include <stdio.h>
uintptr_t __security_cookie=0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x) { (void)x; }
int fShowSVCMsg;
static unsigned al, calls, scenario, seen_class, seen_length;
void setAL(unsigned v) { al=v&255; }
LONG WINAPI NtQueryInformationProcess(HANDLE proc, unsigned info, void *buffer, ULONG length, ULONG *returned) {
  (void)proc; (void)returned; calls++; seen_class=info; seen_length=length;
  *(HANDLE*)buffer = scenario==1 ? NULL : (HANDLE)(uintptr_t)42;
  return scenario==0 ? (LONG)0xc0000001u : 0;
}
void demIsDebug(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
  for(scenario=0;scenario<3;scenario++) for(fShowSVCMsg=0;fShowSVCMsg<2;fShowSVCMsg++) {
    al=255; calls=seen_class=seen_length=0; demIsDebug();
    CHECK(calls==1 && seen_class==7 && seen_length==sizeof(HANDLE));
    CHECK(al==(scenario==2 ? (fShowSVCMsg ? 3 : 1) : 0));
  }
  puts("PASS debug-port query failure/null/present and trace-flag gating (mocked native query)"); return 0;
}
