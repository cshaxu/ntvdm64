#include "precomp.h"
#include "wow_registry_binding.h"

/* Parent NTVDM owns the layered tree and all raw key identities.  This DLL
 * owns only the worker-local token translation required by original guest
 * DWORD HKEY slots. */
typedef LONG (WINAPI *wow_registry_open_key_fn)(HKEY,LPCSTR,PHKEY);
typedef LONG (WINAPI *wow_registry_close_key_fn)(HKEY);
typedef LONG (WINAPI *wow_registry_delete_key_fn)(HKEY,LPCSTR);
typedef LONG (WINAPI *wow_registry_set_value_fn)(HKEY,LPCSTR,DWORD,LPCSTR,DWORD);
typedef LONG (WINAPI *wow_registry_set_value_ex_fn)(HKEY,LPCSTR,DWORD,DWORD,const BYTE *,DWORD);
typedef LONG (WINAPI *wow_registry_query_value_fn)(HKEY,LPCSTR,LPSTR,PLONG);
typedef LONG (WINAPI *wow_registry_query_value_ex_fn)(HKEY,LPCSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
typedef LONG (WINAPI *wow_registry_enum_key_fn)(HKEY,DWORD,LPSTR,DWORD);
typedef LONG (WINAPI *wow_registry_open_key_ex_fn)(HKEY,LPCSTR,DWORD,REGSAM,PHKEY);
typedef LONG (WINAPI *wow_registry_query_info_key_fn)(HKEY,LPTSTR,LPDWORD,
    LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,LPDWORD,PFILETIME);
typedef LONG (WINAPI *wow_registry_enum_value_fn)(HKEY,DWORD,LPSTR,LPDWORD,
    LPDWORD,LPDWORD,LPBYTE,LPDWORD);

static INIT_ONCE wow_registry_parent_once=INIT_ONCE_STATIC_INIT;
static wow_registry_open_key_fn wow_parent_open_key, wow_parent_create_key;
static wow_registry_close_key_fn wow_parent_close_key;
static wow_registry_delete_key_fn wow_parent_delete_key;
static wow_registry_set_value_fn wow_parent_set_value;
static wow_registry_set_value_ex_fn wow_parent_set_value_ex;
static wow_registry_query_value_fn wow_parent_query_value;
static wow_registry_query_value_ex_fn wow_parent_query_value_ex;
static wow_registry_enum_key_fn wow_parent_enum_key;
static wow_registry_open_key_ex_fn wow_parent_open_key_ex;
static wow_registry_query_info_key_fn wow_parent_query_info_key;
static wow_registry_enum_value_fn wow_parent_enum_value;

static BOOL CALLBACK wow_registry_parent_initialize(PINIT_ONCE once,
    PVOID parameter, PVOID *context)
{
    HMODULE parent;
    (void)once; (void)parameter; (void)context;
    parent=GetModuleHandleA(NULL);
    if (!parent) return FALSE;
    wow_parent_open_key=(wow_registry_open_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_open_key");
    wow_parent_create_key=(wow_registry_open_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_create_key");
    wow_parent_close_key=(wow_registry_close_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_close_key");
    wow_parent_delete_key=(wow_registry_delete_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_delete_key");
    wow_parent_set_value=(wow_registry_set_value_fn)GetProcAddress(parent,"ntvdm_shadow_reg_set_value");
    wow_parent_set_value_ex=(wow_registry_set_value_ex_fn)GetProcAddress(parent,"ntvdm_shadow_reg_set_value_ex");
    wow_parent_query_value=(wow_registry_query_value_fn)GetProcAddress(parent,"ntvdm_shadow_reg_query_value");
    wow_parent_query_value_ex=(wow_registry_query_value_ex_fn)GetProcAddress(parent,"ntvdm_shadow_reg_query_value_ex");
    wow_parent_enum_key=(wow_registry_enum_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_enum_key");
    wow_parent_open_key_ex=(wow_registry_open_key_ex_fn)GetProcAddress(parent,"ntvdm_shadow_reg_open_key_ex");
    wow_parent_query_info_key=(wow_registry_query_info_key_fn)GetProcAddress(parent,"ntvdm_shadow_reg_query_info_key");
    wow_parent_enum_value=(wow_registry_enum_value_fn)GetProcAddress(parent,"ntvdm_shadow_reg_enum_value");
    return wow_parent_open_key && wow_parent_create_key && wow_parent_close_key &&
        wow_parent_delete_key && wow_parent_set_value && wow_parent_set_value_ex && wow_parent_query_value && wow_parent_query_value_ex &&
        wow_parent_enum_key && wow_parent_open_key_ex && wow_parent_query_info_key &&
        wow_parent_enum_value;
}

static BOOL wow_registry_parent_ready(void)
{
    return InitOnceExecuteOnce(&wow_registry_parent_once,
        wow_registry_parent_initialize,NULL,NULL);
}

typedef struct wow_registry_token {
    struct wow_registry_token *next;
    DWORD value;
    HKEY parent;
} wow_registry_token;

static INIT_ONCE wow_registry_once=INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION wow_registry_lock;
static wow_registry_token *wow_registry_tokens;
static DWORD wow_registry_next_token=1;

static BOOL CALLBACK wow_registry_initialize(PINIT_ONCE once, PVOID parameter,
    PVOID *context)
{
    (void)once;
    (void)parameter;
    (void)context;
    InitializeCriticalSection(&wow_registry_lock);
    return TRUE;
}

static BOOL wow_registry_ready(void)
{
    return InitOnceExecuteOnce(&wow_registry_once,wow_registry_initialize,NULL,NULL);
}

static BOOL wow_registry_predefined(HKEY key)
{
    return key==HKEY_CLASSES_ROOT || key==HKEY_LOCAL_MACHINE;
}

/* A token protects a parent raw key until the parent call returns.  Do not
 * resolve then unlock: another WOW thread could close the token and retire
 * the parent key between those two operations. */
static HKEY wow_registry_parent_key(HKEY key, BOOL *locked)
{
    wow_registry_token *entry;
    HKEY parent=NULL;

    if(locked)*locked=FALSE;
    if(wow_registry_predefined(key))return key;
    if(!key || !wow_registry_ready())return NULL;
    EnterCriticalSection(&wow_registry_lock);
    for(entry=wow_registry_tokens;entry;entry=entry->next)
        if(entry->value==(DWORD)(ULONG_PTR)key) { parent=entry->parent; break; }
    if(!parent)LeaveCriticalSection(&wow_registry_lock);
    else if(locked)*locked=TRUE;
    return parent;
}

static VOID wow_registry_parent_key_release(BOOL locked)
{
    if(locked)LeaveCriticalSection(&wow_registry_lock);
}

static LONG wow_registry_publish(HKEY parent, PHKEY result)
{
    wow_registry_token *entry;
    DWORD token;

    if(!parent || !result || !wow_registry_ready())return ERROR_INVALID_PARAMETER;
    entry=HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sizeof(*entry));
    if(!entry)return ERROR_NOT_ENOUGH_MEMORY;
    EnterCriticalSection(&wow_registry_lock);
    /* Zero is never a guest handle.  Do not reuse a value after wrap: a
     * stale DWORD must never become authority for a newly opened key. */
    if(!wow_registry_next_token) {
        LeaveCriticalSection(&wow_registry_lock);
        HeapFree(GetProcessHeap(),0,entry);
        return ERROR_NOT_ENOUGH_MEMORY;
    }
    token=wow_registry_next_token++;
    /* Tokens are never reused in a worker. Wrap is terminal rather than
     * allowing a stale guest DWORD to regain authority. */
    entry->value=token;
    entry->parent=parent;
    entry->next=wow_registry_tokens;
    wow_registry_tokens=entry;
    LeaveCriticalSection(&wow_registry_lock);
    *result=(HKEY)(ULONG_PTR)token;
    return ERROR_SUCCESS;
}

static LONG wow_registry_open(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result, BOOL create)
{
    BOOL locked;
    HKEY parent_root=wow_registry_parent_key(root,&locked),parent=NULL;
    LONG status;

    if(!result)return ERROR_INVALID_PARAMETER;
    *result=NULL;
    if(!parent_root)return ERROR_INVALID_HANDLE;
    if(!wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_NOT_READY; }
    status=create ? wow_parent_create_key(parent_root,subkey,&parent) :
        (options || access ? wow_parent_open_key_ex(parent_root,subkey,
            options,access,&parent) : wow_parent_open_key(parent_root,subkey,&parent));
    wow_registry_parent_key_release(locked);
    if(status!=ERROR_SUCCESS)return status;
    status=wow_registry_publish(parent,result);
    if(status!=ERROR_SUCCESS)(void)wow_parent_close_key(parent);
    return status;
}

LONG WINAPI wow_registry_open_key(HKEY root, LPCSTR subkey, PHKEY result)
{
    return wow_registry_open(root,subkey,0,0,result,FALSE);
}

LONG WINAPI wow_registry_create_key(HKEY root, LPCSTR subkey, PHKEY result)
{
    return wow_registry_open(root,subkey,0,0,result,TRUE);
}

LONG WINAPI wow_registry_close_key(HKEY key)
{
    wow_registry_token **link, *entry=NULL;
    LONG status;

    if(wow_registry_predefined(key)) return ERROR_SUCCESS;
    if(!key || !wow_registry_ready())
        return ERROR_INVALID_HANDLE;
    EnterCriticalSection(&wow_registry_lock);
    for(link=&wow_registry_tokens;*link;link=&(*link)->next)
        if((*link)->value==(DWORD)(ULONG_PTR)key) {
            entry=*link;
            *link=entry->next;
            break;
        }
    LeaveCriticalSection(&wow_registry_lock);
    if(!entry)return ERROR_INVALID_HANDLE;
    if(!wow_registry_parent_ready()) status=ERROR_NOT_READY;
    else status=wow_parent_close_key(entry->parent);
    HeapFree(GetProcessHeap(),0,entry);
    return status;
}

LONG WINAPI wow_registry_delete_key(HKEY key, LPCSTR subkey)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_delete_key(parent,subkey); wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_set_value(HKEY key, LPCSTR subkey, DWORD type,
    LPCSTR data, DWORD data_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_set_value(parent,subkey,type,data,data_size); wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_set_value_ex(HKEY key, LPCSTR value, DWORD reserved,
    DWORD type, const BYTE *data, DWORD data_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_set_value_ex(parent,value,reserved,type,data,data_size);
    wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_query_value(HKEY key, LPCSTR subkey, LPSTR data,
    PLONG data_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_query_value(parent,subkey,data,data_size); wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_query_value_ex(HKEY key, LPCSTR value, LPDWORD reserved,
    LPDWORD type, LPBYTE data, LPDWORD data_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_query_value_ex(parent,value,reserved,type,data,data_size);
    wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_enum_key(HKEY key, DWORD index, LPSTR name,
    DWORD name_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_enum_key(parent,index,name,name_size); wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_open_key_ex(HKEY root, LPCSTR subkey, DWORD options,
    REGSAM access, PHKEY result)
{
    return wow_registry_open(root,subkey,options,access,result,FALSE);
}

LONG WINAPI wow_registry_query_info_key(HKEY key, LPTSTR class_name,
    LPDWORD class_length, LPDWORD reserved, LPDWORD subkeys,
    LPDWORD maximum_subkey_length, LPDWORD maximum_class_length,
    LPDWORD values, LPDWORD maximum_value_name_length,
    LPDWORD maximum_value_data_length, LPDWORD security_descriptor,
    PFILETIME last_write_time)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_query_info_key(parent,class_name,class_length,reserved,subkeys,
        maximum_subkey_length,maximum_class_length,values,maximum_value_name_length,
        maximum_value_data_length,security_descriptor,last_write_time);
    wow_registry_parent_key_release(locked); return status;
}

LONG WINAPI wow_registry_enum_value(HKEY key, DWORD index, LPSTR value_name,
    LPDWORD value_name_size, LPDWORD reserved, LPDWORD type, LPBYTE data,
    LPDWORD data_size)
{
    BOOL locked; HKEY parent=wow_registry_parent_key(key,&locked); LONG status;
    if(!parent || !wow_registry_parent_ready()) { wow_registry_parent_key_release(locked); return ERROR_INVALID_HANDLE; }
    status=wow_parent_enum_value(parent,index,value_name,value_name_size,reserved,type,data,data_size);
    wow_registry_parent_key_release(locked); return status;
}

VOID WINAPI wow_registry_binding_shutdown(VOID)
{
    wow_registry_token *entry;
    if(!wow_registry_ready())return;
    EnterCriticalSection(&wow_registry_lock);
    entry=wow_registry_tokens;
    wow_registry_tokens=NULL;
    LeaveCriticalSection(&wow_registry_lock);
    while(entry) {
        wow_registry_token *next=entry->next;
        if(wow_registry_parent_ready()) (void)wow_parent_close_key(entry->parent);
        HeapFree(GetProcessHeap(),0,entry);
        entry=next;
    }
}
