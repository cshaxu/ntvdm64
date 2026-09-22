# M0 T420 S43 — Debugger capability ledger

## Question

Can the selected original `mvdm/dbg` package deliver its debugger events and
return the debugger's resulting state through the standalone CCPU40 worker,
without guest modification, CPU30, private CSR/CSRSS transport, or a
project-invented debugger protocol?

## Inputs and method

- Selected mirror package: `src/mvdm/dbg/sources`, whose sole source is
  `dbg.c`.  Its SHA-256 is
  `5A3A207EFF1570908E270C737D12FB40FE66097895063136D17FE6B971494B90`, equal
  to the pinned OpenNT source.
- Current formal worker selection:
  `tools/build/New-T310OriginalSoftpcNinja.ps1` selects the byte-identical
  `mvdm/dbg/dbg.c` translation unit with `_NTDBG_`, plus only the finite
  `ntvdm-exe/debugger/{dbg_registers,dbg_prompt}.c` worker-local bindings.
  The earlier `dbg_init`, `dbg_state`, `dbg_dispatch`, and `dbg_unavailable`
  replacements are removed and are historical audit inputs only.
- Direct original callers and exports are enumerated with source search.  The
  first controlled test is a native debugger/child pair that verifies the
  original `RaiseException(STATUS_VDM_EVENT, 0, 4, params)` transport before
  moving a production event path.

## Complete selected source inventory

| Original entry family | Original behaviour | Current production disposition | S43 acceptance needed |
| --- | --- | --- | --- |
| `DBGInit` | Queries `ProcessDebugPort`; always returns `TRUE`. | Selected original body. | Attached and unattached worker result. |
| `SendVDMEvent` and `GetNormalContext` | Builds a four-DWORD event record then raises `STATUS_VDM_EVENT`; event parameters point to context, internal information and optional payload inside the worker. | Selected original body. | Verify standard Win32 debugger receives, reads and continues the original four-word form. |
| Segment/module notices | `SegmentLoad`, `SegmentMove`, `SegmentFree`, `ModuleLoad`, `ModuleSegmentMove`, `ModuleFree`. | Selected original body. | At least real module notification and OEM-DBG-PATH, malformed path and cleanup. |
| Fault/return forms | `SingleStep`, `Breakpoint`, `GPFault`, `DivOverflow`, `DllStart`, `TaskStop`. | Selected original body. | Controlled BOP break/fault, debugger continuation, result/register restoration and worker cleanup. |
| Dispatcher | Decodes all `dbgsvc.h` modes from VDM stack. | Selected original body; raw aliases remain only at its bounded CCPU40 call boundary. | All selected modes classified as reached pass, source-proven no caller, or explicit unavailable boundary. |
| Task/WOW exports | `DBGNotifyNewTask`, `DBGNotifyRemoteThreadAddress`, `DBGNotifyDebugged`. | Selected original body. | Non-WOW owner paths here; WOW callers remain assigned to the successor and cannot be credited by S43. |
| DPMI debug registers | `DpmiSetDebugRegisters` calls `ThreadSetDebugContext`. | CCPU40 `MOV_DR` binding and checked six-DWORD copied lease. | Existing DPMI/CCPU positive, invalid-address and teardown tests rerun under S43. |

## Reached consumer ledger

