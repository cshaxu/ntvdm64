#include <windows.h>
#include <stdio.h>
static char path[16]="C:\\TMP";static unsigned char dl;static unsigned short ax,cf;static int calls,errors,force_failure;static char error_drive;
unsigned char getDL(void){return dl;}unsigned short getDS(void){return 0;}unsigned short getSI(void){return 0;}void setAX(unsigned short v){ax=v;}void setCF(unsigned short v){cf=v;}void *Sim32pGetVDMPointer(DWORD a,BOOL b){(void)a;(void)b;return path;}
BOOL WINAPI SetCurrentDirectoryOem(const char *s){(void)s;calls++;if(force_failure){SetLastError(ERROR_ACCESS_DENIED);return FALSE;}return TRUE;}
void demClientError(HANDLE h,char drive){(void)h;errors++;error_drive=drive;ax=(unsigned short)GetLastError();cf=1;}
void demSetDefaultDrive(void);
int main(void){int i;for(i=0;i<4;i++){path[0]= i==2 ? 'c' : 'C';dl=(i==1)?3:2;force_failure=(i==3);calls=errors=0;error_drive=0;ax=0xeeee;cf=0;SetLastError(0);demSetDefaultDrive();fprintf(stderr,"DRIVE case=%d path=%c dl=%u calls=%d errors=%d cf=%u ax=%04x drive=%u\n",i,path[0],dl,calls,errors,cf,ax,(unsigned char)error_drive);if(i==0 && (calls!=1||errors||cf))return 1;if((i==1||i==2)&&(calls||errors||cf!=1||ax!=1))return 2;if(i==3&&(calls!=1||errors!=1||cf!=1||ax!=5||error_drive!='C'))return 3;}return 0;}
