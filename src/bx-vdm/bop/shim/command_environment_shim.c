/* Compatibility globals and private RTL-environment subset for directly
 * imported OpenNT cmdenv.c.  They replace only the non-composable
 * command.lib/SoftPC process-environment closure; the OpenNT transformation
 * algorithm remains in cmdenv.c. */

#include "command_misc_shim.h"

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <wchar.h>
#include <wctype.h>

#define BX_NTVDM_STATUS_NO_MEMORY ((NTSTATUS)0xc0000017L)
#define BX_NTVDM_STATUS_INVALID_PARAMETER ((NTSTATUS)0xc000000dL)

static const CHAR bx_ntvdm_empty_multisz[2] = { '\0', '\0' };

static size_t multisz_wchars(const WCHAR *strings)
{
    size_t index = 0u;
    if (strings == NULL) return 0u;
    for (;;) {
        if (strings[index] == L'\0' && strings[index + 1u] == L'\0')
            return index + 2u;
        ++index;
    }
}

static int unicode_name_compare(const WCHAR *left, size_t left_chars,
    const WCHAR *right, size_t right_chars)
{
    size_t index;
    for (index = 0u; index < left_chars && index < right_chars; ++index) {
        WCHAR a = (WCHAR)towupper(left[index]);
        WCHAR b = (WCHAR)towupper(right[index]);
        if (a != b) return a < b ? -1 : 1;
    }
    return left_chars == right_chars ? 0 : (left_chars < right_chars ? -1 : 1);
}

void RtlInitUnicodeString(PUNICODE_STRING destination, const WCHAR *source)
{
    size_t chars;
    if (destination == NULL) return;
    if (source == NULL) {
        destination->Length = 0u;
        destination->MaximumLength = 0u;
        destination->Buffer = NULL;
        return;
    }
    chars = wcslen(source);
    if (chars > (USHRT_MAX / sizeof(WCHAR)) - 1u) {
        destination->Length = 0u;
        destination->MaximumLength = 0u;
        destination->Buffer = NULL;
        return;
    }
    destination->Length = (USHORT)(chars * sizeof(WCHAR));
    destination->MaximumLength = (USHORT)((chars + 1u) * sizeof(WCHAR));
    destination->Buffer = (PWSTR)source;
}

NTSTATUS RtlCreateEnvironment(BOOLEAN clone_current, PVOID *environment)
{
    PWCHAR result;
    (void)clone_current;
    if (environment == NULL) return BX_NTVDM_STATUS_INVALID_PARAMETER;
    result = (PWCHAR)calloc(2u, sizeof(WCHAR));
    if (result == NULL) return BX_NTVDM_STATUS_NO_MEMORY;
    *environment = result;
    return 0;
}

void RtlDestroyEnvironment(PVOID environment)
{
    free(environment);
}

