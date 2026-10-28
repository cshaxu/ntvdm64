#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uintptr_t __security_cookie = 0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x) { (void)x; }
static unsigned char memory[18];
static unsigned cx, address, mode;
static const char *name;
static int succeed;
unsigned getDS(void) { return 0x1234; }
unsigned getDX(void) { return 0x5678; }
void setCX(unsigned x) { cx = x & 65535; }
void setCH(unsigned x) { cx = (cx & 255) | ((x & 255) << 8); }
unsigned char *Sim32pGetVDMPointer(unsigned a, unsigned char m) { address=a; mode=m; return memory+1; }
BOOL WINAPI GetComputerNameOem(char *out, DWORD *length) {
    size_t n = strlen(name);
    if (!succeed) return FALSE;
    if (*length <= n) return FALSE;
    memcpy(out,name,n+1); *length=(DWORD)n; return TRUE;
}
void demGetComputerName(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
    memset(memory,0xa5,sizeof(memory)); name="BOX"; succeed=1; cx=0; demGetComputerName();
    CHECK(address==0x12345678 && mode==0 && cx==0x1ff);
    CHECK(!memcmp(memory+1,"BOX            ",15) && memory[16]==0 && memory[0]==0xa5 && memory[17]==0xa5);
    memset(memory,0xa5,sizeof(memory)); name="ABCDEFGHIJKLMNO"; demGetComputerName();
    CHECK(!memcmp(memory+1,name,15) && memory[16]==0 && memory[0]==0xa5 && memory[17]==0xa5 && cx==0x1ff);
    memset(memory,0xa5,sizeof(memory)); succeed=0; cx=0xabcd; demGetComputerName();
    CHECK(memory[1]==0 && memory[2]==0xa5 && memory[17]==0xa5 && cx==0x00cd);
    puts("PASS OEM computer name padding, 15-byte boundary, failure and mapped destination (mocked host/guest memory)");
    return 0;
}
