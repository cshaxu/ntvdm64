#include "compat/include/wow_window_capture.h"
#include <dde.h>

typedef mvdm_softpc_wow_window_observation window_observation;

BOOL WINAPI wow_user_window_sample_links(HWND window, wow_user_window_links *output)
{
    wow_user_window_links links = {0};
    WINDOWINFO before = {sizeof(before)};
    DWORD saved = GetLastError();
    if (!window || !output) {
        SetLastError(ERROR_INVALID_PARAMETER); return FALSE;
    }
    if (!GetWindowInfo(window, &before)) return FALSE;
    links.window = window;
    /* Original WND fields are distinct: getset.c maps a top-level parent
     * query to spwndOwner, but the stored spwndParent is still the desktop.
     * GetParent therefore cannot produce this shared-view field. */
    if (window != GetDesktopWindow()) {
        links.next = GetWindow(window, GW_HWNDNEXT);
        links.parent = GetAncestor(window, GA_PARENT);
        links.owner = GetWindow(window, GW_OWNER);
    }
    links.child = GetWindow(window, GW_CHILD);
    /* These are observations, not a native desktop lock. The composed
     * transaction validates references/chains, not later external freshness. */
    *output = links;
    SetLastError(saved);
    return TRUE;
}

static ULONG position_roots(const WINDOWPOS *position, HWND *roots)
{
    ULONG count = 0;
    if (!position) return 0;
    roots[count++] = position->hwnd;
    /* putwindowpos16 copies this typed field even with SWP_NOZORDER. */
    if (position->hwndInsertAfter != HWND_TOP &&
            position->hwndInsertAfter != HWND_BOTTOM &&
            position->hwndInsertAfter != HWND_TOPMOST &&
            position->hwndInsertAfter != HWND_NOTOPMOST)
        roots[count++] = position->hwndInsertAfter;
    return count;
}

/* wmdisp32.c/wstruc.c own the actual thunk, copyback and guest allocation.
 * This seam describes only their native window operands for shared-view
 * publication. In particular owner-draw hwndItem can be an HMENU, and
 * WM_PARENTNOTIFY mouse coordinates are not an HWND. No type guessing from
 * numeric validity and no recursion through arbitrary pointer payloads. */
