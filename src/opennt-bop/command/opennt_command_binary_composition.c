#include "opennt_command_composition.h"

BOOL DontCheckDosBinaryType;
BOOL IsFirstWOWCheckBinary = TRUE;

uint32_t bx_ntvdm_command_binary_scs_address(uint32_t offset)
{
    bx_ntvdm_command_misc_session *session = bx_ntvdm_command_misc_active_session();
    if (!bx_ntvdm_command_misc_session_valid(session) ||
        offset > sizeof(session->scs_info) ||
        session->scs_info_address > 0x100000u - offset) return 0u;
    return session->scs_info_address + offset;
}

void RtlInitString(PSTRING destination, const CHAR *source)
{
    RtlInitAnsiString(destination, source);
}

NTSTATUS RtlUnicodeStringToAnsiString(PANSI_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination)
{
    int bytes;
    PCHAR original_buffer;
    USHORT original_maximum;
    if (destination == NULL || source == NULL || source->Buffer == NULL)
        return (NTSTATUS)-1;
    original_buffer = destination->Buffer;
    original_maximum = destination->MaximumLength;
    bytes = WideCharToMultiByte(CP_ACP, 0, source->Buffer,
        source->Length / (USHORT)sizeof(WCHAR), NULL, 0, NULL, NULL);
    if (bytes <= 0 || bytes > 0x7ffe) return (NTSTATUS)-1;
    if (!allocate_destination) {
        /* OpenNT cmdCreateProcess initializes OEM_STRING with the existing
         * command buffer, then asks RTL to overwrite that bounded buffer.
         * Preserve that non-allocating contract rather than rejecting it. */
        if (destination->Buffer == NULL || destination->MaximumLength < bytes + 1u)
            return (NTSTATUS)-1;
    } else {
        destination->Buffer = (PCHAR)malloc((size_t)bytes + 1u);
        if (destination->Buffer == NULL) return (NTSTATUS)-1;
    }
    if (WideCharToMultiByte(CP_ACP, 0, source->Buffer,
            source->Length / (USHORT)sizeof(WCHAR), destination->Buffer,
            bytes, NULL, NULL) != bytes) {
        /* The OpenNT caller owns a non-allocated destination buffer.  Do not
         * free or clear it on a conversion error; only an RTL-allocated
         * result belongs to RtlFreeAnsiString. */
        if (allocate_destination) {
            free(destination->Buffer);
            destination->Buffer = NULL;
            destination->Length = destination->MaximumLength = 0u;
        } else {
            destination->Buffer = original_buffer;
            destination->MaximumLength = original_maximum;
        }
        return (NTSTATUS)-1;
    }
    destination->Buffer[bytes] = '\0';
    destination->Length = (USHORT)bytes;
    if (allocate_destination) destination->MaximumLength = (USHORT)(bytes + 1);
    return 0;
}

void RtlFreeAnsiString(PANSI_STRING string)
{
    if (string == NULL) return;
    free(string->Buffer); string->Buffer = NULL;
    string->Length = string->MaximumLength = 0u;
}

ULONG RtlNtStatusToDosError(NTSTATUS status)
{
    typedef ULONG (NTAPI *bx_ntvdm_rtl_ntstatus_to_dos_error_fn)(NTSTATUS);
    static bx_ntvdm_rtl_ntstatus_to_dos_error_fn native_converter;
    static int native_converter_probed;

    /* DIVERGENCE(BOP-DIV-033): the imported OpenNT caller directly linked ntdll's
     * RtlNtStatusToDosError.  The standalone target owns the same-shaped
     * facade, so resolve the current OS export once and preserve the original
     * status-to-Win32/DOS conversion instead of collapsing every failure to
     * ERROR_INVALID_DATA.  The fallback covers the exact statuses emitted by
     * the local facade if a future host omits that export. */
    if (!native_converter_probed) {
        HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
        native_converter = ntdll == NULL ? NULL :
            (bx_ntvdm_rtl_ntstatus_to_dos_error_fn)GetProcAddress(ntdll,
                "RtlNtStatusToDosError");
        native_converter_probed = 1;
    }
    if (native_converter != NULL)
        return native_converter(status);

    switch ((ULONG)status) {
    case 0x00000000u: return ERROR_SUCCESS;
    case 0xc000000du: return ERROR_INVALID_PARAMETER;
    case 0xc0000017u: return ERROR_NOT_ENOUGH_MEMORY;
    case 0xc0000034u: return ERROR_FILE_NOT_FOUND;
    case 0xc000003au: return ERROR_PATH_NOT_FOUND;
    case 0xc0000022u: return ERROR_ACCESS_DENIED;
    case 0xc0000008u: return ERROR_INVALID_HANDLE;
    default: return ERROR_INVALID_DATA;
    }
}
