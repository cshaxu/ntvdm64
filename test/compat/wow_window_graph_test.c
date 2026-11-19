#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "compat/include/wow_window_graph.h"

static unsigned long slots[65536];
static unsigned checks;

static int check(const mvdm_softpc_wow_window_links *rows, unsigned long count,
    int expected, DWORD error)
{
    int result;
    memset(slots, 0, sizeof(slots));
    SetLastError(0x1234);
    result = compat_wow_window_graph_validate(count, rows, slots, 0x10001);
    ++checks;
    if (result != expected || GetLastError() != error) {
        fprintf(stderr, "graph case %u result=%d error=%lu\n",
            checks, result, GetLastError());
        return 0;
    }
    return 1;
}

int main(void)
{
    mvdm_softpc_wow_window_links rows[3] = {
        {0x10001, 0, 0, 0, 0},
        {0x20002, 0, 0x10001, 0, 0},
        {0x30003, 0, 0x10001, 0, 0}
    };
    unsigned edge;
    /* Original construction permits a parent without the reciprocal child. */
    if (!check(rows, 3, 1, 0x1234) || slots[1] != 1 ||
        slots[2] != 2 || slots[3] != 3) return 1;
    rows[2].window = 0x40002;
    if (!check(rows, 3, 0, ERROR_INVALID_PARAMETER)) return 1;
    rows[2].window = 0;
    if (!check(rows, 3, 0, ERROR_INVALID_PARAMETER)) return 1;
    rows[2].window = 0x1ffff;
    if (!check(rows, 3, 0, ERROR_INVALID_PARAMETER)) return 1;
    rows[2].window = 0x30003;
    rows[1].owner = 0x40004;
    if (!check(rows, 3, 0, ERROR_INVALID_DATA)) return 1;
    rows[1].owner = 0x40003; /* Existing slot, wrong full generation. */
    if (!check(rows, 3, 0, ERROR_INVALID_DATA)) return 1;
    rows[1].owner = 0;
    rows[0].parent = rows[1].window;
    if (!check(rows, 3, 0, ERROR_INVALID_DATA)) return 1;
    rows[0].parent = 0;
    for (edge = 0; edge < 3; ++edge) {
        if (edge == 0) { rows[1].next = rows[2].window; rows[2].next = rows[1].window; }
        if (edge == 1) { rows[1].parent = rows[2].window; rows[2].parent = rows[1].window; }
        if (edge == 2) { rows[1].owner = rows[2].window; rows[2].owner = rows[1].window; }
        if (!check(rows, 3, 0, ERROR_INVALID_DATA)) return 1;
        rows[1].next = rows[2].next = rows[1].owner = rows[2].owner = 0;
        rows[1].parent = rows[2].parent = 0x10001;
    }
    rows[1].next = rows[2].window;
    rows[0].child = rows[1].window;
    if (!check(rows, 3, 1, 0x1234)) return 1;
    printf("WOW_WINDOW_GRAPH checks=%u errors=0\n", checks);
    return 0;
}