| Consumer | Original action | Scope / disposition |
| --- | --- | --- |
| `softpc.new/host/src/nt_msscs.c::InitialiseDosEmulation` | Calls `DBGInit`. | Selected worker startup; mandatory attached/unattached evidence. |
| `softpc.new/host/src/nt_bop.c::MS_bop_6` | Calls `DBGDispatch` for BOP 6. | Selected worker; mandatory controlled stack/BOP evidence. |
| `dos/dem/demmisc.c::SignalSegmentNotice` | Calls `ModuleLoad`, `ModuleFree` or `ModuleSegmentMove` after `IsDebuggee` and `GetFullPathNameOem`. | Selected DEM consumer; OEM-DBG-PATH is mandatory. |
| `dos/dem/demmisc.c::demDebug`, `demDebugPrompt`, `demIsDebug` | Queries debug port and calls `DbgPrompt` only while debugged. | Selected DEM consumer; must not silently report an unavailable debugger as success. |
| `dpmi32/i386/dpmi386.c::DpmiSetDebugRegisters` | Copies six `DS:SI` DWORDs and applies DR0--DR3/DR6/DR7. | Selected DPMI consumer; existing CCPU40 lease route is in scope. |
| `wow32/wow32.c`, `wow32/wkman.c` | Supplies attached/task/remote-thread notices. | Direct original callers, but runtime WOW recovery and real Win16 evidence remain owned by the queue-head WOW32 successor. S43 does not claim their pass. |

## Boundary finding to test

The original event transport is not a CSR RPC: `dbg.c::SendVDMEvent` directly
uses a continuable `RaiseException` with `STATUS_VDM_EVENT` and four
application-defined arguments.  Modern Windows debugging APIs provide an
`EXCEPTION_DEBUG_EVENT` for a debuggee exception and a debugger resumes it by
`ContinueDebugEvent`.  This makes a source-shaped finite transport plausible.
It does **not** yet prove that a modern debugger can consume the original
payload structures or safely update their pointed-to context.  The first
native transport fixture is therefore evidence only, not a package pass.

### Confirmed transport result

`build/M0-T420/S43/debug-event-r1/debugger-event-transport-test.exe` passed
on the selected MSVC x86 row.  Its un-debugged child entered the original
SEH-fallback direction.  Its `DEBUG_ONLY_THIS_PROCESS` parent received one
`EXCEPTION_DEBUG_EVENT` with `STATUS_VDM_EVENT`, verified all four original
argument slots, read the two child-local pointer payloads with
`ReadProcessMemory`, updated the event payload with `WriteProcessMemory`,
continued the event with `DBG_CONTINUE`, and observed the child resume and
exit `73`.

The separately mirrored original `mvdm/vdmdbg/vdmdbg.txt` independently
describes this exact protocol: `STATUS_VDM_EVENT`, four DWORD arguments,
`WaitForDebugEvent`, `ContinueDebugEvent`, and debugger-side use of
`ReadProcessMemory`/`WriteProcessMemory`. Therefore the former blanket claim
that the event path requires a private CSR transport is disproved.

The S43 x86 rerun rebuilt and passed both standalone native fixtures in
`build/M0-T420/S43/worker-oem-r1`: it emitted
`S43_DEBUGGER_EVENT_TRANSPORT_OK` after the handled and SEH-fallback cases,
and `S43_DEBUGGER_MODULE_EVENT_TRANSPORT_OK` after public
`SEGMENT_NOTE` module-load/move/free payload checks. These are transport and
public-layout evidence only; neither fixture substitutes for the required
real DEM OEM-path consumer.

An unmodified `dbg.c` compile probe stops in historical `ntdbg.h`'s unused
LPC/Dbg subsystem declarations before its body compiles; the source itself
calls no such declaration for its event path. This is a header-composition
boundary, not evidence against its worker algorithm. S43 must keep the mirror
byte-exact and recover only the reached event body in the named worker binding,
unless a minimal source-shaped original-header subset composes later.

### Whole-body composition result

The complete byte-identical `mvdm/dbg/dbg.c` compiles with a translation-unit
`_NTDBG_` definition. That definition skips the included historical `ntdbg.h`;
the reached body does not consume any declaration from that header, while the
otherwise included historical header recursively declares unavailable LPC/Dbg
subsystem structures. The selected source now uses the exact original public
`vdmdbg.h` under `opennt-host/public/sdk/inc` (SHA-256
`A3048CBCAA26E32AE542A73AAE59356E9FA2EB85AF00932A027FF4E8E47F225C`).

