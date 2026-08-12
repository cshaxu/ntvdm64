# R1 Original Chain Closure Ledger

Status: measured diagnostic, 2026-08-07.

> **Superseded execution disposition:** this record preserves its source and
> bounded-fixture evidence, but M57 proves that NT4 x86 selects the V86
> `MONITOR` profile rather than CCPU. The x64 research runtime excludes V86;
> CCPU is therefore an oracle, not an executable-profile candidate. Current
> authority: `M77-HISTORICAL-HOST-CLOSURE-EVIDENCE-AUDIT.md` and M75.

This record concerns only the default-disabled OpenNT compiler island. It is
not a runnable engine, does not contain BYOB files, and is not linked by the
normal runner.

## Measured Target

`ntdos64-opennt-original-chain-closure-fixture` retains these original owners
without a local BOP/SVC or register-adapter replacement:

```text
CCPU archive -> original BIOS[] -> original MS_bop_0 -> original DemDispatch
```

The fixture observes the addresses of `BIOS[0x50]`, `MS_bop_0`, and
`DemDispatch` only. It does not enter CCPU execution, BIOS, BOP, or DEM.
The complete historical `BIOS[]` table is intentionally retained; no reduced
selector table is admissible as a substitute.

The diagnostic is built with:

```text
toolchain-probe/build-opennt-support-provider.cmd \
  ntdos64-opennt-original-chain-closure-fixture
```

Its current output is a link failure after all dedicated R1 `sim32` and DEM
objects compile. The output is evidence about old-host closure, not a runtime
failure and not permission to replace individual BIOS/BOP/DEM handlers.

## Confirmed Resolution

`c_effective_addr` is no longer an unresolved symbol in the current link log.
The fixture now links the separately verified
`overlay/base/mvdm/softpc.new/base/ccpu386/c_effective_addr.c`; it only forwards CCPU effective
address translation and has no BOP, SVC, DEM, device, or IRQ behavior.

The 2026-08-07 closure diagnostic also links `ntdll.lib` as a native import
library, exactly as the existing broader historical closure target does. This
reduced the unique unresolved-symbol count from 279 to 246. Those 33 resolved
names are direct NT imports, not recovered VDM behavior and not a modern-host
shim. Remaining native-looking names such as `NtVdmControl`, TEB/RTL-layout
helpers, and the old console surface remain classified as unavailable or
feature-gated host dependencies; they must not be made available merely to
turn the link green.

## Broader Historical-Source Comparison

The existing default-disabled
`ntdos64-opennt-ccpu-cvidc-video-closure-fixture` was rebuilt on 2026-08-07
as a comparison diagnostic. It is not the selected direct-CCPU runtime
profile: it uses the CVIDC-oriented historical CPU closure and reaches more
video/session host code. It therefore cannot prove the R1 runtime path.

It does establish a useful source-graph result. Against the direct-CCPU R1
closure's 246 unique unresolved symbols, the broader graph has 107. It
resolves 194 R1 names by adding original command, DPMI, debugger, OEM,
device, session, and video source owners, while introducing 55 profile- or
product-surface names not present in the narrower path.

The 107 unresolved names form these mutually exclusive triage groups:

| Group | Count | Examples | Admission conclusion |
| --- | ---: | --- | --- |
| Legacy console/session | 27 | `RegisterConsoleVDM`, `NtVdmControl`, console palette/menu/input, `GetNextVDMCommand` | Not part of the first non-invasive CLI. Keep unavailable or replace only at a documented runner boundary. |
| CPU build ABI | 20 | `getEAX`, `setEAX`, `sas_loadw`, `Cpu_define_outb`, `IntelMSW` | Evidence that CVIDC and selected CCPU generated ABI forms cannot be mixed. Do not bridge with guessed layouts. |
| Lifecycle/error | 5 | `host_start_cpu`, `host_terminate`, `DisplayErrorTerm`, timeslice activity | Outer runner/session and separately admitted host callback work; no legacy `nt_reset.c` product shell. |
| Native RTL/debug | 25 | `Rtl*`, `Nt*`, `Dbg*` helpers | Some are normal imports; the remaining private or deprecated calls need exact capability replacement or feature exclusion. |
| Other historical subsystem seams | 30 | keyboard translation tables, EMS backfill, VGA palette helpers, PIF globals | Retain behind their original feature owner until a live profile reaches them. |

