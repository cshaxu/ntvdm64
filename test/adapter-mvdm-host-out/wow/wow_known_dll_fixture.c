/* Exact original KnownDLL initializer;controlled registry and filesystem boundaries. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef char *PSZ;
#define MAX_KNOWN_DLLS 32
#define LOGDEBUG(a,b) ((void)0)
#define malloc_w_or_die malloc
static char *apszKnownDLL[MAX_KNOWN_DLLS],*pszControlExeWinDirPath,*pszProgmanExeWinDirPath,*pszControlExeSysDirPath,*pszProgmanExeSysDirPath;
static const char *pszWindowsDirectory="O:\\W31",*pszSystemDirectory="O:\\WINNT\\SYSTEM32";
static const char szBackslashControlExe[]="\\control.exe",szBackslashProgmanExe[]="\\progman.exe";
static unsigned queries,checks,failures,mode;
#define CHECK(x) do {++checks;if(!(x)){++failures;printf("FAIL line%d %s\n",__LINE__,#x);}}while(0)
static LONG query(HKEY h,LPCSTR name,LPDWORD reserved,LPDWORD type,LPBYTE data,LPDWORD size) {
 const char value[]="FOO.DLL BAR.DLL";(void)name;(void)reserved;++queries;
 if(!h)return ERROR_INVALID_HANDLE;
 *type=mode==2?REG_DWORD:REG_SZ;CHECK(*size>=sizeof(value));memcpy(data,value,sizeof(value));*size=sizeof(value);return 0;
}
#define RegQueryValueEx query
static DWORD GetFileAttributesOem(LPCSTR p){(void)p;return 0xffffffff;}
static BOOL SetFileAttributesOem(LPCSTR p,DWORD a){(void)p;(void)a;CHECK(0);return FALSE;}
#define WOW32_strchr strchr
#define WOW32_strlwr _strlwr
#include THUNK_SOURCE
int main(void){
 for(mode=0;mode<3;++mode){
  queries=0;WK32InitWowIsKnownDLL(mode?(HANDLE)1:NULL);
  CHECK(queries==1u);
  CHECK(!strcmp(pszControlExeWinDirPath,"O:\\W31\\control.exe"));
  CHECK(!strcmp(pszProgmanExeWinDirPath,"O:\\W31\\progman.exe"));
  CHECK(!strcmp(pszControlExeSysDirPath,"O:\\WINNT\\SYSTEM32\\control.exe"));
  CHECK(!strcmp(pszProgmanExeSysDirPath,"O:\\WINNT\\SYSTEM32\\progman.exe"));
  if(mode==1){CHECK(apszKnownDLL[0]&&!strcmp(apszKnownDLL[0],"foo.dll"));CHECK(apszKnownDLL[1]&&!strcmp(apszKnownDLL[1],"bar.dll"));free(apszKnownDLL[0]);}
  else CHECK(apszKnownDLL[0]==NULL);
  memset(apszKnownDLL,0,sizeof(apszKnownDLL));free(pszControlExeWinDirPath);free(pszProgmanExeWinDirPath);free(pszControlExeSysDirPath);free(pszProgmanExeSysDirPath);
 }
 printf("KNOWN_DLL checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
