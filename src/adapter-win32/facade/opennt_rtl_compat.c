#include "opennt_rtl_compat.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define RUNTIME_STATUS_NO_MEMORY ((NTSTATUS)0xc0000017L)
#define RUNTIME_STATUS_INVALID_PARAMETER ((NTSTATUS)0xc000000dL)

/* DIVERGENCE(WIN32-DIV-004): OpenNT linked these RTL entry points from the
 * NT4 host composition.  The standalone build keeps their source-visible
 * shapes but implements the reached conversion/environment subset with public
 * Win32 APIs and bounded heap storage. */

void RtlInitAnsiString(PANSI_STRING destination, const CHAR *source) {
    size_t length = source == NULL ? 0u : strlen(source);
    if (destination == NULL) return;
    destination->Buffer = (PCHAR)source; destination->Length = (USHORT)length;
    destination->MaximumLength = (USHORT)(length + 1u);
}
void RtlInitString(PSTRING destination, const CHAR *source) { RtlInitAnsiString(destination, source); }
void RtlInitUnicodeString(PUNICODE_STRING destination, const WCHAR *source) {
    size_t chars;
    if (destination == NULL) return;
    if (source == NULL) { destination->Length = destination->MaximumLength = 0u; destination->Buffer = NULL; return; }
    chars = wcslen(source);
    if (chars > (USHRT_MAX / sizeof(WCHAR)) - 1u) { destination->Length = destination->MaximumLength = 0u; destination->Buffer = NULL; return; }
    destination->Length = (USHORT)(chars * sizeof(WCHAR)); destination->MaximumLength = (USHORT)((chars + 1u) * sizeof(WCHAR)); destination->Buffer = (PWSTR)source;
}
NTSTATUS RtlAnsiStringToUnicodeString(PUNICODE_STRING destination, const PANSI_STRING source, BOOL allocate_destination) {
    int characters;
    if (destination == NULL || source == NULL || source->Buffer == NULL || !allocate_destination) return (NTSTATUS)-1;
    characters = MultiByteToWideChar(CP_ACP, 0, source->Buffer, source->Length, NULL, 0);
    if (characters <= 0 || characters > 0x7ffe) return (NTSTATUS)-1;
    destination->Buffer = (PWSTR)malloc(((size_t)characters + 1u) * sizeof(WCHAR));
    if (destination->Buffer == NULL) return (NTSTATUS)-1;
    if (MultiByteToWideChar(CP_ACP, 0, source->Buffer, source->Length, destination->Buffer, characters) != characters) { free(destination->Buffer); destination->Buffer = NULL; return (NTSTATUS)-1; }
    destination->Buffer[characters] = L'\0'; destination->Length = (USHORT)(characters * sizeof(WCHAR)); destination->MaximumLength = (USHORT)((characters + 1) * sizeof(WCHAR)); return 0;
}
NTSTATUS RtlUnicodeStringToOemString(POEM_STRING destination, const PUNICODE_STRING source, BOOL allocate_destination) {
    int characters;
    if (destination == NULL || source == NULL || source->Buffer == NULL || destination->Buffer == NULL || allocate_destination) return (NTSTATUS)-1;
    characters = WideCharToMultiByte(CP_OEMCP, 0, source->Buffer, source->Length / (USHORT)sizeof(WCHAR), destination->Buffer, destination->MaximumLength - 1u, NULL, NULL);
    if (characters <= 0) return (NTSTATUS)-1;
    destination->Buffer[characters] = '\0'; destination->Length = (USHORT)characters; return 0;
}
NTSTATUS RtlUnicodeStringToAnsiString(PANSI_STRING destination, const PUNICODE_STRING source, BOOL allocate_destination) {
    int bytes; PCHAR original_buffer; USHORT original_maximum;
    if (destination == NULL || source == NULL || source->Buffer == NULL) return (NTSTATUS)-1;
    original_buffer = destination->Buffer; original_maximum = destination->MaximumLength;
    bytes = WideCharToMultiByte(CP_ACP, 0, source->Buffer, source->Length / (USHORT)sizeof(WCHAR), NULL, 0, NULL, NULL);
    if (bytes <= 0 || bytes > 0x7ffe) return (NTSTATUS)-1;
    if (!allocate_destination) { if (destination->Buffer == NULL || destination->MaximumLength < bytes + 1u) return (NTSTATUS)-1; }
    else { destination->Buffer = (PCHAR)malloc((size_t)bytes + 1u); if (destination->Buffer == NULL) return (NTSTATUS)-1; }
    if (WideCharToMultiByte(CP_ACP, 0, source->Buffer, source->Length / (USHORT)sizeof(WCHAR), destination->Buffer, bytes, NULL, NULL) != bytes) {
        if (allocate_destination) { free(destination->Buffer); destination->Buffer = NULL; destination->Length = destination->MaximumLength = 0u; }
        else { destination->Buffer = original_buffer; destination->MaximumLength = original_maximum; }
        return (NTSTATUS)-1;
    }
    destination->Buffer[bytes] = '\0'; destination->Length = (USHORT)bytes; if (allocate_destination) destination->MaximumLength = (USHORT)(bytes + 1u); return 0;
}
void RtlFreeAnsiString(PANSI_STRING string) { if (string != NULL) { free(string->Buffer); string->Buffer = NULL; string->Length = string->MaximumLength = 0u; } }
void RtlFreeUnicodeString(PUNICODE_STRING string) { if (string != NULL) { free(string->Buffer); string->Buffer = NULL; string->Length = string->MaximumLength = 0u; } }

