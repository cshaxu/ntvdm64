#include "command_process_compat.h"
#include "vdmapi.h"
#include "console_client.h"

#include <stdio.h>
#include <string.h>
#include <wchar.h>

#undef SetStdHandle
#undef CreateProcess

typedef struct opennt_command_standard_handles {
    unsigned int initialized;
    unsigned int overridden[3];
    HANDLE values[3];
} opennt_command_standard_handles;

static __declspec(thread) opennt_command_standard_handles current_handles;

/* This adapter is reached after the unchanged COMMAND source selected
 * its native executable or historical COMSPEC /c path. NT4 could create a separate
 * NTVDM for a DOS/NE image.  Current x64 Windows cannot.  Keep that product
 * boundary out of cmdexec.c: its worker still creates, waits for and returns
 * the child outcome through the original control flow. */
static const char *opennt_command_comspec_tail(const char *command_line)
{
    char comspec[MAX_PATH];
    const char *cursor;
    const char *begin;
    size_t bytes;
    DWORD result;

    if (command_line == NULL) return NULL;
    result = GetEnvironmentVariableA("COMSPEC", comspec, (DWORD)sizeof(comspec));
    if (result == 0u || result >= sizeof(comspec)) return NULL;
    cursor = command_line;
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    if (*cursor == '"') {
        begin = ++cursor;
        while (*cursor != '\0' && *cursor != '"') ++cursor;
        if (*cursor != '"') return NULL;
        bytes = (size_t)(cursor - begin);
        ++cursor;
    } else {
        begin = cursor;
        while (*cursor != '\0' && *cursor != ' ' && *cursor != '\t') ++cursor;
        bytes = (size_t)(cursor - begin);
    }
    if (bytes != strlen(comspec) || _strnicmp(begin, comspec, bytes) != 0)
        return NULL;
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    if (_strnicmp(cursor, "/c", 2u) != 0 ||
        (cursor[2] != ' ' && cursor[2] != '\t')) return NULL;
    cursor += 2;
    while (*cursor == ' ' || *cursor == '\t') ++cursor;
    return *cursor != '\0' ? cursor : NULL;
}

/* Quote one already-selected COMSPEC tail for CreateProcess.  This is only
 * the Win32 argv boundary: COMMAND.COM remains the sole owner of parsing the
 * tail's redirection, pipe and quoting grammar. */
static BOOL opennt_command_append_windows_argument(char **cursor,
                                                   const char *end,
                                                   const char *argument)
{
    const char *source;
    unsigned int slash_count = 0u;

    if (cursor == NULL || *cursor == NULL || end == NULL || argument == NULL ||
        *cursor >= end) return FALSE;
    if (*cursor + 1 >= end) return FALSE;
    *(*cursor)++ = '"';
    for (source = argument; *source != '\0'; ++source) {
        unsigned int index;

        if (*source == '\\') {
            ++slash_count;
            continue;
        }
        if (*source == '"') {
            for (index = 0u; index < slash_count * 2u + 1u; ++index) {
                if (*cursor >= end) return FALSE;
                *(*cursor)++ = '\\';
            }
            if (*cursor >= end) return FALSE;
            *(*cursor)++ = *source;
            slash_count = 0u;
            continue;
        }
        while (slash_count != 0u) {
            if (*cursor >= end) return FALSE;
            *(*cursor)++ = '\\';
            --slash_count;
        }
        if (*cursor >= end) return FALSE;
        *(*cursor)++ = *source;
    }
    while (slash_count != 0u) {
        if (*cursor + 1 >= end) return FALSE;
        *(*cursor)++ = '\\';
        *(*cursor)++ = '\\';
        --slash_count;
    }
    if (*cursor + 1 >= end) return FALSE;
    *(*cursor)++ = '"';
    **cursor = '\0';
    return TRUE;
}

static int opennt_command_nested_comspec_tail(const char *tail)
{
    const char *cursor=tail;
    const char *begin;
    size_t bytes;

    if (cursor==NULL) return 0;
    while (*cursor==' ' || *cursor=='\t') ++cursor;
    begin=cursor;
    while (*cursor!='\0' && *cursor!=' ' && *cursor!='\t') ++cursor;
    bytes=(size_t)(cursor-begin);
    if (bytes!=sizeof("COMMAND.COM")-1u ||
        _strnicmp(begin,"COMMAND.COM",bytes)!=0) return 0;
    while (*cursor==' ' || *cursor=='\t') ++cursor;
    return _strnicmp(cursor,"/c",2u)==0 &&
        (cursor[2]==' ' || cursor[2]=='\t');
}

