/* Controlled register/error seams, selected demLockOper object, real Win32 file locking. */
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
static WORD ax,bx,bp,cx,dx,si,di,cf;
static DWORD last_error; static unsigned errors;
unsigned short getAL(void){return (unsigned char)ax;}
unsigned short getBX(void){return bx;}
unsigned short getBP(void){return bp;}
unsigned short getCX(void){return cx;}
unsigned short getDX(void){return dx;}
unsigned short getSI(void){return si;}
unsigned short getDI(void){return di;}
void setCF(unsigned short v){cf=v;}
void demClientError(HANDLE h, CHAR drive){(void)h;(void)drive;last_error=GetLastError();errors++;cf=1;}
extern void demLockOper(void);
static void args(HANDLE h,WORD operation){uintptr_t n=(uintptr_t)h;ax=operation;bx=(WORD)(n>>16);bp=(WORD)n;cx=dx=si=0;di=1;cf=9;}
int main(void){CHAR dir[MAX_PATH],file[MAX_PATH];HANDLE a,b;int result=0;
 if(!GetTempPathA(MAX_PATH,dir)||!GetTempFileNameA(dir,"lck",0,file))return 2;
 a=CreateFileA(file,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY,NULL);
 b=CreateFileA(file,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_TEMPORARY,NULL);
 if(a==INVALID_HANDLE_VALUE||b==INVALID_HANDLE_VALUE){result=3;goto done;}
 args(a,0);demLockOper();if(cf!=0||errors!=0){result=4;goto done;}
 args(b,0);demLockOper();printf("competing lock cf=%u error=%lu calls=%u\n",cf,last_error,errors);if(cf!=1||last_error!=ERROR_LOCK_VIOLATION||errors!=1){result=5;goto done;}
 args(a,1);demLockOper();if(cf!=0||errors!=1){result=6;goto done;}
 args(b,0);demLockOper();if(cf!=0||errors!=1){result=7;goto done;}
 args(b,1);demLockOper();if(cf!=0||errors!=1){result=8;goto done;}
 args(INVALID_HANDLE_VALUE,0);demLockOper();printf("invalid handle cf=%u error=%lu calls=%u\n",cf,last_error,errors);if(cf!=1||last_error!=ERROR_INVALID_HANDLE||errors!=2)result=9;
 done: if(a!=INVALID_HANDLE_VALUE)CloseHandle(a);if(b!=INVALID_HANDLE_VALUE)CloseHandle(b);DeleteFileA(file);return result;
}
