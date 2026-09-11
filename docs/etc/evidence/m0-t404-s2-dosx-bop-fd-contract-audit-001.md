# M0 T404 S2 DOSX BOP FD contract audit

## Question

The r036 x86 observation reached `D20B:0006 -> 0000:0000` after `DOSX.EXE`
opened, but did not reach `KRNL386`, WOW32, or `WRITE.EXE`.  This audit maps
that return before treating it as a DOSX or generic CPU semantic defect.

## Original contract

`dpmi/486/dxutil.asm` implements `SwitchToRealMode` for `WOW_x86` by pushing
the exact five-word continuation frame, in order: real-mode `DS`, resumed
`SP`, real-mode `SS`, resumed `IP`, and real-mode `CS`.  It then executes
`FBOP BOP_SWITCHTOREALMODE`; `bop.h` defines that BOP as `FDh`.

The original i386 receiver is
`dpmi32/i386/dpmi386.c:switch_to_real_mode`.  It reads those same five words
from the current stack and changes DS/SP/SS/IP/CS while leaving protected
mode.  The resumed real-mode code removes the three frame arguments and
returns to its original caller.  Thus a zero return at the DOSX real-mode
continuation is a contract failure of the FD transfer/frame path, not proof
that DOSX created a zero return intentionally.

`softpc.new/base/bios/bios.c` owns the BOP dispatch table.  Its source selects
`switch_to_real_mode` for FD only under `NTVDM && MONITOR`; otherwise the
identical FD slot is `illegal_bop`.

## Current formal graph comparison

The r033 generated `build.ninja` creates `obj/bios/bios.obj` with the generic
`cc` rule.  Its common flags contain `NTVDM` but not `MONITOR`, and there is
no per-`bios.c` x86 flag.  Therefore r033 selected the original `illegal_bop`
FD table slot.  The generated graph also does not compile the original
i386-only `dpmi386.c`; its CPU40 DPMI library has no replacement
`switch_to_real_mode` provider.

This exactly conflicts with the retained MVDM-HOST-DIV-229 inventory, which
records the required CPU40 counterpart and x86 `bios.c` `MONITOR` selection.
That inventory is not used as the source of the contract: the assembly, BOP
definition, original i386 receiver, and actual generated r033 graph establish
the contract independently.

## Disposition and confidence

The narrow repair is source-shaped:

1. Compile only original `bios.c` for the selected x86 product with
   `MONITOR`, selecting its existing FD table entry.
2. Add only the CPU40 counterpart of the original `switch_to_real_mode`
   receiver in `dpmi32/modesw.c`: copy the five words before changing mode,
   clear PE, then load those values through CCPU's real-mode cache path.

No DOSX guest binary, DOSX frame layout, BOP number, BaseSrv record, command
line, broker architecture, or generic CCPU instruction behavior changes.
The expected direct runtime result is that the FD continuation no longer
falls through its unavailable slot and r036's zero return is absent or moves
to a later source-owned boundary.

Confidence is high for the missing FD provider: source ownership, exact frame
layout, build flags, and missing symbol/provider all agree.  The association
with the observed `D20B:0006` is high but not absolute until the repaired
formal runtime run records the next continuation; r036 did not include a
dedicated FD-frame observer.  This is sufficient for the bounded repair, but
not for any broader MVDM semantic change.

## Repaired formal runtime result

r037 formally linked the repaired x86 worker (`PE machine 014C`, SHA-256
`10dfda3141ce0019a59d3d2b6d55950f2dd07f29e6f7773ebb35d2f79e456824`).
The same `-a system32\\KRNL386.EXE --command system32\\WRITE.EXE` run in
`O:\ntvdm64\logs\T404-r038-dosx-fd-runtime` changed the formerly zero
return from the r036 form:

```
r036: D20B:0006 -> 0000:0000
r038: D203:0006 -> 1101:00B7
      0216:048A -> 1101:011B
```

The segment value is runtime allocation-dependent; the meaningful result is
the nonzero source-owned continuation.  r039 extended the same run to 45
seconds and again did not reopen the zero-return form.  It progressed into a
later hardware-interrupt sequence (vector `0E`, then vector `08`) without a
`KRNL386`, WOW32, or `WRITE.EXE` DEM-open marker.  That is the next boundary;
it is not evidence that DOSX semantics need further modification, nor does it
authorize a generic CPU repair.

r040 enabled the existing default-off COMMAND handoff reports.  It shows
successful original COMMAND initialization through services 05, 0C, 04, 02,
0D and 0F: its environment, HMA stubs, PDB, and return sites are consistent.
It does not reach service 01 (`cmdGetNextCmd`), service 07
(`cmdCheckBinary`), service 08 (`cmdExec`), or the native-child report; hence
`WRITE.EXE` has not yet been handed off.  The observed repeated vector-08
hardware service is a PIC-reported hardware interrupt, not a CPU exception.
The next read-only investigation is the timer/PIC acknowledgment and return
contract before the first `cmdGetNextCmd`; it is outside this DOSX repair.

## Later WRITE/DOSX regression observation (r043--r045)

The later r043 and r044 formal workers reached the BIOS illegal-instruction
path before any COMMAND handoff.  The original exception frame identifies the
fault as `D1C7:0000`, with `MSW=0010` (real mode); the active CPU state is the
BIOS handler `F000:FF33`.  The five bytes at the fault are
`C7 D1 D3 C9 B7`.  `C7 /2` is not an x86 instruction encoding (`C7` only
permits ModRM `/0`), so this is invalid code data, not an unimplemented DOSX
instruction or a CCPU opcode-selection result.

r045 retained the repaired x86 BIOS FD table and the CPU40 FD receiver, and
added an environment-gated, scalar-only report at that receiver.  Its formal
worker SHA-256 is
`5d8b4145284b5477efa0fab8547daadb50007edbe9472dcd3607f005eb48b6f8`.
The r045 WRITE run emitted the same illegal-instruction record but no FD-frame
record.  The report function is present in the linked image, and the BIOS
object was compiled with `NTVDM` and `MONITOR`; therefore this concrete path
did not execute BOP `FDh` before the fault.