This comparison changes the execution order: source recovery should first use
the existing staged original module owners, but the primary remaining work is
not DOS SVC reconstruction. It is a contained runner/session surface and a
single compatible CCPU ABI profile. The CVIDC target must remain a diagnostic,
not a shortcut around the direct-CCPU route.

## Direct-CCPU Full-Source Diagnostic

On 2026-08-07,
`ntdos64-opennt-direct-ccpu-full-source-closure-fixture` reached the link
stage with the selected direct CCPU archive and broad original host/DOS source
owners. It deliberately excludes the frozen local register adapter and every
local BOP/DEM/BIOS implementation. It has 156 unique unresolved symbols:
90 fewer than the narrow direct-CCPU R1 closure, but 49 more than the CVIDC
comparison target.

That difference is expected and useful. The direct profile still exposes its
own SAS/IRQ callback names (`host_sas_*`, `host_set_hw_int`,
`host_swint_hook`, `c_sas_touch`, `c_VirtualiseInstruction`) and does not have
the CVIDC generated-register/provider arrangement. The remaining 156 also
include old console/session integration, native/RTL helper calls, lifecycle
owners, and full video/EMS feature surfaces. Therefore:

1. direct CCPU remains the only executable-profile candidate;
2. the full-source target is the authoritative source-recovery baseline for
   that candidate;
3. CVIDC remains comparison evidence only;
4. the next admitted implementation work is a narrowly documented direct
   CCPU host foundation callback, chosen only after the first live original
   startup path reaches it.

### Historical Shell/Error Owner Pass

The original `nt_reset.c` translation unit compiles in the historical island
and was included only in the direct-full diagnostic. Its data/functions reduce
that target from 156 to 148 unique unresolved symbols, confirming it owns the
expected historical session/lifecycle names. This does not change its status:
it is the old `-f`/PIF/console/WOW product shell and remains prohibited from
the direct CLI and all normal runtime paths.

The separate original `nt_error.c` owner was also tested as a default-disabled
object target but cannot compile under the fixed modern toolchain without
recreating an obsolete host ABI. It conflicts with the current CRT's `_errno`
declaration and accesses fields of the old concrete TEB layout. No overlay,
TEB layout recreation, or error-dialog substitute was introduced. The
remaining `DisplayErrorTerm`/`host_error` link requirements are therefore an
explicit future runner-diagnostic seam, not a reason to revive the historical
UI shell.

## R1.2 Original Selector-50 Contract

The selected NT4 source establishes the first required dispatcher edge without
inference:

```text
BIOS[0x50] -> MS_bop_0
MS_bop_0:
  DemCmd = *Sim32GetVDMPointer(CS:IP, 1, FALSE)
  DemDispatch(DemCmd)
  IP = IP + 1
  IDLE_disk() except for commands 0x14 and 0x15
```

`bios.c` owns the `BIOS[0x50] = MS_bop_0` table entry. `nt_bop.c` owns the
service-byte read, the call to original `DemDispatch`, the post-dispatch IP
advance, and the historical idle exception. A future runnable fixture must
preserve this order exactly: it may not pre-consume the byte, advance IP before
DEM returns, or add a second BOP/SVC switch. The present closure target proves
symbol retention only; it does not yet run this contract because the complete
original BIOS/host closure remains unresolved.

## Unresolved Families

The current diagnostic reports 562 undefined symbols. That count is not a
work queue: retaining the complete BIOS table intentionally makes inactive
product features visible. The following classification controls admission.

