#include "dialog-lifetime/precomp.h"
#include <stdio.h>
#include <string.h>

/* Links the original dlgmgr.c modal slice, dlgend.c and SetDialogPointer,
 * not copies of their
 * algorithms. All native/creation/navigation dependencies are test doubles.
 * Supplemental source-control-flow evidence only; no guest or USER ABI proof.
 * Stage with the complete S3 group; not a separately executed repair probe. */
static DIALOG dialog;
static DLG state;
static WND owner, child;
static HWND active, focus;
static unsigned failures, locks, unlocks, enables, disables, destroys;
static unsigned shows, idles, posts, zaps, waits, foregrounds, peeks;
static unsigned sequence, hidden_at, posted_at, destroyed_at;
static BOOL fail_create, end_on_create, quit_message, owner_foreground;
static int reposted_quit;
static HWND create_owner;
enum retirement_edge { RETIRE_NONE, RETIRE_CANCEL, RETIRE_PEEK_EMPTY,
    RETIRE_PEEK_MESSAGE, RETIRE_SHOW, RETIRE_UPDATE, RETIRE_DISPATCH,
    RETIRE_QUIT };
static enum retirement_edge retirement_edge;
static unsigned retirement_flags;
static void retire_at(enum retirement_edge edge)
{
    if (retirement_edge != edge) return;
    if (retirement_flags & 1) SetWF(&dialog.wnd, WFDESTROYED);
    if (retirement_flags & 2) dialog.wnd.fnid |= FNID_CLEANEDUP_BIT;
    /* Preserve numeric validation and backing WND, but release PDLG.
     * This models retained callback identity, not a real HWND reuse test. */
    dialog.pdlg = NULL;
}
#define OWNER ((HWND)(ULONG_PTR)0x101)
#define CHILD ((HWND)(ULONG_PTR)0x102)
#define DIALOG_WINDOW ((HWND)(ULONG_PTR)0x103)
#define DESKTOP_WINDOW ((HWND)(ULONG_PTR)0x104)
#define CHECK(c) fixture_check((c), __LINE__)

