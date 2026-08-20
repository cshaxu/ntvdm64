#include "command_misc_shim.h"

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

NTSTATUS RtlOemStringToUnicodeString(PUNICODE_STRING destination,
    const POEM_STRING source, BOOL allocate_destination)
{
    int characters;
    if (destination == NULL || source == NULL || source->Buffer == NULL || !allocate_destination)
        return (NTSTATUS)-1;
    characters = MultiByteToWideChar(CP_OEMCP, 0, source->Buffer, source->Length, NULL, 0);
    if (characters <= 0 || characters > 0x7ffe) return (NTSTATUS)-1;
    destination->Buffer = (PWSTR)malloc(((size_t)characters + 1u) * sizeof(WCHAR));
    if (destination->Buffer == NULL) return (NTSTATUS)-1;
    if (MultiByteToWideChar(CP_OEMCP, 0, source->Buffer, source->Length,
            destination->Buffer, characters) != characters) {
        free(destination->Buffer); destination->Buffer = NULL; return (NTSTATUS)-1;
    }
    destination->Buffer[characters] = L'\0';
    destination->Length = (USHORT)(characters * (int)sizeof(WCHAR));
    destination->MaximumLength = (USHORT)((characters + 1) * (int)sizeof(WCHAR));
    return 0;
}

NTSTATUS RtlUnicodeStringToAnsiString(PANSI_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination)
{
    int bytes;
    if (destination == NULL || source == NULL || source->Buffer == NULL || !allocate_destination)
        return (NTSTATUS)-1;
    bytes = WideCharToMultiByte(CP_ACP, 0, source->Buffer,
        source->Length / (USHORT)sizeof(WCHAR), NULL, 0, NULL, NULL);
    if (bytes <= 0 || bytes > 0x7ffe) return (NTSTATUS)-1;
    destination->Buffer = (PCHAR)malloc((size_t)bytes + 1u);
    if (destination->Buffer == NULL) return (NTSTATUS)-1;
    if (WideCharToMultiByte(CP_ACP, 0, source->Buffer,
            source->Length / (USHORT)sizeof(WCHAR), destination->Buffer,
            bytes, NULL, NULL) != bytes) {
        free(destination->Buffer); destination->Buffer = NULL; return (NTSTATUS)-1;
    }
    destination->Buffer[bytes] = '\0';
    destination->Length = (USHORT)bytes;
    destination->MaximumLength = (USHORT)(bytes + 1);
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
    (void)status;
    return ERROR_INVALID_DATA;
}
