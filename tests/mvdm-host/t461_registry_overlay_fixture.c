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

int main(int argc, char **argv)
{
    HKEY root,child,leaf,stale; char value[32],name[32]; LONG size; DWORD count,written,type,data_size,values;
    HANDLE file; char path[MAX_PATH], executable[MAX_PATH], command_line[MAX_PATH+32];
    PROCESS_INFORMATION first,second; STARTUPINFOA startup;
    static const char baseline[]="Windows Registry Editor Version 5.00\r\n\r\n"
        "[HKEY_CLASSES_ROOT\\Baseline]\r\n@=\"baseline\"\r\n\"Old\"=\"old\"\r\n";

    if (argc==3 && !strcmp(argv[1],"writer")) {
        if (!ntvdm_shadow_registry_initialize()) return 40;
        if (ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"S2",&root)!=ERROR_SUCCESS) return 41;
        if (ntvdm_shadow_reg_set_value_ex(root,argv[2],0,REG_SZ,(const BYTE *)argv[2],
            (DWORD)strlen(argv[2])+1u)!=ERROR_SUCCESS) return 42;
        ntvdm_shadow_reg_close_key(root); ntvdm_shadow_registry_shutdown(); return 0;
    }
    if (common_product_path_a(L"",path,sizeof(path))) return 2;
    /* CREATE_NEW is intentional: never replace an existing product overlay. */
    file=CreateFileA(path,GENERIC_WRITE,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
    if (file==INVALID_HANDLE_VALUE) return 3;
    if (!WriteFile(file,baseline,sizeof(baseline)-1u,&written,NULL) || written!=sizeof(baseline)-1u) return 4;
    CloseHandle(file);
    if (!ntvdm_shadow_registry_initialize()) return 5;
    if (require(ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"Baseline",&root),ERROR_SUCCESS,"open baseline")) return 52;
    if (require(ntvdm_shadow_reg_set_value_ex(root,"Old",0,REG_SZ,(const BYTE *)"new",4),ERROR_SUCCESS,"override baseline")) return 53;
    if (require(ntvdm_shadow_reg_set_value_ex(root,"Added",0,REG_SZ,(const BYTE *)"added",6),ERROR_SUCCESS,"add baseline value")) return 54;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"Old",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"query override") || strcmp(value,"new")) return 55;
    if (require(ntvdm_shadow_reg_query_info_key(root,NULL,NULL,NULL,NULL,NULL,NULL,&values,NULL,NULL,NULL,NULL),ERROR_SUCCESS,"merged query info") || values!=3) return 56;
    if (require(ntvdm_shadow_reg_close_key(root),ERROR_SUCCESS,"close baseline")) return 57;
    if (require(ntvdm_shadow_reg_create_key(HKEY_LOCAL_MACHINE,"Software\\Unadmitted",&root),ERROR_FILE_NOT_FOUND,"unadmitted machine reject")) return 51;
    if (require(ntvdm_shadow_reg_create_key(HKEY_CLASSES_ROOT,"S2",&root),ERROR_SUCCESS,"create root")) return 6;
    if (require(ntvdm_shadow_reg_set_value(root,"",REG_SZ,"value",6),ERROR_SUCCESS,"set default")) return 7;
    size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value(root,"",value,&size),ERROR_SUCCESS,"query default") || size!=6 || strcmp(value,"value")) return 8;
    data_size=sizeof(value); count=sizeof(name);
    if (require(ntvdm_shadow_reg_enum_value(root,0,name,&count,NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"enum default") ||
        count!=0 || type!=REG_SZ || data_size!=6 || strcmp(value,"value")) return 81;
    if (require(ntvdm_shadow_reg_set_value_ex(root,"Named",0,REG_SZ,(const BYTE *)"named",6),ERROR_SUCCESS,"set named")) return 82;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"Named",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"query named") ||
        type!=REG_SZ || data_size!=6 || strcmp(value,"named")) return 83;
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
    /* The serialized footer is part of the sole NTVDM.REG view: reopening
     * proves value replacement and explicit deletion survive a new parent. */
    ntvdm_shadow_registry_shutdown();
    if (!ntvdm_shadow_registry_initialize()) return 19;
    if (require(ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"Baseline",&root),ERROR_SUCCESS,"restart baseline")) return 20;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"Old",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"restart override") || strcmp(value,"new")) return 21;
    if (require(ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"S2",&child),ERROR_SUCCESS,"restart created")) return 22;
    if (require(ntvdm_shadow_reg_open_key(child,"Child",&leaf),ERROR_FILE_NOT_FOUND,"restart tombstone")) return 23;
    if (require(ntvdm_shadow_reg_close_key(child),ERROR_SUCCESS,"restart close created") ||
        require(ntvdm_shadow_reg_close_key(root),ERROR_SUCCESS,"restart close baseline")) return 24;

    /* Two separate parent processes start from the same file.  Both writes
     * must survive; the named commit mutex serializes read-modify-replace. */
    if (!GetModuleFileNameA(NULL,executable,sizeof(executable))) return 29;
    memset(&startup,0,sizeof(startup)); startup.cb=sizeof(startup);
    memset(&first,0,sizeof(first)); memset(&second,0,sizeof(second));
    wsprintfA(command_line,"\"%s\" writer CrossA",executable);
    if (!CreateProcessA(NULL,command_line,NULL,NULL,FALSE,0,NULL,NULL,&startup,&first)) return 30;
    wsprintfA(command_line,"\"%s\" writer CrossB",executable);
    if (!CreateProcessA(NULL,command_line,NULL,NULL,FALSE,0,NULL,NULL,&startup,&second)) return 31;
    WaitForSingleObject(first.hProcess,INFINITE); WaitForSingleObject(second.hProcess,INFINITE);
    GetExitCodeProcess(first.hProcess,&count); CloseHandle(first.hThread); CloseHandle(first.hProcess);
    if (count) { CloseHandle(second.hThread); CloseHandle(second.hProcess); return 32; }
    GetExitCodeProcess(second.hProcess,&count); CloseHandle(second.hThread); CloseHandle(second.hProcess);
    if (count) return 33;
    ntvdm_shadow_registry_shutdown();
    if (!ntvdm_shadow_registry_initialize()) return 34;
    if (require(ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"S2",&root),ERROR_SUCCESS,"concurrent reopen")) return 35;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"CrossA",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"concurrent CrossA") || strcmp(value,"CrossA")) return 36;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"CrossB",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"concurrent CrossB") || strcmp(value,"CrossB")) return 37;
    ntvdm_shadow_reg_close_key(root);

    /* An exclusive unrelated reader prevents replacement.  The failed write
     * must leave both the accepted in-memory view and the durable value intact. */
    /* Permit the transaction's read phase but deny replacement/delete sharing,
     * so this exercises atomic commit failure rather than a preflight read. */
    file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (file==INVALID_HANDLE_VALUE) return 25;
    if (require(ntvdm_shadow_reg_open_key(HKEY_CLASSES_ROOT,"Baseline",&root),ERROR_SUCCESS,"rollback baseline")) return 26;
    if (ntvdm_shadow_reg_set_value_ex(root,"Old",0,REG_SZ,(const BYTE *)"bad",4)==ERROR_SUCCESS) return 27;
    data_size=sizeof(value);
    if (require(ntvdm_shadow_reg_query_value_ex(root,"Old",NULL,&type,(BYTE *)value,&data_size),ERROR_SUCCESS,"rollback query") || strcmp(value,"new")) return 28;
    ntvdm_shadow_reg_close_key(root); CloseHandle(file);
    ntvdm_shadow_registry_shutdown();
    DeleteFileA(path);
    puts("T461_S3_REGISTRY_PERSISTENCE_OK");
    return 0;
}