ULONG WINAPI wow_user_message_window_roots(UINT message, WPARAM wp,
    LPARAM lp, HWND roots[4])
{
    ULONG count = 0;
    /* Original winuserp.h private message numbers, absent from modern SDK
     * message defines. DROPSTRUCT itself remains the SDK's exact ABI type. */
    enum { original_winhelp = 0x0038, original_dropobject = 0x022a,
        original_querydropobject = 0x022b, original_dragloop = 0x022d,
        original_dragselect = 0x022e, original_dragmove = 0x022f };
    switch (message) {
    case original_dropobject: case original_querydropobject:
    case original_dragloop: case original_dragselect: case original_dragmove:
        if (lp) {
            roots[count++] = ((const DROPSTRUCT *)lp)->hwndSource;
            roots[count++] = ((const DROPSTRUCT *)lp)->hwndSink;
        }
        break;
    case WM_CREATE: case WM_NCCREATE:
        if (lp) roots[count++] = ((const CREATESTRUCTA *)lp)->hwndParent;
        break;
    case WM_ACTIVATE: case WM_VKEYTOITEM: case WM_CHARTOITEM:
    case WM_COMMAND: case WM_HSCROLL: case WM_VSCROLL:
    case WM_CTLCOLORMSGBOX: case WM_CTLCOLOREDIT: case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN: case WM_CTLCOLORDLG: case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORSTATIC:
        roots[count++] = (HWND)lp;
        break;
    case WM_SETFOCUS: case WM_KILLFOCUS: case WM_SETCURSOR: case WM_INITDIALOG:
    case WM_MOUSEACTIVATE: case WM_MDIDESTROY: case WM_MDIRESTORE:
    case WM_MDINEXT: case WM_MDIMAXIMIZE: case WM_SIZECLIPBOARD:
    case WM_PAINTCLIPBOARD: case WM_HSCROLLCLIPBOARD: case WM_VSCROLLCLIPBOARD:
    case WM_PALETTEISCHANGING: case WM_PALETTECHANGED:
    case WM_COPYDATA: case original_winhelp:
    case WM_DDE_INITIATE: case WM_DDE_TERMINATE: case WM_DDE_ADVISE:
    case WM_DDE_UNADVISE: case WM_DDE_ACK: case WM_DDE_DATA:
    case WM_DDE_REQUEST: case WM_DDE_POKE: case WM_DDE_EXECUTE:
        roots[count++] = (HWND)wp;
        break;
    case WM_NEXTDLGCTL:
        if (lp) roots[count++] = (HWND)wp;
        break;
    case WM_ENTERIDLE:
        if (wp == MSGF_DIALOGBOX || wp == MSGF_MENU) roots[count++] = (HWND)lp;
        break;
    case WM_PARENTNOTIFY:
        if (LOWORD(wp) == WM_CREATE || LOWORD(wp) == WM_DESTROY)
            roots[count++] = (HWND)lp;
        break;
    case WM_MDIACTIVATE: case WM_CHANGECBCHAIN:
        roots[count++] = (HWND)wp;
        roots[count++] = (HWND)lp;
        break;
    case WM_DRAWITEM:
        if (lp && ((const DRAWITEMSTRUCT *)lp)->CtlType != ODT_MENU)
            roots[count++] = ((const DRAWITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_DELETEITEM:
        if (lp && ((const DELETEITEMSTRUCT *)lp)->CtlType != ODT_MENU)
            roots[count++] = ((const DELETEITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_COMPAREITEM:
        if (lp && ((const COMPAREITEMSTRUCT *)lp)->CtlType != ODT_MENU)
            roots[count++] = ((const COMPAREITEMSTRUCT *)lp)->hwndItem;
        break;
    case WM_WINDOWPOSCHANGING: case WM_WINDOWPOSCHANGED:
        count = position_roots((const WINDOWPOS *)lp, roots);
        break;
    case WM_NCCALCSIZE:
        if (wp && lp)
            count = position_roots(((const NCCALCSIZE_PARAMS *)lp)->lppos, roots);
        break;
    case WM_GETDLGCODE:
        if (lp) {
            const MSG *nested = (const MSG *)lp;
            roots[count++] = nested->hwnd;
            /* putmsg16 converts only COMMAND, DDE and DROPFILES here.
             * DROPFILES has an HDROP, not another window. */
            if (nested->message == WM_COMMAND) roots[count++] = (HWND)nested->lParam;
            else if (nested->message >= WM_DDE_FIRST && nested->message <= WM_DDE_LAST)
                roots[count++] = (HWND)nested->wParam;
        }
        break;
    }
    return count;
}

ULONG WINAPI wow_user_queue_window_roots(const MSG *message, HWND roots[4])
{
    ULONG count = 0;
    if (!message) return 0;
    roots[count++] = message->hwnd;
    /* wstruc.c putmsg16 does not follow arbitrary posted pointer payloads. */
    if (message->message == WM_COMMAND) roots[count++] = (HWND)message->lParam;
    else if (message->message >= WM_DDE_FIRST && message->message <= WM_DDE_LAST)
        roots[count++] = (HWND)message->wParam;
    return count;
}

ULONG WINAPI wow_user_message_return_roots(UINT message, WPARAM wp, LPARAM lp,
    LRESULT result, HWND roots[4])
{
    /* wmsg16.c unthunkwmmsg16: these results/copyback fields, not an
     * arbitrary nonzero LRESULT, are converted to guest window identities.
     * Read the native output synchronously before restoring execution. */
    switch (message) {
    case WM_DRAWITEM: case WM_DELETEITEM: case WM_COMPAREITEM:
        return wow_user_message_window_roots(message, wp, lp, roots);
    case WM_MDICREATE: case WM_MDIGETACTIVE:
        roots[0] = (HWND)result;
        return 1;
    case WM_NEXTMENU:
        if (lp) { roots[0] = ((const MDINEXTMENU *)lp)->hwndNext; return 1; }
        break;
    case WM_WINDOWPOSCHANGING: case WM_WINDOWPOSCHANGED:
        return position_roots((const WINDOWPOS *)lp, roots);
    case WM_NCCALCSIZE:
        if (wp && lp)
            return position_roots(((const NCCALCSIZE_PARAMS *)lp)->lppos, roots);
        break;
    }
    return 0;
}

/* A public USER sample has no desktop lock. Capture a single identity and
 * its fields; only the identity itself is checked again, never neighbours. */
BOOL WINAPI wow_native_window_capture(HWND window, window_observation *row)
{
    WINDOWINFO info = {sizeof(info)};
    wow_user_window_links links;
    DWORD pid = 0, tid;
    int length;
    ZeroMemory(row, sizeof(*row));
    row->thread_id = GetWindowThreadProcessId(window, &row->process_id);
    if (!row->thread_id || !GetWindowInfo(window, &info) ||
            !wow_user_window_sample_links(window, &links)) return FALSE;
    row->links.window = (ULONG)(ULONG_PTR)window;
    row->links.next = (ULONG)(ULONG_PTR)links.next;
    row->links.parent = (ULONG)(ULONG_PTR)links.parent;
    row->links.child = (ULONG)(ULONG_PTR)links.child;
    row->links.owner = (ULONG)(ULONG_PTR)links.owner;
    CopyMemory(row->window_rect, &info.rcWindow, sizeof(RECT));
    CopyMemory(row->client_rect, &info.rcClient, sizeof(RECT));
    row->style = info.dwStyle; row->exstyle = info.dwExStyle;
    SetLastError(ERROR_SUCCESS);
    row->module = (DWORD)GetWindowLongA(window, GWL_HINSTANCE);
    if (!row->module && GetLastError() != ERROR_SUCCESS) return FALSE;
    /* Public native property, not an NT4 private atom or inferred flag.
     * Zero denotes no local ID; original help.c owns ancestor inheritance. */
    row->context_help_id = GetWindowContextHelpId(window);
    /* Match original TestwndChild: WS_POPUP excludes the child-ID role.
     * A real ID zero is valid; a failed native query is not a zero ID that
     * may be committed and then copied into recovered WND.spmenu. */
    if ((row->style & (WS_CHILD | WS_POPUP)) == WS_CHILD) {
        SetLastError(ERROR_SUCCESS);
        row->control_id = (DWORD)GetWindowLongA(window, GWL_ID);
        if (!row->control_id && GetLastError() != ERROR_SUCCESS) return FALSE;
    }
    row->descriptor.atom = info.atomWindowType;
    length = GetClassNameA(window, row->descriptor.name, sizeof(row->descriptor.name));
    if (!length) return FALSE;
    if (length >= sizeof(row->descriptor.name) - 1) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER); return FALSE;
    }
    row->descriptor.name_bytes = (ULONG)length + 1u;
    SetLastError(ERROR_SUCCESS);
    row->descriptor.class_style = GetClassLongA(window, GCL_STYLE);
    if (!row->descriptor.class_style && GetLastError()) return FALSE;
    SetLastError(ERROR_SUCCESS);
    row->descriptor.window_extra = GetClassLongA(window, GCL_CBWNDEXTRA);
    if (!row->descriptor.window_extra && GetLastError()) return FALSE;
    tid = GetWindowThreadProcessId(window, &pid);
    if (tid != row->thread_id || pid != row->process_id) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE); return FALSE;
    }
    return TRUE;
}

