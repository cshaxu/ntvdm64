#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "bop/shim/command_misc_shim.h"

extern BOOL fSeparateWow;

static int multisz_has_prefix(const CHAR *strings, DWORD bytes, const CHAR *prefix)
{
    const CHAR *cursor = strings;
    const CHAR *end = strings + bytes;
    size_t prefix_bytes = strlen(prefix);
    while (cursor < end && *cursor != '\0') {
        size_t bytes_here = strlen(cursor) + 1u;
        if (bytes_here > (size_t)(end - cursor)) return 0;
        if (strncmp(cursor, prefix, prefix_bytes) == 0) return 1;
        cursor += bytes_here;
    }
    return 0;
}

/* This is a double-NUL multi-string, not one C string.  wcsstr would stop at
 * COMSPEC's NUL and falsely fail to observe the following PATH entry. */
static int wide_multisz_has_prefix(const WCHAR *strings, size_t characters,
    const WCHAR *prefix)
{
    const WCHAR *cursor = strings;
    const WCHAR *end = strings + characters;
    size_t prefix_characters = wcslen(prefix);
    while (cursor < end && *cursor != L'\0') {
        size_t characters_here = wcslen(cursor) + 1u;
        if (characters_here > (size_t)(end - cursor)) return 0;
        if (wcsncmp(cursor, prefix, prefix_characters) == 0) return 1;
        cursor += characters_here;
    }
    return 0;
}

int main(void)
{
    bx_ntvdm_command_misc_session session;
    VDMENVBLK block;
    ANSI_STRING transformed;
    CHAR source[1536];
    CHAR autoexec[1536];
    CHAR dos_environment[] = "ZETA=1\0COMSPEC=C:\\DOS.COM\0ALPHA=2\0\0";
    PWCHAR snapshot;
    size_t path_bytes;
    size_t autoexec_bytes;

    memset(source, 0, sizeof(source));
    memcpy(source, "COMSPEC=C:\\STALE.COM", sizeof("COMSPEC=C:\\STALE.COM"));
    path_bytes = strlen("COMSPEC=C:\\STALE.COM") + 1u;
    memcpy(source + path_bytes, "PATH=", sizeof("PATH=") - 1u);
    memset(source + path_bytes + sizeof("PATH=") - 1u, 'P', 1300u);
    source[path_bytes + sizeof("PATH=") - 1u + 1300u] = '\0';
    memcpy(source + path_bytes + sizeof("PATH=") + 1300u,
        "WINDIR=C:\\WINDOWS", sizeof("WINDIR=C:\\WINDOWS"));

    bx_ntvdm_command_misc_session_initialize(&session);
    if (!bx_ntvdm_command_misc_session_set_command_environment(&session, source,
            (uint32_t)(path_bytes + sizeof("PATH=") + 1300u +
                sizeof("WINDIR=C:\\WINDOWS") + 1u)) ||
        session.command_source_environment_bytes <= 1024u ||
        session.command_source_environment == source ||
        !multisz_has_prefix(session.command_source_environment,
            session.command_source_environment_bytes, "PATH=")) return 1;

    snapshot = bx_ntvdm_command_environment_snapshot_session(&session);
    if (snapshot == NULL || !wide_multisz_has_prefix(snapshot,
            session.command_source_environment_bytes, L"COMSPEC=C:\\STALE.COM") ||
        !wide_multisz_has_prefix(snapshot,
            session.command_source_environment_bytes, L"PATH=")) return 10;
    bx_ntvdm_command_environment_free_snapshot(snapshot);

    memset(&transformed, 0, sizeof(transformed));
    if (!cmdXformEnvironment(dos_environment, &transformed) ||
        transformed.Buffer == NULL || strcmp(transformed.Buffer, "ALPHA=2") != 0 ||
        !multisz_has_prefix(transformed.Buffer, transformed.Length + 1u, "ZETA=1") ||
        multisz_has_prefix(transformed.Buffer, transformed.Length + 1u, "COMSPEC=")) return 11;
    RtlFreeAnsiString(&transformed);

    lpszzVDMEnv32 = (CHAR *)malloc(session.command_source_environment_bytes);
    if (lpszzVDMEnv32 == NULL) return 2;
    memcpy(lpszzVDMEnv32, session.command_source_environment,
        session.command_source_environment_bytes);
    cchVDMEnv32 = session.command_source_environment_bytes;

    memset(autoexec, 0, sizeof(autoexec));
    memcpy(autoexec, "EXPANDED", sizeof("EXPANDED"));
    autoexec_bytes = sizeof("EXPANDED");
    memcpy(autoexec + autoexec_bytes, "%PATH%", sizeof("%PATH%"));
    lpszzcmdEnv16 = (CHAR *)calloc(autoexec_bytes + sizeof("%PATH%") + 1u, 1u);
    if (lpszzcmdEnv16 == NULL) return 3;
    memcpy(lpszzcmdEnv16, autoexec, autoexec_bytes + sizeof("%PATH%"));

    strcpy(lpszComSpec, "COMSPEC=C:\\COMMAND.COM");
    cbComSpec = (USHORT)(strlen(lpszComSpec) + 1u);
    fSeparateWow = FALSE;
    memset(&block, 0, sizeof(block));
    if (!cmdCreateVDMEnvironment(&block) || block.lpszzEnv == NULL) return 4;
    if (block.cchEnv <= 1024u) return 5;
    if (!multisz_has_prefix(block.lpszzEnv, block.cchEnv, "COMSPEC=C:\\COMMAND.COM")) return 6;
    if (!multisz_has_prefix(block.lpszzEnv, block.cchEnv, "PATH=")) return 7;
    if (multisz_has_prefix(block.lpszzEnv, block.cchEnv, "WINDIR=")) return 8;
    if (!multisz_has_prefix(block.lpszzEnv, block.cchEnv, "EXPANDED=")) return 9;

    free(block.lpszzEnv);
    free(lpszzcmdEnv16); lpszzcmdEnv16 = NULL;
    free(lpszzVDMEnv32); lpszzVDMEnv32 = NULL; cchVDMEnv32 = 0u;
    bx_ntvdm_command_misc_session_dispose(&session);
    puts("T234 S2 OpenNT dynamic COMMAND environment composition verified");
    return 0;
}
