#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
uintptr_t __security_cookie=0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x) { (void)x; }

static unsigned ax, cx, dx, calls;
static SYSTEMTIME captured;
static int outcome;
void setAL(unsigned v) { ax = (ax & 0xff00) | (v & 255); }
void setCX(unsigned v) { cx = v & 65535; }
void setCH(unsigned v) { cx = (cx & 255) | ((v & 255) << 8); }
void setCL(unsigned v) { cx = (cx & 0xff00) | (v & 255); }
void setDH(unsigned v) { dx = (dx & 255) | ((v & 255) << 8); }
void setDL(unsigned v) { dx = (dx & 0xff00) | (v & 255); }
unsigned getCX(void) { return cx; }
unsigned getCH(void) { return cx >> 8; }
unsigned getCL(void) { return cx & 255; }
unsigned getDH(void) { return dx >> 8; }
unsigned getDL(void) { return dx & 255; }
static void WINAPI fake_get(SYSTEMTIME *t) { *t = (SYSTEMTIME){2024,2,4,29,23,58,57,137}; }
static BOOL WINAPI fake_set(const SYSTEMTIME *t) { captured=*t; calls++; if(outcome==1) return TRUE; SetLastError(outcome==2 ? ERROR_PRIVILEGE_NOT_HELD : ERROR_INVALID_PARAMETER); return FALSE; }
void *hook_getlocal = (void*)fake_get;
void *hook_setlocal = (void*)fake_set;
void demQueryDate(void); void demQueryTime(void); void demSetDate(void); void demSetTime(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
  ax=cx=dx=0; demQueryDate(); CHECK(cx==2024 && dx==0x021d && (ax&255)==4);
  demQueryTime(); CHECK(cx==0x173a && dx==0x390d);
  cx=2000; dx=0x0c1f; outcome=1; calls=0; demSetDate();
  CHECK(calls==1 && (ax&255)==0 && captured.wYear==2000 && captured.wMonth==12 && captured.wDay==31 && captured.wHour==23 && captured.wMilliseconds==137);
  cx=0x0a0b; dx=0x0c0d; outcome=2; calls=0; demSetTime();
  CHECK(calls==1 && (ax&255)==0 && captured.wYear==2024 && captured.wHour==10 && captured.wMinute==11 && captured.wSecond==12 && captured.wMilliseconds==130);
  outcome=0; calls=0; demSetTime(); CHECK(calls==1 && (ax&255)==255);
  puts("PASS query date/time; set date/time preservation; success, privilege and error paths (mocked clock)"); return 0;
}
