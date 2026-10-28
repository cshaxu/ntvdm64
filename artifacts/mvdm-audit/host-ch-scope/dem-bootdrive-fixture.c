#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
uintptr_t __security_cookie=0xbb40e64e;
void __attribute__((fastcall)) __security_check_cookie(uintptr_t x) { (void)x; }
static unsigned al, opens, queries, closes, drive_calls, scenario;
void setAL(unsigned v) { al=v & 255; }
LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY root,LPCSTR sub,DWORD opt,REGSAM sam,PHKEY out) {
  (void)root;(void)sub;(void)opt;(void)sam; opens++; if (scenario==0) return ERROR_FILE_NOT_FOUND; *out=(HKEY)(uintptr_t)42; return ERROR_SUCCESS;
}
LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY key,LPCSTR value,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD length) {
  (void)key;(void)value;(void)reserved; queries++; if(scenario==1) return ERROR_FILE_NOT_FOUND;
  *type=REG_SZ; if(*length<4) return ERROR_MORE_DATA; memcpy(data,scenario==4 ? "e:\\" : "D:\\",4); *length=4; return ERROR_SUCCESS;
}
LONG WINAPI ntvdm_shadow_reg_close_key(HKEY key) { (void)key; closes++; return ERROR_SUCCESS; }
static UINT WINAPI fake_type(LPCSTR root) { drive_calls++; if(root[1]!=':' || root[2]!='\\') return DRIVE_UNKNOWN; return scenario==2 ? DRIVE_REMOTE : DRIVE_FIXED; }
void *hook_type = (void*)fake_type;
void demGetBootDrive(void);
#define CHECK(x) do { if(!(x)) { printf("FAIL line %d: %s\n",__LINE__,#x); return 1; } } while(0)
int main(void) {
  for(scenario=0;scenario<5;scenario++) {
    opens=queries=closes=drive_calls=0; al=255; demGetBootDrive();
    CHECK(opens==1 && queries==(scenario!=0) && closes==(scenario!=0));
    CHECK(drive_calls==(scenario>=2)); CHECK(al==(scenario<3 ? 3 : scenario==3 ? 4 : 5));
  }
  puts("PASS boot-drive fallback on open/query/nonfixed and fixed uppercase/lowercase cases (mocked registry/drive)"); return 0;
}
