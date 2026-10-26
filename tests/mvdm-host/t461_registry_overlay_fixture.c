/* S2 parent-façade contract.  The runner supplies a new, build-owned leaf in
 * T461_REGISTRY_FIXTURE_ROOT.  This fixture refuses pre-existing input and
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
    length=GetEnvironmentVariableA("T461_REGISTRY_FIXTURE_ROOT",path,capacity);
    if (!length || length>=capacity || length+11u>capacity) return ERROR_PATH_NOT_FOUND;
    if (path[length-1u]!='\\') { path[length++]='\\'; path[length]=0; }
    lstrcatA(path,"NTVDM.REG");
    return ERROR_SUCCESS;
}

#include "../../src/ntvdm-exe/softpc/mvdm_shadow_registry.c"

static int require(LONG actual, LONG expected, LPCSTR label)
{
    if (actual==expected) return 0;
    printf("FAIL %s got=%ld expected=%ld\n",label,actual,expected);
    return 1;
}

int main(VOID)
{
    HKEY root,child,leaf,stale; char value[32],name[32]; LONG size; DWORD count,written,type,data_size;
    HANDLE file; char path[MAX_PATH];
    static const char baseline[]="Windows Registry Editor Version 5.00\r\n\r\n"
        "[HKEY_CLASSES_ROOT\\Baseline]\r\n@=\"baseline\"\r\n";

    if (common_product_path_a(L"",path,sizeof(path))) return 2;
    /* CREATE_NEW is intentional: never replace an existing product overlay. */
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return 3;
    if (!WriteFile(file,baseline,sizeof(baseline)-1u,&written,NULL) || written!=sizeof(baseline)-1u) return 4;
    CloseHandle(file);
    if (!ntvdm_shadow_registry_initialize()) return 5;
    if (require(ntvdm_shadow_reg_create_key(HKEY_CLASSES_ROOT,"S2",&root),ERROR_SUCCESS,"create root")) return 6;
    if (require(ntvdm_shadow_reg_set_value(root,"",REG_SZ,"value",6),ERROR_SUCCESS,"set default")) return 7;
    size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value(root,"",value,&size),ERROR_SUCCESS,"query default") || size!=6 || strcmp(value,"value")) return 8;
    data_size=sizeof(value); count=sizeof(name);
    if (require(ntvdm_shadow_reg_enum_value(root,0,name,&count,NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"enum default") ||
        count!=0 || type!=REG_SZ || data_size!=6 || strcmp(value,"value")) return 81;
    if (require(ntvdm_shadow_reg_create_key(root,"Child",&child),ERROR_SUCCESS,"create child")) return 9;
    if (require(ntvdm_shadow_reg_delete_key(root,"Child"),ERROR_SUCCESS,"delete leaf")) return 10;
    if (require(ntvdm_shadow_reg_open_key(root,"Child",&leaf),ERROR_FILE_NOT_FOUND,"deleted leaf absent")) return 11;
    if (require(ntvdm_shadow_reg_create_key(root,"Nested",&child),ERROR_SUCCESS,"create nested")) return 12;
    if (require(ntvdm_shadow_reg_create_key(child,"Leaf",&leaf),ERROR_SUCCESS,"create nested leaf")) return 13;
    if (require(ntvdm_shadow_reg_delete_key(root,"Nested"),ERROR_ACCESS_DENIED,"nonleaf delete")) return 14;
    if (require(ntvdm_shadow_reg_close_key(leaf),ERROR_SUCCESS,"close leaf") || require(ntvdm_shadow_reg_close_key(child),ERROR_SUCCESS,"close nested")) return 15;
    count=0; while (ntvdm_shadow_reg_enum_key(root,count,name,sizeof(name))==ERROR_SUCCESS) ++count;
    if (!count) return 16;
    stale=root;
    if (require(ntvdm_shadow_reg_close_key(root),ERROR_SUCCESS,"close root")) return 17;
    size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value(stale,"",value,&size),ERROR_INVALID_HANDLE,"stale reject")) return 18;
    ntvdm_shadow_registry_shutdown();
    DeleteFileA(path);
    puts("T461_S2_REGISTRY_OVERLAY_OK");
    return 0;
}
