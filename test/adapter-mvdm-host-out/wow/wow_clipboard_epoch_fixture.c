#include <windows.h>
#include <stdio.h>

static DWORD sequence, owner_pid, clears;
static BOOL locked, native_accept = TRUE, reenter;
static unsigned failures, assertions;
#define CHECK(x) do { ++assertions; if (!(x)) { ++failures; \
    printf("failed line %d: %s\n", __LINE__, #x); } } while (0)

DWORD WINAPI epoch_sequence(void) { CHECK(locked); return sequence; }
HWND WINAPI epoch_owner(void) { CHECK(locked); return (HWND)1; }
DWORD WINAPI epoch_owner_pid(HWND window, LPDWORD pid)
{
    (void)window;
    CHECK(locked);
    *pid = owner_pid;
    return 1;
}
DWORD WINAPI epoch_current_pid(void) { return 100; }
#include "../../../src/compat/include/wow_clipboard_epoch.h"
void WINAPI W32EmptyClipboard(void)
{
    CHECK(locked);
    ++clears;
    if (reenter) {
        reenter = FALSE;
        ++sequence;
        wow_clipboard_opened(W32EmptyClipboard);
    }
}
static BOOL WINAPI fixture_open(HWND hwnd)
{
    (void)hwnd;
    if (native_accept) locked = TRUE;
    return native_accept;
}
static BOOL WINAPI fixture_close(void) { locked = FALSE; return TRUE; }
/* Decoded handle carrier only; this fixture does not test guest frame layout. */
typedef struct { ULONG_PTR f1; } OPENCLIPBOARD16, *POPENCLIPBOARD16;
typedef OPENCLIPBOARD16 *PVDMFRAME;
static BOOL WU32NativeClipboardWindow(HWND guest, HWND *native)
{
    *native = guest;
    return TRUE;
}
#define FASTCALL
#define GETARGPTR(frame,size,arg) ((arg)=(frame))
#define FREEARGPTR(arg) ((void)(arg))
#define GETBOOL16(value) (value)
#define RETURN(value) return (value)
#define OpenClipboard fixture_open
#define CloseClipboard fixture_close
#include "selected-clipboard-open-close.inc"

static void cycle(void)
{
    OPENCLIPBOARD16 arg = {0};
    CHECK(WU32OpenClipboard(&arg));
    CHECK(WU32CloseClipboard(NULL));
}
int main(void)
{
    OPENCLIPBOARD16 arg = {0};
    sequence = 42; owner_pid = 200;
    cycle(); CHECK(clears == 1);
    cycle(); CHECK(clears == 1);
    /* Same process: Win16 handles remain valid across tasks and formats. */
    ++sequence; owner_pid = 100;
    cycle(); CHECK(clears == 1);
    /* Writer after unlock: the next successful open retires the old cache. */
    ++sequence; owner_pid = 200;
    cycle(); CHECK(clears == 2);
    /* Failed open must not read sequence, change association or clear cache. */
    ++sequence; native_accept = FALSE;
    CHECK(!WU32OpenClipboard(&arg)); CHECK(!locked); CHECK(clears == 2);
    native_accept = TRUE;
    cycle(); CHECK(clears == 3);
    /* No owner uses original empty-client semantics after a serial change. */
    ++sequence; owner_pid = 0;
    cycle(); CHECK(clears == 4);
    /* Original serial publication precedes the potentially reentrant clear. */
    ++sequence; owner_pid = 200; reenter = TRUE;
    cycle(); CHECK(clears == 6);
    cycle(); CHECK(clears == 6);
    printf("clipboard-open assertions=%u failures=%u clears=%lu\n",
        assertions, failures, clears);
    return failures ? 1 : 0;
}