| Family | Examples from current log | Meaning | R1/R2 disposition |
| --- | --- | --- | --- |
| CCPU timing/IRQ host boundary | `ActivityCheckAfterTimeSlice`, `host_clear_hw_int`, native critical sections, performance counter APIs | Historical `nt_eoi.c`, `nt_timer.c`, and `nt_unix.c` assume old session/thread facilities. | SH-09 only after a live original CCPU trace proves the path. No synthetic IRQ/timer policy now. |
| Historical process/UI shell | `VDMForWOW`, `DisplayErrorTerm`, `TerminateVDM`, PIF globals | The old process shell and console product surface are being pulled by historical host units. | Do not import `nt_reset.c` as a direct CLI shell. A later runner lifecycle may provide only its documented stop/cancellation boundary (SH-08). |
| Console, mouse, event, VDD | `VDMConsoleOperation`, `GetConsoleInputWaitHandle`, `Mouse*`, `VDD*` | Full historical interactive product integration. | Excluded from the first contained CLI cut. No console/menu/mouse shim is admitted merely to close this diagnostic. |
| Storage, COM/LPT, printer | `nt_fdisk_*`, `nt_floppy_*`, `host_com_*`, `host_lpt_*` | Whole BIOS table retains controllers not yet proven by NTIO startup. | Use existing SH-12/SH-14 contracts only when an original live path reaches them. No physical-device fallback. |
| Video, EMS, protected-mode hooks | `setVideo*`, `paint_screen`, `LIM_*`, `c_sas_touch`, `c_VirtualiseInstruction` | Generated/video or unadmitted machine paths outside the first text-mode trace. | Remain fail-closed and separately sourced; do not attach a generic video or monitor substitute. |
| Optional BOP product services | `DpmiDispatch`, `DBGDispatch`, `ms_bop`; later `CmdDispatch` | `nt_bop.c` owns several selector families, all retained by the original translation unit. | Preserve the original dispatcher. Admit the original command source only at the command-start gate; DPMI/debug/VDD remain feature-gated. |
| DEM native/OEM filesystem | `CreateFileOem`, `NtQueryDirectoryFile`, RTL path helpers | DEM has distinct historical file, directory, and raw-media boundaries. | Governed by SH-04/05/06/11/12; no path or DOS-error policy moves into a shim. |

## Rejected Direct Source Candidate

`host/src/nt_cpu.c` is the original source owner of `host_clear_hw_int`; when
`CCPU` is selected, that function is intentionally empty. However, compiling
the whole translation unit currently fails before linkage because it refers to
missing generated VIDC constraint identifiers such as `ConstraintRCL_LS8` and
`ConstraintRCX_LS16`. This is a generated-input blocker, not evidence that
the empty CCPU clear action should acquire a new semantic.

The source is therefore retained as a provenance reference and the closure
fixture does not include it. Any later callback replacement must be admitted
under SH-09, demonstrate equivalence to this CCPU branch, and keep original
ICA/quick-event ownership intact.

## R1.3 Original Register Bridge Evidence

On 2026-08-07, the default-disabled
`ntdos64-opennt-original-register-bridge-fixture` built and exited with status
zero. It links the historical `host/src/nt_aorc.c` object, not the frozen
local `ccpu_register_abi_bridge.c` adapter, and validates initialized CCPU
state through original `getAX`/`getDX`/`getDS`/`getES` plus `setAX`/`setDX`/
`setZF`/`setCF` forwarding.

The fixture initializes and terminates CCPU only. It executes no guest
instruction, BIOS selector, BOP, DEM service, DOS program, or device path.
Its effective-address protected-mode branch is fail-closed fixture support;
the exercised state is real mode. This proves the historical forwarding layer
can be built and called with the selected CCPU archive.

The second R1.3 fixture,
`ntdos64-opennt-original-demlock-register-fixture`, built and exited with
status zero on 2026-08-07. It initializes CCPU, creates one temporary owned
file, passes its handle as original `demLockOper` expects in BX:BP, then
invokes original lock (AL=0) and unlock (AL=1) paths. Both paths returned with
CF clear through original `nt_aorc.c` accessors. The fixture deletes the file
before exit. Its local `demClientError` is a trace-only abort guard: the
successful original paths never call it, so it cannot synthesize a DEM error
result. This is the first runnable original DEM caller proof; it does not
enter a guest instruction, BIOS selector, BOP, SVC dispatcher, DOS program,
or device provider.

`ntdos64-opennt-original-demerror-register-fixture` is the first candidate
for that missing proof. Its intended call is original `demClientErrorEx` on
the access-denied path, which would set AX and CF without entering a SVC or
device path. Its 2026-08-07 link diagnostic remains unresolved: the historical
`demerror.c` translation unit retains unrelated `demRetry`, volume-query, and
symbolic-link helper imports (`CurrentISVC`, `apfnSVC`, and native RTL APIs).
Requesting function-level sections did not remove those imports under this
old COFF build. No local DEM function, SVC table, DOS global, or native API
stub has been introduced to force the test through.

## Next Evidence Order

1. Use the original DEM caller proof to reduce the unresolved families above
   to the first live dispatcher/BIOS dependency.
2. Admit only the narrow corresponding R2 shim after a source owner and
   bounded success/failure fixture are recorded.

This ledger does not relax the requirement that a later runnable chain use
the complete original `BIOS[]` table. It prevents inactive full-product
dependencies from being mistaken for first-boot requirements.

## R1.2 Selector-50 Split Evidence