static size_t multisz_wchars(const WCHAR *s) { size_t n = 0u; if (s == NULL) return 0u; while (s[n] != L'\0' || s[n + 1u] != L'\0') ++n; return n + 2u; }
static int name_compare(const WCHAR *a, size_t an, const WCHAR *b, size_t bn) { size_t i; for (i = 0u; i < an && i < bn; ++i) { WCHAR x = (WCHAR)towupper(a[i]), y = (WCHAR)towupper(b[i]); if (x != y) return x < y ? -1 : 1; } return an == bn ? 0 : (an < bn ? -1 : 1); }
NTSTATUS RtlCreateEnvironment(BOOLEAN clone_current, PVOID *environment) { PWCHAR value; (void)clone_current; if (environment == NULL) return RUNTIME_STATUS_INVALID_PARAMETER; value = (PWCHAR)calloc(2u, sizeof(WCHAR)); if (value == NULL) return RUNTIME_STATUS_NO_MEMORY; *environment = value; return 0; }
void RtlDestroyEnvironment(PVOID environment) { free(environment); }
NTSTATUS RtlSetEnvironmentVariable(PVOID *environment, const PUNICODE_STRING name, const PUNICODE_STRING value) {
    PWCHAR source, replacement; size_t total, name_chars, value_chars, cursor, output; int emitted = 0;
    if (environment == NULL || *environment == NULL || name == NULL || name->Buffer == NULL || (name->Length % sizeof(WCHAR)) != 0u) return RUNTIME_STATUS_INVALID_PARAMETER;
    name_chars = name->Length / sizeof(WCHAR); if (name_chars == 0u || (value != NULL && (value->Buffer == NULL || (value->Length % sizeof(WCHAR)) != 0u))) return RUNTIME_STATUS_INVALID_PARAMETER;
    value_chars = value == NULL ? 0u : value->Length / sizeof(WCHAR); source = (PWCHAR)*environment; total = multisz_wchars(source);
    if (total < 2u || total > (USHRT_MAX / sizeof(WCHAR))) return RUNTIME_STATUS_INVALID_PARAMETER;
    if (total + (value == NULL ? 0u : name_chars + value_chars + 1u) > (USHRT_MAX / sizeof(WCHAR))) return RUNTIME_STATUS_NO_MEMORY;
    replacement = (PWCHAR)calloc(total + (value == NULL ? 0u : name_chars + value_chars + 1u), sizeof(WCHAR)); if (replacement == NULL) return RUNTIME_STATUS_NO_MEMORY;
    cursor = output = 0u;
    while (cursor + 1u < total && source[cursor] != L'\0') {
        WCHAR *entry = source + cursor; WCHAR *equals = wcschr(entry + (entry[0] == L'=' ? 1u : 0u), L'='); size_t chars = wcslen(entry); size_t entry_name = equals == NULL ? chars : (size_t)(equals - entry); int compare = name_compare(name->Buffer, name_chars, entry, entry_name);
        if (!emitted && value != NULL && compare < 0) { memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR)); output += name_chars; replacement[output++] = L'='; memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR)); output += value_chars; replacement[output++] = L'\0'; emitted = 1; }
        if (compare != 0) { memcpy(replacement + output, entry, (chars + 1u) * sizeof(WCHAR)); output += chars + 1u; }
        else if (!emitted && value != NULL) { memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR)); output += name_chars; replacement[output++] = L'='; memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR)); output += value_chars; replacement[output++] = L'\0'; emitted = 1; }
        cursor += chars + 1u;
    }
    if (!emitted && value != NULL) { memcpy(replacement + output, name->Buffer, name_chars * sizeof(WCHAR)); output += name_chars; replacement[output++] = L'='; memcpy(replacement + output, value->Buffer, value_chars * sizeof(WCHAR)); output += value_chars; replacement[output++] = L'\0'; }
    replacement[output++] = L'\0'; free(source); *environment = replacement; return 0;
}
ULONG RtlNtStatusToDosError(NTSTATUS status) {
    typedef ULONG (NTAPI *converter_fn)(NTSTATUS); static converter_fn converter; static int probed;
    if (!probed) { HMODULE ntdll = GetModuleHandleW(L"ntdll.dll"); converter = ntdll == NULL ? NULL : (converter_fn)GetProcAddress(ntdll, "RtlNtStatusToDosError"); probed = 1; }
    if (converter != NULL) return converter(status);
    switch ((ULONG)status) { case 0x00000000u: return ERROR_SUCCESS; case 0xc000000du: return ERROR_INVALID_PARAMETER; case 0xc0000017u: return ERROR_NOT_ENOUGH_MEMORY; case 0xc0000034u: return ERROR_FILE_NOT_FOUND; case 0xc000003au: return ERROR_PATH_NOT_FOUND; case 0xc0000022u: return ERROR_ACCESS_DENIED; case 0xc0000008u: return ERROR_INVALID_HANDLE; default: return ERROR_INVALID_DATA; }
}