/* This index/array is transaction scratch; the page-domain remains the sole
 * retained observation owner. Full generations never alias on a low word. */
BOOL WINAPI wow_native_window_observation_put(window_observation **rows, ULONG *count,
    ULONG *capacity, ULONG *index, const window_observation *value)
{
    ULONG slot = LOWORD(value->links.window), at = index[slot];
    window_observation *grown;
    if (!slot || slot == 0xffffu) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
    if (at) {
        window_observation *previous = &(*rows)[at - 1u];
        if (previous->links.window != value->links.window &&
                previous->process_id == GetCurrentProcessId() &&
                IsWindow((HWND)(ULONG_PTR)previous->links.window)) {
            SetLastError(ERROR_INVALID_STATE); return FALSE;
        }
        *previous = *value;
        return TRUE;
    }
    if (*count == *capacity) {
        ULONG size = *capacity ? *capacity * 2u : 64u;
        if (size > 0xfffeu) size = 0xfffeu;
        if (size <= *capacity) { SetLastError(ERROR_INVALID_DATA); return FALSE; }
        grown = *rows ? HeapReAlloc(GetProcessHeap(), 0, *rows, size * sizeof(**rows)) :
            HeapAlloc(GetProcessHeap(), 0, size * sizeof(**rows));
        if (!grown) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
        *rows = grown; *capacity = size;
    }
    (*rows)[*count] = *value;
    index[slot] = ++*count;
    return TRUE;
}

