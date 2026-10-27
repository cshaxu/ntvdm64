#include <nt.h>
#include <base_classifier.h>
#include "nthook32-dll/hook.h"
#include "common/native_image.h"
static DWORD candidate;
BOOL WINAPI OpenNtBaseGetBinaryTypeW(LPCWSTR path,LPDWORD type) { (void)path; *type=candidate; return TRUE; }
DWORD common_native_process_image(HANDLE p,DWORD *m,DWORD *s) { (void)p;(void)m;(void)s;return ERROR_NOT_SUPPORTED; }
int main(void) {
 DWORD result=0; candidate=SCS_DOS_BINARY; if(nthook_legacy_type(L"C:\\a",&result) || result!=candidate)return 1;
 candidate=SCS_WOW_BINARY; if(nthook_legacy_type(L"C:\\a",&result) || result!=candidate)return 2;
 candidate=SCS_PIF_BINARY; if(nthook_legacy_type(L"C:\\a",&result) || result!=candidate)return 3;
 candidate=SCS_OS216_BINARY; if(nthook_legacy_type(L"C:\\a",&result)!=ERROR_NOT_SUPPORTED)return 4;
 candidate=SCS_32BIT_BINARY; if(nthook_legacy_type(L"C:\\a",&result)!=ERROR_NOT_SUPPORTED)return 5;
 return 0;
}
