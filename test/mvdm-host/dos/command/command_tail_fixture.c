/* Exact selected tail body; this is not a service or guest-lease test. */
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#undef ASSERT
#define ASSERT(x) do { if (!(x)) abort(); } while (0)
static void assemble(char *output, char *app, char *tail)
{
    struct { char *AppName; } VDMInfo = {app};
    char *pSrc, *pDst, *lpszCmd = output, *CmdLine = tail;
    ULONG cb;
#include "cmd-tail-body.inc"
}
static int check(char *app, char *tail, const char *expected)
{
    unsigned char bytes[132];
    size_t n = strlen(expected), i;
    memset(bytes, 0xa5, sizeof(bytes));
    assemble((char *)bytes + 1, app, tail);
    if (bytes[0] != 0xa5 || bytes[1] != 0xa5 || bytes[2] != n ||
        memcmp(bytes + 3, expected, n + 1)) return 0;
    for (i = 4 + n; i < sizeof(bytes); ++i) if (bytes[i] != 0xa5) return 0;
    return 1;
}
int main(void)
{
    char boundary[126], expected[128];
    if (!check("C:\\BIN\\HELLO.COM", "\r\n", "HELLO\r\n") ||
        !check("HELLO.EXE", "-x\r\n", "HELLO -x\r\n") ||
        !check("C:\\BIN\\HELLO.BAT", "  /q\r\n", "HELLO   /q\r\n") ||
        !check("NOEXT", "\r\n", "NOEXT\r\n")) return 1;
    /* Basename 1 + space 1 + tail 123 + CRLF 2 = count 127. */
    memset(boundary, 'x', 123); boundary[123] = 13;
    boundary[124] = 10; boundary[125] = 0;
    expected[0] = 'A'; expected[1] = ' ';
    memset(expected + 2, 'x', 123); expected[125] = 13;
    expected[126] = 10; expected[127] = 0;
    if (!check("A.COM", boundary, expected)) return 2;
    puts("CMD_ORIGINAL_TAIL_5_CASES_EMPTY_SPACES_127_GUARDS_OK");
    return 0;
}