void fixture_check(BOOL condition, int line)
{ if (!condition) { ++failures; printf("FAIL line %d\n", line); } }
static void reset(void)
{
    ZeroMemory(&dialog, sizeof(dialog)); ZeroMemory(&state, sizeof(state));
    ZeroMemory(&owner, sizeof(owner)); ZeroMemory(&child, sizeof(child));
    dialog.wnd.handle = DIALOG_WINDOW; dialog.wnd.alive = TRUE;
    dialog.wnd.spwndOwner = &owner; dialog.pdlg = &state;
    owner.handle = OWNER; owner.alive = TRUE;
    child.handle = CHILD; child.alive = TRUE;
    child.style = WS_CHILD; child.spwndParent = &owner;
    active = OWNER; focus = CHILD;
    locks = unlocks = enables = disables = destroys = 0;
    shows = idles = posts = zaps = waits = foregrounds = peeks = 0;
    sequence = hidden_at = posted_at = destroyed_at = 0;
    fail_create = end_on_create = quit_message = owner_foreground = FALSE;
    reposted_quit = -1; create_owner = NULL;
    retirement_edge = RETIRE_NONE; retirement_flags = 0;
}
PWND ValidateHwnd(HWND hwnd)
{
    if (hwnd == OWNER && owner.alive) return &owner;
    if (hwnd == CHILD && child.alive) return &child;
    if (hwnd == DIALOG_WINDOW && dialog.wnd.alive) return &dialog.wnd;
    return NULL;
}
PWND RevalidateHwnd(HWND hwnd) { return ValidateHwnd(hwnd); }
BOOL ValidateDialogPwnd(PWND window)
{ return window == &dialog.wnd && dialog.pdlg != NULL; }
PWND GetTopLevelWindow(PWND window)
{ CHECK(window == &child); return &owner; }
void ThreadLock(PWND window, TL *lock)
{ CHECK(window == &owner); lock->held = window; ++locks; }
void ThreadUnlock(TL *lock)
{ CHECK(lock->held == &owner); lock->held = NULL; ++unlocks; }
HWND fixture_desktop(void) { return DESKTOP_WINDOW; }
HWND fixture_active(void) { return active; }
HWND fixture_capture(void) { return retirement_edge == RETIRE_CANCEL ? CHILD : NULL; }
HWND fixture_focus(void) { return focus; }
BOOL fixture_is_window(HWND window) { return ValidateHwnd(window) != NULL; }
BOOL fixture_is_child(HWND parent, HWND candidate)
{ return candidate == CHILD && child.spwndParent && HWq(child.spwndParent) == parent; }
int fixture_metric(const char *name)
{ CHECK(!strcmp(name, "MOUSEPRESENT") || !strcmp(name, "SLOWMACHINE")); return 0; }
BOOL NtUserEnableWindow(HWND window, BOOL enabled)
{
    CHECK(window == OWNER);
    if (enabled) { ++enables; owner.style &= ~WS_DISABLED; }
    else { ++disables; owner.style |= WS_DISABLED; }
    return TRUE;
}
HWND NtUserSetFocus(HWND window) { HWND previous = focus; focus = window; return previous; }
HWND NtUserSetActiveWindow(HWND window) { HWND previous = active; active = window; return previous; }
HCURSOR fixture_cursor(HINSTANCE module, LPCSTR name)
{ (void)module; (void)name; CHECK(FALSE); return NULL; }
HCURSOR NtUserSetCursor(HCURSOR cursor) { CHECK(FALSE); return cursor; }
BOOL NtUserShowWindow(HWND window, int command)
{ CHECK(window == DIALOG_WINDOW && command == SHOW_OPENWINDOW && dialog.pdlg); ++shows; dialog.wnd.style |= WS_VISIBLE; retire_at(RETIRE_SHOW); return TRUE; }
BOOL fixture_update(HWND window) { CHECK(window == DIALOG_WINDOW && dialog.pdlg); retire_at(RETIRE_UPDATE); return TRUE; }
BOOL NtUserSetWindowPos(HWND window, HWND after, int x, int y, int cx, int cy, UINT flags)
{
    CHECK(window == DIALOG_WINDOW && after == NULL && !x && !y && !cx && !cy);
    CHECK(flags == (SWP_HIDEWINDOW | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER));
    CHECK(state.fEnd); dialog.wnd.style &= ~WS_VISIBLE;
    hidden_at = ++sequence; return TRUE;
}
BOOL fixture_post(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    CHECK(window == DIALOG_WINDOW && message == WM_NULL && !wp && !lp);
    CHECK(state.fEnd && hidden_at); ++posts; posted_at = ++sequence; return TRUE;
}
ULONG_PTR NtUserCallNoParam(int operation)
{ CHECK(operation == SFI_ZAPACTIVEANDFOCUS); ++zaps; active = focus = NULL; return 0; }
BOOL NtUserDestroyWindow(HWND window)
{
    CHECK(window == DIALOG_WINDOW && dialog.wnd.alive);
    ++destroys; destroyed_at = ++sequence;
    dialog.wnd.alive = FALSE; dialog.pdlg = NULL; return TRUE;
}
ULONG_PTR NtUserQueryWindow(HWND window, int query)
{ CHECK(window == OWNER && query == WindowIsForegroundThread); return owner_foreground; }
ULONG_PTR NtUserGetThreadState(int query)
{ CHECK(query == UserThreadStateIsForeground); return TRUE; }
BOOL NtUserSetForegroundWindow(HWND window)
{ CHECK(window == OWNER); ++foregrounds; return TRUE; }
BOOL NtUserWaitMessage(void) { ++waits; CHECK(FALSE); return FALSE; }
BOOL fixture_peek(MSG *message, HWND window, UINT first, UINT last, UINT flags)
{
    CHECK(!window && !first && !last && flags == PM_REMOVE);
    CHECK(++peeks < 4);
    retire_at(RETIRE_PEEK_EMPTY);
    if (retirement_edge == RETIRE_PEEK_EMPTY) return FALSE;
    if (retirement_edge == RETIRE_PEEK_MESSAGE || retirement_edge == RETIRE_DISPATCH) {
        retire_at(RETIRE_PEEK_MESSAGE);
        ZeroMemory(message, sizeof(*message));
        message->hwnd = DIALOG_WINDOW; message->message = WM_TIMER;
        return TRUE;
    }
    if (quit_message) {
        retire_at(RETIRE_QUIT);
        ZeroMemory(message, sizeof(*message)); message->message = WM_QUIT;
        message->wParam = 37; quit_message = FALSE; return TRUE;
    }
    return FALSE;
}
LRESULT fixture_send(HWND window, UINT message, WPARAM wp, LPARAM lp)
{
    if (message == WM_CANCELMODE) {
        CHECK(window == CHILD && !wp && !lp); retire_at(RETIRE_CANCEL); return 0;
    }
    CHECK(window == OWNER && message == WM_ENTERIDLE && wp == MSGF_DIALOGBOX);
    CHECK((HWND)lp == DIALOG_WINDOW); ++idles;
    CHECK(EndDialog(DIALOG_WINDOW, 73)); return 0;
}
BOOL fixture_dialog_message(HWND window, MSG *message)
{ CHECK(window == DIALOG_WINDOW && message->message == WM_TIMER && dialog.pdlg); return FALSE; }
BOOL fixture_translate(const MSG *message) { CHECK(message->message == WM_TIMER && dialog.pdlg); return FALSE; }
LRESULT fixture_dispatch(const MSG *message) { CHECK(message->message == WM_TIMER && dialog.pdlg); retire_at(RETIRE_DISPATCH); return 0; }
void fixture_quit(int code) { reposted_quit = code; }
HWND InternalCreateDialog(HANDLE module, LPDLGTEMPLATE resource, DWORD bytes,
    HWND window, DLGPROC procedure, LONG parameter, UINT flags)
{
    CHECK(module == NULL && resource == NULL && bytes == 0);
    CHECK(procedure == NULL && parameter == 17 && flags == SCDLG_ANSI);
    create_owner = window;
    if (fail_create) return NULL;
    dialog.wnd.spwndOwner = window ? &owner : NULL;
    if (end_on_create) CHECK(EndDialog(DIALOG_WINDOW, 19));
    return DIALOG_WINDOW;
}
static int modal(HWND window)
{ return InternalDialogBox(NULL, NULL, 0, window, NULL, 17, SCDLG_ANSI); }
static void verify_dialog_pointer(void)
{
    DIALOG retained = {0};
    DLG data = {0};
    retained.resultWP = 37;
    retained.unused = 91;
    retained.wnd.style = WS_VISIBLE;
    SetDialogPointer(&retained.wnd, (LONG)&data);
    CHECK(retained.pdlg == &data && retained.wnd.fnid == FNID_DIALOG);
    CHECK(TestWF(&retained.wnd, WFDIALOGWINDOW));
    CHECK(TestWF(&retained.wnd, WFVISIBLE));
    CHECK(retained.resultWP == 37 && retained.unused == 91);
    SetDialogPointer(&retained.wnd, 0);
    CHECK(!retained.pdlg && !TestWF(&retained.wnd, WFDIALOGWINDOW));
    CHECK(retained.wnd.fnid == (FNID_DIALOG | FNID_CLEANEDUP_BIT));
    CHECK(retained.resultWP == 37 && retained.unused == 91);
    /* The original transition does not replace an existing FNID or clear
     * cleanup status. Its validating caller prevents resurrection. */
    retained.wnd.fnid = FNID_STATIC;
    SetDialogPointer(&retained.wnd, (LONG)&data);
    CHECK(retained.wnd.fnid == FNID_STATIC && retained.pdlg == &data);
    SetDialogPointer(&retained.wnd, 0);
    SetDialogPointer(&retained.wnd, 0);
    CHECK(retained.wnd.fnid == (FNID_STATIC | FNID_CLEANEDUP_BIT));
}
int main(void)
{
    unsigned edge, flags;
    verify_dialog_pointer();
    reset(); CHECK(modal(OWNER) == 73);
    CHECK(disables == 1 && enables == 1 && shows == 1 && idles == 1);
    CHECK(posts == 1 && destroys == 1 && hidden_at < posted_at && posted_at < destroyed_at);
    CHECK(foregrounds == 1 && waits == 0 && !locks && !unlocks);

    reset(); owner.style |= WS_DISABLED; CHECK(modal(OWNER) == 73);
    CHECK(disables == 1 && enables == 0 && state.fDisabled);

    reset(); fail_create = TRUE; CHECK(modal(CHILD) == -1);
    CHECK(create_owner == OWNER && locks == 1 && unlocks == 1);
    CHECK(disables == 1 && enables == 1 && !destroys && !peeks);

    reset(); CHECK(modal((HWND)(ULONG_PTR)0x999) == 0);
    CHECK(!disables && !enables && !create_owner && !peeks);

    reset(); quit_message = TRUE; CHECK(modal(OWNER) == 0);
    CHECK(reposted_quit == 37 && destroys == 1 && !posts && !idles);

    reset(); end_on_create = TRUE; CHECK(modal(OWNER) == 19);
    CHECK(!peeks && !shows && destroys == 1 && posts == 1);

    reset(); end_on_create = TRUE; CHECK(modal(NULL) == 19);
    CHECK(!state.fDisabled && !create_owner && !disables && !enables);
    CHECK(!peeks && !shows && destroys == 1 && posts == 1);
    reset(); fail_create = TRUE; CHECK(modal(NULL) == -1);
    CHECK(!create_owner && !disables && !enables && !destroys && !peeks);

    reset(); child.spwndParent = &dialog.wnd; active = DIALOG_WINDOW;
    CHECK(EndDialog(DIALOG_WINDOW, -7));
    CHECK(state.fEnd && state.result == -7 && focus == DIALOG_WINDOW);
    CHECK(active == OWNER && enables == 1 && !destroys && posts == 1);

    reset(); dialog.wnd.spwndOwner = NULL; active = DIALOG_WINDOW;
    CHECK(EndDialog(DIALOG_WINDOW, 5));
    CHECK(zaps == 1 && !active && !focus && !enables && posts == 1);

    reset(); CHECK(!EndDialog(OWNER, 1)); CHECK(!posts && !enables && !destroys);
    reset(); CHECK(!EndDialog((HWND)(ULONG_PTR)0x999, 1)); CHECK(!posts);
    for (edge = RETIRE_CANCEL; edge <= RETIRE_QUIT; ++edge) {
        for (flags = 1; flags <= 3; ++flags) {
            reset(); retirement_edge = (enum retirement_edge)edge;
            retirement_flags = flags; quit_message = edge == RETIRE_QUIT;
            CHECK(modal(OWNER) == 0);
            CHECK(!dialog.pdlg && !destroys && !posts && !waits && !foregrounds);
            CHECK(disables == 1 && !enables);
            CHECK(reposted_quit == (edge == RETIRE_QUIT ? 37 : -1));
            CHECK(shows == (edge == RETIRE_SHOW || edge == RETIRE_UPDATE ? 1 : 0));
        }
    }
    printf("dialog_lifetime_failures=%u\n", failures);
    return failures != 0;
}
