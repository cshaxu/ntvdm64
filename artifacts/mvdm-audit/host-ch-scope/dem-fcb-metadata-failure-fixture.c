#include <windows.h>
#include <stdio.h>
static int scenario, gft_calls,local_calls,dos_calls,size_calls,errors,closes; static DWORD captured_error;
static BOOL WINAPI fake_gft(HANDLE h,LPFILETIME c,LPFILETIME a,LPFILETIME w){(void)h;(void)c;(void)a;gft_calls++;w->dwLowDateTime=123;w->dwHighDateTime=0;return scenario!=1;}
static BOOL WINAPI fake_local(const FILETIME *a,LPFILETIME b){local_calls++;*b=*a;return scenario!=2;}
static BOOL WINAPI fake_dos(const FILETIME *f,LPWORD d,LPWORD t){(void)f;dos_calls++;*d=1;*t=2;return scenario!=3;}
static DWORD WINAPI fake_size(HANDLE h,LPDWORD high){(void)h;size_calls++;*high=scenario==5?1:0; if(scenario==4)SetLastError(ERROR_ACCESS_DENIED); return scenario==4||scenario==6?0xffffffffUL:123;}
static BOOL WINAPI fake_close(HANDLE h){(void)h;closes++;return TRUE;}
void *hook_gft=(void*)fake_gft,*hook_local=(void*)fake_local,*hook_dos=(void*)fake_dos,*hook_size=(void*)fake_size,*hook_close=(void*)fake_close;
void demClientError(HANDLE h,char c){(void)h;(void)c;captured_error=GetLastError();errors++;}
extern BOOL demGetMiscInfo(HANDLE,WORD*,WORD*,DWORD*);
int main(void){WORD t,d;DWORD size;int i;BOOL ok;for(i=0;i<7;i++){scenario=i;gft_calls=local_calls=dos_calls=size_calls=errors=closes=0;captured_error=0;t=d=0;size=0;SetLastError(0);ok=demGetMiscInfo((HANDLE)0x1234,&t,&d,&size);fprintf(stderr,"FCB case=%d result=%d gft=%d local=%d dos=%d size=%d errors=%d closes=%d error=%lu out=%08lx\n",i,ok,gft_calls,local_calls,dos_calls,size_calls,errors,closes,(unsigned long)captured_error,(unsigned long)size); if(gft_calls!=1)return 1;}return 0;}
