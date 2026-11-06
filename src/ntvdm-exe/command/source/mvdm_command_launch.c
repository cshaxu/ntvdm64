#include "compat/include/nt.h"
#include "ntvdm-exe/command/include/command_launch.h"
#include "ntvdm-exe/command/include/command_text.h"
#include "ntvdm-exe/command/include/mvdm_command_native_child.h"
#include <string.h>

/* The selected original COMMAND environment algorithm. It builds a private,
 * sorted ANSI block and retains host COMSPEC rather than guest COMMAND.COM. */
extern BOOL cmdXformEnvironment(PCHAR, PANSI_STRING);

static DWORD ansi_copy(const char *source, ANSI_STRING *output)
{
    OEM_STRING oem;
    UNICODE_STRING unicode = {0};
    NTSTATUS status;
    size_t length = strlen(source);
    if (length >= UINT16_MAX) return ERROR_FILENAME_EXCED_RANGE;
    oem.Buffer = (PCHAR)source;
    oem.Length = (USHORT)length;
    oem.MaximumLength = (USHORT)(length + 1);
    status = RtlOemStringToUnicodeString(&unicode, &oem, TRUE);
    if (NT_SUCCESS(status)) {
        status = RtlUnicodeStringToAnsiString(output, &unicode, TRUE);
        RtlFreeUnicodeString(&unicode);
    }
    return NT_SUCCESS(status) ? ERROR_SUCCESS : RtlNtStatusToDosError(status);
}

/* Extracted from OpenNT base/mvdm/dos/command/cmdexec.c, cmdCreateProcess.
 * Keep conversion before the original environment algorithm and its error
 * result. Allocated command storage avoids modifying either caller's capture. */
DWORD ntvdm_command_prepare_text(const char *command, const char *environment,
    PANSI_STRING command_out, PANSI_STRING environment_out)
{
    DWORD error;
    if (!command_out || !environment_out || command_out == environment_out)
        return ERROR_INVALID_PARAMETER;
    ZeroMemory(command_out, sizeof(*command_out));
    ZeroMemory(environment_out, sizeof(*environment_out));
    if (!command) return ERROR_INVALID_PARAMETER;
    error = ansi_copy(command, command_out);
    if (!error && environment &&
        !cmdXformEnvironment((PCHAR)environment, environment_out))
        error = ERROR_NOT_ENOUGH_MEMORY;
    if (error) {
        RtlFreeAnsiString(environment_out);
        RtlFreeAnsiString(command_out);
        ZeroMemory(command_out, sizeof(*command_out));
        ZeroMemory(environment_out, sizeof(*environment_out));
    }
    return error;
}

DWORD ntvdm_command_launch_prepare_guest(const mvdm_command_capture_view *view,
    const HANDLE defaults[3], ntvdm_command_launch **output)
{
    ANSI_STRING command = {0}, directory = {0}, environment = {0};
    HANDLE streams[3];
    size_t bytes, i;
    DWORD error;
    if (output) *output = NULL;
    if (!view || !defaults || !output || !view->command || !view->directory ||
        !view->environment || view->environment_bytes < 2 ||
        view->service > MVDM_COMMAND_SERVICE_EXECUTE) return ERROR_INVALID_PARAMETER;
    /* Capture already bounds the source, but reject a malformed view before
     * the original transform walks its double-NUL-terminated input. */
    for (bytes = 0; bytes + 1 < view->environment_bytes; ++bytes)
        if (!view->environment[bytes] && !view->environment[bytes + 1]) break;
    if (bytes + 2 != view->environment_bytes) return ERROR_INVALID_DATA;
    error = ntvdm_command_prepare_text(view->command, view->environment,
        &command, &environment);
    if (error) goto done;
    error = ansi_copy(view->directory, &directory);
    if (error) goto done;
    /* The original transform's Length is an allocation bound and can include
     * spare bytes after duplicate variables. Pass only the actual block. */
    for (bytes = 0; bytes + 1 < environment.Length; ++bytes)
        if (!environment.Buffer[bytes] && !environment.Buffer[bytes + 1]) break;
    if (bytes + 1 >= environment.Length) { error = ERROR_INVALID_DATA; goto done; }
    for (i = 0; i < 3; ++i)
        streams[i] = view->standard_handles[i] == UINT32_MAX ? defaults[i] :
            (HANDLE)(ULONG_PTR)view->standard_handles[i];
    /* Resolve and duplicate while the source handles are still owned by this
     * CPU admission; later executor threads never consult ambient handles. */
    error = ntvdm_command_launch_prepare(view->service, command.Buffer,
        environment.Buffer, bytes + 2, directory.Buffer, streams, output);
done:
    RtlFreeAnsiString(&environment);
    RtlFreeAnsiString(&directory);
    RtlFreeAnsiString(&command);
    return error;
}
