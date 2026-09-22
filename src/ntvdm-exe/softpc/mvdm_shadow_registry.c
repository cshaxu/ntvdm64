#include "mvdm_shadow_registry.h"
#include <stdlib.h>
#include <string.h>

#define MVDM_SHADOW_KEY_MAGIC 0x53485247u
#define MVDM_SHADOW_KEY_FILE 0u
#define MVDM_SHADOW_KEY_SERIALCOMM 1u
#define MVDM_SERIALCOMM_SUBKEY "HARDWARE\\DEVICEMAP\\SERIALCOMM"
#ifndef STATUS_NO_MORE_ENTRIES
#define STATUS_NO_MORE_ENTRIES ((NTSTATUS)0x8000001AL)
#endif
typedef struct _MVDM_SHADOW_KEY {
    DWORD magic;
    struct _MVDM_SHADOW_KEY *next;
    DWORD source;
    DWORD serial_ports;
    struct _MVDM_SHADOW_VALUE *host_values;
    char section[512];
} MVDM_SHADOW_KEY;

typedef struct _MVDM_SHADOW_VALUE {
    struct _MVDM_SHADOW_VALUE *next;
    char *name;
    DWORD type;
    BYTE *data;
    DWORD data_size;
} MVDM_SHADOW_VALUE;

static char *shadow_contents;
static DWORD shadow_size;
static BOOL shadow_initialized;
static char shadow_trace_path[MAX_PATH];
static MVDM_SHADOW_KEY *shadow_keys;

static VOID free_host_values(MVDM_SHADOW_VALUE *value);