ULONG * WINAPI wow_native_window_observation_edge(window_observation *row, unsigned edge)
{
    if (edge == 0) return &row->links.next;
    if (edge == 1) return &row->links.parent;
    if (edge == 2) return &row->links.owner;
    return &row->links.child;
}

/* Native concurrent sampling can join an old external edge to a new local
 * chain. Break only an external edge of such a cycle. A local-only cycle
 * rejects this acquisition; the caller may reacquire once before publication.
 * Gray/black traversal itself remains linear. */
BOOL WINAPI wow_native_window_normalize_chains(window_observation *rows, ULONG count,
    const ULONG *index, BOOL *capture_raced)
{
    BYTE *state = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, count);
    ULONG start, at, next, cut;
    unsigned edge;
    DWORD pid = GetCurrentProcessId();
    if (capture_raced) *capture_raced = FALSE;
    if (!state) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return FALSE; }
    for (edge = 0; edge < 3u; ++edge) {
        ZeroMemory(state, count);
        for (start = 0; start < count; ++start) {
            at = start;
            while (!state[at]) {
                state[at] = 1;
                next = *wow_native_window_observation_edge(&rows[at], edge);
                if (!next) break;
                at = index[LOWORD(next)] - 1u;
            }
            if (state[at] == 1 && *wow_native_window_observation_edge(&rows[at], edge)) {
                cut = at;
                do {
                    if (rows[cut].process_id != pid) break;
                    cut = index[LOWORD(*wow_native_window_observation_edge(&rows[cut], edge))] - 1u;
                } while (cut != at);
                if (rows[cut].process_id == pid) {
                    /* ADAPTER-WOW-102: public local queries are not atomic.
                     * This transaction has not published any captured state. */
                    if (capture_raced) *capture_raced = TRUE;
                    HeapFree(GetProcessHeap(), 0, state);
                    SetLastError(ERROR_INVALID_DATA); return FALSE;
                }
                /* Mark the whole cycle before cutting it; the now-detached
                 * remainder must not be mistaken for another gray cycle. */
                next = at;
                do {
                    state[next] = 2;
                    next = index[LOWORD(*wow_native_window_observation_edge(&rows[next], edge))] - 1u;
                } while (next != at);
                *wow_native_window_observation_edge(&rows[cut], edge) = 0;
            }
            at = start;
            while (state[at] == 1) {
                state[at] = 2;
                next = *wow_native_window_observation_edge(&rows[at], edge);
                if (!next) break;
                at = index[LOWORD(next)] - 1u;
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, state);
    return TRUE;
}
