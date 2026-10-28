/* Selected DEM object, real process CWD and controlled guest/OEM/environment seams. */
#include <windows.h>
#include <stdio.h>
#include <string.h>
static char guest_path[MAX_PATH];static WORD cf;static unsigned calls,env_calls,errors;static DWORD last_error;static BOOL env_result;
unsigned short getDS(void){return 0;} unsigned short getDX(void){return 0;} void setCF(unsigned short v){cf=v;}
void *Sim32pGetVDMPointer(DWORD p,DWORD n,BOOL m){(void)p;(void)n;(void)m;return guest_path;}
BOOL WINAPI SetCurrentDirectoryOem(const char *p){++calls;return SetCurrentDirectoryA(p);}
BOOL WINAPI SetEnvironmentVariableOem(const char *n,const char *v){(void)n;(void)v;++env_calls;return env_result;}
void demClientError(HANDLE h,CHAR drive){(void)h;(void)drive;++errors;last_error=GetLastError();cf=1;}
extern void demSetCurrentDir(void);
int main(void){char temp[MAX_PATH],root[MAX_PATH],before[MAX_PATH],after[MAX_PATH];int rc=0;
 if(!GetCurrentDirectoryA(MAX_PATH,before)||!GetTempPathA(MAX_PATH,temp)||!GetTempFileNameA(temp,"cwd",0,root))return 2;
 if(!DeleteFileA(root)||!CreateDirectoryA(root,NULL))return 3;
 strcpy(guest_path,root);env_result=FALSE;cf=9;demSetCurrentDir();GetCurrentDirectoryA(MAX_PATH,after);
 printf("environment failure cf=%u cwd_changed=%d env_calls=%u errors=%u\n",cf,_stricmp(after,root)==0,env_calls,errors);
 if(cf!=1||_stricmp(after,root)||env_calls!=1||errors!=0){rc=4;goto done;}
 strcpy(guest_path,"?:\\missing");cf=9;demSetCurrentDir();printf("invalid drive cf=%u host_calls=%u errors=%u\n",cf,calls,errors);
 if(cf!=1||calls!=1||errors!=0){rc=5;goto done;}
 snprintf(guest_path,sizeof guest_path,"%s\\missing",root);cf=9;demSetCurrentDir();printf("missing cwd cf=%u host_calls=%u error=%lu errors=%u\n",cf,calls,last_error,errors);
 if(cf!=1||calls!=2||errors!=1||last_error!=ERROR_FILE_NOT_FOUND){rc=6;goto done;}
 done:SetCurrentDirectoryA(before);RemoveDirectoryA(root);return rc;
}