static VOID shadow_trace(LPCSTR event)
{
    HANDLE handle;
    DWORD written;

    if (!shadow_trace_path[0]) return;
    handle=CreateFileA(shadow_trace_path,FILE_APPEND_DATA,FILE_SHARE_READ,
        NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    if (handle==INVALID_HANDLE_VALUE) return;
    WriteFile(handle,event,lstrlenA(event),&written,NULL);
    WriteFile(handle,"\r\n",2,&written,NULL);
    CloseHandle(handle);
}

static VOID shadow_trace_section(LPCSTR event, LPCSTR section)
{
    char message[544];

    if (!shadow_trace_path[0]) return;
    lstrcpyA(message,event);
    lstrcatA(message,":");
    lstrcatA(message,section);
    shadow_trace(message);
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
    GetEnvironmentVariableA("NTVDM_SHADOW_REGISTRY_TRACE",shadow_trace_path,
        sizeof(shadow_trace_path));
    if (!shadow_path(path,sizeof(path))) return FALSE;
    handle=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (handle==INVALID_HANDLE_VALUE) {
        if (GetLastError()==ERROR_FILE_NOT_FOUND || GetLastError()==ERROR_PATH_NOT_FOUND) {
            shadow_initialized=TRUE;
            shadow_trace("LOAD_ABSENT");
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
    shadow_trace("LOAD_OK");
    return TRUE;
}

VOID ntvdm_shadow_registry_shutdown(VOID)
{
    while (shadow_keys) {
        MVDM_SHADOW_KEY *key=shadow_keys;
        shadow_keys=key->next;
        free_host_values(key->host_values);
        free(key);
    }
    free(shadow_contents);
    shadow_contents=NULL;
    shadow_size=0;
    shadow_initialized=FALSE;
    shadow_trace_path[0]='\0';
}

static const char *line_end(const char *line)
{
    const char *end=shadow_contents+shadow_size;
    while (line<end && *line!='\r' && *line!='\n') ++line;
    return line;
}

static const char *next_line(const char *line)
{
    const char *end=line_end(line);
    while (end<shadow_contents+shadow_size && (*end=='\r' || *end=='\n')) ++end;
    return end;
}

static const char *find_section(const char *section)
{
    const char *line=shadow_contents;
    size_t length;

    if (!shadow_contents || !section) return NULL;
    length=strlen(section);
    while (line<shadow_contents+shadow_size) {
        const char *end=line_end(line);
        if (line[0]=='[' && (size_t)(end-line)==length+2u &&
            _strnicmp(line+1,section,length)==0 && line[length+1]==']')
            return next_line(line);
        line=next_line(line);
    }
    return NULL;
}

static BOOL value_line(const char *line, const char **name, DWORD *name_length,
    const char **data, const char **data_end)
{
    const char *end=line_end(line), *cursor=line, *equals;

    while (cursor<end && (*cursor==' ' || *cursor=='\t')) ++cursor;
    if (cursor==end || *cursor==';' || *cursor!='"') return FALSE;
    ++cursor;
    *name=cursor;
    while (cursor<end && *cursor!='"') ++cursor;
    if (cursor==end) return FALSE;
    *name_length=(DWORD)(cursor-*name);
    ++cursor;
    equals=cursor;
    while (equals<end && (*equals==' ' || *equals=='\t')) ++equals;
    if (equals==end || *equals!='=') return FALSE;
    ++equals;
    while (equals<end && (*equals==' ' || *equals=='\t')) ++equals;
    *data=equals;
    *data_end=end;
    return TRUE;
}

static BOOL find_value(const MVDM_SHADOW_KEY *key, LPCSTR wanted,
    const char **data, const char **data_end)
{
    const char *line=find_section(key->section);
    size_t wanted_length;

    if (!line || !wanted) return FALSE;
    wanted_length=strlen(wanted);
    while (line<shadow_contents+shadow_size && line[0]!='[') {
        const char *name, *value, *end;
        DWORD name_length;
        if (value_line(line,&name,&name_length,&value,&end) &&
            name_length==wanted_length && _strnicmp(name,wanted,name_length)==0) {
            *data=value; *data_end=end; return TRUE;
        }
        line=next_line(line);
    }
    return FALSE;
}

/* Only original roots selected for this worker composition may observe host
 * configuration. This is deliberately a root allow-list, not a generic HKLM
 * facade which could turn later, unreviewed code into a host-registry reader. */
static BOOL admitted_host_section(LPCSTR section)
{
    static const char * const exact[] = {
        "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\VirtualDeviceDrivers",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WOW\\Compatibility",
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Setup",
        "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybCodes",
        "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\Keyboard Layout\\DosKeybIDs",
        NULL
    };
    static const char wow_prefix[]=
        "HKEY_LOCAL_MACHINE\\SYSTEM\\CurrentControlSet\\Control\\WOW";
    DWORD index;

    if (!section) return FALSE;
    for (index=0; exact[index]; ++index)
        if (!_stricmp(section,exact[index])) return TRUE;
    return !_strnicmp(section,wow_prefix,sizeof(wow_prefix)-1u) &&
        (section[sizeof(wow_prefix)-1u]=='\0' ||
         section[sizeof(wow_prefix)-1u]=='\\');
}

static VOID free_host_values(MVDM_SHADOW_VALUE *value)
{
    while (value) {
        MVDM_SHADOW_VALUE *next=value->next;
        free(value->name);
        free(value->data);
        free(value);
        value=next;
    }
}

static MVDM_SHADOW_VALUE *host_value(const MVDM_SHADOW_KEY *key, LPCSTR name)
{
    MVDM_SHADOW_VALUE *value;

    if (!key || !name) return NULL;
    for (value=key->host_values; value; value=value->next)
        if (!_stricmp(value->name,name)) return value;
    return NULL;
}

static LONG snapshot_host_values(MVDM_SHADOW_KEY *key)
{
    HKEY host_key;
    DWORD subkeys=0, values=0, maximum_name=0, maximum_data=0, index;
    char *name=NULL;
    BYTE *data=NULL;
    LONG status;

    if (!key || !admitted_host_section(key->section)) return ERROR_FILE_NOT_FOUND;
    status=RegOpenKeyExA(HKEY_LOCAL_MACHINE,key->section+19u,0,KEY_READ,&host_key);
    if (status!=ERROR_SUCCESS) return status;
    status=RegQueryInfoKeyA(host_key,NULL,NULL,NULL,&subkeys,NULL,NULL,&values,
        &maximum_name,&maximum_data,NULL,NULL);
    if (status!=ERROR_SUCCESS) { RegCloseKey(host_key); return status; }
    name=(char *)malloc((SIZE_T)maximum_name+2u);
    data=(BYTE *)malloc((SIZE_T)maximum_data+2u);
    if (!name || !data) {
        free(name); free(data); RegCloseKey(host_key); return ERROR_NOT_ENOUGH_MEMORY;
    }
    for (index=0; index<values; ++index) {
        DWORD name_size=maximum_name+1u, data_size=maximum_data+1u, type=0;
        MVDM_SHADOW_VALUE *value;

        status=RegEnumValueA(host_key,index,name,&name_size,NULL,&type,data,&data_size);
        if (status==ERROR_NO_MORE_ITEMS) break;
        if (status!=ERROR_SUCCESS) { free_host_values(key->host_values); key->host_values=NULL;
            free(name); free(data); RegCloseKey(host_key); return status; }
        value=(MVDM_SHADOW_VALUE *)calloc(1,sizeof(*value));
        if (!value) { free_host_values(key->host_values); key->host_values=NULL;
            free(name); free(data); RegCloseKey(host_key); return ERROR_NOT_ENOUGH_MEMORY; }
        value->name=(char *)malloc((SIZE_T)name_size+1u);
        value->data=(BYTE *)malloc((SIZE_T)data_size+1u);
        if (!value->name || !value->data) { free(value->name); free(value->data); free(value);
            free_host_values(key->host_values); key->host_values=NULL;
            free(name); free(data); RegCloseKey(host_key); return ERROR_NOT_ENOUGH_MEMORY; }
        memcpy(value->name,name,name_size); value->name[name_size]='\0';
        memcpy(value->data,data,data_size); value->data[data_size]=0;
        value->type=type; value->data_size=data_size;
        value->next=key->host_values; key->host_values=value;
    }
    free(name); free(data); RegCloseKey(host_key);
    shadow_trace_section("HOST_SNAPSHOT_OK",key->section);
    return ERROR_SUCCESS;
}

/* VCD asks for the historical SERIALCOMM value list.  Modern Windows still
 * exposes the corresponding public DOS-device namespace, whereas the
 * Registry itself is expressly outside this product's host boundary. */
static DWORD serial_port_mask(VOID)
{
    char *names;
    const char *name;
    DWORD capacity=32768,length=0,mask=0;

    names=(char *)malloc(capacity);
    if (!names) return 0;
    for (;;) {
        length=QueryDosDeviceA(NULL,names,capacity);
        if (length) break;
        if (GetLastError()!=ERROR_INSUFFICIENT_BUFFER || capacity>=1048576u) {
            free(names); return 0;
        }
        capacity*=2u;
        free(names);
        names=(char *)malloc(capacity);
        if (!names) return 0;
    }
    for (name=names; *name; name+=lstrlenA(name)+1) {
        const char *digits;
        DWORD number=0;

        if (_strnicmp(name,"COM",3)!=0) continue;
        digits=name+3;
        if (!*digits) continue;
        while (*digits>='0' && *digits<='9') {
            number=number*10u+(DWORD)(*digits-'0');
            ++digits;
        }
        if (*digits==0 && number>=1u && number<=9u)
            mask|=1u<<(number-1u);
    }
    free(names);
    return mask;
}

static DWORD serial_port_at(DWORD mask, DWORD index)
{
    DWORD number;

    for (number=1; number<=9u; ++number)
        if (mask&(1u<<(number-1u))) {
            if (index==0) return number;
            --index;
        }
    return 0;
}

static DWORD serial_port_count(DWORD mask)
{
    DWORD number,count=0;

    for (number=1; number<=9u; ++number)
        if (mask&(1u<<(number-1u))) ++count;
    return count;
}

static int hex_digit(char character)
{
    if (character>='0' && character<='9') return character-'0';
    if (character>='a' && character<='f') return character-'a'+10;
    if (character>='A' && character<='F') return character-'A'+10;
    return -1;
}

static LONG decode_value(const char *data, const char *end, DWORD *type,
    BYTE **bytes, DWORD *length)
{
    BYTE *result;
    DWORD count=0;

    if (data>=end) return ERROR_BAD_FORMAT;
    result=(BYTE *)malloc((SIZE_T)shadow_size+2u);
    if (!result) return ERROR_NOT_ENOUGH_MEMORY;
    if (*data=='"') {
        ++data;
        while (data<end && *data!='"') {
            if (*data=='\\' && data+1<end && (data[1]=='\\' || data[1]=='"')) ++data;
            result[count++]=(BYTE)*data++;
        }
        if (data==end) { free(result); return ERROR_BAD_FORMAT; }
        result[count++]=0;
        *type=REG_SZ;
    } else if ((end-data)>=7 && !_strnicmp(data,"hex(7):",7)) {
        data+=7;
        while (data<end) {
            int high,low;
            if (*data=='\\') {
                const char *next;
                ++data;
                while (data<end && (*data==' ' || *data=='\t')) ++data;
                if (data!=end) { free(result); return ERROR_BAD_FORMAT; }
                next=next_line(end);
                if (next>=shadow_contents+shadow_size || next[0]=='[') {
                    free(result); return ERROR_BAD_FORMAT;
                }
                data=next;
                end=line_end(data);
                continue;
            }
            if (*data==' ' || *data=='\t' || *data==',') { ++data; continue; }
            high=hex_digit(data[0]);
            low=(data+1<end)?hex_digit(data[1]):-1;
            if (high<0 || low<0) { free(result); return ERROR_BAD_FORMAT; }
            result[count++]=(BYTE)((high<<4)|low);
            data+=2;
        }
        if (count<2 || result[count-1]!=0 || result[count-2]!=0) {
            free(result); return ERROR_BAD_FORMAT;
        }
        *type=REG_MULTI_SZ;
    } else {
        free(result); return ERROR_BAD_FORMAT;
    }
    *bytes=result;
    *length=count;
    return ERROR_SUCCESS;
}

static MVDM_SHADOW_KEY *validate_key(HKEY key)
{
    MVDM_SHADOW_KEY *shadow;

    /* HKEY is an opaque identity.  Never probe an arbitrary native/predefined
     * value: stale or foreign handles must simply be rejected. */
    for (shadow=shadow_keys; shadow; shadow=shadow->next)
        if ((HKEY)shadow==key && shadow->magic==MVDM_SHADOW_KEY_MAGIC)
            return shadow;
    return NULL;
}

static VOID register_key(MVDM_SHADOW_KEY *key)
{
    key->magic=MVDM_SHADOW_KEY_MAGIC;
    key->next=shadow_keys;
    shadow_keys=key;
}

static VOID unregister_key(MVDM_SHADOW_KEY *key)
{
    MVDM_SHADOW_KEY **link;

    for (link=&shadow_keys; *link; link=&(*link)->next)
        if (*link==key) {
            *link=key->next;
            key->magic=0;
            return;
        }
}

static NTSTATUS status_from_error(LONG status)
{
    if (status==ERROR_SUCCESS) return STATUS_SUCCESS;
    if (status==ERROR_FILE_NOT_FOUND) return STATUS_OBJECT_NAME_NOT_FOUND;
    if (status==ERROR_MORE_DATA) return STATUS_BUFFER_TOO_SMALL;
    if (status==ERROR_NOT_ENOUGH_MEMORY) return STATUS_NO_MEMORY;
    if (status==ERROR_INVALID_HANDLE) return STATUS_INVALID_HANDLE;
    return STATUS_UNSUCCESSFUL;
}

static NTSTATUS section_from_nt_name(PUNICODE_STRING name, char *section,
    DWORD capacity)
{
    static const WCHAR nt_prefix[]=L"\\REGISTRY\\MACHINE\\";
    DWORD prefix_length=(DWORD)(sizeof(nt_prefix)/sizeof(nt_prefix[0])-1u);
    int converted;

    if (!name || !name->Buffer || name->Length/sizeof(WCHAR)<prefix_length ||
        _wcsnicmp(name->Buffer,nt_prefix,prefix_length)!=0)
        return STATUS_OBJECT_NAME_NOT_FOUND;
    if (name->Length/sizeof(WCHAR)==prefix_length) return STATUS_OBJECT_NAME_NOT_FOUND;
    if (capacity<20u) return STATUS_BUFFER_TOO_SMALL;
    lstrcpyA(section,"HKEY_LOCAL_MACHINE\\");
    converted=WideCharToMultiByte(CP_ACP,0,name->Buffer+prefix_length,
        (int)(name->Length/sizeof(WCHAR)-prefix_length),section+19,
        (int)(capacity-19u),NULL,NULL);
    if (!converted) return STATUS_OBJECT_NAME_NOT_FOUND;
    section[19+converted]='\0';
    return STATUS_SUCCESS;
}

static NTSTATUS native_host_value(const MVDM_SHADOW_VALUE *value, PVOID information,
    ULONG information_length, PULONG result_length)
{
    PKEY_VALUE_FULL_INFORMATION full=(PKEY_VALUE_FULL_INFORMATION)information;
    int name_chars;
    ULONG required,data_offset;

    if (!value || !information || !result_length) return STATUS_INVALID_HANDLE;
    name_chars=MultiByteToWideChar(CP_ACP,0,value->name,-1,NULL,0)-1;
    if (name_chars<0) return STATUS_UNSUCCESSFUL;
    required=(ULONG)offsetof(KEY_VALUE_FULL_INFORMATION,Name)+
        (ULONG)name_chars*sizeof(WCHAR);
    required=(required+sizeof(ULONG)-1u)&~(sizeof(ULONG)-1u);
    data_offset=required;
    required+=value->data_size;
    *result_length=required;
    if (information_length<required) return STATUS_BUFFER_TOO_SMALL;
    full->TitleIndex=0;
    full->Type=value->type;
    full->NameLength=(ULONG)name_chars*sizeof(WCHAR);
    full->DataOffset=data_offset;
    full->DataLength=value->data_size;
    MultiByteToWideChar(CP_ACP,0,value->name,-1,full->Name,name_chars+1);
    memcpy((PBYTE)full+data_offset,value->data,value->data_size);
    shadow_trace("NT_ENUM_HOST_OK");
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI ntvdm_shadow_nt_open_key(PHANDLE result, ULONG desired_access,
    POBJECT_ATTRIBUTES attributes)
{
    MVDM_SHADOW_KEY *key;
    char section[sizeof(key->section)];
    NTSTATUS status;

    (void)desired_access;
    if (!result || !attributes || !shadow_initialized) return STATUS_OBJECT_NAME_NOT_FOUND;
    *result=NULL;
    status=section_from_nt_name(attributes->ObjectName,section,sizeof(section));
    if (!NT_SUCCESS(status)) return status;
    if (!admitted_host_section(section)) return STATUS_OBJECT_NAME_NOT_FOUND;
    key=(MVDM_SHADOW_KEY *)calloc(1,sizeof(*key));
    if (!key) return STATUS_NO_MEMORY;
    lstrcpyA(key->section,section);
    status=snapshot_host_values(key);
    if (!find_section(section) && status!=ERROR_SUCCESS) {
        shadow_trace("NT_OPEN_ABSENT");
        free_host_values(key->host_values);
        free(key);
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }
    register_key(key);
    *result=(HANDLE)key;
    shadow_trace("NT_OPEN_OK");
    shadow_trace_section("NT_OPEN_SECTION",key->section);
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI ntvdm_shadow_nt_query_value_key(HANDLE handle, PVOID raw_name,
    ULONG information_class, PVOID information, ULONG information_length,
    PULONG result_length)
{
    (void)handle;
    (void)raw_name;
    (void)information_class;
    (void)information;
    (void)information_length;
    (void)result_length;
    shadow_trace("NT_QUERY_UNAVAILABLE");
    return STATUS_OBJECT_NAME_NOT_FOUND;
}

NTSTATUS NTAPI ntvdm_shadow_nt_enumerate_value_key(HANDLE handle, ULONG index,
    ULONG information_class, PVOID information,
    ULONG information_length, PULONG result_length)
{
    MVDM_SHADOW_KEY *key=validate_key((HKEY)handle);
    const char *line;
    ULONG current=0;

    if (!key || !information || !result_length) return STATUS_INVALID_HANDLE;
    if (information_class!=KeyValueFullInformation) return STATUS_UNSUCCESSFUL;
    line=find_section(key->section);
    if (!line) {
        MVDM_SHADOW_VALUE *value=key->host_values;
        while (value && index) { value=value->next; --index; }
        if (!value) { shadow_trace("NT_ENUM_END"); return STATUS_NO_MORE_ENTRIES; }
        return native_host_value(value,information,information_length,result_length);
    }
    while (line<shadow_contents+shadow_size && line[0]!='[') {
        const char *name,*raw,*end;
        DWORD name_length;
        if (value_line(line,&name,&name_length,&raw,&end)) {
            BYTE *decoded;
            DWORD type,length;
            LONG decoded_status=decode_value(raw,end,&type,&decoded,&length);
            if (decoded_status==ERROR_SUCCESS && current++==index) {
                PKEY_VALUE_FULL_INFORMATION full=(PKEY_VALUE_FULL_INFORMATION)information;
                int name_chars,data_chars;
                ULONG required;

                if (type!=REG_SZ) {
                    free(decoded);
                    shadow_trace("NT_ENUM_MALFORMED");
                    return STATUS_UNSUCCESSFUL;
                }
                name_chars=MultiByteToWideChar(CP_ACP,0,name,(int)name_length,NULL,0);
                data_chars=MultiByteToWideChar(CP_ACP,0,(LPCSTR)decoded,(int)(length-1u),NULL,0);
                required=(ULONG)offsetof(KEY_VALUE_FULL_INFORMATION,Name)+
                    (ULONG)name_chars*sizeof(WCHAR);
                required=(required+sizeof(ULONG)-1u)&~(sizeof(ULONG)-1u);
                required+=(ULONG)data_chars*sizeof(WCHAR);
                *result_length=required;
                if (name_chars==0 || information_length<required) {
                    free(decoded); return STATUS_BUFFER_TOO_SMALL;
                }
                full->TitleIndex=0;
                full->Type=REG_SZ;
                full->NameLength=(ULONG)name_chars*sizeof(WCHAR);
                full->DataOffset=required-(ULONG)data_chars*sizeof(WCHAR);
                full->DataLength=(ULONG)data_chars*sizeof(WCHAR);
                MultiByteToWideChar(CP_ACP,0,name,(int)name_length,full->Name,name_chars);
                MultiByteToWideChar(CP_ACP,0,(LPCSTR)decoded,(int)(length-1u),
                    (PWSTR)((PBYTE)full+full->DataOffset),data_chars);
                free(decoded);
                shadow_trace("NT_ENUM_OK");
                return STATUS_SUCCESS;
            }
            if (decoded_status==ERROR_SUCCESS) free(decoded);
            else shadow_trace("NT_ENUM_MALFORMED");
        }
        line=next_line(line);
    }
    {
        MVDM_SHADOW_VALUE *host;
        for (host=key->host_values; host; host=host->next) {
            const char *raw,*end;
            if (find_value(key,host->name,&raw,&end)) continue;
            if (current++==index)
                return native_host_value(host,information,information_length,result_length);
        }
    }
    shadow_trace("NT_ENUM_END");
    return STATUS_NO_MORE_ENTRIES;
}

NTSTATUS NTAPI ntvdm_shadow_nt_close(HANDLE handle)
{
    NTSTATUS status=status_from_error(ntvdm_shadow_reg_close_key((HKEY)handle));
    shadow_trace("NT_CLOSE");
    return status;
}

LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result)
{
    MVDM_SHADOW_KEY *key;
    (void)options; (void)access;
    if (!result || root!=HKEY_LOCAL_MACHINE || !subkey || !shadow_initialized)
        return ERROR_FILE_NOT_FOUND;
    *result=NULL;
    key=(MVDM_SHADOW_KEY *)calloc(1,sizeof(*key));
    if (!key) return ERROR_NOT_ENOUGH_MEMORY;
    if (lstrlenA(subkey)+20u>=sizeof(key->section)) {
        free(key); return ERROR_FILE_NOT_FOUND;
    }
    lstrcpyA(key->section,"HKEY_LOCAL_MACHINE\\");
    lstrcatA(key->section,subkey);
    if (_stricmp(subkey,MVDM_SERIALCOMM_SUBKEY)==0) {
        key->source=MVDM_SHADOW_KEY_SERIALCOMM;
        key->serial_ports=serial_port_mask();
        if (!key->serial_ports) {
            shadow_trace("SERIAL_DISCOVERY_ABSENT");
            free(key); return ERROR_FILE_NOT_FOUND;
        }
        register_key(key);
        *result=(HKEY)key;
        shadow_trace("SERIAL_DISCOVERY_OK");
        shadow_trace_section("OPEN_SECTION",key->section);
        return ERROR_SUCCESS;
    }
    if (!admitted_host_section(key->section)) {
        shadow_trace("OPEN_UNADMITTED");
        free(key); return ERROR_FILE_NOT_FOUND;
    }
    {
        LONG host_status=snapshot_host_values(key);
        if (!find_section(key->section) && host_status!=ERROR_SUCCESS) {
            shadow_trace("OPEN_ABSENT");
            shadow_trace_section("OPEN_ABSENT_SECTION",key->section);
            free_host_values(key->host_values);
            free(key); return ERROR_FILE_NOT_FOUND;
        }
    }
    register_key(key);
    *result=(HKEY)key;
    shadow_trace("OPEN_OK");
    shadow_trace_section("OPEN_SECTION",key->section);
    return ERROR_SUCCESS;
}

LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY handle, LPCSTR value,
    LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    const char *raw,*end;
    BYTE *decoded;
    DWORD decoded_type,length;
    LONG status;

    (void)reserved;
    if (!key || !data_size) {
        shadow_trace("QUERY_ABSENT");
        if (key && value) shadow_trace_section("QUERY_ABSENT_SECTION",key->section);
        return ERROR_FILE_NOT_FOUND;
    }
    if (!find_value(key,value,&raw,&end)) {
        MVDM_SHADOW_VALUE *host=host_value(key,value);
        if (!host) {
            shadow_trace("QUERY_ABSENT");
            shadow_trace_section("QUERY_ABSENT_SECTION",key->section);
            return ERROR_FILE_NOT_FOUND;
        }
        if (type) *type=host->type;
        if (!data || *data_size<host->data_size) {
            *data_size=host->data_size;
            return ERROR_MORE_DATA;
        }
        memcpy(data,host->data,host->data_size);
        *data_size=host->data_size;
        shadow_trace("QUERY_HOST_OK");
        return ERROR_SUCCESS;
    }
    status=decode_value(raw,end,&decoded_type,&decoded,&length);
    if (status!=ERROR_SUCCESS) {
        shadow_trace("QUERY_MALFORMED");
        return status;
    }
    if (type) *type=decoded_type;
    if (!data || *data_size<length) { *data_size=length; free(decoded); return ERROR_MORE_DATA; }
    memcpy(data,decoded,length);
    *data_size=length;
    free(decoded);
    shadow_trace("QUERY_OK");
    return ERROR_SUCCESS;
}

LONG WINAPI ntvdm_shadow_reg_query_info_key(HKEY handle, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    const char *line;
    DWORD count=0,max_name=0,max_data=0;

    (void)class_name; (void)class_length; (void)reserved; (void)maximum_subkey_length;
    (void)maximum_class_length; (void)security_descriptor; (void)last_write_time;
    if (!key) return ERROR_FILE_NOT_FOUND;
    if (key->source==MVDM_SHADOW_KEY_SERIALCOMM) {
        if (subkeys) *subkeys=0;
        if (values) *values=serial_port_count(key->serial_ports);
        if (maximum_value_name_length) *maximum_value_name_length=4;
        if (maximum_value_data_length) *maximum_value_data_length=5;
        return ERROR_SUCCESS;
    }
    if (!(line=find_section(key->section))) {
        MVDM_SHADOW_VALUE *value;
        for (value=key->host_values; value; value=value->next) {
            DWORD name_length=(DWORD)lstrlenA(value->name);
            ++count;
            if (name_length>max_name) max_name=name_length;
            if (value->data_size>max_data) max_data=value->data_size;
        }
        if (!count) return ERROR_FILE_NOT_FOUND;
        if (subkeys) *subkeys=0;
        if (values) *values=count;
        if (maximum_value_name_length) *maximum_value_name_length=max_name;
        if (maximum_value_data_length) *maximum_value_data_length=max_data;
        return ERROR_SUCCESS;
    }
    while (line<shadow_contents+shadow_size && line[0]!='[') {
        const char *name,*raw,*end;
        BYTE *decoded;
        DWORD name_length,type,length;
        if (value_line(line,&name,&name_length,&raw,&end) &&
            decode_value(raw,end,&type,&decoded,&length)==ERROR_SUCCESS) {
            ++count;
            if (name_length>max_name) max_name=name_length;
            if (length>max_data) max_data=length;
            free(decoded);
        }
        line=next_line(line);
    }
    {
        MVDM_SHADOW_VALUE *host;
        for (host=key->host_values; host; host=host->next) {
            const char *raw,*end;
            DWORD name_length=(DWORD)lstrlenA(host->name);
            if (find_value(key,host->name,&raw,&end)) continue;
            ++count;
            if (name_length>max_name) max_name=name_length;
            if (host->data_size>max_data) max_data=host->data_size;
        }
    }
    if (subkeys) *subkeys=0;
    if (values) *values=count;
    if (maximum_value_name_length) *maximum_value_name_length=max_name;
    if (maximum_value_data_length) *maximum_value_data_length=max_data;
    return ERROR_SUCCESS;
}

LONG WINAPI ntvdm_shadow_reg_enum_value(HKEY handle, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    const char *line;
    DWORD current=0;

    (void)reserved;
    if (!key || !value_name_size) return ERROR_FILE_NOT_FOUND;
    if (key->source==MVDM_SHADOW_KEY_SERIALCOMM) {
        char port[5];
        DWORD number=serial_port_at(key->serial_ports,index);
        DWORD length;

        if (!number) return ERROR_NO_MORE_ITEMS;
        wsprintfA(port,"COM%lu",number);
        length=(DWORD)lstrlenA(port);
        if (*value_name_size<=length || !data_size || (!data && *data_size)) {
            *value_name_size=length;
            return ERROR_MORE_DATA;
        }
        if (*data_size<length+1u) {
            *value_name_size=length;
            *data_size=length+1u;
            return ERROR_MORE_DATA;
        }
        lstrcpyA(value_name,port);
        *value_name_size=length;
        if (type) *type=REG_SZ;
        if (data) memcpy(data,port,length+1u);
        *data_size=length+1u;
        shadow_trace("SERIAL_ENUM_OK");
        return ERROR_SUCCESS;
    }
    if (!(line=find_section(key->section))) {
        MVDM_SHADOW_VALUE *host=key->host_values;
        DWORD name_length;
        while (host && index) { host=host->next; --index; }
        if (!host) return ERROR_NO_MORE_ITEMS;
        name_length=(DWORD)lstrlenA(host->name);
        if (*value_name_size<=name_length || !data_size || (!data && *data_size)) {
            *value_name_size=name_length;
            return ERROR_MORE_DATA;
        }
        if (*data_size<host->data_size) {
            *value_name_size=name_length;
            *data_size=host->data_size;
            return ERROR_MORE_DATA;
        }
        memcpy(value_name,host->name,name_length+1u);
        *value_name_size=name_length;
        if (type) *type=host->type;
        if (data) memcpy(data,host->data,host->data_size);
        *data_size=host->data_size;
        shadow_trace("ENUM_HOST_OK");
        return ERROR_SUCCESS;
    }
    while (line<shadow_contents+shadow_size && line[0]!='[') {
        const char *name,*raw,*end;
        DWORD name_length;
        if (value_line(line,&name,&name_length,&raw,&end)) {
            BYTE *decoded;
            DWORD decoded_type,length;
            LONG status=decode_value(raw,end,&decoded_type,&decoded,&length);
            if (status==ERROR_SUCCESS && current++==index) {
                if (*value_name_size<=name_length || !data_size || (!data && *data_size)) {
                    *value_name_size=name_length; free(decoded); return ERROR_MORE_DATA;
                }
                if (*data_size<length) { *value_name_size=name_length; *data_size=length; free(decoded); return ERROR_MORE_DATA; }
                memcpy(value_name,name,name_length); value_name[name_length]=0;
                *value_name_size=name_length;
                if (type) *type=decoded_type;
                if (data) memcpy(data,decoded,length);
                *data_size=length;
                free(decoded);
                return ERROR_SUCCESS;
            }
            if (status==ERROR_SUCCESS) free(decoded);
        }
        line=next_line(line);
    }
    {
        MVDM_SHADOW_VALUE *host;
        for (host=key->host_values; host; host=host->next) {
            const char *raw,*end;
            DWORD name_length;
            if (find_value(key,host->name,&raw,&end)) continue;
            if (current++!=index) continue;
            name_length=(DWORD)lstrlenA(host->name);
            if (*value_name_size<=name_length || !data_size || (!data && *data_size)) {
                *value_name_size=name_length;
                return ERROR_MORE_DATA;
            }
            if (*data_size<host->data_size) {
                *value_name_size=name_length;
                *data_size=host->data_size;
                return ERROR_MORE_DATA;
            }
            memcpy(value_name,host->name,name_length+1u);
            *value_name_size=name_length;
            if (type) *type=host->type;
            if (data) memcpy(data,host->data,host->data_size);
            *data_size=host->data_size;
            shadow_trace("ENUM_HOST_OK");
            return ERROR_SUCCESS;
        }
    }
    return ERROR_NO_MORE_ITEMS;
}

LONG WINAPI ntvdm_shadow_reg_close_key(HKEY handle)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    if (!key) return ERROR_INVALID_HANDLE;
    unregister_key(key);
    free_host_values(key->host_values);
    free(key);
    shadow_trace("CLOSE");
    return ERROR_SUCCESS;
}
