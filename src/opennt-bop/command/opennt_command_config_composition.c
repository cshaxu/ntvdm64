/* Compatibility seam for the directly imported OpenNT cmdconf.c owner.
 * OpenNT source references: base/mvdm/softpc.new/host/src/nt_pif.c
 * (GetPIFConfigFiles) and ntdll RTL string helpers.  The historic host
 * composition cannot link into this standalone process, so this file retains
 * the same source-selected input/default/failure contract with public Win32
 * conversion APIs.  It does not parse CONFIG.SYS or AUTOEXEC.BAT. */

#include "opennt_command_composition.h"

#include <stdlib.h>
#include <string.h>

PCHAR lpszzcmdEnv16;

void bx_ntvdm_command_config_set_inputs(bx_ntvdm_command_misc_session *session,
    const CHAR *config_path, const CHAR *autoexec_path)
{
    if (!bx_ntvdm_command_misc_session_valid(session)) return;
    strncpy(session->config_input_path, config_path == NULL ? "" : config_path,
        sizeof(session->config_input_path) - 1u);
    session->config_input_path[sizeof(session->config_input_path) - 1u] = '\0';
    strncpy(session->autoexec_input_path, autoexec_path == NULL ? "" : autoexec_path,
        sizeof(session->autoexec_input_path) - 1u);
    session->autoexec_input_path[sizeof(session->autoexec_input_path) - 1u] = '\0';
}

int bx_ntvdm_command_config_set_bootstrap_command(
    bx_ntvdm_command_misc_session *session, const CHAR *command_path)
{
    size_t bytes;
    if (!bx_ntvdm_command_misc_session_valid(session) || command_path == NULL)
        return 0;
    bytes = strlen(command_path) + 1u;
    /* OpenNT sysconf.asm copies SHELL's executable name into the fixed
     * `commnd db "\\COMMAND.COM", 0, 51 dup (0)` record.  Refuse a source
     * bundle path that cannot fit that original guest storage; do not truncate
     * it or create an alternate guest-path mapping. */
    /* sysconf.asm uses the first literal space to end the executable token.
     * Its historical SHELL= grammar has no quoting escape, so reject rather
     * than silently publish a different executable name. */
    if (bytes <= 1u || bytes > sizeof(session->bootstrap_command_path) ||
        strchr(command_path, ' ') != NULL) return 0;
    memcpy(session->bootstrap_command_path, command_path, bytes);
    return 1;
}

const CHAR *bx_ntvdm_command_config_bootstrap_command(void)
{
    bx_ntvdm_command_misc_session *session = bx_ntvdm_command_misc_active_session();
    return session != NULL && bx_ntvdm_command_misc_session_valid(session) &&
        session->bootstrap_command_path[0] != '\0' ?
        session->bootstrap_command_path : NULL;
}

void GetPIFConfigFiles(BOOL bConfig, CHAR *file_name)
{
    bx_ntvdm_command_misc_session *session = bx_ntvdm_command_misc_active_session();
    const CHAR *configured = session == NULL ? NULL :
        (bConfig ? session->config_input_path : session->autoexec_input_path);
    DWORD length;

    /* Exact nt_pif.c selection order: PIF-selected path first; otherwise
     * the system directory's config.nt or autoexec.nt.  The session field is
     * the non-invasive CLI replacement for historical PIF global state. */
    if (configured == NULL || configured[0] == '\0') {
        length = GetSystemDirectoryA(file_name, MAX_PATH);
        if (length == 0u || file_name[length - 1u] != '\\') strcat(file_name, "\\");
        strcat(file_name, bConfig ? "config.nt" : "autoexec.nt");
    } else if (ExpandEnvironmentStringsA(configured, file_name, MAX_PATH + 12u) == 0u) {
        file_name[0] = '\0';
    }
}

void RtlInitAnsiString(PANSI_STRING destination, const CHAR *source)
{
    size_t length = source == NULL ? 0u : strlen(source);
    if (destination == NULL) return;
    destination->Buffer = (PCHAR)source;
    destination->Length = (USHORT)length;
    destination->MaximumLength = (USHORT)(length + 1u);
}

NTSTATUS RtlAnsiStringToUnicodeString(PUNICODE_STRING destination,
    const PANSI_STRING source, BOOL allocate_destination)
{
    int characters;
    if (destination == NULL || source == NULL || source->Buffer == NULL || !allocate_destination)
        return (NTSTATUS)-1;
    characters = MultiByteToWideChar(CP_ACP, 0, source->Buffer, source->Length,
        NULL, 0);
    if (characters <= 0 || characters > 0x7ffe) return (NTSTATUS)-1;
    destination->Buffer = (PWSTR)malloc(((size_t)characters + 1u) * sizeof(WCHAR));
    if (destination->Buffer == NULL) return (NTSTATUS)-1;
    if (MultiByteToWideChar(CP_ACP, 0, source->Buffer, source->Length,
            destination->Buffer, characters) != characters) {
        free(destination->Buffer); destination->Buffer = NULL; return (NTSTATUS)-1;
    }
    destination->Buffer[characters] = L'\0';
    destination->Length = (USHORT)(characters * (int)sizeof(WCHAR));
    destination->MaximumLength = (USHORT)((characters + 1) * (int)sizeof(WCHAR));
    return 0;
}

NTSTATUS RtlUnicodeStringToOemString(POEM_STRING destination,
    const PUNICODE_STRING source, BOOL allocate_destination)
{
    int characters;
    if (destination == NULL || source == NULL || source->Buffer == NULL ||
        destination->Buffer == NULL || allocate_destination) return (NTSTATUS)-1;
    characters = WideCharToMultiByte(CP_OEMCP, 0, source->Buffer,
        source->Length / (USHORT)sizeof(WCHAR), destination->Buffer,
        destination->MaximumLength - 1u, NULL, NULL);
    if (characters <= 0) return (NTSTATUS)-1;
    destination->Buffer[characters] = '\0';
    destination->Length = (USHORT)characters;
    return 0;
}

void RtlFreeUnicodeString(PUNICODE_STRING string)
{
    if (string == NULL) return;
    free(string->Buffer);
    string->Buffer = NULL;
    string->Length = string->MaximumLength = 0u;
}