This removes the previous causal claim that the FD repair explains this later
`D1C7:0000` failure.  The FD contract remains source-proven and the earlier
zero-return observation remains repaired, but it is not the active cause of
the current WRITE/DOSX failure.  The active boundary is earlier real-mode code
placement/continuation: determine why `D1C7:0000` contains invalid data before
changing DOSX, CCPU instruction semantics, or the FD receiver.

## T398 historical-host A/B (2026-09-10)

The retained T398 evidence records a successful WRITE observation from the
short staged root (recorded in the retained raw history), using worker SHA-256
`f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0`.
That exact worker remains in both
`build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe` and
`build/M0-T398/S2/runtime-x86/ntvdm32.exe`.  The retained companion DLLs are
also available in the latter directory:

| file | T398 SHA-256 |
| --- | --- |
| `ntvdm32.exe` | `f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0` |
| `WOW32.DLL` | `daa7b098243eca014aa6312d7cc62652962d4e59e0e652d089d00d65c4a1d1a6` |
| `VDMREDIR.DLL` | `e86749928371e9689e0cf9634bd4c26e62a85868dcb1d7f63fa1d3c1a45c5877` |

Two controlled launches used exactly
`-f -o -w -a system32\\KRNL386.EXE --command system32\\WRITE.EXE` from the
current `O:\ntvdm64` root, with no diagnostic environment variables:

1. Replacing only the current deployed worker with the T398 worker produced no
   running WOW/Write window after 12 seconds.
2. Replacing the full retained T398 host triplet above produced the same
   result: no running WOW/Write window after 12 seconds.

The current guest-media inputs in both runs were held constant:
`DOSX.EXE` `c5af29a29abf167b243daabf877459e8278b8c9a339bf8e1e2576ead5f6ceeff`,
`KRNL386.EXE` `d404576899cc11d23f368f2b30554e6108657cd37809837b341fd37e6b1a7af4`,
and `WRITE.EXE` `08ee1659788880fb593815ff9609bfe76743c1182d05e6702764bf3dff68b30b`.
This is a negative, bounded result: neither the old worker alone nor the
available old host triplet is sufficient in the current stage.  It does not
show that the host code is correct or that guest media is defective.  T398
also depended on its then-frozen short-path stage, whose complete file layout
and media hashes were not retained, so this is not a reproduction of T398.

After each run, every process whose resolved executable path was exactly
`O:\ntvdm64\ntvdm32.exe` was stopped and the deployed triplet was restored.
The restored SHA-256 values are: `ntvdm32.exe`
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`,
`WOW32.DLL` `9529e5ddbdf65b73bda23afb17b5c0c939c2547397520cfcb1c097574f2680c7`,
and `VDMREDIR.DLL`
`74dfd1f242b2169f8bc589e677ad242196e8034c4404a4e9ae226b231ec52198`.

## r047 host-only low-memory write observation (2026-09-10)

The earlier r046 attempt was not causal evidence: its SAS control variables
were inherited by `cmdenv.c`, changing the guest environment and moving the
fault.  r047 captures and removes those three diagnostic controls before the
original guest-environment snapshot.  It retains only host-side report paths
and scalar configuration; it does not expose a host pointer to guest code or
change a guest register, byte, branch, or dispatch decision.

The r047 x86 worker first linked at SHA-256
`7d522ad53f1243550fb872ec97c402829db2625dfb28cbe426ea18889b875a1a`.
After increasing the bounded report cap from eight to sixty-four entries and
relinking the same formal graph, the test worker SHA-256 was
`08c0a1a2f6c4a8e6e3cf8809609ca87d773bd48f271fcb1ee487975f41545aad`.
Both controlled WRITE launches used the same command and package root as the
earlier runs.  The host-only run reproduced the exact active fault without
changing its position:

```
MVDM-CPU-ILLEGAL fault=D1C7:0000 linear=000D1C70 op=C7D1D3C9B7 \
    live=F000:FF33 msw=0010
