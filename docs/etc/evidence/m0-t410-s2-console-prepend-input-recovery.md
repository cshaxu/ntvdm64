# M0 T410 S2 — Console returned-input ordering recovery

## Scope

This record implements D28, the returned-record/NOWAIT/NOREMOVE and
Alt+Enter-pair portions of D29, and the source-composable character branch of
D24. It does not claim Console layout ownership, graphics presentation, or
interactive COMMAND/EDIT acceptance.

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

The same fixture then queues Alt+Enter down, Alt+Enter up, and `K`. The
adapter consumes the reserved pair, including when the caller asks for
`NOREMOVE`, and exposes only `K`; the later ordinary read still obtains `K`.
This matches the original `HandleSysKeyEvent` direction: the Console Server
performed the historical fullscreen action on key-down and returned `FALSE`,
so neither half of that system-key pair entered VDM input. The modern adapter
performs no fullscreen substitute because T410 S1 retired the only project
window receiver.

## Character packet recovery and layout limit

The original `ntcon/server/clipbrd.c::DoStringPaste` is not imported as a
whole: clipboard ownership, Console locking, chunking, CR/LF filtering and
input-buffer insertion remain Console-Server concerns. Its finite
character-to-key mechanics are nevertheless directly applicable when a modern
Console/RDP record has no scan code or usable virtual key. `nt_event` now
uses the original-shaped AltGr record (`ENHANCED_KEY`, left Ctrl and right
Alt), Shift record, character make/break records, and the OEM Alt+numpad
fallback for a character that `VkKeyScanExW` cannot map. Existing scan-bearing
and virtual-key-bearing records still pass through without character
synthesis. Isolated UTF-16 surrogates remain rejected: neither source path
defines a single-key PC representation for one surrogate.

The remaining D30 limitation is explicit: OpenNT's
`SrvGetConsoleKeyboardLayoutName` activates the Console's `hklActive` before
querying the name. Modern public Console has no corresponding active-layout
handle. The selected adapter and packet normalizer therefore use the worker
thread's public layout; it is a bounded fallback, not an ownership-equivalent
replacement. A separate real Console layout-switch observation is required
before this limitation can be closed or replaced.

## Resource-owner recovery

T410 also moves the three immutable embedded-ROM identities and public Win32
resource loading out of original `nt_rez.c` into the existing SoftPC firmware
adapter. The original mirror retains its file read/write ordering; only its
`ROMS_REZ_ID` boundary delegates to the adapter. The adapter accepts exactly
`bios1.rom`, `bios4.rom`, and `v7vga.rom`, copies into the original
caller-owned buffer only when it fits, and returns failure for every other
name or absent resource. In particular a missing embedded ROM does not fall
through to CWD, PATH, or arbitrary firmware files. Writable profile/CMOS
resources retain the original file route. The selected-path retry in
`host_write_resource` remains separately retained because restoring the bare
name would redirect an already-resolved package resource to process CWD.

The repaired `New-T310FirmwareResourceNinja.ps1` no longer emits two empty
source entries (which previously became a nonexistent `mapping_manager.c`)
and now links its generated source indices correctly. Its fresh x86
`verify` fixture passes both the package firmware lookup checks and the new
unknown-embedded-ROM negative check.

The fixture and adapter compiled and linked as x86 from
`build/M0-T388/S7/console-contract-x86`. The noninteractive automation
environment supplies a pipe rather than `CONIN$`; the executable exits `18`
at its deliberate "not a Console input handle" guard. That is an environment
limitation, not a runtime pass. The fixture remains the required real
conhost/Terminal acceptance command for the next interactive run.

`git diff --check` and documentation governance remain required before the S2
packet commit. A formal worker relink is recorded with that commit; no runtime
product artifact is deployed until the real-Console fixture passes.