static int opennt_command_simple_shell_tail(const char *tail)
{
    return tail != NULL && strpbrk(tail, "|&<>") == NULL &&
        !opennt_command_nested_comspec_tail(tail);
}

/* The locator is added only to the native child block, never guest memory.
 * The inherited handle is a restricted duplicate of the broker-proven root. */
static BOOL create_frontend_child(LPCSTR application,LPSTR command,
    LPSECURITY_ATTRIBUTES process_attributes,LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit,DWORD flags,LPVOID environment,LPCSTR directory,
    LPSTARTUPINFOA startup,LPPROCESS_INFORMATION process)
{
    static const char name[]="NTVDM_FRONTEND_CAPABILITY=";
    HANDLE capability=NULL;
    BOOL wide=(flags&CREATE_UNICODE_ENVIRONMENT)!=0,result;
    void *inherited=NULL,*copy=NULL;
    const BYTE *cursor;
    BYTE *destination;
    size_t unit=wide ? sizeof(WCHAR) : 1,bytes=unit,length,entry_bytes,index;
    char value[80];
    WCHAR wide_value[80];
    DWORD error;
    if (!ntvdm_console_inherit_frontend_capability(&capability)) return FALSE;
    if (!capability) return CreateProcessA(application,command,process_attributes,
        thread_attributes,inherit,flags,environment,directory,startup,process);
    if (!inherit) { CloseHandle(capability);SetLastError(ERROR_INVALID_PARAMETER);return FALSE; }
    sprintf_s(value,sizeof(value),"%s%lx",name,(unsigned long)(ULONG_PTR)capability);
    for (index=0;index<=strlen(value);++index) wide_value[index]=(WCHAR)value[index];
    if (!environment) {
        inherited=wide ? (void *)GetEnvironmentStringsW() : (void *)GetEnvironmentStringsA();
        if (!inherited) { error=GetLastError();CloseHandle(capability);SetLastError(error);return FALSE; }
        environment=inherited;
    }
    for (cursor=environment;(length=wide ? wcslen((const WCHAR *)cursor) : strlen((const char *)cursor))!=0;
         cursor+=(length+1)*unit) bytes+=(length+1)*unit;
    bytes+=(strlen(value)+1)*unit;
    copy=HeapAlloc(GetProcessHeap(),0,bytes);
    if (!copy) { error=ERROR_NOT_ENOUGH_MEMORY;result=FALSE;goto done; }
    destination=copy;
    for (cursor=environment;(length=wide ? wcslen((const WCHAR *)cursor) : strlen((const char *)cursor))!=0;
         cursor+=(length+1)*unit) {
        if (wide ? !_wcsnicmp((const WCHAR *)cursor,L"NTVDM_FRONTEND_CAPABILITY=",sizeof(name)-1) :
                   !_strnicmp((const char *)cursor,name,sizeof(name)-1)) continue;
        entry_bytes=(length+1)*unit;
        memcpy(destination,cursor,entry_bytes);destination+=entry_bytes;
    }
    entry_bytes=(strlen(value)+1)*unit;
    memcpy(destination,wide ? (const void *)wide_value : (const void *)value,entry_bytes);
    destination+=entry_bytes;
    ZeroMemory(destination,unit);
    result=CreateProcessA(application,command,process_attributes,thread_attributes,
        inherit,flags,copy,directory,startup,process);
    error=GetLastError();
done:
    if (copy) HeapFree(GetProcessHeap(),0,copy);
    if (inherited) {
        if (wide) FreeEnvironmentStringsW(inherited);else FreeEnvironmentStringsA(inherited);
    }
    CloseHandle(capability);SetLastError(error);
    return result;
}

