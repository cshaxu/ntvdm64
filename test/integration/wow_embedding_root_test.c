/* S2 parent-façade contract.  The runner supplies a new, build-owned leaf in
 * T478_EMBEDDING_ROOT.  This fixture refuses pre-existing input and
 * only removes the exact file it created. */
#include <windows.h>
#include <stdio.h>
#include <string.h>

/* The fixture exercises only the Win32 façade half.  Supply the small NT
 * declaration subset reached by the production source's native-reader
 * helpers, rather than pulling the full original SDK into this standalone
 * test binary. */
#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS ((NTSTATUS)0)
#endif
#ifndef STATUS_OBJECT_NAME_NOT_FOUND
#define STATUS_OBJECT_NAME_NOT_FOUND ((NTSTATUS)0xC0000034L)
#endif
#ifndef STATUS_BUFFER_TOO_SMALL
#define STATUS_BUFFER_TOO_SMALL ((NTSTATUS)0xC0000023L)
#endif
#ifndef STATUS_UNSUCCESSFUL
#define STATUS_UNSUCCESSFUL ((NTSTATUS)0xC0000001L)
#endif
#ifndef STATUS_INVALID_HANDLE
#define STATUS_INVALID_HANDLE ((NTSTATUS)0xC0000008L)
#endif
#ifndef STATUS_NO_MEMORY
#define STATUS_NO_MEMORY ((NTSTATUS)0xC0000017L)
#endif
#ifndef KeyValueFullInformation
#define KeyValueFullInformation 2u
typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG TitleIndex,Type,DataOffset,DataLength,NameLength;
    WCHAR Name[1];
} KEY_VALUE_FULL_INFORMATION,*PKEY_VALUE_FULL_INFORMATION;
#endif

DWORD common_product_path_a(LPCWSTR relative, LPSTR path, DWORD capacity)
{
    DWORD length;
    (void)relative;
    length=GetEnvironmentVariableA("T478_EMBEDDING_ROOT",path,capacity);
    if (!length || length>=capacity || length+11u>capacity) return ERROR_PATH_NOT_FOUND;
    if (path[length-1u]!='\\') { path[length++]='\\'; path[length]=0; }
    lstrcatA(path,"NTVDM.REG");
    return ERROR_SUCCESS;
}

#include "embedding_provider_path.h"
#include <setjmp.h>


