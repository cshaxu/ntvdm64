# M0 T310 S8 P4 Console-menu call-contract closure

## Question

Can the selected original SoftPC mouse and host-control callers compile with
the exact `ConsoleMenuControl` call contract on both supported host
architectures without inventing a replacement Console Server?

## Inputs

- `src/mvdm-host/softpc.new/host/src/nt_mouse.c`
- `src/mvdm-host/softpc.new/host/src/nt_hosts.c`
- `src/mvdm-host/softpc.new/host/src/nt_event.c`
- `O:/repos.external/OpenNT/public/internal/windows/inc/conapi.h`
- `O:/repos.external/OpenNT/windows/core/ntcon/client/private.c`
- `O:/repos.external/OpenNT/windows/core/ntcon/server/private.c`

## Procedure and observations

The original public declaration is `HMENU ConsoleMenuControl(HANDLE, UINT,
UINT)`. The original NT4 client sends the output handle and command range to
Console Server. Its server implementation both returns the buffer's menu and
records `CommandIdLow`/`CommandIdHigh` on that buffer. The selected SoftPC
mouse caller then appends `IDM_POINTER`; the original event path consumes that
command in `nt_event.c`.

The current modern declaration bridge had omitted this function. Under both
MSVC targets `nt_mouse.c` therefore compiled it as an implicit `int` call,
which is an invalid `HMENU` return ABI on x64. The bridge now exposes the
exact source-shaped declaration. Focused formal Ninja rebuilds of
`obj/host/nt_mouse.obj` on x86 and x64 contain no `ConsoleMenuControl` C4013
diagnostic.

## Interpretation

This closes the declaration/call-width record only. It does not claim a
modern runtime implementation: public Win32 does not expose the NT4 Console
Server command-range registration and input-event route. Returning a system
menu through `GetSystemMenu` would make `AppendMenu` appear to work while
silently losing the original `IDM_POINTER` event delivery, so it is expressly
rejected. A future console-provider recovery must retain the original
range-to-event contract or report the original call as unavailable.

No guest pointer, native identity token, mapping-manager lease, controller
algorithm or warning suppression was added.

## Follow-up

Keep `ConsoleMenuControl` in the SoftPC console-control boundary ledger until
a source-shaped console event provider is admitted. The remaining warnings
from this focused object are historical macro, unused-parameter and fixed
guest-scalar records; they are not hidden by this change.