static BOOL opennt_command_launch_vdm_child(
    const char *tail,
    BOOL direct_command,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information)
{
    char launcher[MAX_PATH];
    char child_command[MAX_PATH * 2u + MAXIMUM_VDM_COMMAND_LENGTH * 2u + 32u];
    char *leaf;
    DWORD launcher_bytes;
    int formatted;
    char *tail_cursor;

    /* COMMAND has selected either a direct native command or a COMSPEC tail.
     * Use the sibling public launcher for both: it owns native target lifetime
     * and the original BaseClient admission of DOS/NE targets. Original
     * cmdCreateProcess still owns suspension, waiting and guest re-entry. */
    launcher_bytes = GetModuleFileNameA(NULL, launcher, (DWORD)sizeof(launcher));
    if (launcher_bytes == 0u || launcher_bytes >= sizeof(launcher) ||
        (leaf = strrchr(launcher, '\\')) == NULL ||
        (size_t)(leaf - launcher) + sizeof("run16.exe") >= sizeof(launcher)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    memcpy(leaf + 1, "run16.exe", sizeof("run16.exe"));
    if (direct_command || opennt_command_simple_shell_tail(tail)) {
        formatted = snprintf(child_command, sizeof(child_command),
            "\"%s\" %s", launcher, tail);
        if (formatted < 0 || (size_t)formatted >= sizeof(child_command)) {
            SetLastError(ERROR_FILENAME_EXCED_RANGE);
            return FALSE;
        }
        return create_frontend_child(NULL, child_command, process_attributes,
            thread_attributes, inherit_handles, creation_flags, environment,
            current_directory, startup_info, process_information);
    }
    formatted = snprintf(child_command, sizeof(child_command),
        "\"%s\" COMMAND.COM /c ", launcher);
    if (formatted < 0 || (size_t)formatted >= sizeof(child_command)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    tail_cursor = child_command + formatted;
    if (!opennt_command_append_windows_argument(&tail_cursor,
            child_command + sizeof(child_command) - 1u, tail)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    return create_frontend_child(NULL, child_command, process_attributes,
        thread_attributes, inherit_handles, creation_flags, environment,
        current_directory, startup_info, process_information);
}

static int opennt_command_standard_handle_index(DWORD standard_handle)
{
    switch (standard_handle) {
    case STD_INPUT_HANDLE: return 0;
    case STD_OUTPUT_HANDLE: return 1;
    case STD_ERROR_HANDLE: return 2;
    default: return -1;
    }
}

static void opennt_command_initialize_standard_handles(void)
{
    if (current_handles.initialized != 0u) return;
    current_handles.values[0] = GetStdHandle(STD_INPUT_HANDLE);
    current_handles.values[1] = GetStdHandle(STD_OUTPUT_HANDLE);
    current_handles.values[2] = GetStdHandle(STD_ERROR_HANDLE);
    current_handles.initialized = 1u;
}

BOOL opennt_command_set_std_handle(DWORD standard_handle, HANDLE handle)
{
    int index = opennt_command_standard_handle_index(standard_handle);
    if (index < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    current_handles.values[index] = handle;
    current_handles.overridden[index] = 1u;
    return TRUE;
}

BOOL opennt_command_create_process_a(
    LPCSTR application_name,
    LPSTR command_line,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information)
{
    STARTUPINFOA local_startup;
    LPSTARTUPINFOA effective_startup;
    const char *comspec_tail;
    int use_child_streams;

    if (startup_info == NULL) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    opennt_command_initialize_standard_handles();
    use_child_streams = current_handles.overridden[0] != 0u ||
        current_handles.overridden[1] != 0u ||
        current_handles.overridden[2] != 0u;
    effective_startup = startup_info;
    if (use_child_streams) {
        local_startup = *startup_info;
        local_startup.dwFlags |= STARTF_USESTDHANDLES;
        local_startup.hStdInput = current_handles.values[0];
        local_startup.hStdOutput = current_handles.values[1];
        local_startup.hStdError = current_handles.values[2];
        effective_startup = &local_startup;
    }
    comspec_tail = opennt_command_comspec_tail(command_line);
    if (comspec_tail != NULL) {
        /* BOP 54:08's COMMAND worker has already selected its COMSPEC /c
         * execution boundary.  It must not classify the child: relaunch this
         * product and let its single app-entry disposition resolve DOS/Win16,
         * native PE, and an unresolved shell token in one place. */
        return opennt_command_launch_vdm_child(comspec_tail, FALSE,
            process_attributes, thread_attributes, inherit_handles,
            creation_flags, environment, current_directory, effective_startup,
            process_information);
    }
    /* cmdCreateProcess also receives resolved native images without COMSPEC.
     * Keep its original suspended-create/wait/re-entry contract, but let the
     * same launcher own this native target and its frontend lifetime pair.
     * The already selected command line is not shell syntax to reinterpret. */
    if (application_name == NULL && command_line != NULL && *command_line) {
        return opennt_command_launch_vdm_child(command_line, TRUE,
            process_attributes, thread_attributes, inherit_handles,
            creation_flags, environment, current_directory, effective_startup,
            process_information);
    }
    return create_frontend_child(application_name, command_line,
        process_attributes, thread_attributes, inherit_handles,
        creation_flags, environment, current_directory, effective_startup,
        process_information);
}
