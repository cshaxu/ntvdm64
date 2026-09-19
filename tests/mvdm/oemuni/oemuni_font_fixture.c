#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include <stdio.h>
#include <wchar.h>

static int calls, bad_name;
static BOOL provider_result;
static NTSTATUS conversion_failure;
static BOOL WINAPI font_provider(LPCWSTR name)
{
    ++calls;
    if (wcscmp(name, L"\x00e9.FON")) bad_name = 1;
    if (!provider_result) SetLastError(ERROR_FILE_NOT_FOUND);
    return provider_result;
}
static NTSTATUS NTAPI font_conversion(PUNICODE_STRING dst, PCOEM_STRING src, BOOLEAN allocate)
{
    if (conversion_failure) return conversion_failure;
    return RtlOemStringToUnicodeString(dst, src, allocate);
}

/* Compile the original owner. Only its external GDI call and injected
   conversion failures are substituted; no installed font is touched. */
#define RemoveFontResourceW font_provider
#define RtlOemStringToUnicodeString font_conversion
#include "../../../src/mvdm/oemuni/file.c"

int main(void)
{
    CHAR name[] = "\x82.FON";
    PWSTR original_buffer = NtCurrentTeb()->StaticUnicodeString.Buffer;
    USHORT original_capacity = NtCurrentTeb()->StaticUnicodeString.MaximumLength;
    if (GetOEMCP() != 437) return 1;
    provider_result = TRUE;
    if (!RemoveFontResourceOem(name) || calls != 1 || bad_name) return 2;
    provider_result = FALSE;
    if (RemoveFontResourceOem(name) || calls != 2 || GetLastError() != ERROR_FILE_NOT_FOUND) return 3;
    conversion_failure = STATUS_BUFFER_OVERFLOW;
    if (RemoveFontResourceOem(name) || calls != 2 || GetLastError() != ERROR_FILENAME_EXCED_RANGE) return 4;
    conversion_failure = STATUS_NO_MEMORY;
    if (RemoveFontResourceOem(name) || calls != 2 || GetLastError() != ERROR_NOT_ENOUGH_MEMORY) return 5;
    if (NtCurrentTeb()->StaticUnicodeString.Buffer != original_buffer ||
        NtCurrentTeb()->StaticUnicodeString.MaximumLength != original_capacity) return 6;
    conversion_failure = 0;
    provider_result = TRUE;
    if (!RemoveFontResourceOem(name) || calls != 3 || bad_name) return 7;
    puts("S37_FONT_ORIGINAL_OEM_CONVERSION_RESULT_FAILURE_TEB_REUSE_OK");
    return 0;
}