The formal x86 product link succeeds with original `DBGInit`, `DBGDispatch`,
`SendVDMEvent`, all segment/module and fault/return families, and the original
WOW notification exports selected from `mvdm/dbg/dbg.c`. The only remaining
non-original bodies in the selected debugger archive are: (1) the existing
CCPU40 bounded six-DWORD debug-register copy plus original DR0--DR3/DR6/DR7
write order, because the original owner was a kernel-VDM service; and (2) a
thin dynamic call to the still-exported NTDLL `DbgPrompt`. Earlier adapter
implementations of init, state, event, dispatch and module notices are no
longer selected and must be retired before closure.

The corrected whole-process `DEBUG_PROCESS` observer terminates at its
absolute 30-second deadline, but did not receive `DBG_MODLOAD` for
`run16 MEM.EXE`: its event counts were `creates=1, vdm-events=0`. The source
shows that `run16` creates the worker, yet this observer saw only `run16`.
Thus launch-time debug inheritance is not established for this worker-startup
configuration and is not a valid OEM-DBG-PATH witness. This is not a negative
result against `dbg.c`; the next test attaches directly to the already-created
worker, then submits `MEM` through that worker's Console. No real-worker
debugger event is claimed until that test passes.

### Real-worker module result

The corrected observer starts an ordinary `run16 COMMAND.COM` worker under
`DEBUG_PROCESS`, so the worker has its DebugPort before original `DBGInit`.
It supplies an ASCII temporary `MEM.EXE` command through standard input. The
worker's original COMMAND BOP 54:08 path creates the nested `run16.exe`, and
the debugger receives and continues the original `STATUS_VDM_EVENT` whose
event kind is `DBG_MODLOAD`. It verifies
`SEGMENT_NOTE.Module == "MEM"` and the full temporary path, then emits
`S43_DEBUGGER_REAL_WORKER_OEM_MODULE_OK` (the marker name predates the later
OEM-specific check).

This is production-path evidence for the original
`demmisc.c::SignalSegmentNotice → dbg.c::ModuleLoad → SendVDMEvent` path, but
it is **not yet OEM-DBG-PATH acceptance**. An early UTF-8 standard-input probe
whose temporary directory contains `é` reached COMMAND and created nested
`run16.exe`, but did not yield a `MEM` event. That observation is not a
reclassification finding: the later direct-launch A/B stops at the same
post-handoff observer boundary for an ASCII directory. The test creates only
a temporary `MEM.EXE` copy and cleans its worker/broker processes; it never
alters guest media.

### Reproducibility correction

The first direct-attach witness established the ordinary `MEM` notification,
but its Console-input timing is not yet stable enough to serve as the required
non-ASCII acceptance fixture.  A follow-up observer attaches before a direct
`run16 <non-ASCII-path>\\MEM.EXE` worker executes.  It reproducibly receives
13 original VDM events, including `ntio`, `ntdos`, `HIMEM`, `COMMAND`,
`MSCDEXNT`, `REDIR` and `DOSX`, but not the later `MEM` load: suspending the
worker before the parent completes command-record handoff changes the product
startup progression.  This is a test-placement finding, not a debugger or
OEM conversion failure.  S43 remains open until it observes the original
`MEM` event through a stable post-handoff attach route and validates the
non-ASCII path bytes.

An identical temporary-directory probe whose name used only ASCII produced
the same seven-event boundary (`ntio`, `ntdos`, `HIMEM`, `COMMAND`) and no
`MEM` event. Therefore the direct-launch observer's current failure is not
an OEM/non-ASCII conversion result. It establishes a command-record/$exec
handoff investigation before the OEM-DBG-PATH assertion may be retried.

### Debug-host exception disposition experiment

The current DebugPort observer receives normal `STATUS_BREAKPOINT`
(`0x80000003`) and segment notification (`0x40000005`) events before the
worker emits its original module notices.  After the `COMMAND` notice, a
worker thread can raise the MSVC C++ exception `0xE06D7363`.  The observer
correctly returns `DBG_EXCEPTION_NOT_HANDLED` for it: forcing
`DBG_CONTINUE` in a disposable observer A/B makes that worker terminate with
`0xC0000005`.  This establishes that a debugger must not suppress that
exception merely to make the test advance.

