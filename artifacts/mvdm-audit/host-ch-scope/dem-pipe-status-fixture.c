#include <windows.h>
#include <stdint.h>
#include <stdio.h>
uintptr_t __security_cookie=0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x) { (void)x; }
static unsigned ax,bp,cf,zf,data,eof,sizecalls; static uintptr_t seen;
unsigned getAX(void) { return ax; } unsigned getBP(void) { return bp; }
void setAX(unsigned v) { ax=v&65535; } void setBP(unsigned v) { bp=v&65535; }
void setCF(unsigned v) { cf=v&1; } void setZF(unsigned v) { zf=v&1; }
BOOL cmdPipeFileDataEOF(HANDLE h,BOOL *out) { seen=(uintptr_t)h; *out=eof; return data; }
BOOL cmdPipeFileEOF(HANDLE h) { seen=(uintptr_t)h; return eof; }
static DWORD WINAPI fake_size(HANDLE h,LPDWORD hi) { seen=(uintptr_t)h; sizecalls++; *hi=0; return 0x1234abcdu; }
void *hook_size=(void*)fake_size;
void demPipeFileDataEOF(void); void demPipeFileEOF(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s data=%u eof=%u\n",__LINE__,#x,data,eof); return 1; } } while(0)
int main(void) {
 for(eof=0;eof<2;eof++) for(data=0;data<2;data++) {
   ax=0x7654;bp=0x3210;cf=zf=9;sizecalls=0;seen=0;demPipeFileDataEOF();
   CHECK(seen==0x76543210u && cf==eof && zf==!data && sizecalls==eof);
   CHECK(ax==(eof?0x1234:0x7654) && bp==(eof?0xabcd:0x3210));
 }
 for(eof=0;eof<2;eof++) {
   ax=0x7654;bp=0x3210;cf=9;sizecalls=0;seen=0;demPipeFileEOF();
   CHECK(seen==0x76543210u && cf==eof && sizecalls==eof);
   CHECK(ax==(eof?0x1234:0x7654) && bp==(eof?0xabcd:0x3210));
 }
 puts("PASS pipe data/EOF flags and EOF size projection (mocked command/file boundaries)"); return 0;
}