NTSTATUS RtlSetEnvironmentVariable(PVOID *environment,
    const PUNICODE_STRING name, const PUNICODE_STRING value)
{
    PWCHAR source;
    PWCHAR replacement;
    size_t source_chars;
    size_t name_chars;
    size_t value_chars;
    size_t replacement_chars;
    size_t cursor;
    size_t output;
    int emitted = 0;
    if (environment == NULL || *environment == NULL || name == NULL ||
        name->Buffer == NULL || (name->Length % sizeof(WCHAR)) != 0u)
        return BX_NTVDM_STATUS_INVALID_PARAMETER;
    name_chars = name->Length / sizeof(WCHAR);
    if (name_chars == 0u || (value != NULL &&
        (value->Buffer == NULL || (value->Length % sizeof(WCHAR)) != 0u)))
        return BX_NTVDM_STATUS_INVALID_PARAMETER;
    value_chars = value == NULL ? 0u : value->Length / sizeof(WCHAR);
    source = (PWCHAR)*environment;
    source_chars = multisz_wchars(source);
    if (source_chars < 2u || source_chars > (USHRT_MAX / sizeof(WCHAR)))
        return BX_NTVDM_STATUS_INVALID_PARAMETER;
    replacement_chars = source_chars + (value == NULL ? 0u : name_chars + value_chars + 1u);
    if (replacement_chars > (USHRT_MAX / sizeof(WCHAR)))
        return BX_NTVDM_STATUS_NO_MEMORY;
    replacement = (PWCHAR)calloc(replacement_chars, sizeof(WCHAR));
    if (replacement == NULL) return BX_NTVDM_STATUS_NO_MEMORY;
    cursor = 0u;
    output = 0u;
    while (cursor + 1u < source_chars && source[cursor] != L'\0') {
        WCHAR *entry = source + cursor;
        /* Drive-current-directory entries have the original `=C:=path`
         * spelling.  Their leading '=' belongs to the name, so find the
         * separator after it exactly as cmdenv.c does. */
        WCHAR *equals = wcschr(entry + (entry[0] == L'=' ? 1u : 0u), L'=');
        size_t entry_chars = wcslen(entry);
        size_t entry_name_chars = equals == NULL ? entry_chars : (size_t)(equals - entry);
        int compare = unicode_name_compare(name->Buffer, name_chars, entry,
            entry_name_chars);
        if (!emitted && value != NULL && compare < 0) {
            memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR));
            output += name_chars;
            replacement[output++] = L'=';
            memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR));
            output += value_chars;
            replacement[output++] = L'\0';
            emitted = 1;
        }
        if (compare != 0) {
            memcpy(replacement + output, entry, (entry_chars + 1u) * sizeof(WCHAR));
            output += entry_chars + 1u;
        } else if (!emitted && value != NULL) {
            memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR));
            output += name_chars;
            replacement[output++] = L'=';
            memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR));
            output += value_chars;
            replacement[output++] = L'\0';
            emitted = 1;
        }
        cursor += entry_chars + 1u;
    }
    if (!emitted && value != NULL) {
        memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR));
        output += name_chars;
        replacement[output++] = L'=';
        memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR));
        output += value_chars;
        replacement[output++] = L'\0';
    }
    replacement[output++] = L'\0';
    free(source);
    *environment = replacement;
    return 0;
}

PWCHAR bx_ntvdm_command_environment_snapshot_session(
    const bx_ntvdm_command_misc_session *session)
{
    const CHAR *source = bx_ntvdm_empty_multisz;
    uint32_t bytes = sizeof(bx_ntvdm_empty_multisz);
    int characters;
    PWCHAR snapshot;
    if (session != NULL && bx_ntvdm_command_misc_session_valid(session) &&
        session->command_source_environment != NULL &&
        session->command_source_environment_bytes >= 2u) {
        source = session->command_source_environment;
        bytes = session->command_source_environment_bytes;
    }
    if (bytes > (uint32_t)INT_MAX || source[bytes - 2u] != '\0' ||
        source[bytes - 1u] != '\0') return NULL;
    characters = MultiByteToWideChar(CP_ACP, 0, source, (int)bytes, NULL, 0);
    if (characters <= 0) return NULL;
    snapshot = (PWCHAR)malloc((size_t)characters * sizeof(WCHAR));
    if (snapshot == NULL) return NULL;
    if (MultiByteToWideChar(CP_ACP, 0, source, (int)bytes, snapshot,
            characters) != characters) {
        free(snapshot);
        return NULL;
    }
    return snapshot;
}

PWCHAR bx_ntvdm_command_environment_snapshot(void)
{
    return bx_ntvdm_command_environment_snapshot_session(
        bx_ntvdm_command_misc_active_session());
}

void bx_ntvdm_command_environment_free_snapshot(PWCHAR snapshot)
{
    free(snapshot);
}

BOOL fSeparateWow;
CHAR comspec[] = "COMSPEC=";
CHAR *lpszzInitEnvironment;
WORD cchInitEnvironment;
CHAR *lpszzVDMEnv32;
DWORD cchVDMEnv32;

int RcMessageBox(UINT error, PVOID first, PVOID second, UINT flags)
{
    return bx_ntvdm_opennt_rc_message_box(error, (CHAR *)first,
        (CHAR *)second, (ULONG)flags);
}
