/* Selected DEM object, controlled guest/register and OEM/error seams, real Win32 directory operations. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static WORD bx,si,ds,dx,cf; static char guest_path[MAX_PATH];
static DWORD last_error; static unsigned errors,creates,removes;
unsigned short getBX(void){return bx;} unsigned short getSI(void){return si;}
unsigned short getDS(void){return ds;} unsigned short getDX(void){return dx;}
void setCF(unsigned short v){cf=v;}
void *Sim32pGetVDMPointer(DWORD p, DWORD size, BOOL mode){(void)p;(void)size;(void)mode;return guest_path;}
BOOL WINAPI CreateDirectoryOem(const char *p, SECURITY_ATTRIBUTES *a){creates++;return CreateDirectoryA(p,a);}
BOOL WINAPI RemoveDirectoryOem(const char *p){removes++;return RemoveDirectoryA(p);}
void demClientError(HANDLE h, CHAR drive){(void)h;(void)drive;last_error=GetLastError();errors++;cf=1;}
extern void demCreateDir(void); extern void demDeleteDir(void);
int main(void){char tmp[MAX_PATH],root[MAX_PATH];int rc=0;
 if(!GetTempPathA(MAX_PATH,tmp)||!GetTempFileNameA(tmp,"dmc",0,root))return 2;
 if(!DeleteFileA(root)||!CreateDirectoryA(root,NULL))return 3;
 if(snprintf(guest_path,sizeof guest_path,"%s\\child",root)<0){rc=4;goto end;}
 bx=si=0;cf=9;demCreateDir();if(cf!=0||creates!=1||errors){rc=5;goto end;}
 cf=9;demCreateDir();printf("duplicate create cf=%u error=%lu errors=%u\n",cf,last_error,errors);
 if(cf!=1||last_error!=ERROR_ALREADY_EXISTS||errors!=1){rc=6;goto end;}
 cf=9;demDeleteDir();if(cf!=0||removes!=1||errors!=1){rc=7;goto end;}
 cf=9;demDeleteDir();printf("missing delete cf=%u error=%lu errors=%u\n",cf,last_error,errors);
 if(cf!=1||last_error!=ERROR_FILE_NOT_FOUND||errors!=2){rc=8;goto end;}
 bx=1;si=0;cf=9;demCreateDir();printf("extended-attributes cf=%u create_calls=%u errors=%u\n",cf,creates,errors);
 if(cf!=9||creates!=2||errors!=2){rc=9;goto end;}
 end:RemoveDirectoryA(guest_path);RemoveDirectoryA(root);return rc;
}
