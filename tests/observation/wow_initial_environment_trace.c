/* Diagnostic-only composition of the unchanged original owner. Never linked
 * into release; records sizes and arena tags, not environment contents. */
#define cmdGetNextCmd observed_cmdGetNextCmd
#include "../../src/mvdm/dos/command/cmdmisc.c"
#undef cmdGetNextCmd

VOID cmdGetNextCmd(VOID)
{
    CMDINFO UNALIGNED *info = (CMDINFO *)GetVDMAddr((USHORT)getDS(), (USHORT)getDX());
    WORD segment = FETCHWORD(info->EnvSeg), capacity = FETCHWORD(info->EnvSize);
    BYTE *arena = (BYTE *)GetVDMAddr((segment - 1), 0);
    WORD paragraphs = (WORD)(arena[3] | ((WORD)arena[4] << 8));
    BYTE *next = (BYTE *)GetVDMAddr((segment + paragraphs), 0);
    BYTE before = next[0];
    FILE *log = fopen(S35_ENVIRONMENT_TRACE_LOG, "a");
    if (log) {
        fprintf(log, "pid=%lu before seg=%04X capacity=%u arena-paragraphs=%u next-tag=%02X\n",
            GetCurrentProcessId(), segment, capacity, paragraphs, before);
        fflush(log);
    }
    observed_cmdGetNextCmd();
    if (log) {
        fprintf(log, "pid=%lu after seg=%04X reported-size=%u arena-bytes=%lu next-tag=%02X cf=%u ax=%u\n",
            GetCurrentProcessId(), FETCHWORD(info->EnvSeg), FETCHWORD(info->EnvSize),
            (DWORD)paragraphs * 16u, next[0], (unsigned)getCF(), (unsigned)getAX());
        fclose(log);
    }
}
