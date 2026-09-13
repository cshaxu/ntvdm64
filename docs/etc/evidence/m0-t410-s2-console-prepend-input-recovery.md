# M0 T410 S2 — Console returned-input ordering recovery

## Scope

This record implements only D28 and the NOWAIT/NOREMOVE portion of D29. It
does not claim keyboard normalization, layout ownership, Alt+Enter pairing,
graphics presentation, or interactive COMMAND/EDIT acceptance.

## Original contract and selected boundary

OpenNT `windows/core/ntcon/client/iostubs.c` sends `WriteConsoleInputVDMW`
with `Append == FALSE`. The corresponding server body in
`windows/core/ntcon/server/directio.c` selects `PrependInputBuffer` while
holding the Console lock. Thus a VDM's returned records are observed before
later public Console input; public `WriteConsoleInputW` append is not
equivalent.

The original client/server transport cannot be imported into a standalone
modern Console process: it depends on the retired Console Server request and
Console-lock ownership. The selected adapter therefore supplies only its
unavailable mechanical portion: a process-local, lock-protected returned
record deque and manual-reset wait event. The original `nt_event` loop is
still the sole record consumer. It waits on its original public input and
suspend handles plus the adapter event, then calls the same
`ReadConsoleInputExW(sc.InputHandle, ..., CONSOLE_READ_NOWAIT)` shape.

No background reader, GUI route, guest-memory record, mouse synthesis, or
new application queue is added. The selected worker has one stable Console
input handle; multi-worker/session reuse is explicitly outside this S2 seam.

## Verification

`tests/adapter-mvdm-host-out/win32/console_input_contract_fixture.c` now
places a public `T` record in `CONIN$`, returns a VDM `F` record, proves the
private event is signalled, verifies `NOWAIT|NOREMOVE` observes `F` without
consuming it, consumes `F`, observes that the private event resets, then
consumes public `T`. This is the direct positive/negative ordering contract.

The fixture and adapter compiled and linked as x86 from
`build/M0-T388/S7/console-contract-x86`. The noninteractive automation
environment supplies a pipe rather than `CONIN$`; the executable exits `18`
at its deliberate "not a Console input handle" guard. That is an environment
limitation, not a runtime pass. The fixture remains the required real
conhost/Terminal acceptance command for the next interactive run.

`git diff --check` and documentation governance remain required before the S2
packet commit. A formal worker relink is recorded with that commit; no runtime
product artifact is deployed until the real-Console fixture passes.