A second disposable A/B copied the whole package under a short non-ASCII
root.  Its worker obtained the initial command reply but cleanly ran
`ExitVDM` before any module notice.  A non-ASCII package root is therefore
not an interchangeable substitute for a non-ASCII DOS application path.
Neither experiment is OEM-DBG-PATH acceptance or a product repair.

The then-next observer revision resolved the recurring `0xE06D7363` exception
address with the target process's module map: it is
`KERNELBASE.dll!RaiseException`, not an ntvdm/mvdm image address.  Its stack
has the standard MSVC C++ exception shape (`0x19930520`); resolving its
ThrowInfo pointers identifies `CoreMessaging.dll` and `CoreUIComponents.dll`
as the concrete owners.  `STATUS_VDM_EVENT` is itself the original public
alias of `STATUS_SEGMENT_NOTIFICATION` (`0x40000005`), so the observer has
always acknowledged those VDM events; it continues to return
`DBG_EXCEPTION_NOT_HANDLED` for that C++ exception. The latter still repeats while a worker is debug-launched
with a Console, and direct OEM-path `MEM` as well as interactive COMMAND stop
before a `MEM` module notice.  Thus this is a reproducible **debug-host/
Console observation boundary**, not evidence of an OpenNT debugger, OEM
conversion, CCPU, or guest failure.  It cannot be converted into a pass by
claiming the C++ exception; doing so was already shown to cause `0xC0000005`.
The direct-start placement remained unsuitable, but the later ordinary
`DEBUG_PROCESS` observer supplies the required OEM witness without altering
the worker's Console startup.

### First-command trace finding

A clean formal-package `run16 MEM.EXE` run under the pre-existing S34 trace
reached the worker's original `GetNextVDMCommand` three times. The first
request was an initialization/PIF shape, the second returned success with an
empty `CmdLine` (`get-command` followed by no bytes), and the third entered
the normal wait state. `run16` itself returned zero.

This alone does **not** establish that `MEM.EXE` was lost. In the original
`BaseCheckVDM` contract, `AppName` and `CmdLine` are separate fields:
`AppName` carries the executable, while `CmdLine` carries only its argument
tail. An argument-free `MEM.EXE` legitimately has an empty `CmdLine`; the
trace records only that latter field. The next production witness must inspect
the returned `AppName` or observe the actual
`NTDOS.SYS,$exec → demLoadDosAppSym` transition before assigning a fault to
run16, BaseCheckVDM or the broker payload.

### OEM application-path boundary finding

The authorized disposable test copied `O:\\winnt\\MEM.EXE` to the DOS-shaped
`O:\\winnt\\tests\\D43Eé\\MEM.EXE` and launched that exact path through the
deployed `run16`.  `run16` returned zero, but the guest printed `Cannot
execute O:\\WINNT\\TESTS\\D43E<OEM-byte>\\MEM.EXE`; no `MEM` module loaded.
In the same session, `run16 COMMAND.COM /c "DIR O:\\winnt\\tests\\D43Eé"`
printed `Directory of O:\\WINNT\\TESTS` followed by `File not found`.  The
test ended its exact worker/broker processes and removed that disposable
directory.  A previous `SUBST` probe remains non-acceptance evidence because
the selected product does not support a cross-process `SUBST` drive as a DOS
medium.

This result proves only that the current **application-record to guest EXEC**
route cannot resolve the selected CP437 character.  It does not prove a
failure in `dbg.c`, `SendVDMEvent`, or the debugger observer: `MEM` never
loads, so no `DBG_MODLOAD` can be emitted.  The next source audit must follow
the original byte-preserving route
`BaseCheckVDM (Unicode→OEM) → BaseSrv DOS record → cmdGetNextCmd → COMMAND
$exec → DEM/OEM file thunk`, recording the first point at which the CP437
byte ceases to denote U+00E9.  No guest patch or lossy ANSI fallback is
admitted.

