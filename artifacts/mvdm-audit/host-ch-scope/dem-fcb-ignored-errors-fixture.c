#include <windows.h>
#include <stdio.h>
static int scenario, errors,closes,opens; static WORD ax,bp,bx,cx,dx,si,cf;static char path[]="C:\\FCB.TMP";
static BOOL WINAPI gft(HANDLE h,LPFILETIME c,LPFILETIME a,LPFILETIME w){(void)h;(void)c;(void)a;w->dwLowDateTime=123;w->dwHighDateTime=0;return scenario!=1;}
static BOOL WINAPI localtime_mock(const FILETIME *a,LPFILETIME b){*b=*a;return scenario!=2;}
static BOOL WINAPI dostime(const FILETIME *f,LPWORD d,LPWORD t){(void)f;*d=1;*t=2;return TRUE;}
static DWORD WINAPI sz(HANDLE h,LPDWORD high){(void)h;*high=0;return 123;}
static BOOL WINAPI closeh(HANDLE h){(void)h;closes++;return TRUE;}
void *hook_gft=(void*)gft,*hook_local=(void*)localtime_mock,*hook_dos=(void*)dostime,*hook_size=(void*)sz,*hook_close=(void*)closeh;
HANDLE WINAPI CreateFileOem(LPCSTR p,DWORD a,DWORD sh,LPSECURITY_ATTRIBUTES sa,DWORD cd,DWORD at,HANDLE t){(void)p;(void)a;(void)sh;(void)sa;(void)cd;(void)at;(void)t;opens++;return (HANDLE)0x1234;}
DWORD WINAPI GetFileAttributesOem(LPCSTR p){(void)p;return 0;}
unsigned short demCreateLabel(char d,char *l){(void)d;(void)l;return 1;}
void* Sim32pGetVDMPointer(DWORD a,BOOL b){(void)a;(void)b;return path;}
unsigned char getAL(void){return 0;} unsigned short getDS(void){return 0;} unsigned short getSI(void){return 0;}
void setAX(WORD v){ax=v;}void setBP(WORD v){bp=v;}void setBX(WORD v){bx=v;}void setCX(WORD v){cx=v;}void setDX(WORD v){dx=v;}void setSI(WORD v){si=v;}void setDI(WORD v){(void)v;}void setCF(WORD v){cf=v;}
void demClientError(HANDLE h,char c){(void)h;(void)c;errors++;setAX((WORD)GetLastError());setCF(1);}
void demOpenFCB(void);void demGetFileInfo(void);
int main(void){int i;for(i=0;i<6;i++){scenario=i%3;opens=closes=errors=0;ax=bp=bx=cx=dx=si=0xeeee;cf=0;SetLastError(0);if(i<3)demOpenFCB();else demGetFileInfo();fprintf(stderr,"CALL case=%d open=%d close=%d err=%d cf=%u ax=%04x bp=%04x\n",i,opens,closes,errors,cf,ax,bp);if(opens!=1 || closes!=(i<3?0:1) || errors!=0 || cf!=0 || ((i<3) && (ax!=0 || bp!=0x1234)))return 1;}return 0;}
