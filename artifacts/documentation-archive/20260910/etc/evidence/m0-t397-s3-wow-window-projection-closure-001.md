# M0 T397 S3 — WOW window projection foundation closure

## Result

S3 closes the first public-Win32 WOW projection cohort. It does not claim a
complete Win16 user interface or lifecycle.

The new `adapter-mvdm-host-out/wow/wow_window_projection` adapter replaces
only the reached NT4-private class/window-word storage contract. It copies the
original `WC` values, registers a public class with a native proxy, keeps the
matching class/window records under the current `session`, and retires them
through the existing teardown list. Native `HWND`, `WNDPROC`, host pointer and
lease identity remain adapter-local. The only callback HWND written into the
original `WM32MSGPARAMEX` is a session-generated 16-bit ID.

## Original call-site disposition

| Original site | S3 disposition |
| --- | --- |
| `wow32/wuclass.c:WU32RegisterClass` | The reached `pfnRegisterClassWOWA` private slot becomes one copied-state public-class registration call. Failed public registration publishes no record. |
| `wow32/walias.c:FindPWC`, `FindPWW`, `FindClass16` | The three private-word lookups use only the projection's copied records. |
| `wow32/wmdisp32.c:W32Win16WndProcEx` and its create-message helpers | Initial state lookup and writes use the projection; callback HWND is opaque rather than a truncated native HWND. Existing message thunk and `CallBack16` order are retained. |

## Verification

1. The x86 fixture `build/M0-T397/S3/wow-window-projection-fixture.exe`
   completed with exit code zero. It proves public registration, copied class
   lookup, native proxy entry, nonzero opaque callback ID distinct from the
   native `HWND`, and stale class rejection after session teardown.
2. `wuclass.c`, `walias.c`, `wmdisp32.c`, and the adapter were individually
   compiled under the formal x86 provider flags. The full provider then linked
   without `/FORCE` against a freshly relinked parent import library.
3. The parent was relinked with only the existing `session_thread_current`
   and `session_register_teardown` lifecycle exports added to its WOW import
   contract. No DLL-local substitute session was introduced.
4. Fixed Console-owning observation
   `build/M0-T397/S3/wowexec-window-projection-observation.txt` ran
   `O:\t396\ntvdm32.exe -f -o --command system32\WOWEXEC.EXE` for 30,266 ms,
   loaded the new `O:\t396\WOW32.DLL`, and recorded no modal selection or CPU
   fault terminal. Its main stack contains `WOW32.DLL+0x41154`, which maps to
   original `WU32DefDlgProc` (`wumsg.c`); the preceding old
   `WU32RegisterClass` frontier is therefore crossed.

## Deferred boundary

The new terminal is an original dialog-procedure/message path, not evidence
of a complete dialog, menu, message-pump, task, DDE, or general Win16 UI
implementation. S4 owns its source-first disposition.
