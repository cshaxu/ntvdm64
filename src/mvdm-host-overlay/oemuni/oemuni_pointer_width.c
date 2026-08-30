/*
 * DIVERGENCE(SUPPORT-OVERLAY-001): The matching byte-exact OpenNT OEM source
 * derives copy sizes and output pointers by narrowing host pointers to ULONG
 * or DWORD. This x64-only overlay retains the source API and public Win32
 * sequence but reconstructs offsets without narrowing native pointers.
 */
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <oemuni.h>
#include "oem.h"

#include <stddef.h>

static BOOL oem_set_file_part(LPSTR output, PWSTR wide_base, PWSTR wide_part,
    LPSTR *file_part)
{
    UNICODE_STRING prefix;
    OEM_STRING oem_prefix;
    NTSTATUS status;

    if (wide_part == NULL) {
        *file_part = NULL;
        return TRUE;
    }
    prefix.Buffer = wide_base;
    prefix.Length = (USHORT)((wide_part - wide_base) * sizeof(WCHAR));
    prefix.MaximumLength = prefix.Length;
    status = RtlUnicodeStringToOemString(&oem_prefix, &prefix, TRUE);
    if (!NT_SUCCESS(status)) {
        BaseSetLastNTError(status);
        return FALSE;
    }
    *file_part = output + oem_prefix.Length;
    RtlFreeOemString(&oem_prefix);
    return TRUE;
}

/* DIVERGENCE(SUPPORT-OVERLAY-001): source-derived replacement for the
 * original pointer-subtraction copy size. */
HANDLE WINAPI FindFirstFileOem(LPSTR file_name, LPWIN32_FIND_DATAA output)
{
    PUNICODE_STRING unicode = &NtCurrentTeb()->StaticUnicodeString;
    OEM_STRING oem;
    WIN32_FIND_DATAW find_data;
    NTSTATUS status;
    HANDLE result;

    InitOemString(&oem, file_name);
    status = RtlOemStringToUnicodeString(unicode, &oem, FALSE);
    if (!NT_SUCCESS(status)) {
        if (status == STATUS_BUFFER_OVERFLOW) SetLastError(ERROR_FILENAME_EXCED_RANGE);
        else BaseSetLastNTError(status);
        return INVALID_HANDLE_VALUE;
    }
    result = FindFirstFileW(unicode->Buffer, &find_data);
    if (result == INVALID_HANDLE_VALUE) return result;
    RtlMoveMemory(output, &find_data, offsetof(WIN32_FIND_DATAW, cFileName));
    RtlInitUnicodeString(unicode, find_data.cFileName);
    oem.Buffer = output->cFileName;
    oem.MaximumLength = MAX_PATH;
    status = RtlUnicodeStringToOemString(&oem, unicode, FALSE);
    if (NT_SUCCESS(status)) {
        RtlInitUnicodeString(unicode, find_data.cAlternateFileName);
        oem.Buffer = output->cAlternateFileName;
        oem.MaximumLength = 14;
        status = RtlUnicodeStringToOemString(&oem, unicode, FALSE);
    }
    if (!NT_SUCCESS(status)) {
        BaseSetLastNTError(status);
        FindClose(result);
        return INVALID_HANDLE_VALUE;
    }
    return result;
}

/* DIVERGENCE(SUPPORT-OVERLAY-001): source-derived replacement for the
 * original pointer-subtraction copy size. */
BOOL APIENTRY FindNextFileOem(HANDLE find_handle, LPWIN32_FIND_DATAA output)
{
    OEM_STRING oem;
    UNICODE_STRING unicode;
    WIN32_FIND_DATAW find_data;
    NTSTATUS status;

    if (!FindNextFileW(find_handle, &find_data)) return FALSE;
    RtlMoveMemory(output, &find_data, offsetof(WIN32_FIND_DATAW, cFileName));
    RtlInitUnicodeString(&unicode, find_data.cFileName);
    oem.Buffer = output->cFileName;
    oem.MaximumLength = MAX_PATH;
    status = RtlUnicodeStringToOemString(&oem, &unicode, FALSE);
    if (NT_SUCCESS(status)) {
        RtlInitUnicodeString(&unicode, find_data.cAlternateFileName);
        oem.Buffer = output->cAlternateFileName;
        oem.MaximumLength = 14;
        status = RtlUnicodeStringToOemString(&oem, &unicode, FALSE);
    }
    if (!NT_SUCCESS(status)) {
        BaseSetLastNTError(status);
        return FALSE;
    }
    return TRUE;
}

/* DIVERGENCE(SUPPORT-OVERLAY-001): source-derived replacement for the
 * original ULONG-based output-pointer reconstruction. */
