#include "command_process_compat.h"
#include "vdmapi.h"

#include "adapter-mvdm-host-out/basesrv/include/mvdm_image_classification.h"

#include <stdio.h>
#include <string.h>

#undef SetStdHandle
#undef CreateProcess

typedef struct opennt_command_standard_handles {
    unsigned int initialized;
    unsigned int overridden[3];
    HANDLE values[3];
} opennt_command_standard_handles;

static __declspec(thread) opennt_command_standard_handles current_handles;

/* This adapter is reached only after the unchanged COMMAND source selected
 * its historical COMSPEC /c path.  NT4's cmd.exe could create a separate
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

static int opennt_command_simple_shell_tail(const char *tail)
{
    return tail != NULL && strpbrk(tail, "|&<>") == NULL;
}

static BOOL opennt_command_launch_vdm_child(
    const char *tail,
    LPSECURITY_ATTRIBUTES process_attributes,
    LPSECURITY_ATTRIBUTES thread_attributes,
    BOOL inherit_handles,
    DWORD creation_flags,
    LPVOID environment,
    LPCSTR current_directory,
    LPSTARTUPINFOA startup_info,
    LPPROCESS_INFORMATION process_information)
{
    char product[MAX_PATH];
    char child_command[MAX_PATH + MAXIMUM_VDM_COMMAND_LENGTH + 4u];
    DWORD product_bytes;
    int formatted;

    product_bytes = GetModuleFileNameA(NULL, product, (DWORD)sizeof(product));
    if (product_bytes == 0u || product_bytes >= sizeof(product)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    formatted = snprintf(child_command, sizeof(child_command), "\"%s\" %s",
        product, tail);
    if (formatted < 0 || (size_t)formatted >= sizeof(child_command)) {
        SetLastError(ERROR_FILENAME_EXCED_RANGE);
        return FALSE;
    }
    return CreateProcessA(NULL, child_command, process_attributes,
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
    mvdm_image_kind image_kind;
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
    if (opennt_command_simple_shell_tail(comspec_tail)) {
        /* DIVERGENCE(ADAPTER-WIN32-049): modern Windows has no NT4 VDM
         * disposition behind COMSPEC. Reuse the direct-entry classifier for
         * exactly one resolved tail; compound/native tails remain the
         * original CreateProcess path below. */
        image_kind = mvdm_image_classify_command_line(comspec_tail);
        if (image_kind == MVDM_IMAGE_DOS) {
            return opennt_command_launch_vdm_child(comspec_tail,
                process_attributes, thread_attributes, inherit_handles,
                creation_flags, environment, current_directory, effective_startup,
                process_information);
        }
        if (image_kind == MVDM_IMAGE_WIN16) {
            /* DIVERGENCE(ADAPTER-WIN32-049): NT4's COMSPEC could hand this
             * image to WOW.  Its bootstrap is not admitted yet; retain an
             * explicit worker-visible failure instead of invoking the modern
             * host's unsupported-16-bit dialog. */
            SetLastError(ERROR_NOT_SUPPORTED);
            return FALSE;
        }
    }
    return CreateProcessA(application_name, command_line,
        process_attributes, thread_attributes, inherit_handles,
        creation_flags, environment, current_directory, effective_startup,
        process_information);
}