```

The sixty-four-entry run recorded exactly eight completed CPU40
`c_sas_store`-family writes overlapping `D1C70..D1C74`: five `C5` bytes at
`D1C70..D1C74`, then `20`, `F7`, and `F1` at `D1C70..D1C72`.  It recorded no
later CPU40 scalar store before the illegal fetch, although the fetched bytes
were `C7 D1 D3 C9 B7`.

This is a coverage result, not a claim that memory did not change.  The
observer is deliberately attached only after `c_sas_store`, `c_sas_storew`,
and `c_sas_storedw`; it does not observe every host mapping, raw backing-store
write, descriptor/cache alias, or loader transfer.  Therefore the active
next audit is the source owner of the real-mode `D1C7` code placement and its
mapping/cache transition.  The result rejects a simple "ordinary CPU store
corrupted the opcode" explanation and does not authorize a CCPU semantic or
DXCODE-writability repair.

The test process was stopped by its known PID after each launch and
`O:\ntvdm64\ntvdm32.exe` was restored and hash-verified to
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.

## Retained T398-stage reconstruction limit (2026-09-10)

The retained `build/M0-T398/S2/runtime-x86` directory is not itself the
historical short-path workload stage. It has the same SHA-256 values as the
current package for `NTDOS.SYS`, `NTIO.SYS`, `COMMAND.COM`, `DOSX.EXE`, and
`KRNL386.EXE`, but it does not retain `system32\\WRITE.EXE`.  Direct launches
also established that its old CLI rejects the newer `-w -a KRNL386.EXE`
form (exit `64`), and a direct process without the historical Console owner
exits `1`.

For a bounded reconstruction only, the current immutable `WRITE.EXE`
(`08ee1659788880fb593815ff9609bfe76743c1182d05e6702764bf3dff68b30b`) was
made available to that build-stage as a temporary hard link, and a temporary
otherwise-absent short-path junction supplied the retained seven-character
root.  The historical Console-owning observer then ran the original old-CLI
form `-f -o --command system32\\WRITE.EXE --without-diagnostics` for thirty
seconds.  It timed out with no WOW32 module and no visible top-level window.
The temporary hard link and junction were both removed in `finally`.

This contradicts neither the historical record nor the current fault data:
the record documents an observation in an earlier live stage, whereas the
complete stage inputs and external execution conditions were not retained.
It does prove that neither the old worker nor its retained core guest-media
subset is a reproducible low-memory baseline now.  No old binary, host DLL,
or undocumented stage artifact is therefore a justified source of a repair.

## T397 deeper-path provenance and replay limit (2026-09-10)

T397 retained both stronger historical observations and a more complete
runtime directory.  Its transition report records successful WOW32-provider
initialization, dispatches, and an orderly kernel exit.  However, the
Console-owned WRITE observation that records that deeper path identifies its
then-deployed worker by SHA-256
`6d266a4c8bd3fe75470e7025e5ec7b3b2b665a6b1e7d849d43804ea0cf97f834`.
That exact file is no longer present under `build/M0-T397`.  The retained
`runtime-control-a200` directory is complete (worker, `WOW32.DLL`, guest
media, and `WRITE.EXE`), but its worker is a distinct later product:
`a2000cbb16c8de2743aff57288e15834266a115df567f43887516b0ecff3ce20`.

The retained `a200` package was replayed in place using the same historical
Console-owned command form, first directly under its long build path and then
through a temporary seven-character short-path junction, matching the path
recorded by T397.  Both thirty-second runs timed out before `WOW32.DLL` was
loaded.  The junction was removed in `finally`; no file in the retained
package or in `O:\ntvdm64` was changed.

Accordingly, the T397 documents establish that a deeper path genuinely
existed, but they do not make the retained `a200` worker a reproducing
baseline.  The exact successful worker has not been retained, so binary-level
diffing against it is impossible.

The nearest retained source snapshot is non-ancestor commit `ef1c3cb28`
(`wip: preserve pending product implementation`), whose parent is the
pre-governance fork point `e6ba6572f`; current `HEAD` is on the other branch.
It differs from current `HEAD` across the DOS command path, DPMI/mode switch,
SoftPC mapping and CCPU transition code, WOW32, and a large amount of
diagnostic/provider work.  It is therefore an audit comparison source, not a
safe bulk-recovery candidate.  An isolated x86 build of that snapshot also
did not reach WOW32 when run against the current external package, which
independently rejects treating the whole WIP as a causal repair.

## r048 DOS EXEC RETF control (2026-09-10)

r048 adds no guest-memory or execution change.  It captures the existing
`MVDM_BOP_RETURN_REPORT_PATH` before `cmdenv.c` snapshots the host
environment, exactly as the prior host-only controls do; this prevents the
report selector itself from changing the DOS executable-load environment.
The full x86 formal graph linked successfully as
`4252ef141e07eac0c5aa94797c59f49fec4b777704e48691a7044b195f8bea72`
(PE machine `014c`).

The first 15-second controlled WRITE launch recorded the HMA low-memory stub
RETF as `00A7:1113 -> D2B8:0010`, a nonzero EXE-entry-shaped destination,
and emitted no illegal-instruction report.  A separate 28-second run,
without the r047 SAS store reporter enabled, again reached COMMAND services
and emitted no `D1C7:0000`; it eventually reached a different illegal frame
at `0000:0036`.

This is not a success claim and it does not make the later frame correct.
It does falsify the assertion that the active path deterministically sets
the DOSX entry to its image base or that this RETF deterministically pops the
wrong values.  The r047 SAS reporter calls a host observer after every
selected store; although it does not change guest bytes, the different
outcome establishes that it is not a no-timing-impact baseline.  Future
causal tests must therefore use a low-frequency, captured-path observer and
compare controlled repetitions.

For both r048 launches, only the observer-created known child PID was ended.
`O:\ntvdm64\ntvdm32.exe` was restored in `finally` and SHA-256 verified as
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.

## Standalone-boundary and SoftPC-overlay source audit (2026-09-10)

The owner requested a concurrent audit of the current standalone/CLI boundary
and the retained `mvdm-host`/overlay differences, on the hypothesis that an
unnecessary change inside the original SoftPC closure may have created the
current difficult-to-reproduce DOSX frontier. This is a source-only audit;
it changes no production source or runtime package.

The formal r048 graph selects Win32/x86 `CPU_40_STYLE`, `CCPU`, `C_VID`, and
`PROD`; it does not define `SFELLOW`. The audit therefore classifies the
selected CPU40 path rather than dormant alternate CPU branches.

| Surface | Source finding | Current disposition |
| --- | --- | --- |
| `DEMFASTREAD` (`DIV-188`) | The DOS `$READ` code calls `FastOrSlow`, then `SVC_DEMFASTREAD`, and on carry takes its unchanged `SVC_DEMREAD` slow path. The original fast handler was NT kernel `NTFastDOSIO`; the overlay is a user-mode substitute that performs only a bounded synchronous regular-file read and otherwise returns carry. Historical T198 records both an AX:BP handle-order repair and a native committed `50:42` fast-read observation. r048's reached DOSX load uses `50:16` only, not `50:42`. | Retain now. This is a real standalone seam, not x64 residue. A forced-carry A/B is not a current DOSX causal experiment because the reached loader does not call it; reserve it for a workload that actually reaches `50:42`. |
| CPU event bitmap (`DIV-214`) | This is a semantic change within `c_main.c`: ordinary shared-bit read/modify/write was replaced by atomic snapshot/set/take. The selected graph has an external timer path (`nt_timer.c -> cpu_interrupt(CPU_TIMER_TICK)`) and a PIC path (`ica.c -> host_set_hw_int -> cpu_interrupt(CPU_HW_INT)`), so this bitmap is genuinely reached from host notification paths outside ordinary guest instruction execution. | Retain pending a focused event-ownership audit. Blindly restoring the non-atomic source would reintroduce lost-event races; conversely atomicity can change delivery timing. Any test must instrument only aggregate raise/take ordering and compare repeated runs before considering a serialized delivery seam. |
| `localfm.c` overlay (`DIV-137`) | The original carrier defines `Gdp`, `Cpu`, `Sas`, and `Video`. The selected `ccpusas4.c` already owns `Sas`; compiling the original carrier would create a second `Sas` object. The overlay keeps the original remaining global names and uses the selected CCPU/C-VID generated declarations. | Required composition shim; not an executable-semantics replacement and not removable without a replacement for the duplicated-source ownership. |
| `sas_overwrite_memory` overlay (`DIV-138/189`) | The wrapper is the original `ntstubs.c`-shaped forwarding call to `c_sas_overwrite_memory`; the selected CCPU provider is itself a no-op cache-invalidation routine. The C-VID binder assigns that exact provider into the otherwise-null generated SAS vector slot. | Retain. It does not write or transform guest code and prior source evidence explicitly excluded the no-op invalidator as a fault cause. |
| C-VID vector binder (`DIV-156`) | The selected manifest retains the CCPU/C-VID tables but lacks their historical generated binding translation unit. The overlay binds the original-named CPU/SAS/Video tables once at the existing `setup_vga_globals` point; it also retains the source-derived initial jump calibration. | Required generated-unit replacement. Removing it produces unbound vector state rather than restoring an original compiled unit. |
| CPU diagnostic hooks | The current branch has already removed a larger WIP set of instruction-window and DPMI hardware-interrupt hooks from `c_main.c`. The remaining reached observers are default-off scalar reporters. r047/r048 prove that high-frequency reporting can alter timing despite not changing guest bytes. | Keep only while diagnosing; a later release-composition pass should omit them rather than normalize them as SoftPC behavior. Do not remove the current bounded observations before a lower-frequency replacement preserves the evidence path. |

The audit rejects a bulk “restore all original SoftPC files” approach: the
three CCPU/C-VID overlay units are minimal composition seams, and `DIV-214`
has a real modern host-concurrency rationale. If the historical-worker
comparison cannot isolate the frontier, the next bounded experiment is to
measure CPU-event delivery ordering; `DEMFASTREAD` is deferred until a reached
workload calls `50:42`. Neither experiment is authorized by the current S2
file allowlist.

## T397 formal-graph composition comparison (2026-09-10)

The retained T397 S3 formal-x86 and S4 runtime-x86 `source-manifest.json`
files were mechanically compared with r048. All selected original-machine
lists match: the 131 CCPU files, CCPU/C-VID overlays, BIOS, keymouse, system,
support, video, C-VID, DEM, COMMAND, DPMI, and host-root lists are identical.
Both graphs declare x86, CCPU40, MSVC `/MT` through `VsDevCmd`, and no `_X86_`
global define. Their CCPU-facing compiler profiles select the same
`CPU_40_STYLE`, `CCPU`, `C_VID`, and `PROD` conditions.

The only manifest inputs retained by T397 and absent from r048 are the
retired mapping/identity and observation adapters
(`mapping_manager.c`, `session_termination.c`, `mvdm_host_identity.c`,
`mvdm_softpc_machine_observation.c`) plus the old `dialog_context.c` adapter.
T403 intentionally removed the first four as x64/mapping-manager residue;
the remaining old dialog adapter was replaced in the current pending T404
worktree. T397 also has no separately declared VDD adapter list, while r048
does. Build-rule comparison shows no selected CCPU/SoftPC definition change;
only generated-include-root and adapter-include differences remain.

Therefore the retained formal graphs do not support a claim that current
execution selects a different CCPU, DOS, DEM, DPMI, WOW, or SoftPC host source
set. The historical deep path still cannot be reproduced from its retained
runtime directory, so source-content and runtime-condition comparison remain
necessary; but the next causal audit should prioritize the retired/replaced
host-adapter boundary and package runtime inputs rather than bulk restoring
the original-machine composition.

## T397 dialog-adapter startup-order check (2026-09-10)

The one historical graph input not retired as mapping/identity residue,
`dialog_context.c`, contains only `SetWindowLongPtr`/`GetWindowLongPtr`
wrappers. Its functions have no static constructor, process initialization,
or caller before a host window/dialog exists. The current hard-error dialog
facade similarly allocates and presents only when `NtRaiseHardError` is
called from the WOW error path. That call occurs after DOSX/bootstrap; neither
adapter participates in original `ntvdm.c -> host_main -> config -> cpu_init`
startup.

It is therefore not a plausible direct owner of the earlier real-mode
`D1C7:0000` failure. The adapter remains relevant to the later WOW UI
boundary, but is removed from the current early-DOSX causal shortlist.

## DOSX image-load and entry-transfer check (2026-09-10)

The proposed `DEMFASTREAD` fallback experiment is not a current causal test.
The r048 BOP report records ordinary `50:16` `DEMREAD` requests during the
reached DOSX load and records no `50:42` `DEMFASTREAD` request.  The same
run's DEM-open record identifies `O:\NTVDM64\SYSTEM32\DOSX.EXE`; its file
length is 36,616 (`0x8F08`) bytes.

The captured DOSX `DEMREAD` has `DS:DX=CA00:0000`, offset
`0000:0200`, request length `8E00`, and a successful completion of `8D08`
bytes with carry clear.  `0x8D08` is exactly the full DOSX load module after
its `0x200` MZ header.  The MZ initial entry is `CS:IP=08B8:0010`, and the
separate existing DOS EXEC return report records `00A7:1113 -> D2B8:0010`.
The identity `CA00 + 08B8 = D2B8` directly ties that successful full-file
load to the original DOSX entry transfer.

This is positive evidence that the current reached r048 path did not obtain a
truncated DOSX image through the fast-I/O substitute and did not simply return
to the image base.  It supersedes the prior suggestion that a forced
`DEMFASTREAD` carry/fallback run should be the first next experiment.  That
fallback remains a valid later boundary check only if a trace actually reaches
service `50:42`.

As a separate source-location check, the other r048 control run loaded DOSX
at `C9FD:0000`.  Under that recorded base, the historical `D1C7:0000`
physical address would correspond to DOSX file offset `0x7EA0` after allowing
for the MZ header.  The immutable file bytes there are
`06 C7 46 0C 00 48 C3 80 FC 04 75 0B C7 46 00 00`, not the historical illegal
fetch bytes `C7 D1 D3 C9 B7`; that five-byte sequence is absent from the
immutable DOSX file.  The `D1C7` report and this alternate allocation-base
run are not the same execution, so this is not a same-run address proof.
It does, however, rule out treating the current evidence as a simple partial
file-read explanation.

The remaining highest-value investigation is therefore a low-frequency,
source-owned comparison of post-load real-mode mapping/cache and continuation
state, followed by the first protected-mode handoff when it is reached.  It
must not alter the loader, guest image, CCPU instruction semantics, or the
CPU event map without a newly admitted causal scope.

## Retained deep-path CPU40 transition delta (2026-09-10)

The previously retained deep-path source snapshot `ef1c3cb28` was compared
against the current main-line source, rather than inferred from its name or
from the unavailable T397 executable.  Four DPMI/CPU files alone differ by
527 added and 7 removed lines in that snapshot:

| file | snapshot delta | disposition for the current frontier |
| --- | ---: | --- |
| `dpmi32/modesw.c` | +325/-6 | Contains a reconstructed `53:01` CPU40 protected-mode frame handoff, GDT/IDT carrier, private TSS carrier, real-mode cache reloads, and the later FD receiver. These are substantial CCPU/NT-kernel-VDM substitutions, not a recoverable original-file restoration. |
| `dpmi32/dpmi32.c` | +101/-1 | Adds descriptor-shadow, IDT-source, and FastWOW TEB projection setup around source DPMI dispatch. |
| `dpmi32/dpmiselr.c` | +19/-0 | Copies published DOSX descriptors into the snapshot's private CCPU LDT shadow. |
| `softpc.new/base/ccpu386/c_seg.c` | +82/-0 | Adds real-mode cache reloads and several fixed selector/cache substitutes, including the previously rejected writable `00CFh` DXCODE special case. |

The retained T395 record independently says that its deeper DOSX bootstrap
crossed `53:01` using `DIV-222` through `DIV-226`; this source delta supplies
the concrete implementation family behind that historical depth.  It also
explains why matching only the T397 source-file manifest is insufficient: the
translation-unit lists and compile conditions are equal, while their contents
can differ materially.

This is not authority to restore the snapshot.  It is a non-ancestor WIP
whose protected-mode implementation combines multiple speculative carriers,
including `00CFh`, and the present `D1C7` observation is real mode with no
reached `53:01`/FD report.  None of these protected-mode additions can
directly explain or repair that earlier fault.  Conversely, once the
real-mode frontier is crossed, current main cannot be expected to reach the
historical T395/T397 WOW depth without a new source-first audit of each
individual DPMI transition seam.

Finally, `DIV-214` atomic CPU-event code predates the retained deep-path
snapshot (commit `4720ebb00` is its ancestor).  It therefore cannot by itself
explain the historical-deep-versus-current-shallow distinction, although its
timing remains a separate measurement concern.

## r049 target-only RETF observation (2026-09-10)

The existing `RETF` observer formerly emitted up to 4,096 records, which made
it unsuitable for the timing-sensitive `D1C7` frontier.  Within the admitted
adapter observation surface, it now returns immediately for every completed
far return except `target=D1C7:0000`, where it captures one scalar source and
destination record.  It neither reads guest memory nor changes the decoded
return, stack, selector, or instruction dispatch.

The x86 formal worker
`build/M0-T404/S2/r049-d1c7-retf-x86/original-softpc-process.exe` linked at
SHA-256 `6e6f74a8e512273ff9570f582f201b61751a905fb7ece129255208966b0cec1c`.
The independent `/force` closure-audit DLL retained its expected unresolved
provider diagnostics; that target is not a prerequisite of the normally
linked worker and no claim is made that it passed.

One console-owned, non-interactive 20-second run used the fixed package and
the existing command
`-f -o -w -a system32\\KRNL386.EXE --command system32\\WRITE.EXE`.  It did
not reach `D1C7:0000`, so produced no target-only RETF record.  Instead it
showed the public illegal-instruction dialog for a later frame:

```
MVDM-CPU-ILLEGAL fault=0000:0036 linear=00000036 op=6305A20063 \
    live=F000:FF33 msw=0010
