/* Exact selected source bodies; all Registry, spooler and file mutations
 * are isolated providers. This is not evidence of a deployed fax driver. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winspool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, failures, calls, fail_alloc, live, closes, queries;
static unsigned driver_calls, printer_calls, printer_closes;
static int open_fail, query_fail, empty_list, wrong_type, create_fail;
static int directory_fail, driver_mode, printer_mode;
static void *blocks[16];
static const char names[]="Zulu\0alpha\0Middle\0";
static void check(int yes,const char *name) {
    ++checks; if(!yes){++failures;printf("FAIL %s\n",name);}
}
static void *alloc_w(size_t n) {
    unsigned i; void *p;
    if(++calls==fail_alloc) return NULL;
    p=malloc(n); if(!p) abort();
    memset(p,0xa5,n);
    for(i=0;i<16;i++) if(!blocks[i]){blocks[i]=p;++live;return p;}
    abort();
}
static void release_w(void *p) {
    unsigned i;
    if(!p)return;
    for(i=0;i<16;i++)if(blocks[i]==p){free(p);blocks[i]=NULL;--live;return;}
    check(0,"foreign/double heap release");
}
static void reset(void) {
    unsigned i;
    for(i=0;i<16;i++)if(blocks[i])release_w(blocks[i]);
    calls=fail_alloc=closes=queries=0;
    driver_calls=printer_calls=printer_closes=0;
    open_fail=query_fail=empty_list=wrong_type=create_fail=0;
    directory_fail=driver_mode=printer_mode=0;
}
static LONG reg_open(HKEY a,LPCSTR b,DWORD c,REGSAM d,PHKEY out) {
    (void)a;(void)b;(void)c;(void)d;
    if(open_fail)return ERROR_FILE_NOT_FOUND;
    *out=(HKEY)7;return 0;
}
static LONG reg_query(HKEY a,LPCSTR b,LPDWORD c,LPDWORD type,LPBYTE data,LPDWORD bytes) {
    DWORD size=empty_list?2:sizeof(names);
    (void)a;(void)b;(void)c;
    if(++queries==(unsigned)query_fail)return ERROR_ACCESS_DENIED;
    *type=wrong_type?REG_SZ:REG_MULTI_SZ;
    if(data){if(empty_list)memset(data,0,2);else memcpy(data,names,size);}
    *bytes=size;return 0;
}
static LONG reg_close(HKEY key){check(key==(HKEY)7||key==(HKEY)8,"owned key close");++closes;return 0;}
static LONG reg_create(HKEY a,LPCSTR b,PHKEY out){(void)a;(void)b;if(create_fail)return 5;*out=(HKEY)8;return 0;}
static LONG reg_set(HKEY a,LPCSTR b,DWORD c,DWORD d,const BYTE *p,DWORD n){(void)a;(void)b;(void)c;(void)d;(void)p;(void)n;return 0;}
typedef struct { char *name; ULONG (*lpfn)(); } DLLENTRYPOINTS;
enum { WOW_ClosePrinter=8,WOW_GetPrinterDriverDirectory=11,WOW_AddPrinter,
       WOW_AddPrinterDriver,WOW_AddPortEx,WOW_SPOOLERAPI_COUNT };
static DLLENTRYPOINTS spoolerapis[WOW_SPOOLERAPI_COUNT];
static ULONG directory(LPSTR a,LPSTR b,DWORD level,LPSTR out,DWORD cap,LPDWORD needed){
    (void)a;(void)b;(void)level;(void)cap;*needed=4;strcpy(out,"X:\\");return !directory_fail;
}
static ULONG add_driver(LPSTR a,DWORD b,DRIVER_INFO_2 *info){
    (void)a;(void)b;check(info->cVersion==1&&info->pConfigFile!=NULL,"driver arguments");++driver_calls;
    SetLastError(driver_mode==1?ERROR_PRINTER_DRIVER_ALREADY_INSTALLED:ERROR_ACCESS_DENIED);
    return driver_mode==0;
}
static ULONG add_printer(LPSTR a,DWORD b,PRINTER_INFO_2 *info){
    (void)a;(void)b;check(!strcmp(info->pPortName,"NULL"),"original NULL port");++printer_calls;
    SetLastError(printer_mode==1?ERROR_PRINTER_ALREADY_EXISTS:ERROR_ACCESS_DENIED);
    return printer_mode==0?19:0;
}
static ULONG close_printer(HANDLE h){check(h==(HANDLE)19,"owned printer close");++printer_closes;return TRUE;}
static ULONG add_port(){return TRUE;}
static BOOL load_apis(char *a,DLLENTRYPOINTS *b,int n){(void)a;(void)b;(void)n;return FALSE;}
static BOOL copy_file(LPCSTR a,LPCSTR b,BOOL c){(void)a;(void)b;(void)c;return TRUE;}
static int load_string(HINSTANCE a,UINT b,LPSTR s,int n){(void)a;(void)b;(void)n;strcpy(s,"Local Port");return 10;}
static PSZ duplicate(PSZ s){size_t n=strlen(s)+1;PSZ p=alloc_w(n);if(p)memcpy(p,s,n);return p;}
static HANDLE hmodWOW32;
static PSZ pszSystemDirectory="X:\\fixture";
#define WOWFAX_DLL_NAME_A "WOWFAX.DLL"
#define WOWFAXUI_DLL_NAME_A "WOWFAXUI.DLL"
#define iszWowFaxLocalPort 1
#define LOGDEBUG(a,b) ((void)0)
#define malloc_w alloc_w
#define free_w release_w
#undef RegOpenKeyEx
#undef RegQueryValueEx
#undef RegCreateKey
#undef RegSetValueEx
#undef CopyFile
#undef LoadString
#define RegOpenKeyEx reg_open
#define RegQueryValueEx reg_query
#define RegCloseKey reg_close
#define RegCreateKey reg_create
#define RegSetValueEx reg_set
#define LoadLibraryAndGetProcAddresses load_apis
#define CopyFile copy_file
#define LoadString load_string
#define StrDup duplicate
/* Preserve exact source, including original assignment-in-condition idioms. */
#include "fax_exact.h"
static void list_case(int which) {
    UINT count=99; LPSTR *list=NULL; DWORD exception=0;
    reset();
    switch(which){case 1:open_fail=1;break;case 2:query_fail=1;break;
      case 3:wrong_type=1;break;case 4:fail_alloc=1;break;
      case 5:query_fail=2;break;case 6:empty_list=1;break;case 7:fail_alloc=2;break;}
    __try { list=GetSupportedFaxDrivers(&count); }
    __except(EXCEPTION_EXECUTE_HANDLER){exception=GetExceptionCode();}
    check(!exception,"list no access violation");
    if(which==0){
      check(count==3&&list,"three supported drivers");
      if(list&&count==3)check(!strcmp(list[0],"alpha")&&!strcmp(list[1],"Middle")&&!strcmp(list[2],"Zulu"),"original sorted names");
      check(live==2,"array retains its backing MULTI_SZ");
    }else{
      check(count==0,"empty/failure count zero");
      /* Old uninitialized pointer is observed but never dereferenced. */
      check(list==NULL,"empty/failure pointer null");
      check(live==0,"empty/failure heap retired");
    }
    check(closes==(which==1?0u:1u),"list key retired");
}
int main(void){
    int i,d,p;BOOL result;
    spoolerapis[WOW_GetPrinterDriverDirectory].lpfn=(ULONG(*)())directory;
    spoolerapis[WOW_AddPrinterDriver].lpfn=(ULONG(*)())add_driver;
    spoolerapis[WOW_AddPrinter].lpfn=(ULONG(*)())add_printer;
    spoolerapis[WOW_ClosePrinter].lpfn=(ULONG(*)())close_printer;
    spoolerapis[WOW_AddPortEx].lpfn=(ULONG(*)())add_port;
    for(i=0;i<8;i++)list_case(i);
    for(d=0;d<3;d++)for(p=0;p<3;p++){
      reset();driver_mode=d;printer_mode=p;
      result=InstallWowFaxPrinter("devices","Fax","driver,port");
      check(result==(d!=2&&p!=2),"installation result");
      check(driver_calls==1&&printer_calls==(d==2?0u:1u),"installation ordering");
      check(printer_closes==(d!=2&&p==0?1u:0u),"successful printer retired");
      check(live==0&&closes==2,"installer heap and keys retired");
    }
    for(i=0;i<4;i++){
      reset();open_fail=i==0;create_fail=i==1;directory_fail=i==2;
      if(i==3)spoolerapis[WOW_GetPrinterDriverDirectory].lpfn=NULL;
      check(!InstallWowFaxPrinter("devices","Fax","driver,port"),"early install failure");
      check(!driver_calls&&!printer_calls&&!live,"early failure no spool mutation");
      check(closes==(i==0?0u:i==1?1u:2u),"early failure keys retired");
    }
    reset();printf("WOW_FAX_FAILURE checks=%u failures=%u\n",checks,failures);
    return failures?1:0;
}
