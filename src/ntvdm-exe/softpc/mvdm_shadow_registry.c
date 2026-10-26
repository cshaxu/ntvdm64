#include "mvdm_shadow_registry.h"
#include "common/system_root.h"
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

/* S2 keeps the product layer in process only.  This is intentionally not a
 * second provider: it is merely the mutable top layer of NTVDM.REG's existing
 * view.  S3 owns durable serialization and atomic writing. */
typedef struct _MVDM_SHADOW_OVERLAY_VALUE {
    struct _MVDM_SHADOW_OVERLAY_VALUE *next;
    char *name;
    DWORD type;
    BYTE *data;
    DWORD data_size;
    BOOL deleted;
} MVDM_SHADOW_OVERLAY_VALUE;

typedef struct _MVDM_SHADOW_OVERLAY_KEY {
    struct _MVDM_SHADOW_OVERLAY_KEY *next;
    char *section;
    BOOL created;
    BOOL deleted;
    MVDM_SHADOW_OVERLAY_VALUE *values;
} MVDM_SHADOW_OVERLAY_KEY;

static char *shadow_contents;
static DWORD shadow_size;
static DWORD shadow_base_size;
static BOOL shadow_initialized;
static char shadow_trace_path[MAX_PATH];
static MVDM_SHADOW_KEY *shadow_keys;
static MVDM_SHADOW_OVERLAY_KEY *shadow_overlay_keys;
static HANDLE shadow_commit_mutex;

/* The on-disk overlay is deliberately comments, rather than a second .reg
 * provider.  The ordinary NTVDM.REG body remains the original parser input;
 * this footer only records the parent-owned mutable layer and tombstones. */
static const char shadow_overlay_header[]=";NTVDM64-OVERLAY-V1\r\n";

static VOID free_host_values(MVDM_SHADOW_VALUE *value);
static VOID free_overlay_keys(VOID);
static MVDM_SHADOW_OVERLAY_KEY *overlay_key(LPCSTR section, BOOL add);
static MVDM_SHADOW_OVERLAY_VALUE *overlay_value(MVDM_SHADOW_OVERLAY_KEY *key,
    LPCSTR name, BOOL add);
static BOOL shadow_load_overlay_footer(VOID);

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
    DWORD error=common_product_path_a(L"system32\\NTVDM.REG",path,capacity);
    if(error)SetLastError(error);
    return error==ERROR_SUCCESS;
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
            shadow_commit_mutex=CreateMutexA(NULL,FALSE,
                "Local\\NTVDM64.Registry.Commit.1");
            if (!shadow_commit_mutex) return FALSE;
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
    shadow_base_size=shadow_size;
    if (!shadow_load_overlay_footer()) {
        free(shadow_contents); shadow_contents=NULL; shadow_size=0;
        return FALSE;
    }
    shadow_commit_mutex=CreateMutexA(NULL,FALSE,"Local\\NTVDM64.Registry.Commit.1");
    if (!shadow_commit_mutex) {
        free_overlay_keys(); free(shadow_contents); shadow_contents=NULL; shadow_size=0;
        return FALSE;
    }
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
    free_overlay_keys();
    free(shadow_contents);
    shadow_contents=NULL;
    shadow_size=0;
    shadow_base_size=0;
    shadow_initialized=FALSE;
    if (shadow_commit_mutex) CloseHandle(shadow_commit_mutex);
    shadow_commit_mutex=NULL;
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
    if (cursor==end || *cursor==';') return FALSE;
    if (*cursor=='@') {
        *name=cursor; *name_length=0; ++cursor;
    } else if (*cursor=='"') {
        ++cursor;
        *name=cursor;
        while (cursor<end && *cursor!='"') ++cursor;
        if (cursor==end) return FALSE;
        *name_length=(DWORD)(cursor-*name);
        ++cursor;
    } else return FALSE;
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
    static const char fax_prefix[]=
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\WOW\\WowFax";
    DWORD index;

    if (!section) return FALSE;
    for (index=0; exact[index]; ++index)
        if (!_stricmp(section,exact[index])) return TRUE;
    if (!_strnicmp(section,wow_prefix,sizeof(wow_prefix)-1u) &&
        (section[sizeof(wow_prefix)-1u]=='\0' ||
         section[sizeof(wow_prefix)-1u]=='\\')) return TRUE;
    /* WOWFAX is an original selected caller.  Admit only its exact product
     * subtree; never widen the machine-root view to generic HKLM. */
    return !_strnicmp(section,fax_prefix,sizeof(fax_prefix)-1u) &&
        (section[sizeof(fax_prefix)-1u]=='\0' ||
         section[sizeof(fax_prefix)-1u]=='\\');
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

