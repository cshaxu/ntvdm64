#include "mvdm_softpc_vdd_configuration.h"
#include <stdlib.h>

static char *shadow_contents;
static DWORD shadow_size;
static BOOL shadow_initialized;
#define MVDM_SHADOW_VDD_KEY ((HKEY)(ULONG_PTR)0x56444431u)

static int hex_digit(char character)
{
    if (character>='0' && character<='9') return character-'0';
    if (character>='a' && character<='f') return character-'a'+10;
    if (character>='A' && character<='F') return character-'A'+10;
    return -1;
}

static BOOL shadow_path(char *path, DWORD capacity)
{
    char *slash;
    DWORD length=GetModuleFileNameA(NULL,path,capacity);

    if (length==0 || length>=capacity) return FALSE;
    slash=strrchr(path,'\\');
    if (slash==NULL || (DWORD)(slash-path)+9u>=capacity) return FALSE;
    lstrcpyA(slash+1,"NTVDM.REG");
    return TRUE;
}

BOOL ntvdm_shadow_registry_initialize(VOID)
{
    char path[MAX_PATH];
    HANDLE handle;
    DWORD read;

    if (shadow_initialized) return TRUE;
    if (!shadow_path(path,sizeof(path))) return FALSE;
    handle=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (handle==INVALID_HANDLE_VALUE) {
        if (GetLastError()==ERROR_FILE_NOT_FOUND || GetLastError()==ERROR_PATH_NOT_FOUND) {
            shadow_initialized=TRUE;
            return TRUE;
        }
        return FALSE;
    }
    shadow_size=GetFileSize(handle,NULL);
    if (shadow_size==INVALID_FILE_SIZE || shadow_size>65535u) {
        CloseHandle(handle); SetLastError(ERROR_FILE_TOO_LARGE); return FALSE;
    }
    shadow_contents=(char *)malloc((SIZE_T)shadow_size+1u);
    if (!shadow_contents) { CloseHandle(handle); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    if (!ReadFile(handle,shadow_contents,shadow_size,&read,NULL) || read!=shadow_size) {
        free(shadow_contents); shadow_contents=NULL; CloseHandle(handle); return FALSE;
    }
    CloseHandle(handle);
    shadow_contents[shadow_size]='\0';
    shadow_initialized=TRUE;
    return TRUE;
}

VOID ntvdm_shadow_registry_shutdown(VOID)
{
    free(shadow_contents);
    shadow_contents=NULL;
    shadow_size=0;
    shadow_initialized=FALSE;
}

static char *shadow_value(PCSTR section, PCSTR value, PCSTR prefix)
{
    char section_header[512], value_header[512], *cursor, *section_end;

    if (!shadow_initialized || !shadow_contents || !section || !value ||
        lstrlenA(section)+3>=sizeof(section_header) ||
        lstrlenA(value)+lstrlenA(prefix)+4>=sizeof(value_header)) return NULL;
    wsprintfA(section_header,"[%s]",section);
    wsprintfA(value_header,"\"%s\"=%s",value,prefix);
    cursor=strstr(shadow_contents,section_header);
    if (!cursor) return NULL;
    cursor=strchr(cursor,'\n');
    if (!cursor) return NULL;
    ++cursor;
    section_end=strstr(cursor,"\n[");
    if (!section_end) section_end=shadow_contents+shadow_size;
    while (cursor && cursor<section_end) {
        if (!strncmp(cursor,value_header,lstrlenA(value_header)))
            return cursor+lstrlenA(value_header);
        cursor=strchr(cursor,'\n');
        if (cursor) ++cursor;
    }
    return NULL;
}

enum mvdm_softpc_vdd_configuration
mvdm_softpc_open_installable_vdd_registry(HKEY *key_out)
{
    char *value;

    if (key_out == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return MVDM_SOFTPC_VDD_CONFIGURATION_ERROR;
    }
    *key_out = NULL;
    if (!shadow_initialized) {
        SetLastError(ERROR_INVALID_STATE);
        return MVDM_SOFTPC_VDD_CONFIGURATION_ERROR;
    }
    if (!shadow_contents)
        return MVDM_SOFTPC_VDD_CONFIGURATION_NONE;
    if (!mvdm_softpc_read_installable_vdd_shadow(&value)) {
        if (GetLastError()==ERROR_FILE_NOT_FOUND) return MVDM_SOFTPC_VDD_CONFIGURATION_NONE;
        return MVDM_SOFTPC_VDD_CONFIGURATION_ERROR;
    }
    free(value);
    *key_out=MVDM_SHADOW_VDD_KEY;
    return MVDM_SOFTPC_VDD_CONFIGURATION_PRESENT;
}

LONG WINAPI mvdm_shadow_vdd_query_info_key(HKEY key, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time)
{
    char *value;
    DWORD length;

    (void)class_name; (void)class_length; (void)reserved;
    (void)maximum_subkey_length; (void)maximum_class_length;
    (void)security_descriptor; (void)last_write_time;
    if (key!=MVDM_SHADOW_VDD_KEY || !mvdm_softpc_read_installable_vdd_shadow(&value))
        return ERROR_FILE_NOT_FOUND;
    length=1;
    while (value[length-1] || value[length]) ++length;
    if (subkeys) *subkeys=0;
    if (values) *values=1;
    if (maximum_value_name_length) *maximum_value_name_length=3;
    if (maximum_value_data_length) *maximum_value_data_length=length+1;
    free(value);
    return ERROR_SUCCESS;
}

LONG WINAPI mvdm_shadow_vdd_query_value_ex(HKEY key, LPCTSTR value_name,
    LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    char *value;
    DWORD length;

    (void)reserved;
    if (key!=MVDM_SHADOW_VDD_KEY || !value_name || lstrcmpA(value_name,"VDD") ||
        !data_size || !mvdm_softpc_read_installable_vdd_shadow(&value)) return ERROR_FILE_NOT_FOUND;
    length=1;
    while (value[length-1] || value[length]) ++length;
    ++length;
    if (*data_size<length) { *data_size=length; free(value); return ERROR_MORE_DATA; }
    if (type) *type=REG_MULTI_SZ;
    if (data) memcpy(data,value,length);
    *data_size=length;
    free(value);
    return ERROR_SUCCESS;
}

LONG WINAPI mvdm_shadow_vdd_close_key(HKEY key)
{
    return key==MVDM_SHADOW_VDD_KEY ? ERROR_SUCCESS : ERROR_INVALID_HANDLE;
}

BOOL mvdm_softpc_read_installable_vdd_shadow(char **value_out)
{
    char *cursor, *end, *bytes;
    DWORD count=0;
    BOOL continued=FALSE;

    if (value_out==NULL || !shadow_initialized) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    *value_out=NULL;
    cursor=shadow_value("HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers","VDD","hex(7):");
    if (cursor==NULL) { SetLastError(ERROR_FILE_NOT_FOUND); return FALSE; }
    bytes=(char *)malloc(65536u);
    if (bytes==NULL) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    end=shadow_contents+shadow_size;
    while (cursor<end) {
        int high, low;
        if (*cursor=='\\') { continued=TRUE; ++cursor; continue; }
        if (*cursor=='\r' || *cursor=='\n') { if (!continued) break; continued=FALSE; ++cursor; continue; }
        if (*cursor==' ' || *cursor=='\t' || *cursor==',') { ++cursor; continue; }
        high=hex_digit(cursor[0]);
        low=hex_digit(cursor[1]);
        if (high<0 || low<0 || count>=65535u) { free(bytes); SetLastError(ERROR_BAD_FORMAT); return FALSE; }
        bytes[count++]=(char)(BYTE)((high<<4)|low);
        cursor+=2;
    }
    if (count<2u || bytes[count-1]!='\0' || bytes[count-2]!='\0') { free(bytes); SetLastError(ERROR_BAD_FORMAT); return FALSE; }
    *value_out=bytes;
    return TRUE;
}