That audit is now complete for this test character.  The S34 trace proves
that the worker receives `0x82` in the command tail, `CHCP` reports `437`,
and `DIR O:\\winnt\\tests` enumerates the directory.  A `£` (CP437) sibling
directory lists and runs `MEM.EXE` successfully, excluding a general OEM or
DEM conversion loss.  Most decisively, after creating a separate ASCII
`D43EE` directory, an explicit guest request for `D43Eé` resolved and ran
`D43EE\\MEM.EXE`.  The selected original DOS/COMMAND filename normalization
therefore folds this `é` input to `E`; it is a guest filename-rule limitation,
not a host debugger, BaseSrv, or OpenNT mirror defect.  The immutable guest
must not be patched for it.  S43 uses the proven CP437 `£` path for its actual
non-ASCII module-event witness.

### Real OEM `DBG_MODLOAD` witness

The freshly built and deployed S43 package was debug-launched through the
standard Windows DebugPort.  Its observer accepted the original
`STATUS_VDM_EVENT` transport and received module notes in this order:
`ntio`, `ntdos`, `HIMEM`, `COMMAND`, `MSCDEXNT`, `REDIR`, `DOSX`, then
`MEM`.  The last note's path was
`O:\\WINNT\\TESTS\\D<process-id>£\\MEM.EXE`; the observer checked the OEM
byte for `£`, printed `S43_DEBUGGER_REAL_WORKER_OEM_MODULE_OK`, and exited
zero.  All test-created processes and the disposable directory had ended by
the post-run check.  This is the required end-to-end non-ASCII `DBG_MODLOAD`
evidence, without a guest change or an ANSI fallback.

### Module-note ABI correction

All three pinned public `vdmdbg.h` copies define `MAX_MODULE_NAME` as `8 + 1`
and declare `SEGMENT_NOTE.Module[MAX_MODULE_NAME + 1]`: the debugger-visible
field is therefore exactly ten bytes.  Both selected copies of `dbg.c` instead
use their private `MAX_MODULE 64` copy bound for that field.  This is an
upstream source/header inconsistency, not an ABI extension available to the
standalone worker.  The recovered non-mirror binding uses the public ten-byte
layout and truncates longer external names at that boundary.  DEM's ordinary
8.3 module token remains unchanged; the controlled module-event fixture uses
an overlong token to prove that the public field is not overwritten into
`FileName`.

## Current exclusions

- No guest binary or original media is changed.
- CPU30, kernel VDM monitor and private CSR/CSRSS transport remain prohibited.
- The SoftPC `/DPROD` trace/Yoda package was closed separately in S31; it is
  not this `mvdm/dbg` package and is not reopened here.
- WOW task/remote-thread runtime acceptance is transferred to the WOW32
  successor, not excluded or silently treated as passed.

## S43 formal build and DOS regression

Fresh root `build/M0-T420/S43/formal-oem-r2` generated 491 x86 graph steps
and produced `run16.exe`, `basesrv.exe`, and `ntvdm.exe`.  The deployed
`O:\\winnt` hashes match that root exactly: `run16` is
`CE694D83B208139EF64C9F1245C026C275C1873B95B8AF4B80761BFFB0ED0709`,
`basesrv` is
`DD005C1763D85C4ACF292840B10A300089E1A1B12AE3248380468DDD58765617`, and
`ntvdm` is
`FEEE9268611AC1E3281EB8181D21B46ECC977356A6FAB7009FF503140B9BE23D`.

On that package, the rebuilt OEM-uppercase fixture passed, as did the checked
DPMI debug-register, native event-transport, and module-event fixtures.  The
existing 17-route Console regression with log prefix
`s43-debugger-regression-r1` passed every route: direct/interactive/nested
`COMMAND`, repeated and direct `MEM`, native streams/EOF, direct `/c`, guest
and native exit-status routes, and `EDIT` return.  Its summary is
`O:\\winnt\\logs\\s43-debugger-regression-r1-summary.json`; its exact test
processes all exited.