On 2026-08-07, the default-disabled
`ntdos64-opennt-original-demdispatch-date-fixture` built and exited with
status zero. It initializes CCPU/SAS, calls original `DemDispatch(0x14)`, and
checks the documented `demQueryDate` register result through original
`nt_aorc.c` forwarding. It executes no guest instruction, BIOS selector,
BOP, DOS image, file, or device path. This proves that the original dispatcher
table and the selected side-effect-free date service are not the blocker for
the first selector-50 trace.

The initial `0xC0000005` in the companion default-disabled
`ntdos64-opennt-original-msbop0-date-fixture`, and the initial non-returning
guest-stream trace, were traced to an omitted historical `SIM32` compilation
role. Without it, `sim32.c` conditionally emits no implementation and
`/FORCE:UNRESOLVED` masks the absent `Sim32pGetVDMPointer` entry as an invalid
call. This was a build-profile defect, not a missing DEM, BOP, or machine
semantic.

After enabling the original `SIM32` role, the source's unused host-SAS
allocation definitions were link-name isolated only so the existing
trace-only CCPU SAS aperture remained the sole allocation owner. The original
`Sim32pGetVDMPointer` implementation itself is unchanged. The default-disabled
`ntdos64-opennt-original-sim32-pointer-fixture` now exits with status zero:
both original `c_GetLinAdd(3)` and original `Sim32pGetVDMPointer(3, FALSE)`
return the initialized SAS byte at address three.

The original `ntdos64-opennt-original-msbop0-date-fixture` then exits with
status zero, proving `MS_bop_0` reads the service byte via original Sim32,
calls original `DemDispatch`, and advances IP from three to four. Finally,
the default-disabled `ntdos64-opennt-original-selector50-trace-fixture`
exits with status zero for guest bytes `C4 C4 50 14 C4 C4 FE`. It traverses
original CCPU, the complete original `BIOS[]` table at selector `50h`,
original `MS_bop_0`, original `DemDispatch`, and original `demQueryDate`,
then returns through CCPU's BOP `FE` path. The fixture's `14h` date service
does not call `IDLE_disk`; no local BOP, DEM, SVC, DOS, or device behavior is
linked into the executed route.

The selector-50 and companion idle fixtures were rerun unchanged after the
modern-runner isolation build on 2026-08-07; both again exited with status
zero. They are therefore reproducible bounded-oracle evidence for a future
self-designed monitor's BOP/IP/DEM conformance tests, not a bootstrap or DOS
runtime substitute.

This is bounded connectivity evidence, not historical runtime link closure:
the fixture still uses `/FORCE:UNRESOLVED` only for inactive entries retained
by the complete BIOS table. The next admission question is the first live
event/IRQ dependency after this trace, not timer, console, filesystem, or DOS
bootstrap policy in the abstract.

## R1.4 First Live Idle/Event Evidence

On 2026-08-07, the default-disabled
`ntdos64-opennt-original-selector50-idle-trace-fixture` built and exited with
status zero for guest bytes `C4 C4 50 FF C4 C4 FE`. The out-of-range service
causes original `DemDispatch` to set CF; original `MS_bop_0` then takes its
non-date `IDLE_disk` branch. Under the selected NTVDM source profile,
`IDLE_disk()` is original `HostIdleNoActivity()`: it clears the historical
activity flag and pulses an existing idle event only if the host is already
waiting. The bounded fixture has no wait state, and its fail-closed CCPU
callback count remains zero.

Therefore the first live non-date host behavior does not require timer, PIC,
IRQ injection, console, or a blocking wait shim. Those source families remain
unadmitted until a later original trace actually reaches their callers.

## Sim32 Source-Closure Update

The R1.2 mapping recovery also corrected the link diagnostic. With `SIM32`
absent, the historical `sim32.c` translation unit emitted no mapper and the
old closure listed `_Sim32pGetVDMPointer` as unresolved. With its required
historical compilation role restored, that symbol is resolved by original
source and the current original-chain closure contains 248 unique unresolved
symbols. The only newly exposed names are `InitIntelMemory` and
`FreeIntelMemory`, reached from `sim32.c`'s separate host-SAS allocation
bodies; the trace profile deliberately keeps those bodies uncalled because its
single SAS aperture is owned by the fail-closed instrumentation foundation.

This is not an admission for a memory allocator shim. Function-level section
selection was tested and did not alter this old COFF closure, so it was not
retained. The original Sim32 pointer mapper remains selected and independently
executes successfully; the two allocation imports remain inactive diagnostic
evidence only.