static LONG WINAPI ntvdm_shadow_reg_open_key_ex_readonly(HKEY root, LPCSTR subkey, DWORD options,
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

static LONG WINAPI ntvdm_shadow_reg_query_value_ex_readonly(HKEY handle, LPCSTR value,
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

static LONG WINAPI ntvdm_shadow_reg_query_info_key_readonly(HKEY handle, LPTSTR class_name,
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

static LONG WINAPI ntvdm_shadow_reg_enum_value_readonly(HKEY handle, DWORD index, LPSTR value_name,
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

static char *shadow_duplicate(LPCSTR text)
{
    size_t length;
    char *copy;
    if (!text) return NULL;
    length=strlen(text)+1u;
    copy=(char *)malloc(length);
    if (copy) memcpy(copy,text,length);
    return copy;
}

static VOID free_overlay_values(MVDM_SHADOW_OVERLAY_VALUE *value)
{
    while (value) {
        MVDM_SHADOW_OVERLAY_VALUE *next=value->next;
        free(value->name); free(value->data); free(value);
        value=next;
    }
}

static VOID free_overlay_keys(VOID)
{
    while (shadow_overlay_keys) {
        MVDM_SHADOW_OVERLAY_KEY *key=shadow_overlay_keys;
        shadow_overlay_keys=key->next;
        free(key->section); free_overlay_values(key->values); free(key);
    }
}

static MVDM_SHADOW_OVERLAY_KEY *overlay_key(LPCSTR section, BOOL add)
{
    MVDM_SHADOW_OVERLAY_KEY *key;
    for (key=shadow_overlay_keys; key; key=key->next)
        if (!_stricmp(key->section,section)) return key;
    if (!add) return NULL;
    key=(MVDM_SHADOW_OVERLAY_KEY *)calloc(1,sizeof(*key));
    if (!key) return NULL;
    key->section=shadow_duplicate(section);
    if (!key->section) { free(key); return NULL; }
    key->next=shadow_overlay_keys;
    shadow_overlay_keys=key;
    return key;
}

static MVDM_SHADOW_OVERLAY_VALUE *overlay_value(MVDM_SHADOW_OVERLAY_KEY *key,
    LPCSTR name, BOOL add)
{
    MVDM_SHADOW_OVERLAY_VALUE *value;
    for (value=key ? key->values : NULL; value; value=value->next)
        if (!_stricmp(value->name,name ? name : "")) return value;
    if (!add || !key) return NULL;
    value=(MVDM_SHADOW_OVERLAY_VALUE *)calloc(1,sizeof(*value));
    if (!value) return NULL;
    value->name=shadow_duplicate(name ? name : "");
    if (!value->name) { free(value); return NULL; }
    value->next=key->values; key->values=value;
    return value;
}

static char *hex_decode(LPCSTR text, DWORD length, DWORD *bytes)
{
    char *result;
    DWORD index;
    if ((length&1u)!=0) return NULL;
    result=(char *)malloc(length/2u+1u);
    if (!result) return NULL;
    for (index=0; index<length; index+=2u) {
        int high=hex_digit(text[index]), low=hex_digit(text[index+1u]);
        if (high<0 || low<0) { free(result); return NULL; }
        result[index/2u]=(char)((high<<4)|low);
    }
    result[length/2u]='\0';
    if (bytes) *bytes=length/2u;
    return result;
}

static DWORD hex_length(DWORD bytes)
{
    return bytes>0x7fffffffu ? 0xffffffffu : bytes*2u;
}

static char *hex_append(char *cursor, const BYTE *bytes, DWORD length)
{
    static const char digits[]="0123456789ABCDEF";
    DWORD index;
    for (index=0; index<length; ++index) {
        *cursor++=digits[bytes[index]>>4];
        *cursor++=digits[bytes[index]&15u];
    }
    return cursor;
}

static BOOL shadow_load_overlay_footer(VOID)
{
    char *footer, *line;
    size_t header_length=sizeof(shadow_overlay_header)-1u;

    if (!shadow_contents) return TRUE;
    footer=strstr(shadow_contents,shadow_overlay_header);
    if (!footer) return TRUE;
    shadow_base_size=(DWORD)(footer-shadow_contents);
    line=footer+(DWORD)header_length;
    while (*line) {
        char *end=strpbrk(line,"\r\n");
        size_t length=end ? (size_t)(end-line) : strlen(line);
        if (length && !_strnicmp(line,";K:",3u)) {
            char *first, *decoded;
            DWORD section_bytes;
            MVDM_SHADOW_OVERLAY_KEY *key;
            if (length<6u || (line[3]!='0' && line[3]!='1') ||
                (line[4]!='0' && line[4]!='1') || line[5]!=':') goto malformed;
            first=line+6u;
            decoded=hex_decode(first,(DWORD)(length-(first-line)),&section_bytes);
            if (!decoded || !section_bytes) { free(decoded); goto malformed; }
            key=overlay_key(decoded,TRUE); free(decoded);
            if (!key) goto malformed;
            key->created=line[3]=='1'; key->deleted=line[4]=='1';
        } else if (length && !_strnicmp(line,";V:",3u)) {
            char *fields[5], *cursor=line+3u, *decoded_section, *decoded_name,
                *decoded_data;
            DWORD part, section_bytes, name_bytes, data_bytes, type, data_characters;
            MVDM_SHADOW_OVERLAY_KEY *key;
            MVDM_SHADOW_OVERLAY_VALUE *value;
            for (part=0; part<5u; ++part) {
                fields[part]=cursor;
                cursor=(char *)memchr(cursor,':',(size_t)(line+length-cursor));
                if (!cursor && part<4u) goto malformed;
                if (cursor) *cursor++='\0';
            }
            if (strlen(fields[0])!=1u || (fields[0][0]!='0' && fields[0][0]!='1') ||
                strlen(fields[1])!=8u) goto malformed;
            type=0;
            for (part=0; part<8u; ++part) { int digit=hex_digit(fields[1][part]); if (digit<0) goto malformed; type=(type<<4)|(DWORD)digit; }
            data_characters=(DWORD)(line+length-fields[4]);
            decoded_section=hex_decode(fields[2],(DWORD)strlen(fields[2]),&section_bytes);
            decoded_name=hex_decode(fields[3],(DWORD)strlen(fields[3]),&name_bytes);
            decoded_data=hex_decode(fields[4],data_characters,&data_bytes);
            if (!decoded_section || !decoded_name || !decoded_data || !section_bytes) {
                free(decoded_section); free(decoded_name); free(decoded_data); goto malformed;
            }
            key=overlay_key(decoded_section,TRUE);
            value=key ? overlay_value(key,decoded_name,TRUE) : NULL;
            free(decoded_section); free(decoded_name);
            if (!value) { free(decoded_data); goto malformed; }
            free(value->data); value->data=(BYTE *)decoded_data;
            value->data_size=data_bytes; value->type=type; value->deleted=fields[0][0]=='1';
        }
        if (!end) break;
        line=end; while (*line=='\r' || *line=='\n') ++line;
    }
    return TRUE;
malformed:
    free_overlay_keys(); SetLastError(ERROR_BAD_FORMAT); return FALSE;
}

static BOOL shadow_read_current(VOID)
{
    char path[MAX_PATH], *contents;
    HANDLE file;
    DWORD size, read;
    if (!shadow_path(path,sizeof(path))) return FALSE;
    file=CreateFileA(path,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (file==INVALID_HANDLE_VALUE) {
        if (GetLastError()!=ERROR_FILE_NOT_FOUND && GetLastError()!=ERROR_PATH_NOT_FOUND) return FALSE;
        free(shadow_contents); shadow_contents=NULL; shadow_size=shadow_base_size=0;
        free_overlay_keys(); return TRUE;
    }
    size=GetFileSize(file,NULL);
    if (size==INVALID_FILE_SIZE || size>65535u) { CloseHandle(file); SetLastError(ERROR_FILE_TOO_LARGE); return FALSE; }
    contents=(char *)malloc((SIZE_T)size+1u);
    if (!contents) { CloseHandle(file); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    if (!ReadFile(file,contents,size,&read,NULL) || read!=size) { free(contents); CloseHandle(file); return FALSE; }
    CloseHandle(file); contents[size]='\0';
    free(shadow_contents); shadow_contents=contents; shadow_size=size; shadow_base_size=size;
    free_overlay_keys();
    return shadow_load_overlay_footer();
}

static MVDM_SHADOW_OVERLAY_KEY *shadow_clone_overlay_keys(BOOL *ok)
{
    MVDM_SHADOW_OVERLAY_KEY *source, *head=NULL, **key_link=&head;
    *ok=FALSE;
    for (source=shadow_overlay_keys; source; source=source->next) {
        MVDM_SHADOW_OVERLAY_KEY *copy=(MVDM_SHADOW_OVERLAY_KEY *)calloc(1,sizeof(*copy));
        MVDM_SHADOW_OVERLAY_VALUE *source_value, **value_link;
        if (!copy) goto fail;
        copy->section=shadow_duplicate(source->section);
        if (!copy->section) { free(copy); goto fail; }
        copy->created=source->created; copy->deleted=source->deleted;
        value_link=&copy->values;
        for (source_value=source->values; source_value; source_value=source_value->next) {
            MVDM_SHADOW_OVERLAY_VALUE *value=(MVDM_SHADOW_OVERLAY_VALUE *)calloc(1,sizeof(*value));
            if (!value) { free(copy->section); free(copy); goto fail; }
            value->name=shadow_duplicate(source_value->name);
            value->type=source_value->type; value->data_size=source_value->data_size;
            value->deleted=source_value->deleted;
            if (!value->name || (value->data_size && !(value->data=(BYTE *)malloc(value->data_size)))) {
                free(value->name); free(value->data); free(value); free(copy->section); free(copy); goto fail;
            }
            if (value->data_size) memcpy(value->data,source_value->data,value->data_size);
            *value_link=value; value_link=&value->next;
        }
        *key_link=copy; key_link=&copy->next;
    }
    *ok=TRUE; return head;
fail:
    while (head) { MVDM_SHADOW_OVERLAY_KEY *next=head->next; free(head->section); free_overlay_values(head->values); free(head); head=next; }
    return NULL;
}

static BOOL shadow_build_persisted(char **output, DWORD *size)
{
    MVDM_SHADOW_OVERLAY_KEY *key;
    SIZE_T total=shadow_base_size ? shadow_base_size : 38u;
    char *contents, *cursor;
    static const char default_header[]="Windows Registry Editor Version 5.00\r\n\r\n";

    if (shadow_base_size && shadow_contents &&
        shadow_contents[shadow_base_size-1u]!='\n') ++total;
    total+=sizeof(shadow_overlay_header)-1u;
    for (key=shadow_overlay_keys; key; key=key->next) {
        MVDM_SHADOW_OVERLAY_VALUE *value;
        total+=6u+hex_length((DWORD)lstrlenA(key->section))+2u;
        for (value=key->values; value; value=value->next)
            total+=3u+1u+1u+8u+1u+hex_length((DWORD)lstrlenA(key->section))+1u+
                hex_length((DWORD)lstrlenA(value->name))+1u+hex_length(value->data_size)+2u;
    }
    if (total>65535u || total>0xffffffffu-1u) { SetLastError(ERROR_FILE_TOO_LARGE); return FALSE; }
    contents=(char *)malloc(total+1u);
    if (!contents) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    cursor=contents;
    if (shadow_base_size && shadow_contents) {
        memcpy(cursor,shadow_contents,shadow_base_size); cursor+=shadow_base_size;
        if (cursor>contents && cursor[-1]!='\n') { *cursor++='\r'; *cursor++='\n'; }
    } else { memcpy(cursor,default_header,sizeof(default_header)-1u); cursor+=sizeof(default_header)-1u; }
    memcpy(cursor,shadow_overlay_header,sizeof(shadow_overlay_header)-1u); cursor+=sizeof(shadow_overlay_header)-1u;
    for (key=shadow_overlay_keys; key; key=key->next) {
        MVDM_SHADOW_OVERLAY_VALUE *value;
        *cursor++=';'; *cursor++='K'; *cursor++=':'; *cursor++=key->created?'1':'0';
        *cursor++=key->deleted?'1':'0'; *cursor++=':';
        cursor=hex_append(cursor,(const BYTE *)key->section,(DWORD)lstrlenA(key->section));
        *cursor++='\r'; *cursor++='\n';
        for (value=key->values; value; value=value->next) {
            int count=wsprintfA(cursor,";V:%c:%08lX:",value->deleted?'1':'0',(unsigned long)value->type);
            cursor+=count;
            cursor=hex_append(cursor,(const BYTE *)key->section,(DWORD)lstrlenA(key->section)); *cursor++=':';
            cursor=hex_append(cursor,(const BYTE *)value->name,(DWORD)lstrlenA(value->name)); *cursor++=':';
            cursor=hex_append(cursor,value->data,value->data_size); *cursor++='\r'; *cursor++='\n';
        }
    }
    *cursor='\0'; *output=contents; *size=(DWORD)(cursor-contents); return TRUE;
}

static LONG shadow_commit_overlay(VOID)
{
    char path[MAX_PATH], temporary[MAX_PATH], *contents;
    DWORD size, written;
    HANDLE file;
    BOOL replaced;
    if (!shadow_build_persisted(&contents,&size)) return GetLastError();
    if (!shadow_path(path,sizeof(path)) || lstrlenA(path)+5u>sizeof(temporary)) { free(contents); return GetLastError(); }
    lstrcpyA(temporary,path); lstrcatA(temporary,".tmp");
    file=CreateFileA(temporary,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,
        FILE_ATTRIBUTE_TEMPORARY|FILE_FLAG_WRITE_THROUGH,NULL);
    if (file==INVALID_HANDLE_VALUE) { free(contents); return GetLastError(); }
    if (!WriteFile(file,contents,size,&written,NULL) || written!=size || !FlushFileBuffers(file)) {
        DWORD error=GetLastError(); CloseHandle(file); DeleteFileA(temporary); free(contents); return error;
    }
    CloseHandle(file);
    if (GetFileAttributesA(path)==INVALID_FILE_ATTRIBUTES && GetLastError()==ERROR_FILE_NOT_FOUND)
        replaced=MoveFileExA(temporary,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH);
    else replaced=ReplaceFileA(path,temporary,NULL,REPLACEFILE_WRITE_THROUGH,NULL,NULL);
    if (!replaced) { DWORD error=GetLastError(); DeleteFileA(temporary); free(contents); return error; }
    free(shadow_contents); shadow_contents=contents; shadow_size=size;
    shadow_base_size=shadow_size; free_overlay_keys();
    if (!shadow_load_overlay_footer()) return GetLastError();
    shadow_trace("COMMIT_OK"); return ERROR_SUCCESS;
}

static LONG shadow_begin_mutation(MVDM_SHADOW_OVERLAY_KEY **prior)
{
    BOOL copied;
    DWORD wait;
    if (!shadow_commit_mutex) return ERROR_INVALID_HANDLE;
    wait=WaitForSingleObject(shadow_commit_mutex,INFINITE);
    if (wait!=WAIT_OBJECT_0 && wait!=WAIT_ABANDONED) return ERROR_GEN_FAILURE;
    *prior=shadow_clone_overlay_keys(&copied);
    if (!copied) { ReleaseMutex(shadow_commit_mutex); return ERROR_NOT_ENOUGH_MEMORY; }
    if (!shadow_read_current()) { LONG error=GetLastError(); free_overlay_keys(); shadow_overlay_keys=*prior; *prior=NULL; ReleaseMutex(shadow_commit_mutex); return error; }
    return ERROR_SUCCESS;
}

static LONG shadow_finish_mutation(MVDM_SHADOW_OVERLAY_KEY *prior, LONG status)
{
    if (status==ERROR_SUCCESS) status=shadow_commit_overlay();
    if (status!=ERROR_SUCCESS) { free_overlay_keys(); shadow_overlay_keys=prior; prior=NULL; }
    if (prior) { MVDM_SHADOW_OVERLAY_KEY *saved=shadow_overlay_keys; shadow_overlay_keys=prior; free_overlay_keys(); shadow_overlay_keys=saved; }
    ReleaseMutex(shadow_commit_mutex);
    return status;
}

static BOOL section_is_child(LPCSTR parent, LPCSTR section, LPCSTR *child,
    DWORD *length)
{
    size_t prefix;
    const char *slash,*end;
    if (!parent || !section) return FALSE;
    prefix=strlen(parent);
    if (_strnicmp(parent,section,prefix) || section[prefix]!='\\') return FALSE;
    *child=section+prefix+1u;
    slash=strchr(*child,'\\');
    end=slash ? slash : *child+strlen(*child);
    *length=(DWORD)(end-*child);
    return *length!=0;
}

static BOOL section_baseline_exists(LPCSTR section)
{
    const char *line;
    size_t length;
    if (!section) return FALSE;
    if (!_stricmp(section,"HKEY_CLASSES_ROOT") ||
        !_stricmp(section,"HKEY_LOCAL_MACHINE")) return TRUE;
    if (find_section(section)) return TRUE;
    length=strlen(section);
    for (line=shadow_contents; line && line<shadow_contents+shadow_size;
         line=next_line(line)) {
        const char *end=line_end(line);
        if (line[0]=='[' && (size_t)(end-line)>length+2u &&
            !_strnicmp(line+1,section,length) && line[length+1]=='\\') return TRUE;
    }
    return FALSE;
}

static BOOL section_visible(LPCSTR section)
{
    char parent[512];
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    size_t length;

    if (!section) return FALSE;
    lstrcpynA(parent,section,sizeof(parent));
    for (;;) {
        overlay=overlay_key(parent,FALSE);
        if (overlay && overlay->deleted) return FALSE;
        length=strlen(parent);
        while (length && parent[length-1u]!='\\') --length;
        if (!length) break;
        parent[length-1u]='\0';
    }
    overlay=overlay_key(section,FALSE);
    return section_baseline_exists(section) || (overlay && overlay->created);
}

static BOOL section_admitted(LPCSTR section)
{
    static const char classes[]="HKEY_CLASSES_ROOT";
    if (!_strnicmp(section,classes,sizeof(classes)-1u) &&
        (section[sizeof(classes)-1u]=='\0' || section[sizeof(classes)-1u]=='\\'))
        return TRUE;
    return admitted_host_section(section);
}

static LONG section_from_root(HKEY root, LPCSTR subkey, char *section,
    DWORD capacity)
{
    MVDM_SHADOW_KEY *key;
    LPCSTR base;
    size_t length;
    if (root==HKEY_CLASSES_ROOT) base="HKEY_CLASSES_ROOT";
    else if (root==HKEY_LOCAL_MACHINE) base="HKEY_LOCAL_MACHINE";
    else {
        key=validate_key(root);
        if (!key) return ERROR_INVALID_HANDLE;
        base=key->section;
    }
    length=strlen(base);
    if (subkey && *subkey) length+=1u+strlen(subkey);
    if (length+1u>capacity) return ERROR_FILE_NOT_FOUND;
    lstrcpyA(section,base);
    if (subkey && *subkey) { lstrcatA(section,"\\"); lstrcatA(section,subkey); }
    return ERROR_SUCCESS;
}

static LONG open_section(HKEY root, LPCSTR subkey, BOOL create, PHKEY result)
{
    MVDM_SHADOW_KEY *key;
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    char section[512];
    LONG status;
    BOOL host_exists=FALSE;
    if (!result || !shadow_initialized) return ERROR_FILE_NOT_FOUND;
    *result=NULL;
    status=section_from_root(root,subkey,section,sizeof(section));
    if (status!=ERROR_SUCCESS) return status;
    if (!section_admitted(section)) return ERROR_FILE_NOT_FOUND;
    if (admitted_host_section(section)) {
        MVDM_SHADOW_KEY probe;
        memset(&probe,0,sizeof(probe)); lstrcpyA(probe.section,section);
        host_exists=snapshot_host_values(&probe)==ERROR_SUCCESS;
        free_host_values(probe.host_values);
    }
    if (!section_visible(section) && !host_exists) {
        if (!create) return ERROR_FILE_NOT_FOUND;
        overlay=overlay_key(section,TRUE);
        if (!overlay) return ERROR_NOT_ENOUGH_MEMORY;
        overlay->created=TRUE; overlay->deleted=FALSE;
    }
    key=(MVDM_SHADOW_KEY *)calloc(1,sizeof(*key));
    if (!key) return ERROR_NOT_ENOUGH_MEMORY;
    lstrcpyA(key->section,section);
    if (!_stricmp(section,"HKEY_LOCAL_MACHINE\\" MVDM_SERIALCOMM_SUBKEY)) {
        key->source=MVDM_SHADOW_KEY_SERIALCOMM; key->serial_ports=serial_port_mask();
    } else if (admitted_host_section(section)) {
        (void)snapshot_host_values(key);
    }
    register_key(key); *result=(HKEY)key;
    return ERROR_SUCCESS;
}

static LONG query_overlay_value(MVDM_SHADOW_KEY *key, LPCSTR name,
    LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    MVDM_SHADOW_OVERLAY_KEY *overlay=overlay_key(key->section,FALSE);
    MVDM_SHADOW_OVERLAY_VALUE *value=overlay_value(overlay,name,FALSE);
    if (!value) return ERROR_FILE_NOT_FOUND;
    if (value->deleted) return ERROR_FILE_NOT_FOUND;
    if (type) *type=value->type;
    if (!data || !data_size || *data_size<value->data_size) {
        if (data_size) *data_size=value->data_size;
        return ERROR_MORE_DATA;
    }
    memcpy(data,value->data,value->data_size); *data_size=value->data_size;
    return ERROR_SUCCESS;
}

LONG WINAPI ntvdm_shadow_reg_open_key(HKEY root, LPCSTR subkey, PHKEY result)
{
    return open_section(root,subkey,FALSE,result);
}

LONG WINAPI ntvdm_shadow_reg_create_key(HKEY root, LPCSTR subkey, PHKEY result)
{
    MVDM_SHADOW_OVERLAY_KEY *prior;
    LONG status;
    if (!result) return ERROR_INVALID_PARAMETER;
    status=shadow_begin_mutation(&prior);
    if (status!=ERROR_SUCCESS) return status;
    status=open_section(root,subkey,TRUE,result);
    if (status==ERROR_SUCCESS) {
        status=shadow_finish_mutation(prior,status);
        if (status!=ERROR_SUCCESS) {
            (void)ntvdm_shadow_reg_close_key(*result);
            *result=NULL;
        }
        return status;
    }
    return shadow_finish_mutation(prior,status);
}

LONG WINAPI ntvdm_shadow_reg_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result)
{
    (void)options; (void)access;
    return open_section(root,subkey,FALSE,result);
}

LONG WINAPI ntvdm_shadow_reg_set_value(HKEY handle, LPCSTR subkey, DWORD type,
    LPCSTR data, DWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    MVDM_SHADOW_OVERLAY_VALUE *value;
    HKEY child;
    LONG status;
    MVDM_SHADOW_OVERLAY_KEY *prior;
    if (!key || type!=REG_SZ || !data) return ERROR_INVALID_PARAMETER;
    status=shadow_begin_mutation(&prior);
    if (status!=ERROR_SUCCESS) return status;
    status=open_section(handle,subkey,TRUE,&child);
    if (status!=ERROR_SUCCESS) return shadow_finish_mutation(prior,status);
    key=validate_key(child);
    overlay=overlay_key(key->section,TRUE);
    value=overlay_value(overlay,"",TRUE);
    if (!overlay || !value) { ntvdm_shadow_reg_close_key(child); return shadow_finish_mutation(prior,ERROR_NOT_ENOUGH_MEMORY); }
    free(value->data); value->data=(BYTE *)malloc(data_size);
    if (!value->data) { value->data_size=0; ntvdm_shadow_reg_close_key(child); return shadow_finish_mutation(prior,ERROR_NOT_ENOUGH_MEMORY); }
    memcpy(value->data,data,data_size); value->data_size=data_size; value->type=type;
    value->deleted=FALSE; overlay->created=TRUE; overlay->deleted=FALSE;
    status=ntvdm_shadow_reg_close_key(child);
    return shadow_finish_mutation(prior,status);
}

LONG WINAPI ntvdm_shadow_reg_set_value_ex(HKEY handle, LPCSTR name, DWORD reserved,
    DWORD type, const BYTE *data, DWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    MVDM_SHADOW_OVERLAY_VALUE *value;
    BYTE *copy;
    (void)reserved;
    MVDM_SHADOW_OVERLAY_KEY *prior;
    LONG status;
    if (!key || (!data && data_size)) return ERROR_INVALID_PARAMETER;
    status=shadow_begin_mutation(&prior);
    if (status!=ERROR_SUCCESS) return status;
    overlay=overlay_key(key->section,TRUE);
    value=overlay_value(overlay,name ? name : "",TRUE);
    if (!overlay || !value) return shadow_finish_mutation(prior,ERROR_NOT_ENOUGH_MEMORY);
    copy=data_size ? (BYTE *)malloc(data_size) : NULL;
    if (data_size && !copy) return shadow_finish_mutation(prior,ERROR_NOT_ENOUGH_MEMORY);
    if (data_size) memcpy(copy,data,data_size);
    free(value->data); value->data=copy; value->data_size=data_size; value->type=type;
    value->deleted=FALSE; overlay->created=TRUE; overlay->deleted=FALSE;
    return shadow_finish_mutation(prior,ERROR_SUCCESS);
}

LONG WINAPI ntvdm_shadow_reg_query_value(HKEY handle, LPCSTR subkey, LPSTR data,
    PLONG data_size)
{
    HKEY child; DWORD size,type; LONG status;
    if (!data_size) return ERROR_INVALID_PARAMETER;
    status=open_section(handle,subkey,FALSE,&child);
    if (status!=ERROR_SUCCESS) return status;
    size=(DWORD)*data_size;
    status=ntvdm_shadow_reg_query_value_ex(child,"",NULL,&type,(LPBYTE)data,&size);
    *data_size=(LONG)size; ntvdm_shadow_reg_close_key(child);
    return status;
}

LONG WINAPI ntvdm_shadow_reg_query_value_ex(HKEY handle, LPCSTR value,
    LPDWORD reserved, LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    if (!key) return ERROR_INVALID_HANDLE;
    {
        MVDM_SHADOW_OVERLAY_KEY *overlay=overlay_key(key->section,FALSE);
        if (overlay_value(overlay,value ? value : "",FALSE))
            return query_overlay_value(key,value ? value : "",type,data,data_size);
    }
    return ntvdm_shadow_reg_query_value_ex_readonly(handle,value,reserved,type,data,data_size);
}

LONG WINAPI ntvdm_shadow_reg_delete_key(HKEY handle, LPCSTR subkey)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    char section[512], child[512]; HKEY target; LONG status;
    MVDM_SHADOW_OVERLAY_KEY *prior;
    if (!key || !subkey || !*subkey) return ERROR_INVALID_PARAMETER;
    status=shadow_begin_mutation(&prior);
    if (status!=ERROR_SUCCESS) return status;
    status=section_from_root(handle,subkey,section,sizeof(section));
    if (status!=ERROR_SUCCESS || !section_visible(section)) return shadow_finish_mutation(prior,ERROR_FILE_NOT_FOUND);
    status=open_section(handle,subkey,FALSE,&target);
    if (status!=ERROR_SUCCESS) return shadow_finish_mutation(prior,status);
    status=ntvdm_shadow_reg_enum_key(target,0,child,sizeof(child));
    ntvdm_shadow_reg_close_key(target);
    if (status==ERROR_SUCCESS) return shadow_finish_mutation(prior,ERROR_ACCESS_DENIED);
    if (status!=ERROR_NO_MORE_ITEMS) return shadow_finish_mutation(prior,status);
    overlay=overlay_key(section,TRUE);
    if (!overlay) return shadow_finish_mutation(prior,ERROR_NOT_ENOUGH_MEMORY);
    overlay->deleted=TRUE; return shadow_finish_mutation(prior,ERROR_SUCCESS);
}

LONG WINAPI ntvdm_shadow_reg_enum_key(HKEY handle, DWORD index, LPSTR name,
    DWORD name_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    const char *line; MVDM_SHADOW_OVERLAY_KEY *overlay; DWORD current=0;
    if (!key || !name || !name_size) return ERROR_INVALID_HANDLE;
    for (line=shadow_contents; line && line<shadow_contents+shadow_size; line=next_line(line)) {
        const char *child; DWORD length; const char *end=line_end(line);
        if (line[0]!='[' || end<=line+2) continue;
        { char section[512]; DWORD section_length=(DWORD)(end-line-2);
          if (section_length>=sizeof(section)) continue;
          memcpy(section,line+1,section_length); section[section_length]=0;
          const char *prior;
          BOOL duplicate=FALSE;
          if (!section_is_child(key->section,section,&child,&length) || !section_visible(section)) continue;
          for (prior=shadow_contents; prior<line; prior=next_line(prior)) {
              const char *old_child; DWORD old_length; const char *prior_end=line_end(prior);
              if (prior[0]=='[' && prior_end>prior+2) { char old[512]; DWORD n=(DWORD)(prior_end-prior-2);
                  if (n<sizeof(old)) { memcpy(old,prior+1,n); old[n]=0;
                      if (section_is_child(key->section,old,&old_child,&old_length) &&
                          old_length==length && !_strnicmp(old_child,child,length)) { duplicate=TRUE; break; }
                  }
              }
          }
          if (duplicate) continue;
          if (current++==index) { if (length+1u>name_size) return ERROR_MORE_DATA; memcpy(name,child,length); name[length]=0; return ERROR_SUCCESS; }
        }
    }
    for (overlay=shadow_overlay_keys; overlay; overlay=overlay->next) {
        const char *child; DWORD length; DWORD prior=0; const char *scan;
        if (overlay->deleted || !section_is_child(key->section,overlay->section,&child,&length)) continue;
        for (scan=shadow_contents; scan && scan<shadow_contents+shadow_size; scan=next_line(scan)) {
            const char *old_child; DWORD old_length; const char *end=line_end(scan);
            if (scan[0]=='[' && end>scan+2) { char old[512]; DWORD n=(DWORD)(end-scan-2);
                if (n<sizeof(old)) { memcpy(old,scan+1,n); old[n]=0;
                    if (section_is_child(key->section,old,&old_child,&old_length) && old_length==length && !_strnicmp(old_child,child,length)) { prior=1; break; } }
            }
        }
        if (!prior) {
            MVDM_SHADOW_OVERLAY_KEY *earlier;
            for (earlier=shadow_overlay_keys; earlier!=overlay; earlier=earlier->next) {
                const char *old_child; DWORD old_length;
                if (!earlier->deleted && section_is_child(key->section,earlier->section,
                    &old_child,&old_length) && old_length==length &&
                    !_strnicmp(old_child,child,length)) { prior=1; break; }
            }
        }
        if (prior) continue;
        if (current++==index) { if (length+1u>name_size) return ERROR_MORE_DATA; memcpy(name,child,length); name[length]=0; return ERROR_SUCCESS; }
    }
    return ERROR_NO_MORE_ITEMS;
}

LONG WINAPI ntvdm_shadow_reg_query_info_key(HKEY handle, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time)
{
    MVDM_SHADOW_KEY *key=validate_key(handle); char child[512], value_name[512];
    DWORD count=0,max=0,value_count=0,value_name_max=0,value_data_max=0;
    LONG status;
    if (!key) return ERROR_INVALID_HANDLE;
    while ((status=ntvdm_shadow_reg_enum_key(handle,count,child,sizeof(child)))==ERROR_SUCCESS) {
        DWORD length=(DWORD)lstrlenA(child); if (length>max) max=length; ++count;
    }
    if (status!=ERROR_NO_MORE_ITEMS) return status;
    if (subkeys) *subkeys=count;
    if (maximum_subkey_length) *maximum_subkey_length=max;
    for (;;) {
        DWORD name_size=sizeof(value_name), data_size=0;
        status=ntvdm_shadow_reg_enum_value(handle,value_count,value_name,&name_size,
            NULL,NULL,NULL,&data_size);
        if (status==ERROR_NO_MORE_ITEMS) break;
        if (status!=ERROR_SUCCESS && status!=ERROR_MORE_DATA) return status;
        ++value_count;
        if (name_size>value_name_max) value_name_max=name_size;
        if (data_size>value_data_max) value_data_max=data_size;
    }
    if (values) *values=value_count;
    if (maximum_value_name_length) *maximum_value_name_length=value_name_max;
    if (maximum_value_data_length) *maximum_value_data_length=value_data_max;
    (void)class_name; (void)reserved; (void)maximum_class_length;
    (void)security_descriptor; (void)last_write_time;
    if (class_length) *class_length=0;
    return ERROR_SUCCESS;
}

static LONG copy_overlay_value(const MVDM_SHADOW_OVERLAY_VALUE *value,
    LPSTR value_name, LPDWORD value_name_size, LPDWORD type, LPBYTE data,
    LPDWORD data_size)
{
    DWORD length;
    if (!value || !value_name_size) return ERROR_INVALID_PARAMETER;
    length=(DWORD)lstrlenA(value->name);
    if (*value_name_size<=length || !data_size || (!data && *data_size)) {
        *value_name_size=length; return ERROR_MORE_DATA;
    }
    if (*data_size<value->data_size) {
        *value_name_size=length; *data_size=value->data_size; return ERROR_MORE_DATA;
    }
    memcpy(value_name,value->name,length+1u); *value_name_size=length;
    if (type) *type=value->type;
    if (data && value->data_size) memcpy(data,value->data,value->data_size);
    *data_size=value->data_size;
    return ERROR_SUCCESS;
}

LONG WINAPI ntvdm_shadow_reg_enum_value(HKEY handle, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size)
{
    MVDM_SHADOW_KEY *key=validate_key(handle);
    MVDM_SHADOW_OVERLAY_KEY *overlay;
    MVDM_SHADOW_OVERLAY_VALUE *value;
    const char *line;
    DWORD current=0;
    (void)reserved;
    if (!key || !value_name_size) return ERROR_INVALID_HANDLE;
    overlay=overlay_key(key->section,FALSE);
    if (!overlay) return ntvdm_shadow_reg_enum_value_readonly(handle,index,value_name,
        value_name_size,reserved,type,data,data_size);
    /* Preserve the original static/host enumeration order.  An overlay entry
     * replaces its same-named baseline value in place; a new value follows
     * the baseline.  Deleted values are absent from both views. */
    line=find_section(key->section);
    while (line && line<shadow_contents+shadow_size && line[0]!='[') {
        const char *name,*raw,*end;
        DWORD name_length;
        if (value_line(line,&name,&name_length,&raw,&end)) {
            char candidate[512];
            if (name_length<sizeof(candidate)) {
                memcpy(candidate,name,name_length); candidate[name_length]=0;
                value=overlay_value(overlay,candidate,FALSE);
                if (value && value->deleted) { line=next_line(line); continue; }
                if (current++==index) {
                    if (value) return copy_overlay_value(value,value_name,value_name_size,type,data,data_size);
                    { BYTE *decoded; DWORD decoded_type,decoded_size;
                      LONG status=decode_value(raw,end,&decoded_type,&decoded,&decoded_size);
                      if (status!=ERROR_SUCCESS) return status;
                      if (*value_name_size<=name_length || !data_size || (!data && *data_size)) {
                          *value_name_size=name_length; free(decoded); return ERROR_MORE_DATA; }
                      if (*data_size<decoded_size) { *value_name_size=name_length; *data_size=decoded_size;
                          free(decoded); return ERROR_MORE_DATA; }
                      memcpy(value_name,name,name_length); value_name[name_length]=0; *value_name_size=name_length;
                      if (type) *type=decoded_type; if (data && decoded_size) memcpy(data,decoded,decoded_size);
                      *data_size=decoded_size; free(decoded); return ERROR_SUCCESS; }
                }
            }
        }
        line=next_line(line);
    }
    {
        MVDM_SHADOW_VALUE *host;
        for (host=key->host_values; host; host=host->next) {
            const char *raw,*end;
            if (find_value(key,host->name,&raw,&end)) continue;
            value=overlay_value(overlay,host->name,FALSE);
            if (value && value->deleted) continue;
            if (current++==index) {
                if (value) return copy_overlay_value(value,value_name,value_name_size,type,data,data_size);
                { DWORD length=(DWORD)lstrlenA(host->name);
                  if (*value_name_size<=length || !data_size || (!data && *data_size)) { *value_name_size=length; return ERROR_MORE_DATA; }
                  if (*data_size<host->data_size) { *value_name_size=length; *data_size=host->data_size; return ERROR_MORE_DATA; }
                  memcpy(value_name,host->name,length+1u); *value_name_size=length;
                  if (type) *type=host->type; if (data && host->data_size) memcpy(data,host->data,host->data_size);
                  *data_size=host->data_size; return ERROR_SUCCESS; }
            }
        }
    }
    for (value=overlay->values; value; value=value->next) {
        const char *raw,*end;
        if (value->deleted || find_value(key,value->name,&raw,&end) || host_value(key,value->name)) continue;
        if (current++==index) return copy_overlay_value(value,value_name,value_name_size,type,data,data_size);
    }
    return ERROR_NO_MORE_ITEMS;
}