## Remaining S43 checklist

All selected non-WOW debugger obligations are now evidenced.  The retained
WOW producer families are transferred to the named WOW32 successor, rather
than being represented as a passing S43 result.

## BOP 6 route audit

`MS_bop_6` is selected from the original `softpc.new/host/src/nt_bop.c` and
does exactly one thing: call the original `dbg.c::DBGDispatch`.  The switch is
not the delivery path for ordinary DOS module notices.  Original DEM
`SignalSegmentNotice` calls `ModuleLoad`, `ModuleFree`, or
`ModuleSegmentMove` directly; the real CP437 `£` `MEM.EXE` witness exercised
the paired load/free branch and now prints
`S43_DEBUGGER_REAL_WORKER_OEM_MODULE_PAIR_OK`.

| `dbgsvc.h` mode family | Current original producer | S43 disposition |
| --- | --- | --- |
| `DBG_SEGLOAD` | three original DOSX `dpmi/dxboot.asm` sites | selected non-WOW BOP consumer; controlled witness remains required |
| `DBG_SEGMOVE`, `DBG_SEGFREE`, `DBG_MODFREE`, `DBG_SINGLESTEP`, `DBG_BREAK`, `DBG_GPFAULT`, `DBG_DIVOVERFLOW`, `DBG_DLLSTART`, `DBG_TASKSTOP`, `DBG_TOOLHELP` | original WOW16 `kernel31` code | WOW32-successor owned; not claimed by S43 |
| `DBG_MODLOAD` | no BOP producer; DEM emits it directly | original `DBGDispatch` explicitly performs a no-op; direct DEM production pass recorded above |
| `DBG_INSTRFAULT`, `DBG_TASKSTART`, `DBG_DLLSTOP` | no selected BOP producer | original switch has no case and falls through to its original `AX=0` unhandled result; source-proven no-current-caller, not a standalone omission |
| `DBG_ATTACH` | no selected producer | original explicit no-op; source-proven no-current-caller |

This audit is based on the selected source tree, excluding generated listings:
the only non-WOW `BOP BOP_DEBUGGER` calls are `dxboot.asm`'s three
`DBG_SEGLOAD` frames; all remaining assembly producers reside in
`wow16/kernel31`.  It eliminates a false requirement to manufacture guest
break/fault events or to add a replacement dispatcher in the standalone host.

### Controlled BOP 6 and teardown result

The rebuilt `debugger-worker-module-observer.exe` ran against the deployed
formal package and printed, in one worker session, three original
`kind=0` (`DBG_SEGLOAD`) notes for `DOSX.EXE`, followed by `kind=3`
(`DBG_MODLOAD`) and `kind=4` (`DBG_MODFREE`) for the CP437 `£` `MEM.EXE`
copy.  It ended with
`S43_DEBUGGER_REAL_WORKER_OEM_MODULE_PAIR_AND_SEGLOAD_OK` and exit zero.
The temporary copy was removed, and no `run16`, `basesrv`, or `ntvdm` process
remained after the debug host ended.  This is the controlled production BOP 6
witness, module-pair cleanup evidence, and debugger-session teardown result.

For failure direction, the native original-event fixture separately proves
both paths without suppressing an unrelated exception: a debugger that calls
`ContinueDebugEvent(DBG_CONTINUE)` resumes the child and preserves its payload
mutation; with no debugger the same continuable exception takes the original
SEH fallback.  The real worker observer returns
`DBG_EXCEPTION_NOT_HANDLED` for MSVC's unrelated `0xE06D7363` event; the
recorded disposable A/B shows that falsely claiming it causes `0xC0000005`.
Thus there is no synthetic success or exception swallowing in the production
debugger boundary.
