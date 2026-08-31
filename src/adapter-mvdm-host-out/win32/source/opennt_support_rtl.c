/*
 * DIVERGENCE(ADAPTER-WIN32-001): NT4 supplied these interfaces from its
 * private user-mode RTL/PEB product composition.  The original MVDM support
 * sources keep their declarations and call order; this adapter implements
 * the reached subset with public Win32 APIs.  The compatibility TEB/PEB is
 * adapter-private and never exposes a real host pointer to a guest boundary.
 */
#include <nt.h>
#include <ntrtl.h>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

typedef struct _OPENNT_SUPPORT_THREAD_STATE {
    WCHAR StaticBuffer[32767];
    OPENNT_SUPPORT_TEB Teb;
    OPENNT_SUPPORT_PROCESS_PARAMETERS Parameters;
    OPENNT_SUPPORT_PEB Peb;
    WCHAR CurrentDirectory[MAX_PATH + 2];
} OPENNT_SUPPORT_THREAD_STATE;

static __declspec(thread) OPENNT_SUPPORT_THREAD_STATE opennt_support_thread_state;
static INIT_ONCE opennt_support_lock_once = INIT_ONCE_STATIC_INIT;
static CRITICAL_SECTION opennt_support_lock;

static BOOL CALLBACK opennt_support_initialize_lock(PINIT_ONCE once, PVOID parameter, PVOID *context)
{
    (void)once;
    (void)parameter;
    (void)context;
    InitializeCriticalSection(&opennt_support_lock);
    return TRUE;
}

static OPENNT_SUPPORT_THREAD_STATE *opennt_support_thread(void)
{
    OPENNT_SUPPORT_THREAD_STATE *state = &opennt_support_thread_state;
    if (state->Teb.StaticUnicodeString.Buffer == NULL) {
        state->Teb.StaticUnicodeString.Buffer = state->StaticBuffer;
        state->Teb.StaticUnicodeString.MaximumLength = (USHORT)sizeof(state->StaticBuffer);
        state->Parameters.CurrentDirectory.DosPath.Buffer = state->CurrentDirectory;
        state->Parameters.CurrentDirectory.DosPath.MaximumLength = (USHORT)sizeof(state->CurrentDirectory);
        state->Parameters.ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        state->Peb.ProcessParameters = &state->Parameters;
        state->Peb.ProcessHeap = GetProcessHeap();
    }
    return state;
}

static NTSTATUS opennt_support_last_error_status(void)
{
    DWORD error = GetLastError();
    return error == ERROR_NOT_ENOUGH_MEMORY ? STATUS_NO_MEMORY : STATUS_INVALID_PARAMETER;
}

/* DIVERGENCE(ADAPTER-WIN32-026): NT4 supplied the Rtl environment block
 * routines from its private RTL/PEB composition.  Retain their counted-string
 * and mutable MULTI_SZ contract with public Win32 heap/environment APIs; the
 * block remains host-private and is never a guest pointer or mapping token. */
static NTSTATUS opennt_support_environment_units(PWCHAR environment, SIZE_T *units)
{
    SIZE_T index;
    if (environment == NULL || units == NULL) return STATUS_INVALID_PARAMETER;
    for (index = 0; index < 32767u; ++index) {
        if (environment[index] == L'\0' && environment[index + 1u] == L'\0') {
            *units = index + 2u;
            return STATUS_SUCCESS;
        }
    }
    return STATUS_INVALID_PARAMETER;
}

static SIZE_T opennt_support_environment_name_length(PCWSTR entry)
{
    SIZE_T index = entry[0] == L'=' ? 1u : 0u;
    while (entry[index] != L'=' && entry[index] != L'\0') ++index;
    return index;
}