static unsigned checks,failures,calls,updated;
static HKEY enum_override;
static jmp_buf budget;
static void check(int ok,const char *name){++checks;if(!ok){++failures;printf("FAIL %s\n",name);}}
static LONG enum_original(HKEY h,DWORD i,LPSTR n,DWORD c){if(++calls>16)longjmp(budget,1);return ntvdm_shadow_reg_enum_key(enum_override?enum_override:h,i,n,c);}
static VOID UpdateEmbeddingKey(LPSTR name){check(!strcmp(name,"Alpha")||!strcmp(name,"Beta"),"valid original callback key");++updated;}
#undef RegEnumKey
#define RegEnumKey enum_original
#undef RegCreateKey
#undef RegSetValue
#undef RegCloseKey
#define RegCreateKey ntvdm_shadow_reg_create_key
#define RegSetValue ntvdm_shadow_reg_set_value
#define RegCloseKey ntvdm_shadow_reg_close_key
static char szServerKey[]="protocol\\StdFileEditing\\server";
#include "embedding_exact.h"
static void enumeration(HKEY handle){calls=updated=0;enum_override=handle;if(!setjmp(budget)){UpdateEmbeddingAllKeys();check(calls==4&&updated==2,"original skip-too-long then end");}else check(0,"original enumeration terminates");}
int main(void){
 char path[MAX_PATH],data[1024],longname[271],name[32];DWORD written;HANDLE f;HKEY opened;LONG status;char profile[]="Description,Class,Server,Forms",out[32];LONG size;
 if(common_product_path_a(L"",path,sizeof(path)))return 2;
 memset(longname,'x',270);longname[270]=0;
 sprintf_s(data,sizeof(data),"Windows Registry Editor Version 5.00\r\n[HKEY_CLASSES_ROOT\\Alpha]\r\n[HKEY_CLASSES_ROOT\\%s]\r\n[HKEY_CLASSES_ROOT\\Beta]\r\n",longname);
 f=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);if(f==INVALID_HANDLE_VALUE)return 3;
 if(!WriteFile(f,data,(DWORD)strlen(data),&written,NULL)){CloseHandle(f);DeleteFileA(path);return 4;}CloseHandle(f);
 check(ntvdm_shadow_registry_initialize(),"initialize actual shadow provider");
 enumeration(NULL);
 check(ntvdm_shadow_reg_enum_key(HKEY_CLASSES_ROOT,0,name,2)==ERROR_MORE_DATA,"short name buffer unchanged error");
 check(ntvdm_shadow_reg_enum_key(HKEY_CLASSES_ROOT,0,name,0)==ERROR_INVALID_HANDLE,"zero name capacity rejected");
 check(ntvdm_shadow_reg_enum_key(HKEY_LOCAL_MACHINE,0,name,sizeof(name))==ERROR_INVALID_HANDLE,"no new HKLM root enumeration");
 status=ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"",&opened);check(status==ERROR_SUCCESS,"opened root control");
 if(!status){enumeration(opened);check(ntvdm_shadow_reg_close_key(opened)==ERROR_SUCCESS,"close root control");check(ntvdm_shadow_reg_enum_key(opened,0,name,sizeof(name))==ERROR_INVALID_HANDLE,"stale opened root rejected");}
 UpdateClassesRootSubKey("FromEmbedding",profile);
 check(profile[17]==0&&profile[24]==0,"original parser delimiter mutation retained");
 check(shadow_keys==NULL,"original parser closes both handles");
 memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"FromEmbedding",out,&size);
 check(status==ERROR_SUCCESS&&size==6&&!memcmp(out,"Class",6),"original class stored with terminator");
 memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"FromEmbedding\\protocol\\StdFileEditing\\server",out,&size);
 check(status==ERROR_SUCCESS&&size==7&&!memcmp(out,"Server",7),"original server stored with terminator");
 status=ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"FromEmbedding",&opened);check(status==ERROR_SUCCESS,"open legacy size control");
 if(!status){
  check(ntvdm_shadow_reg_set_value(opened,NULL,REG_SZ,"Zero",0)==ERROR_SUCCESS,"legacy ignored size write");
  memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(opened,NULL,out,&size);
  check(status==ERROR_SUCCESS&&size==5&&!memcmp(out,"Zero",5),"legacy size computed from string");
  check(ntvdm_shadow_reg_close_key(opened)==ERROR_SUCCESS,"legacy control handle retired");
 }
 check(ntvdm_shadow_reg_set_value(HKEY_CLASSES_ROOT,"Direct",REG_SZ,"Direct",999)==ERROR_SUCCESS,"legacy direct-root set");
 memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"Direct",out,&size);
 check(status==ERROR_SUCCESS&&size==7&&!memcmp(out,"Direct",7),"legacy direct-root query");
 check(ntvdm_shadow_reg_delete_key(HKEY_CLASSES_ROOT,"Direct")==ERROR_SUCCESS,"legacy direct-root delete");
 size=sizeof(out);check(ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"Direct",out,&size)==ERROR_FILE_NOT_FOUND,"deleted root child absent");
 check(ntvdm_shadow_reg_enum_key((HKEY)(ULONG_PTR)123,0,name,sizeof(name))==ERROR_INVALID_HANDLE,"foreign key rejected");
 ntvdm_shadow_registry_shutdown();check(ntvdm_shadow_reg_enum_key(HKEY_CLASSES_ROOT,0,name,sizeof(name))==ERROR_INVALID_HANDLE,"shutdown root rejected");check(ntvdm_shadow_registry_initialize(),"restart persisted provider");
 memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"FromEmbedding",out,&size);
 check(status==ERROR_SUCCESS&&size==5&&!memcmp(out,"Zero",5),"legacy string survives persistence");
 memset(out,0xcc,sizeof(out));size=sizeof(out);status=ntvdm_shadow_reg_query_value(HKEY_CLASSES_ROOT,"FromEmbedding\\protocol\\StdFileEditing\\server",out,&size);
 check(status==ERROR_SUCCESS&&size==7&&!memcmp(out,"Server",7),"original server survives persistence");
 ntvdm_shadow_registry_shutdown();check(DeleteFileA(path),"fixture file retired");
 printf("EMBEDDING_ROOT checks=%u failures=%u\n",checks,failures);return failures?1:0;
}