```

The observer terminated only its own child after the timeout.  The deployed
package worker was restored and hash-verified to
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.
This run does not identify the `D1C7` transfer source; it strengthens the
existing conclusion that the frontier is observation/timing-sensitive and
that no current individual overlay can be blamed without a same-run causal
record.

## r050 isolated RETF observation (2026-09-11)

The r049 observer still used the shared `MVDM_BOP_RETURN_REPORT_PATH` naming
route.  That route also enables the pre-existing broad BOP return reporting,
so it was not an isolated timing experiment despite its target filter.  r050
corrected only the admitted adapter diagnostic surface: it captures a private
`MVDM_RETF_TARGET_REPORT_PATH`, removes that host-only name before the guest
command environment is formed, and emits one record only when a completed
far return targets `D1C7:0000`.  It does not enable BOP reporting, inspect
guest memory, or modify control transfer.

The normally linked x86 worker
`build/M0-T404/S2/r050-d1c7-retf-isolated-x86/original-softpc-process.exe`
has SHA-256
`217037c5418c2a8a4a414a30039555f002c40aea7ed181500dc3151224cb412b`.
One fixed-package, non-interactive 20-second WRITE launch was performed with
only that private target-report variable and the existing one-record illegal
instruction report.  It again did not reach `D1C7:0000`, produced no target
RETF record, and instead stopped at the later real-mode fault:

```
MVDM-CPU-ILLEGAL fault=0000:0036 linear=00000036 op=6305A20063 \
    live=F000:FF33 msw=0010