DWORD APIENTRY GetFullPathNameOem(LPCSTR file_name, DWORD buffer_length,
    LPSTR output, LPSTR *file_part)
{
    OEM_STRING oem;
    UNICODE_STRING input;
    UNICODE_STRING result;
    OEM_STRING converted;
    PWSTR wide_buffer;
    PWSTR wide_part;
    ULONG wide_length;
    NTSTATUS status;

    InitOemString(&oem, file_name);
    status = RtlOemStringToUnicodeString(&input, &oem, TRUE);
    if (!NT_SUCCESS(status)) {
        BaseSetLastNTError(status);
        return 0;
    }
    wide_buffer = RtlAllocateHeap(RtlProcessHeap(), 0, (MAX_PATH << 1) + sizeof(WCHAR));
    if (wide_buffer == NULL) {
        RtlFreeUnicodeString(&input);
        BaseSetLastNTError(STATUS_NO_MEMORY);
        return 0;
    }
    wide_part = NULL;
    wide_length = RtlGetFullPathName_U(input.Buffer, (MAX_PATH << 1), wide_buffer,
        file_part == NULL ? NULL : &wide_part);
    wide_length >>= 1;
    if (wide_length != 0 && wide_length < buffer_length) {
        RtlInitUnicodeString(&result, wide_buffer);
        status = RtlUnicodeStringToOemString(&converted, &result, TRUE);
        if (NT_SUCCESS(status) && converted.Length < buffer_length) {
            RtlMoveMemory(output, converted.Buffer, converted.Length + 1);
            if (file_part != NULL) {
                if (!oem_set_file_part(output, wide_buffer, wide_part, file_part)) {
                    wide_length = 0;
                }
            }
            RtlFreeOemString(&converted);
        } else {
            if (NT_SUCCESS(status)) SetLastError(ERROR_FILENAME_EXCED_RANGE);
            else BaseSetLastNTError(status);
            wide_length = 0;
        }
    }
    RtlFreeUnicodeString(&input);
    RtlFreeHeap(RtlProcessHeap(), 0, wide_buffer);
    return wide_length;
}

/* DIVERGENCE(SUPPORT-OVERLAY-001): source-derived replacement for the
 * original DWORD-based output-pointer reconstruction. */
DWORD APIENTRY SearchPathOem(LPCSTR path, LPCSTR file_name, LPCSTR extension,
    DWORD buffer_length, LPSTR output, LPSTR *file_part)
{
    UNICODE_STRING wide_path = { 0 };
    UNICODE_STRING wide_extension = { 0 };
    UNICODE_STRING *name = &NtCurrentTeb()->StaticUnicodeString;
    OEM_STRING oem;
    PWSTR wide_buffer;
    PWSTR wide_part = NULL;
    DWORD result;
    NTSTATUS status;

    InitOemString(&oem, file_name);
    status = RtlOemStringToUnicodeString(name, &oem, FALSE);
    if (!NT_SUCCESS(status)) { BaseSetLastNTError(status); return 0; }
    if (path != NULL) { InitOemString(&oem, path); status = RtlOemStringToUnicodeString(&wide_path, &oem, TRUE); if (!NT_SUCCESS(status)) { BaseSetLastNTError(status); return 0; } }
    if (extension != NULL) { InitOemString(&oem, extension); status = RtlOemStringToUnicodeString(&wide_extension, &oem, TRUE); if (!NT_SUCCESS(status)) { RtlFreeUnicodeString(&wide_path); BaseSetLastNTError(status); return 0; } }
    wide_buffer = RtlAllocateHeap(RtlProcessHeap(), 0, buffer_length << 1);
    if (wide_buffer == NULL) { RtlFreeUnicodeString(&wide_path); RtlFreeUnicodeString(&wide_extension); BaseSetLastNTError(STATUS_NO_MEMORY); return 0; }
    result = SearchPathW(path == NULL ? NULL : wide_path.Buffer, name->Buffer,
        extension == NULL ? NULL : wide_extension.Buffer, buffer_length,
        wide_buffer, file_part == NULL ? NULL : &wide_part);
    if (result != 0 && result <= buffer_length) {
        UNICODE_STRING wide_result;
        RtlInitUnicodeString(&wide_result, wide_buffer);
        oem.Buffer = output;
        oem.MaximumLength = (USHORT)(buffer_length + 1);
        status = RtlUnicodeStringToOemString(&oem, &wide_result, FALSE);
        if (!NT_SUCCESS(status)) { BaseSetLastNTError(status); result = 0; }
        else if (file_part != NULL) {
            if (!oem_set_file_part(output, wide_buffer, wide_part, file_part)) result = 0;
        }
    }
    RtlFreeHeap(RtlProcessHeap(), 0, wide_buffer);
    RtlFreeUnicodeString(&wide_path);
    RtlFreeUnicodeString(&wide_extension);
    return result;
}