static int opennt_support_compare_environment_name(PCWSTR entry, PCUNICODE_STRING name)
{
    SIZE_T entry_length = opennt_support_environment_name_length(entry);
    SIZE_T name_length = name->Length / sizeof(WCHAR);
    int comparison;
    comparison = CompareStringOrdinal(entry, (int)entry_length, name->Buffer, (int)name_length, TRUE);
    return comparison == CSTR_LESS_THAN ? -1 : comparison == CSTR_GREATER_THAN ? 1 : 0;
}

static VOID opennt_support_copy_environment_assignment(PWCHAR *destination, PCUNICODE_STRING name, PCUNICODE_STRING value)
{
    SIZE_T name_units = name->Length / sizeof(WCHAR);
    SIZE_T value_units = value->Length / sizeof(WCHAR);
    memcpy(*destination, name->Buffer, name->Length);
    *destination += name_units;
    *(*destination)++ = L'=';
    if (value_units != 0u) {
        memcpy(*destination, value->Buffer, value->Length);
        *destination += value_units;
    }
    *(*destination)++ = L'\0';
}

NTSTATUS NTAPI RtlCreateEnvironment(BOOLEAN clone_current_environment, PVOID *environment)
{
    PWCHAR source = NULL;
    PWCHAR copy;
    SIZE_T units;
    NTSTATUS status;
    if (environment == NULL) return STATUS_INVALID_PARAMETER;
    if (clone_current_environment) {
        source = GetEnvironmentStringsW();
        if (source == NULL) return opennt_support_last_error_status();
        status = opennt_support_environment_units(source, &units);
        if (!NT_SUCCESS(status)) {
            FreeEnvironmentStringsW(source);
            return status;
        }
    } else {
        units = 2u;
    }
    if (units > SIZE_MAX / sizeof(WCHAR)) {
        if (source != NULL) FreeEnvironmentStringsW(source);
        return STATUS_NO_MEMORY;
    }
    copy = (PWCHAR)HeapAlloc(GetProcessHeap(), 0, units * sizeof(WCHAR));
    if (copy == NULL) {
        if (source != NULL) FreeEnvironmentStringsW(source);
        return STATUS_NO_MEMORY;
    }
    if (source != NULL) {
        memcpy(copy, source, units * sizeof(WCHAR));
        FreeEnvironmentStringsW(source);
    } else {
        copy[0] = L'\0';
        copy[1] = L'\0';
    }
    *environment = copy;
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI RtlDestroyEnvironment(PVOID environment)
{
    if (environment == NULL) return STATUS_INVALID_PARAMETER;
    HeapFree(GetProcessHeap(), 0, environment);
    return STATUS_SUCCESS;
}

NTSTATUS NTAPI RtlSetEnvironmentVariable(PVOID *environment, PCUNICODE_STRING name, PCUNICODE_STRING value)
{
    PWCHAR source;
    PWCHAR replacement;
    PWCHAR cursor;
    PWCHAR output;
    SIZE_T source_units;
    SIZE_T name_units;
    SIZE_T value_units = 0u;
    SIZE_T replacement_units;
    SIZE_T assignment_units = 0u;
    BOOL found = FALSE;
    BOOL inserted = FALSE;
    NTSTATUS status;

    if (environment == NULL || name == NULL || name->Buffer == NULL || name->Length == 0u || (name->Length % sizeof(WCHAR)) != 0u || (value != NULL && (value->Length % sizeof(WCHAR)) != 0u) || (value != NULL && value->Length != 0u && value->Buffer == NULL)) return STATUS_INVALID_PARAMETER;
    if (*environment == NULL) {
        status = RtlCreateEnvironment(FALSE, environment);
        if (!NT_SUCCESS(status)) return status;
    }
    source = (PWCHAR)*environment;
    status = opennt_support_environment_units(source, &source_units);
    if (!NT_SUCCESS(status)) return status;
    name_units = name->Length / sizeof(WCHAR);
    if (value != NULL) {
        value_units = value->Length / sizeof(WCHAR);
        if (name_units > SIZE_MAX - value_units - 2u) return STATUS_NO_MEMORY;
        assignment_units = name_units + value_units + 2u;
    }

    replacement_units = 1u;
    for (cursor = source; *cursor != L'\0'; cursor += wcslen(cursor) + 1u) {
        SIZE_T entry_units = wcslen(cursor) + 1u;
        int comparison = opennt_support_compare_environment_name(cursor, name);
        if (comparison == 0) {
            found = TRUE;
            if (value != NULL) replacement_units += assignment_units;
        } else {
            replacement_units += entry_units;
        }
    }
    if (value != NULL && !found) replacement_units += assignment_units;
    if (replacement_units > SIZE_MAX / sizeof(WCHAR)) return STATUS_NO_MEMORY;
    replacement = (PWCHAR)HeapAlloc(GetProcessHeap(), 0, replacement_units * sizeof(WCHAR));
    if (replacement == NULL) return STATUS_NO_MEMORY;

    output = replacement;
    for (cursor = source; *cursor != L'\0'; cursor += wcslen(cursor) + 1u) {
        SIZE_T entry_units = wcslen(cursor) + 1u;
        int comparison = opennt_support_compare_environment_name(cursor, name);
        if (value != NULL && !inserted && comparison > 0) {
            opennt_support_copy_environment_assignment(&output, name, value);
            inserted = TRUE;
        }
        if (comparison == 0) continue;
        memcpy(output, cursor, entry_units * sizeof(WCHAR));
        output += entry_units;
    }
    if (value != NULL && !inserted) opennt_support_copy_environment_assignment(&output, name, value);
    *output = L'\0';
    HeapFree(GetProcessHeap(), 0, source);
    *environment = replacement;
    return STATUS_SUCCESS;
}

static NTSTATUS opennt_support_convert_to_unicode(PUNICODE_STRING destination, PCOEM_STRING source, UINT code_page, BOOLEAN allocate)
{
    int characters;
    SIZE_T bytes;
    if (destination == NULL || source == NULL || (source->Length != 0 && source->Buffer == NULL)) return STATUS_INVALID_PARAMETER;
    characters = MultiByteToWideChar(code_page, 0, source->Buffer, source->Length, NULL, 0);
    if (characters == 0 && source->Length != 0) return opennt_support_last_error_status();
    bytes = ((SIZE_T)characters + 1u) * sizeof(WCHAR);
    if (bytes > USHRT_MAX) return STATUS_BUFFER_OVERFLOW;
    if (allocate) {
        destination->Buffer = (PWSTR)HeapAlloc(GetProcessHeap(), 0, bytes);
        if (destination->Buffer == NULL) return STATUS_NO_MEMORY;
        destination->MaximumLength = (USHORT)bytes;
    } else if (destination->Buffer == NULL || destination->MaximumLength < bytes) {
        return STATUS_BUFFER_OVERFLOW;
    }
    if (characters != 0 && MultiByteToWideChar(code_page, 0, source->Buffer, source->Length, destination->Buffer, characters) != characters) {
        if (allocate) RtlFreeUnicodeString(destination);
        return opennt_support_last_error_status();
    }
    destination->Buffer[characters] = L'\0';
    destination->Length = (USHORT)(characters * sizeof(WCHAR));
    return 0;
}

static NTSTATUS opennt_support_convert_from_unicode(PANSI_STRING destination, PCUNICODE_STRING source, UINT code_page, BOOLEAN allocate)
{
    int bytes;
    SIZE_T needed;
    if (destination == NULL || source == NULL || (source->Length != 0 && source->Buffer == NULL) || (source->Length % sizeof(WCHAR)) != 0) return STATUS_INVALID_PARAMETER;
    bytes = WideCharToMultiByte(code_page, 0, source->Buffer, source->Length / sizeof(WCHAR), NULL, 0, NULL, NULL);
    if (bytes == 0 && source->Length != 0) return opennt_support_last_error_status();
    needed = (SIZE_T)bytes + 1u;
    if (needed > USHRT_MAX) return STATUS_BUFFER_OVERFLOW;
    if (allocate) {
        destination->Buffer = (PCHAR)HeapAlloc(GetProcessHeap(), 0, needed);
        if (destination->Buffer == NULL) return STATUS_NO_MEMORY;
        destination->MaximumLength = (USHORT)needed;
    } else if (destination->Buffer == NULL || destination->MaximumLength < needed) {
        return STATUS_BUFFER_OVERFLOW;
    }
    if (bytes != 0 && WideCharToMultiByte(code_page, 0, source->Buffer, source->Length / sizeof(WCHAR), destination->Buffer, bytes, NULL, NULL) != bytes) {
        if (allocate) RtlFreeAnsiString(destination);
        return opennt_support_last_error_status();
    }
    destination->Buffer[bytes] = '\0';
    destination->Length = (USHORT)bytes;
    return 0;
}

POPENNT_SUPPORT_TEB NTAPI opennt_support_current_teb(VOID)
{
    return &opennt_support_thread()->Teb;
}

POPENNT_SUPPORT_PEB NTAPI NtCurrentPeb(VOID)
{
    OPENNT_SUPPORT_THREAD_STATE *state = opennt_support_thread();
    DWORD characters = GetCurrentDirectoryW((DWORD)(sizeof(state->CurrentDirectory) / sizeof(state->CurrentDirectory[0])), state->CurrentDirectory);
    if (characters >= (sizeof(state->CurrentDirectory) / sizeof(state->CurrentDirectory[0]))) characters = 0;
    state->Parameters.CurrentDirectory.DosPath.Length = (USHORT)(characters * sizeof(WCHAR));
    return &state->Peb;
}

PVOID NTAPI RtlProcessHeap(VOID) { return GetProcessHeap(); }
PVOID NTAPI RtlAllocateHeap(PVOID heap, ULONG flags, SIZE_T size) { return HeapAlloc((HANDLE)heap, flags, size); }
BOOLEAN NTAPI RtlFreeHeap(PVOID heap, ULONG flags, PVOID value) { (void)flags; return (BOOLEAN)HeapFree((HANDLE)heap, 0, value); }
VOID NTAPI RtlInitString(PSTRING value, PCSZ text) { RtlInitAnsiString((PANSI_STRING)value, text); }
VOID NTAPI RtlInitAnsiString(PANSI_STRING value, PCSZ text) { SIZE_T length = text == NULL ? 0u : strlen(text); if (value != NULL) { value->Buffer = (PCHAR)text; value->Length = (USHORT)(length > USHRT_MAX ? USHRT_MAX : length); value->MaximumLength = (USHORT)(value->Length == USHRT_MAX ? USHRT_MAX : value->Length + 1u); } }
VOID NTAPI RtlInitUnicodeString(PUNICODE_STRING value, PCWSTR text) { SIZE_T length = text == NULL ? 0u : wcslen(text); if (value != NULL) { value->Buffer = (PWSTR)text; value->Length = (USHORT)(length > (USHRT_MAX / sizeof(WCHAR)) ? 0u : length * sizeof(WCHAR)); value->MaximumLength = (USHORT)(value->Length == 0u && length != 0u ? 0u : value->Length + sizeof(WCHAR)); } }
NTSTATUS NTAPI RtlOemStringToUnicodeString(PUNICODE_STRING destination, PCOEM_STRING source, BOOLEAN allocate) {
    OPENNT_SUPPORT_THREAD_STATE *state = opennt_support_thread();
    /* DIVERGENCE(ADAPTER-WIN32-002): the reached OpenNT bodies reuse
     * TEB.StaticUnicodeString as scratch after RtlInitUnicodeString has
     * temporarily pointed it at a caller buffer. Restore adapter-private TLS
     * capacity instead of exposing a real host TEB. */
    if (!allocate && destination == &state->Teb.StaticUnicodeString) {
        destination->Buffer = state->StaticBuffer;
        destination->MaximumLength = (USHORT)sizeof(state->StaticBuffer);
    }
    return opennt_support_convert_to_unicode(destination, source, CP_OEMCP, allocate);
}
/* DIVERGENCE(ADAPTER-WIN32-027): preserve the original fixed-buffer RTL
 * contract while routing its OEM conversion through the public modern API.
 * The caller owns both buffers; no host pointer becomes a guest identity. */
NTSTATUS NTAPI RtlOemToUnicodeN(PWSTR destination, ULONG maximum_bytes,
                                PULONG converted_bytes, PCHAR source,
                                ULONG source_bytes) {
    int characters;
    ULONG required_bytes;
    if (converted_bytes != NULL) *converted_bytes = 0u;
    if (source_bytes == 0u) return STATUS_SUCCESS;
    if (source == NULL || destination == NULL || source_bytes > (ULONG)INT_MAX) {
        return STATUS_INVALID_PARAMETER;
    }
    characters = MultiByteToWideChar(CP_OEMCP, 0, source, (int)source_bytes,
                                     NULL, 0);
    if (characters == 0 && source_bytes != 0u) return opennt_support_last_error_status();
    if ((ULONG)characters > ULONG_MAX / (ULONG)sizeof(WCHAR)) return STATUS_BUFFER_OVERFLOW;
    required_bytes = (ULONG)characters * (ULONG)sizeof(WCHAR);
    if (converted_bytes != NULL) *converted_bytes = required_bytes;
    if (required_bytes > maximum_bytes) return STATUS_BUFFER_OVERFLOW;
    if (characters != 0 &&
        MultiByteToWideChar(CP_OEMCP, 0, source, (int)source_bytes,
                            destination, characters) != characters) {
        return opennt_support_last_error_status();
    }
    return STATUS_SUCCESS;
}
NTSTATUS NTAPI RtlAnsiStringToUnicodeString(PUNICODE_STRING destination, PCANSI_STRING source, BOOLEAN allocate) { return opennt_support_convert_to_unicode(destination, (PCOEM_STRING)source, CP_ACP, allocate); }
NTSTATUS NTAPI RtlUnicodeStringToOemString(POEM_STRING destination, PCUNICODE_STRING source, BOOLEAN allocate) { return opennt_support_convert_from_unicode((PANSI_STRING)destination, source, CP_OEMCP, allocate); }
NTSTATUS NTAPI RtlUnicodeStringToAnsiString(PANSI_STRING destination, PCUNICODE_STRING source, BOOLEAN allocate) { return opennt_support_convert_from_unicode(destination, source, CP_ACP, allocate); }
VOID NTAPI RtlFreeUnicodeString(PUNICODE_STRING value) { if (value != NULL && value->Buffer != NULL) HeapFree(GetProcessHeap(), 0, value->Buffer); if (value != NULL) { value->Buffer = NULL; value->Length = value->MaximumLength = 0; } }
VOID NTAPI RtlFreeOemString(POEM_STRING value) { RtlFreeAnsiString((PANSI_STRING)value); }
VOID NTAPI RtlFreeAnsiString(PANSI_STRING value) { if (value != NULL && value->Buffer != NULL) HeapFree(GetProcessHeap(), 0, value->Buffer); if (value != NULL) { value->Buffer = NULL; value->Length = value->MaximumLength = 0; } }
BOOLEAN NTAPI RtlEqualUnicodeString(PCUNICODE_STRING left, PCUNICODE_STRING right, BOOLEAN insensitive) { if (left == NULL || right == NULL || left->Length != right->Length) return FALSE; return (BOOLEAN)(CompareStringOrdinal(left->Buffer, left->Length / sizeof(WCHAR), right->Buffer, right->Length / sizeof(WCHAR), insensitive) == CSTR_EQUAL); }
ULONG NTAPI RtlOemStringToUnicodeSize(PCOEM_STRING value) { int characters; if (value == NULL || (value->Length != 0 && value->Buffer == NULL)) return 0; characters = MultiByteToWideChar(CP_OEMCP, 0, value->Buffer, value->Length, NULL, 0); return characters < 0 ? 0u : (ULONG)((characters + 1) * sizeof(WCHAR)); }
ULONG NTAPI RtlGetFullPathName_U(PCWSTR path, ULONG size, PWSTR buffer, PWSTR *part) { DWORD result = GetFullPathNameW(path, size / sizeof(WCHAR), buffer, part); return result * sizeof(WCHAR); }
ULONG NTAPI RtlGetCurrentDirectory_U(ULONG size, PWSTR buffer) { DWORD result = GetCurrentDirectoryW(size / sizeof(WCHAR), buffer); return result * sizeof(WCHAR); }
NTSTATUS NTAPI RtlSetCurrentDirectory_U(PCUNICODE_STRING path) { WCHAR local[MAX_PATH + 2]; SIZE_T chars; if (path == NULL || path->Buffer == NULL || (path->Length % sizeof(WCHAR)) != 0) return STATUS_INVALID_PARAMETER; chars = path->Length / sizeof(WCHAR); if (chars >= sizeof(local) / sizeof(local[0])) return STATUS_BUFFER_OVERFLOW; memcpy(local, path->Buffer, path->Length); local[chars] = L'\0'; return SetCurrentDirectoryW(local) ? 0 : opennt_support_last_error_status(); }
ULONG NTAPI RtlNtStatusToDosError(NTSTATUS status) { if (status == 0) return ERROR_SUCCESS; if (status == STATUS_NO_MEMORY) return ERROR_NOT_ENOUGH_MEMORY; if (status == STATUS_BUFFER_OVERFLOW) return ERROR_FILENAME_EXCED_RANGE; return ERROR_INVALID_PARAMETER; }
VOID NTAPI RtlAcquirePebLock(VOID) { InitOnceExecuteOnce(&opennt_support_lock_once, opennt_support_initialize_lock, NULL, NULL); EnterCriticalSection(&opennt_support_lock); }
VOID NTAPI RtlReleasePebLock(VOID) { LeaveCriticalSection(&opennt_support_lock); }

/* DIVERGENCE(ADAPTER-WIN32-038): the selected original SoftPC timer and
 * CCPU copy helpers use the documented NT4 RTL integer/fill entrypoints.
 * Current x64 NTDLL import libraries do not expose all three historical
 * spellings.  Keep their source-facing ABI and perform only host-local,
 * fixed-width arithmetic/copying; neither result is a guest pointer or a
 * host-identity token. */
LARGE_INTEGER NTAPI RtlExtendedLargeIntegerDivide(LARGE_INTEGER dividend,
                                                   ULONG divisor,
                                                   PULONG remainder)
{
    LARGE_INTEGER quotient;
    LONGLONG value;

    value = dividend.QuadPart;
    quotient.QuadPart = divisor == 0u ? 0 : value / (LONGLONG)divisor;
    if (remainder != NULL) {
        *remainder = divisor == 0u ? 0u : (ULONG)(value % (LONGLONG)divisor);
    }
    return quotient;
}

LARGE_INTEGER NTAPI RtlExtendedIntegerMultiply(LARGE_INTEGER multiplicand,
                                                LONG multiplier)
{
    LARGE_INTEGER result;
    result.QuadPart = multiplicand.QuadPart * (LONGLONG)multiplier;
    return result;
}

VOID NTAPI RtlFillMemoryUlong(PVOID destination, SIZE_T length, ULONG pattern)
{
    unsigned char *cursor = (unsigned char *)destination;

    while (length >= sizeof(pattern)) {
        memcpy(cursor, &pattern, sizeof(pattern));
        cursor += sizeof(pattern);
        length -= sizeof(pattern);
    }
    if (length != 0u) memcpy(cursor, &pattern, length);
}