```

The process was gone at observer completion.  The deployed worker was then
restored and SHA-256 verified as
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.
Accordingly, r050 yields no transfer source and no repair candidate.  It does
show that simply avoiding the shared BOP-report environment still does not
reproduce the historical `D1C7` frame under this fixed launch; that frame
must be treated as a sequence-sensitive symptom until a same-run capture can
be obtained.

## Current-worktree mirror-diff triage (2026-09-11)

The tracked working-tree differences under `src/mvdm-host` were reviewed
directly against `HEAD`; this is intentionally distinct from the historical
snapshot comparison above.  The result is not a claim that all current
changes are product candidates.

| surface | current change | risk and disposition |
| --- | --- | --- |
| `dpmi32/modesw.c` | Adds the CPU40 `switch_to_real_mode` BOP-FD provider and two scalar observations around DOSX mode changes. | High-risk execution seam, but it cannot be the direct repair for the observed real-mode-before-`53:01` D1C7 frame. It is needed to reach the later CPU40 real-mode return contract; audit its source-frame contract individually before treating it as a release change. |
| `dpmi32/dpmi32.c`, `illegalp.c` | Add optional DOSX-init and illegal-instruction reporters. | Pure default-off diagnostics; do not retain in a release composition once the active frontier is closed. They are not candidates for an execution repair. |
| `inc/softpc.h` | Adds `WINAPI` to the two host thread declarations. | Compiler/ABI seam rather than x64 behavior. A direct source restoration without an x86 calling-convention proof risks stack corruption, so it is not safely removable by inspection. |
| `wow32/*` | Pending private-hard-error/UI integration changes. | Late WOW presentation/integration boundary. It runs after the DOSX/DPMI frontier and is not a direct candidate for D1C7. |
| retired T403 mapping/identity overlays | Not present in the selected graph. | Already removed. Their reintroduction would expand the standalone closure and is rejected. |
| retained CCPU/C-VID and DEM overlays | `DIV-137/138/156`, `DIV-188`, and `DIV-214`. | The earlier source audit establishes concrete provider/ownership or host-boundary reasons. No one is safely removable without replacing an omitted source owner or reintroducing a kernel service. |

Thus the only changes that can be cleanly removed on release grounds today
are the default-off diagnostic reporters after their evidence value is no
longer needed.  There is no evidence-supported bulk restoration of original
SoftPC code, no current fast-read causal path, and no remaining mapping-
manager/identity residue to delete.  The next source-first repair candidate,
if a future admission permits one, must be a narrowly proven CPU40/DOSX mode
transition contract—not a CCPU instruction decoder change or a broad overlay
rollback.

## Low-IVT fault classification (2026-09-11)

The low-impact r050 run's repeated frame `CS:IP=0000:0036` is materially
different from a normal code address: physical address `0x36` lies in the
real-mode interrupt-vector table (IVT).  It therefore cannot be described as
a generic unimplemented CCPU opcode.

The selected original CPU40 implementation in
`softpc.new/base/ccpu386/c_intr.c` services a real-mode interrupt by reading
the two words at `vector * 4` and `vector * 4 + 2`, then loading that exact
`CS:IP`.  Separately, the selected x86 compile profile defines `NTVDM`,
`CPU_40_STYLE`, `CCPU`, and `C_VID`, but not `X86GFX`; the original
`reset.c` consequently takes its own low-memory clear and `setup_ivt()` path.
That original setup writes the expected IRQ6/vector-`0E` destination as
`F000:EF57` (the BIOS `diskette_int` stub), not `0000:0036`.

The r050 control stream did report one accepted hardware vector `0E`, but it
does not include a source-owned IVT snapshot or timestamped transfer record.
Consequently the evidence proves neither that vector `0E` was the immediate
predecessor nor that a particular guest write corrupted an IVT entry.  It
does prove that the observed execution has left the initialized BIOS vector
contract by the time of the illegal frame.  The imported
`MVDM_SOFTPC_PATCH_CCPU_VECTOR_DEFAULTS_ONLY` object is unrelated: it exports
only unselected generated MIPS/PPC CCPU-vector defaults and an immediate
debug-break fallback; it neither initializes nor reads the real-mode IVT.

Under the active packet, the allowed observer may not read guest buffers or
change other CCPU source files, so recording an IVT tuple at IRQ acceptance
or fault time requires renewed admission.  The next admissible repair must
first establish the exact transfer producer and the before/after IVT owner;
there is no basis to rewrite `setup_ivt`, CCPU decoding, or the IRQ vector
from this evidence alone.

## Historical-delta and standalone-closure disposition (2026-09-11)

The historical-deeper and standalone-closure reviews were cross-checked at
the source level.  T397 and r048 select the same original-machine translation
unit lists and the same x86 CPU40/C-VID conditions.  The retained deeper WIP
snapshot `ef1c3cb28` is therefore useful only for comparing the contents of
those units, never as a bulk source candidate.

The execution-affecting `c_main.c` change most relevant to interrupt timing is
the signed `ica_intack()` rejection guard (`DIV-221`): a stale host notification
returns `-1`, and the guard avoids narrowing it to an artificial INT FF.  It
was introduced by `4dbd8a5ff` on 2026-09-04 and is an ancestor of both current
`HEAD` and `ef1c3cb28`; the file has no content delta between that commit and
current `HEAD`.  It can influence timing, but cannot explain the difference
between that retained deeper snapshot and current source, and removing it
would restore a demonstrated bad host-notification conversion rather than
original guest behavior.

This leaves the current dispositions deliberately narrow:

| class | disposition | reason |
| --- | --- | --- |
| Retired x64 mapping/identity managers | Already removed; do not restore. | T403 restored the original x86 AX:BP and pointer-width carriers. |
| CCPU/C-VID composition overlays and `DEMFASTREAD` | Retain. | They replace omitted generated or kernel-only providers; the reached DOSX loader does not call fast-read. |
| CPU event atomicity and stale-IRQ guard | Retain pending dedicated ordering evidence. | Both are real host-concurrency boundaries and predate the retained historical snapshot. |
| Current default-off observers | Remove from release composition after this frontier is closed. | They are evidence aids, not product behavior; r047/r048 show high-frequency observation can perturb timing. |
| `ef1c3cb28` 53:01/GDT/IDT/TSS/cache additions | Do not restore. | They are post-real-mode speculative substitutions and the active fault precedes their reachable transition. |

No currently selected original SoftPC execution source is therefore an
evidence-supported bulk-removal candidate.  The only unresolved, source-owned
fact at the active `0000:0036` frontier is which real-mode transfer reads or
writes the IVT entry.  Capturing that fact requires a newly admitted,
low-frequency IVT transfer observation; it is outside the current scalar-only
allowlist and is not implemented here.

## IVT transfer-owner sweep (2026-09-11)

The static sweep narrows the possible transfer readers without attributing a
writer.  The original CPU40 `c_intr.c:do_intrupt` reads the IVT for ordinary
real-mode interrupt delivery.  `reset.c:setup_ivt()` initially publishes
vector `0E` as `F000:EF57`, and r050's existing control stream records an
accepted vector `0E`; that is a plausible transfer family but not a proven
predecessor of the later `0000:0036` frame.

Independently, unmodified original `dpmi32/stack.c:DpmiPushRmInt` reads an IVT
entry through `Sim32GetVDMPointer(0, ...)` and directly loads its CS:IP.  Its
callers are the `DPMI_EXEC_INT` paths in `int21map.c`, and therefore it is
reachable only after the DOSX/DPMI setup that provides the DOSX stack frame.
The r050 retained logs contain no admitted DOSX-init scalar record, so they do
not establish that this DPMI owner had become reachable in that run.  It is a
second candidate transfer reader, not a diagnosis or a change request.

The r050 BOP-return stream ends with repeated accepted hardware vector `08`
and its EOI records; `c_main.c` calls `do_intrupt` immediately after the
existing vector-service record.  The illegal-frame record is written through a
separate report sink, so the retained files do not provide a shared ordered
timestamp and cannot prove that the final vector `08` is the exact predecessor
of `0000:0036`.  It nevertheless makes that real-mode `do_intrupt` read the
first and sufficient observation point.  A renewed observation should record
`(vector, IVT offset, IVT segment, pre-transfer CS:IP)` only for vector `08`
and only when its chosen target is `0000:0036`, with no guest write, no
alteration of the target, and no high-frequency general SAS store tracing.
The DPMI reader remains a deferred fallback if this source-local proof is not
obtained.  A writer audit is warranted only if the captured hardware-transfer
tuple is already corrupt.

## r051 IRQ 08 IVT-transfer observation (2026-09-11)

Under the renewed, exact-scope admission, r051 adds a default-off witness at
the original CPU40 real-mode `do_intrupt` reader.  It receives only the IVT
offset and segment already read by that function, and only records once when
hardware vector `08` has selected `0000:0036`; it makes no guest-memory read
or write and does not alter CS, IP, the stack, interrupt dispatch, or error
handling.  The private `MVDM_CPU_IVT08_REPORT_PATH` is captured and removed
before the guest command environment is constructed.

The normally linked formal x86 worker
`build/M0-T404/S2/r051-ivt08-target-x86/original-softpc-process.exe` has
SHA-256
`a4979c42cd4337aa903be72b7d2984c53a4cef95e10f1999998f60ad7c068eb2`.
One fixed-package, non-interactive 20-second WRITE launch used
`-f -o -w -a system32\\KRNL386.EXE --command system32\\WRITE.EXE`, the
private IVT witness, and the existing one-record illegal-frame report.  The
run again produced:

```
MVDM-CPU-ILLEGAL fault=0000:0036 linear=00000036 op=6305A20063 \
    live=F000:FF33 msw=0010
```

The existing BOP stream also again contains repeated accepted hardware vector
`08` records.  However, the private IVT report is absent: no ordinary
real-mode vector-`08` `do_intrupt` delivery selected `0000:0036` in this same
run.  This is a negative causal result: IRQ 08 service cannot be the direct
reader that transferred execution to the observed low-IVT frame.  It neither
identifies an IVT writer nor supports a repair to CCPU, the IRQ vector, or
`setup_ivt`; the deferred DPMI/other-transfer-reader branch remains the next
research boundary.

The observer had no surviving product process at completion.  The published
worker was restored from the retained T404-r048 stable backup and hash-verified
as `0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.

## r052--r056 low-frame transfer elimination (2026-09-11)

Inspection of the original `illegal_op_int` changes the meaning of the
previous fault record: `0000:0036` is copied from the exception stack frame,
while live `F000:FF33` is the BIOS BOP-06 illegal-op receiver.  Thus the
record proves that the emulator's saved fault context names `0000:0036`; it
does not by itself prove that the error receiver transferred there.

Five successively target-latched, default-off x86 workers were built and run
once each against the same fixed-package 20-second WRITE command.  Each
observed only already-decoded scalar data, reported at most once, and restored
the stable deployed worker after completion.

| Run | Added exact witness | Result |
| --- | --- | --- |
| r052 `039dc7b248bed0c31c9e3ae3295a106e77ed3458e081d897859efd707a8acd3b` | RETF and real/V86 IRET target `0000:0036` | No witness; same illegal frame. |
| r053 `dee3348e956d688cd3a2b2a0dfd2ee2ca39910c962b88831384ec3eb8e9bd78c` | far and near indirect CALL/JMP target | No witness; same illegal frame. |
| r054 `aa9fc2b6b4794a5601a3d133d4a1e49dea3d97022369f56e6086d67e9f335b2d` | shared relative-transfer helper target | No witness; same illegal frame. |
| r055 `f557afaa10823213f8918534905ad8babd26125154e8af78a3d7fc175e19b520` | every original real-mode IVT read target | No witness; same illegal frame. |
| r056 `00758844abcd40c3357754d6284c0befbf798ff3a9e081966bfab99197e2368a` | original host `setIP`/`setCS` carrier target | No witness; same illegal frame. |

Every run still emitted exactly:

```
MVDM-CPU-ILLEGAL fault=0000:0036 linear=00000036 op=6305A20063 \
    live=F000:FF33 msw=0010
```

The `0036` physical address is itself inside the IVT; the following word
`D1C7` is existing IVT data, which also explains why treating these bytes as
ordinary program opcodes was misleading.  Together, r051--r056 rule out a
direct ordinary real-mode interrupt, RETF/IRET, far/near/relative CCPU
transfer, and the original host setter carrier as the producer of the saved
low frame in these runs.  The next owner is therefore the CCPU saved-fault
context (`CCPU_save_EIP` and its context-save/restore producers), not an IVT
writer or a speculative CCPU instruction-semantic repair.  No repair is
supported by these negative results.

After r056, `O:\ntvdm64\ntvdm32.exe` was restored and SHA-256 verified as
`0b46c95c9d2fb063356b7d67c7f8268eac359936d4774fbf9e813c9f7b32a06b`.

## r058--r060 CPU40 DPMI provider recovery control (2026-09-11)

The retained A200 runtime worker loads `WOW32.DLL` under the historical
console-owning observer, whereas the current r048 worker does not.  Its map
contains the CPU40 `DpmiCpu40SwitchToProtectedMode` family; current r048
maps `53:01` through `DpmiIllegalFunction`.  The original i386 provider is
not directly composable because it mutates NT kernel-VDM state.  The selected
first recovery rung is therefore the smallest source-shaped CPU40 seam from
the retained `ef1c3cb28` historical snapshot: original DOSX `53:00` table
publication and `53:01` frame ordering, with only a session-local GDT/IDT/TSS
carrier.  Mapping, host identity, FastWOW TEB, `00CF` writable CS, and all
other snapshot changes were excluded.

The r058 formal x86 worker linked successfully and its map contains all three
provider symbols.  It was then run through the old observer with the same A200
media, a temporary removed short-path junction, `-f -o --command system32\WRITE.EXE`, no
diagnostics, and a 30-second timeout.  Its SHA-256 was
`b125cbd4c0c3b59268e5af5d0dd2e12b30ce6964a3b73a2439b5a2ed2fb3d7e2`.
The result was a timeout after two modal continuations and `loaded-wow32=no`.
The temporary candidate and junction were removed.

r059 added the immediately reached source-shaped `53:11` PM-stack-info
projection: the source `VDM_DPMIINFO` layout is allocated in CPU40 guest
linear memory and its address is returned in CX:DX, replacing the unavailable
kernel `VDM_TIB` pointer without publishing a host pointer.  The formal x86
link again succeeded (SHA-256
`f14fa897becc2b4abe27fd036b7cd93c0280df3316c8a8a61bc250e77cfdc1c`).
The same A200 control again timed out after two modal continuations with
`loaded-wow32=no`; the temporary copy and junction were removed.

r060 repeated r059 with the historical observer's default diagnostics solely
to identify a reached `53:xx` service.  Its BOP record contains only `50:xx`
and `54:xx` entries and no `53:xx` entry; diagnostic timing altered the
earlier sequence and cannot refute the no-diagnostics controls.  It does show
that no further DPMI-provider edit is source-proven by this observation.  The
next bounded research step is a source/map comparison of the historical WIP's
CPU40 selector-cache carriers before any such carrier is admitted.  The
deployed `O:\ntvdm64\ntvdm32.exe` was not changed by r058--r060.

### Deferred selector/TEB carrier admission boundary

The retained deep-path maps contain `Cpu40WowFastTebAddress`; r059 does not.
This is not an inference from a generic Windows TEB convention.  The selected
original `src/mvdm-host/wow32/i386/fastwow.asm` explicitly loads
`KGDT_R3_TEB | RPL_MASK` into FS, reads `fs:[PcTeb]`, then reads
`[TEB + TbWOW32Reserved]`.  It uses that resulting task-data pointer for
`vpStack`, `vpCBStack`, and `WtdFastWowEsp` through the callback/return path.
Thus a CPU40 guest-linear projection of the NT VDM-era TEB prefix is a
source-shaped prerequisite for the reached FastWOW path; it cannot be
substituted with a host TEB pointer.

The historical snapshot also adds CPU40 segment-cache carriers in `c_seg.c`:
`0040` for the original BIOS-data descriptor, `0023` for inherited flat
user-data, and `003B` for the TEB projection.  The snapshot's `00CF` writable
code selector is a distinct claim with no independent causal observation and
is excluded.  Since the active packet expressly excludes SoftPC cache edits,
no selector/TEB code change has been made.  A renewed bounded admission must
name exactly the three documented selector carriers and the TEB prefix, retain
the original descriptor layout/access rules, and prohibit `00CF` and every
other CCPU behavior change.
