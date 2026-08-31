# M0 T318 S2 P19 — Windowed Console Registration Closure

## Question

Can the selected original CPU40 SoftPC startup retain the original
`nt_det.c::initTextSection` registration call on a modern host without
claiming that the retired NT4 Console Server fullscreen protocol exists?

## Source Finding

After P18 removed the C-VID null-vector transfer, the bounded x86
console-owned observation displayed the original `DisplayErrorTerm` text
“This function is not supported on this system.”  That text is the system
message for `ERROR_CALL_NOT_IMPLEMENTED` (120).

The first reached producer is original
`softpc.new/host/src/nt_det.c::initTextSection`.  It calls
`RegisterConsoleVDM(CONSOLE_REGISTER_VDM, ..., &stateLength, &videoState,
..., textBufferSize, &textBuffer)` and routes a false result directly through
`ErrorExit`.  The previous same-shaped adapter deliberately returned 120 for
every registration, even though this selected windowed path only needs the
returned host-local text buffer.  The original code already accepts a zero
state length when fullscreen hardware is unavailable.

## Change

`adapter-mvdm-host-out/win32/source/console_compat.c` keeps the exact
`RegisterConsoleVDM` signature and now implements the smallest source-shaped
windowed registration:

- validates the original registration selector and output arguments;
- obtains the currently bound active session;
- allocates a zeroed host-local text buffer sized by the original `COORD`;
- returns `state == NULL` and `stateLength == 0`, preserving the original
  windowed/fullscreen-disabled branch;
- owns the allocation per session and releases it through the existing
  session teardown contract; and
- preserves `CONSOLE_UNREGISTER_VDM` as an idempotent release.

This is registered as `ADAPTER-WIN32-032`.  It does **not** supply NT4
Console Server shared mappings, fullscreen ownership, hardware events,
controller handshakes, system-menu routing, or a replacement guest display.
The returned pointers are host-private SoftPC buffers, never guest addresses,
so the guest-memory mapping manager is intentionally not involved.

## Verification

Both formal product rows rebuilt and linked normally:

```text
build/M0-T318/S2/cpu40-v7vga-r4/run-ninja-parallel.cmd original-softpc-process.exe
build/M0-T318/S2/cpu40-v7vga-r4-x64/run-ninja-parallel.cmd original-softpc-process.exe
```

A fresh debugger-neutral, console-owning x86 observation of the staged
product remained alive for the bounded interval and consumed 2859 ms CPU;
the previous error dialog was absent:

```text
state=1964 timeout 259 cpu-ms 2859
display-error not-installed target 0x00000000
```

This establishes only the original windowed console-registration prerequisite.
It does not establish NTIO, NTDOS, guest `EXEC`, parent restoration, or
`54:0B` continuity.

## Disposition

The P18-to-P19 startup blocker is closed for the selected windowed CPU40
profile.  Fullscreen Console Server protocol remains a separately explicit
host-capability boundary; the next runtime observation must classify the next
original stop without extending the console adapter beyond this contract.
