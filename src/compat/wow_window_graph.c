#include "compat/include/wow_window_graph.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Validate copied reader structure, not whether native USER stayed unchanged.
 * Parent/child symmetry is deliberately not required: original creation and
 * destruction expose unlinked WNDs. Scratch belongs only to this transaction. */
int compat_wow_window_graph_validate(ULONG count,
    const mvdm_softpc_wow_window_links *links, ULONG *index, ULONG desktop)
{
    ULONG i, edge, row, next;
    BYTE *state;
    DWORD error = ERROR_INVALID_DATA, saved = GetLastError();
    int result = 0;
    state = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count);
    if (!state) { error = ERROR_NOT_ENOUGH_MEMORY; goto done; }
    for (i = 0; i < count; ++i) {
        WORD slot = LOWORD(links[i].window);
        if (!slot || slot == 0xffffu || index[slot]) {
            error = ERROR_INVALID_PARAMETER; goto done;
        }
        index[slot] = i + 1u;
    }
    for (i = 0; i < count; ++i) {
        ULONG targets[4];
        if (links[i].window == desktop &&
                (links[i].next || links[i].parent || links[i].owner)) goto done;
        targets[0] = links[i].next; targets[1] = links[i].parent;
        targets[2] = links[i].child; targets[3] = links[i].owner;
        for (edge = 0; edge < 4u; ++edge) if (targets[edge]) {
            row = index[LOWORD(targets[edge])];
            if (!row || links[row - 1u].window != targets[edge]) goto done;
        }
    }
    /* Each relation is a functional graph. Mark a path grey, then black;
     * encountering grey is a cycle. This is linear, with no recursive stack
     * or per-node desktop traversal. Child-list traversal follows next. */
    for (edge = 0; edge < 3u; ++edge) {
        ZeroMemory(state, count);
        for (i = 0; i < count; ++i) {
            row = i + 1u;
            while (row && !state[row - 1u]) {
                const mvdm_softpc_wow_window_links *node = &links[row - 1u];
                state[row - 1u] = 1;
                next = edge == 0 ? node->next : edge == 1 ? node->parent : node->owner;
                row = next ? index[LOWORD(next)] : 0;
            }
            if (row && state[row - 1u] == 1) goto done;
            row = i + 1u;
            while (row && state[row - 1u] == 1) {
                const mvdm_softpc_wow_window_links *node = &links[row - 1u];
                state[row - 1u] = 2;
                next = edge == 0 ? node->next : edge == 1 ? node->parent : node->owner;
                row = next ? index[LOWORD(next)] : 0;
            }
        }
    }
    result = 1;
done:
    if (state) HeapFree(GetProcessHeap(), 0, state);
    SetLastError(result ? saved : error);
    return result;
}
