# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S36**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S36 — Ordinary Mode, single implementer/reviewer. |
| Candidate Proposal | [MVDM runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md), S36. |
| Admission And Approval | Standing owner authorization automatically admits each sequential S; S35 closed and was pushed as 23d12c9f5. |
| Objective | Verify the complete selected original suballocator capability through real XMS and DPMI-backed callers: allocation, relocation, fragmentation, exhaustion, release and teardown. |
| Non-goals | No guest-media changes, replacement allocator, new CPU semantics or false success for an unavailable guest path. |
| Reference Baseline | S16 original-source/host-fixture closure and S35 real XMS growth/shrink/error/reuse evidence; neither proves real forced relocation or the complete DPMI caller. |
| Files And ABI Surface | Original suballoc.c, suballcp.h and inc/suballoc.h; XMS and DPMI allocation callers; existing commit/decommit/move bindings; independent tests only unless a source-proven defect is found. |
| Applicable Rules | Execution, source policy, mirror/minimal-diff, guest immutability, transcript-gated acceptance and build/runtime output hygiene. |
| Verification | Freeze source and caller inventory; audit every allocator operation and callback; original-source fixture plus real guest normal/failure/relocation/release workloads; formal x86 and all 17 product regressions. |
| Expected Markers | Data survives forced relocation, failed growth preserves the old block, released capacity is reusable, callback/lease lifetimes balance; unreachable or externally unavailable paths are explicitly evidenced, not fabricated. |
| Asset Needs | Pinned OpenNT, formal x86 graph, disposable probes under build/M0-T420/S36 and runtime tests; logs under the approved runtime log directory. |
| Reporting Requirements | Byte/normalized mirror comparison, retained binding rationale, original callers, positive/negative/teardown results and exact residual debt; report changed production footprint separately from tests. |
| Stop Conditions | Required guest-media mutation, new CPU/guest-memory ABI, unexplained product regression, or original-owner boundary expansion. |
| Exit Criteria | Complete original-owner capability dispositions; formal build and real guest/product tests pass or owner-approved limitation is recorded; reviewed evidence and state committed/pushed. |
| Original Owner Request | Automatically admit every S and recover/verify each complete selected package with minimal original-source differences. |
| Similar-Issue Sweep | Both XMS and DPMI callers, page granularity, rounding/overflow, in-place versus moved growth, shrink, allocation failure, callback failure and resource cleanup. |

## S35 Closure Record

The [S35 evidence](../etc/evidence/m0-t420-s35-xms-capability-progress.md)
records the complete selected XMS capability review: allocate/free/reuse,
move/overlap, growth/shrink/failure preservation, A20, UMB, INT15 and worker
lifecycle. Seven of eight manifest files now match pinned OpenNT bytewise;
the prototype-name-only DIV-198 was retired as unnecessary, not an ABI fix.

Nine direct header consumers were recompiled in the formal x86 graph.
All four XMS routes and all 17 product routes pass on the five hash-matched
artifacts deployed to `O:\winnt`. Guest media remain unchanged.
The proven low-DOS COMMAND INIT/environment lifetime bug remains explicitly
unresolved in [TODO](TODO.md), as approved by the owner; it is not a pass
and no environment, allocator or guest-patching workaround is admitted.

## S34 Closure Record

The merged COMMAND stream/pipe scope is complete.  The formal x86 graph
`build/M0-T420/S34/formal-x86-r1` linked the three programs and `VDMREDIR.dll`.
Published `O:\winnt` artifacts then passed the text-gated 17-route product
matrix, including direct and interactive COMMAND, three nested MEM reports,
direct MEM, `COMMAND.COM /c`, native stdout/stderr/EOF, guest exit status and
the EDIT return route.  The host/guest stream matrix passed three fresh runs:
host stdout pipe, host file output, host stdin pipe, guest `>`, `>>`, `<`, and
`|`, plus the pipe at first, second and third nested COMMAND depth.  Every
guest pipeline emitted the expected child-to-child marker.

The former empty-output report was not a product failure: the test used an
illegal long DOS result basename, whereas DOS correctly created its 8.3
spelling.  The final matrix uses `G1.OUT`, `G2.OUT` and `G3.OUT` and checks
each row's error level.  The retained original COMMAND mirror is now
byte-identical; only finite product boundaries remain: inherited host-stream
capture in `run16`, typed receipt delivery in `basesrv`, and the unavailable
system-VDM spawn adapter in `ntvdm`.  No raw HANDLE, guest pointer or Console
pseudo-handle crosses the broker record.  See the
[S34 stream recovery evidence](../etc/evidence/m0-t420-s34-command-stream-recovery.md).

## S30 Closure Context

The owner-expanded S30 scope is delivered at `91a5be61a`: task inputs enabled
`EMM=RAM`, and a real DOS guest completed INT 67 allocation, map/remap,
unmap and free on the formal product. The complete 17-route product matrix
also passed. The closure record below is the current disposition; it was not
withdrawn.

S30 now audits the original COM/LPT configuration and host providers against
actual Windows endpoints, then adds the default DOS, HIMEM/DOSX and explicit
EMS profile matrix. No S31/debug work is admitted.

The first real matrix results are: default startup reaches both public XMS
and DPMI queries (`S30_HIMEM_DOSX_OK`); the task PIF override reaches XMS
without DOSX and completes original EMS map/remap/unmap/free
(`S30_HIMEM_ONLY_OK`, `S30_EMS_*`); a task PIF with no DOSX also completes
normally (`S30_PURE_DOS_OK`); and guest COM3 opens the discovered Windows USB
serial endpoint through unchanged original `nt_com.c` (`S30_COM3_OPEN_OK`).
The first pure-DOS probe incorrectly set the original PIF editor's default
EMS reservation to zero, shrinking the worker address-space allocation and
causing the initial stall. Its subsequent assumption that XMS absence proves
HIMEM absence was also false: selected worker XMS handling is a process
baseline. The corrected profile proves the actual PIF-owned distinction,
which is absence of DOSX/DPMI.

The original UART controller itself is real-guest proven: after the COM3
host open, `C31.COM` writes the original COM3 8250 MCR loopback bit and
round-trips `5A` through direct TX/RX port I/O
(`S30_COM3_LOOPBACK_TX_RX_OK`). This verifies selected SoftPC serial TX/RX
without transmitting to an unknown external protocol.

The discovered `COM3` is a Windows `usbser` USB-C Billboard endpoint. Its
real-open probe passes, but original INT 14h transmit correctly times out:
the unchanged `rs232_io.c` waits for UART CTS before it calls the original
host write path, and this endpoint does not assert CTS. This is a real
medium capability condition, not a missing NTVDM provider or a reason to
weaken the original CTS rule. Complete COM transmit/receive acceptance needs
a CTS-capable loopback or paired serial endpoint. The host has no persistent
physical/network LPT endpoint, but the original LPT `CreateFile("LPT1")` path
is separately verified below through a temporary Windows device mapping that
is removed after the test.

The host inventory confirms that this is not an unsearched Windows facility:
`HKLM\\HARDWARE\\DEVICEMAP\\SERIALCOMM` contains only
`\\Device\\USBSER000 = COM3`; PnP reports only the started `usbser.inf`
USB Serial Device (COM3). A native non-writing open reports `cts=False`,
`dsr=False` and no received bytes. Thus no installed physical or virtual
paired COM medium can complete the original CTS-gated external transmit or
host-driven receive acceptance.

The deferred prerequisite is registered in the [debt ledger](TODO.md) with
its original caller/provider, zero retained product diff and the exact retest
condition.  The supporting [external COM boundary record](../etc/evidence/m0-t420-s30-external-com-boundary.md)
retains the full current-host audit; the unavailable medium is not a pass.

To avoid leaving the provider's success route untested merely because this
host lacks a peer, a test-only direct-source mock now compiles unchanged
`nt_com.c` and supplies a controlled CTS-capable Win32 serial contract. Its
`open → write → receive queue → read → close` lifecycle passes with
`T420_S30_NT_COM_MOCK_OPEN_WRITE_READ_CLOSE_OK`. This fixture is not linked
into the product and does not close the real external-medium debt.

The original LPT write path is now independently end-to-end proven without a
product substitute. The test observer temporarily mapped the Windows `LPT1`
DOS-device name to a private named pipe, then removed that mapping after the
run. `L30.COM` invoked INT 17h byte output; unchanged `nt_lpt.c` opened
`LPT1`, buffered it, and flushed during worker close. The pipe received `5A`
and the guest reported `S30_LPT1_WRITE_OK`; no mapping, pipe, or product
process remained afterward.

An independent post-closure `C31.COM` ConPTY rerun also completed normally:
the launcher returned `exit=0`, emitted `S30_COM3_LOOPBACK_TX_RX_OK`, and
had no guest command-resolution failure. The earlier aggregate-run timeout
is therefore not a reproducible worker-exit defect.

The owner's clarification applies to the entire T420 capability sequence:
every capability that the selected standalone NTVDM product itself owns must
be verified through its original guest-to-host path before its owning S can
close. A missing external medium is recorded as a platform limitation, never
silently converted into a pass; the remaining S31--S43 rows retain the same
per-package acceptance rule.

## S30 Reopened Closure

The owner returned to S30 and its reopened matrix passed again: DOSX/HIMEM,
pure-DOS, EMS, COM3 controller
loopback, LPT1 byte output and the isolated original `nt_com.c` success mock.
The Windows serial map remains only `\Device\USBSER000 = COM3`, so the
recorded absence of a CTS-capable external peer is unchanged and may not be
turned into a product-side substitute. See the S30 closure evidence's
2026-09-18 reopened-verification record.

## S31 Closure Record

The complete original eight-source debug manifest is byte-identical to pinned
OpenNT and selected into the formal archive. `/DPROD` excludes all interactive
debug/trace implementations; original `force_yoda` resolves to the CCPU stub.
The sole source-proven release fault was `CCPU_SHOW_EXCEPTIONS`: it could send
the product into `fprintf(NULL)` after the original production profile left
`trace_file` unbound. DIV-271 makes only that non-product environment switch
inert. Formal x86, 27 current-source exception cases, an environment-enabled
real `FAULT.EXE`, and all 17 text-gated COMMAND/MEM/EDIT routes passed.

[T420 S31 debug capability closure](../etc/evidence/m0-t420-s31-debug-capability-closure.md)

## S32 Closure Record

[T420 S32 host capability closure](../etc/evidence/m0-t420-s32-host-capability-closure.md)
closes all 47 selected original `softpc.new/host/src` units. Twenty are
byte-identical; every remaining difference is a registered finite x86 ABI,
CCPU40 execution, worker-local platform-binding or public-Console/lifecycle
carrier. The retirement removes 40 mirror-side observation/unused-carrier
lines from `nt_bop.c` and `nt_graph.c`, leaving no new worker shell or
adapter lifecycle. Fresh formal x86 lifecycle proof and all 17 transcript-
gated COMMAND/MEM/EDIT product routes passed from matching `O:\winnt`
artifacts.

## S33 Closure Record

[T420 S33 DEM file capability closure](../etc/evidence/m0-t420-s33-dem-capability-closure.md)
closes the selected original normal file-operation family through direct DOS
guest calls: create/write/commit/open/read/seek/share/lock/find/rename/FCB,
attribute and directory services all pass. Missing-file and host sharing
failures retain their observed original DOS mappings (`AX=2` and `AX=5`), and
all fixture files/directories are removed. Existing S28 no-media evidence
remains the explicit raw-media/IOCTL boundary; S33 adds no provider or
product implementation.

## S1 Closure Record

[T420 S1 CCPU386 package audit](../etc/evidence/m0-t420-s1-ccpu386-package-audit.md)
was delivered at `2d128cbaa`. It freezes the complete CCPU manifest and its
C-VID state boundary; S2 owns that next package without reopening CCPU
semantics.

## S2 Closure Record

[T420 S2 C-VID package recovery](../etc/evidence/m0-t420-s2-cvidc-package-recovery.md)
closed at the T419-integrated formal product revision. It records all 39
selected C-VID sources, one `Gdp` allocation owner, the profile-null vector
dispositions, focused vector/map proof, the final four-executable x86 build,
and deployed `COMMAND/MEM/EDIT` regression. T420 may next admit S3 (`video`)
only on a new owner request.

## S3 Closure Record

[T420 S3 video package recovery](../etc/evidence/m0-t420-s3-video-package-recovery.md)
closes all 19 selected original video sources. It records the retained CCPU40
and C-VID ABI declarations, the finite worker-local public-Console boundary,
fresh four-program x86 linkage, C-VID contract fixture, six Console geometry
observations and direct/interactive COMMAND/MEM/EDIT regression.

## S4 Closure Record

[T420 S4 system package recovery](../etc/evidence/m0-t420-s4-system-package-recovery.md)
closes the eleven-unit system manifest: 41 mirror-side diagnostic lines were
removed, nine units now match the pinned OpenNT source exactly, and the two
remaining source-first boundaries are the relocated public-header include and
the original ROM-residency body selection. Fresh x86 product linkage,
VdmTib storage audit, HALT/RESET fixture, and deployed COMMAND/MEM/EDIT plus
Console geometry regression passed.

## S5 Closure Record

[T420 S5 keymouse package recovery](../etc/evidence/m0-t420-s5-keymouse-package-recovery.md)
closes all five selected keyboard/mouse units.  It removes 86 mirror-side
diagnostic lines while retaining and proving the only three finite source-first
CPU40/guest-memory boundaries.  Fresh x86 linkage, deployed COMMAND/MEM/EDIT,
and the Console mouse/geometry plus five short-window EDIT→MEM matrix passed.

## S6 Closure Record

[T420 S6 BIOS package recovery](../etc/evidence/m0-t420-s6-bios-package-recovery.md)
closes all fourteen selected BIOS units.  It removes 13 EMS-only mirror
observation lines, leaving ten selected units byte-identical to pinned OpenNT
and three finite source-first declaration/CPU40-continuation boundaries.
Fresh x86 linkage, VdmTib ownership audit, deployed direct/interactive
COMMAND/MEM/EDIT, six Console geometry modes, mouse move/click/release and
five short-window EDIT→MEM runs passed.

## S7 Closure Record

[T420 S7 support package recovery](../etc/evidence/m0-t420-s7-support-package-recovery.md)
closes all six selected support units.  It removes 13 phase-only mirror
observations; four units remain byte-identical to pinned OpenNT, while the
two finite remaining source-profile boundaries retain CCPU40 single-state
ownership and original C-VID/CPU startup ordering.  Fresh x86 linkage,
focused CCPU/C-VID contracts, deployed COMMAND/MEM/EDIT, Console geometry,
mouse and five short-window EDIT→MEM runs passed.

## S8 Closure Record

[T420 S8 disks package recovery](../etc/evidence/m0-t420-s8-disks-package-recovery.md)
closes all ten selected disks units as byte-exact pinned OpenNT. Fresh x86
linkage, deployed direct/interactive COMMAND/MEM/EDIT, Console geometry,
mouse and five short-window EDIT→MEM runs passed. The preceding S8 P restored
all proven S1--S8 format-only drift and made that audit mandatory for later S
closures.

## S9 Closure Record

[T420 S9 comms package recovery](../etc/evidence/m0-t420-s9-comms-package-recovery.md)
closes all five selected communications units as byte-exact pinned OpenNT.
Fresh x86 linkage, deployed direct/interactive COMMAND/MEM/EDIT, Console
geometry, mouse and five short-window EDIT→MEM runs passed.

## S10 Closure Record

[T420 S10 DOS package recovery](../etc/evidence/m0-t420-s10-dos-package-recovery.md)
closes the one-unit DOS manifest. It restores its pinned OpenNT line endings,
retains only the allocator declaration and original CCPU40 LIM-writeback
selection, and records direct x86 compilation plus freshly linked four-EXE
and deployed COMMAND/Console regression.

## S11 Closure Record

[T420 S11 debug package recovery](../etc/evidence/m0-t420-s11-debug-package-recovery.md)
closes all eight selected original SoftPC debug units, restores `trace.c` to
upstream formatting, and records x86 compilation, fresh product linkage and
deployed regression.

## S12 Closure Record

[T420 S12 host package recovery](../etc/evidence/m0-t420-s12-host-package-recovery.md)
closes the selected original host package: six source files are restored to
pinned OpenNT bytes, `stubs.c` retains only its established VDM_TIB storage
binding, and the owner-supplied BaseSrv grace constant is included in the
fresh x86 four-program and deployed regression evidence.

## S13 Closure Record

[T420 S13 DEM package recovery](../etc/evidence/m0-t420-s13-dem-and-residual-diff-recovery.md)
closes the DEM package and its source-first residual boundary ledger.

## S14 Closure Record

[T420 S14 COMMAND package recovery](../etc/evidence/m0-t420-s14-command-package-recovery.md)
closes COMMAND as a source-first package.  It removes both discovered empty
x86 pointer/handle carriers, preserves the necessary guest-memory, async and
cross-process resource bindings, passes fresh formal x86 linkage and the
owner-accepted direct/interactive COMMAND, MEM and EDIT route.

## S15 Closure Record

[T420 S15 XMS package recovery](../etc/evidence/m0-t420-s15-xms-package-recovery.md)
closes all eight selected XMS units. Six now match the pinned OpenNT sources;
the two remaining deltas are the source-proven `xmsMoveMemory` callback ABI
and a bounded CPU40 guest-memory lease in the original move-block path.
Focused allocation/mapping tests, fresh x86 linkage, and deployed direct
`MEM` plus `COMMAND /C MEM` passed.

## S16 Closure Record

[T420 S16 SubAlloc package recovery](../etc/evidence/m0-t420-s16-suballoc-package-recovery.md)
closes the one-unit production manifest with zero allocator-body diff and
zero product adapter additions.  The retained x86-granularity header condition
is source-proven; focused allocation/relocation/release, fresh formal x86,
and deployed direct `MEM` plus `COMMAND /C MEM` passed.

## S17 Closure Record

[T420 S17 OEMUNI package recovery](../etc/evidence/m0-t420-s17-oemuni-package-recovery.md)
closes both selected OEM/Unicode sources. It retains the original conversion
owner, adds only deterministic local failure sentinels under DIV-269, passes
the original conversion suite and focused failure proof, fresh formal x86,
and deployed Console-attached MEM regressions.

## S18 Closure Record

[T420 S18 DPMI32 package recovery](../etc/evidence/m0-t420-s18-dpmi32-package-recovery.md)
closes all fifteen selected DPMI32 bodies and their reached header surface. It
removes every proven inert source drift, retains only classified CPU40 versus
kernel-VDM carriers, passes a fresh x86 product build and descriptor-domain
fixture, and deploys direct MEM plus COMMAND /C MEM proof.

## S19 Closure Record

[T420 S19 guest DPMI/DOSX package recovery](../etc/evidence/m0-t420-s19-dpmi-guest-package-recovery.md)
closes the full load-only DOSX package with all OpenNT sources exact, all
OpenNT-src-2 intermediate products provenance-matched and the deployed DOSX
hash verified. It proves original BOP 53 activation/return ownership, passes
fresh x86 linkage and deployed direct/COMMAND MEM proof.

## S20 Closure Record

[T420 S20 VDMREDIR package recovery](../etc/evidence/m0-t420-s20-vdmredir-package-recovery.md)
closes the full selected redirector DLL package. The matching formal x86
deployment passes all selected real DOS-guest named-pipe, mailslot, local
NetAPI, NetBIOS, BOP 7 and lifecycle families plus `COMMAND -> MEM -> EDIT ->
MEM`. Remote RAP and DLC are explicit owner-approved modern-host exclusions;
the DLC guest probe preserves the original `07` unavailable result rather
than adding a provider or a fake-success shim.

## S21 Closure Record

[T420 S21 CCPU386 capability closure](../etc/evidence/m0-t420-s21-guest-console-witness.md)
closes the selected CCPU40 capability unit at `31cbee1f0`. Its current formal
x86 deployment passes 21 real guest instruction/FPU/exception/event witnesses
plus the one source-shaped CPL3 HLT boundary, descriptor-domain ownership,
normal-and-abnormal real CCPU worker lifetime, the complete normalized
residual-diff ledger, and all 17 Console-text-gated COMMAND/MEM/EDIT routes.
Protected DPMI callback-provider lifecycle remains owned by S38; S21 adds no
duplicate CCPU substitute.

## S22 Closure Record

[T420 S22 C-VID capability baseline](../etc/evidence/m0-t420-s22-cvidc-capability-baseline.md)
closes the selected C-VID publication unit at `23558cadd`. It accounts for all
209 CPU-table slots (180 original providers, three finite timing endpoints
and 26 source-profile nulls), proves 38 typed accessors and all 81 table-slot
selection families, preserves the one original `Gdp` and quick-event restart
owners, and shows that all nine historical short-layout consumers are clean
under the current formal x86 flags. The freshly linked formal package matches
the deployed five-artifact package and passes six Console geometry/mouse/
resize witnesses, five short-window `EDIT -> MEM` sequences, and all 17
Console-text-gated COMMAND/MEM/EDIT routes. Actual video writer workload is
now S23's explicit owner; no C-VID duplicate is introduced.

## S23 Closure Record

[T420 S23 video capability closure](../etc/evidence/m0-t420-s23-video-capability-baseline.md)
closes the selected original video capability within the public-Console
boundary.  Real guest fixtures prove original INT 10 write/scroll, direct
`B800:` C-VID text read/write/presentation and direct mode-13 `A000:` graphics
read/write with text recovery.  The full six-row geometry/mouse/resize matrix
and five short-window EDIT-to-MEM repetitions pass from raw guest transcripts.
Graphics pixel presentation is explicitly not fabricated: the source/graph
gate proves the selected providers exist, the public Console is text-only and
no graphics sink is installed; queued `kvm-window` exclusively owns that
future worker-local presentation capability.

## S24 Closure Record

[T420 S24 system capability baseline](../etc/evidence/m0-t420-s24-system-capability-baseline.md)
closes the complete selected system unit. It proves real guest timer/PIC,
CMOS/RTC-periodic, DMA-port and ROM paths; source-proves the no-media DMA
transfer profile-null and the intentionally destructive failure endpoints; and
records a fresh formal x86 link/deployment. The direct interactive
`COMMAND -> MEM -> EDIT -> MEM` regression is gated on the guest transcript,
including both MEM reports and absence of command-resolution failure.

## S25 Closure Record

[T420 S25 keymouse capability closure](../etc/evidence/m0-t420-s25-keymouse-capability-baseline.md)
closes the complete selected keymouse capability unit.  It adds no production
code: three formal-package guest runs prove PPI, keyboard/modifier, INT 33
callback move/down/up and disable teardown, followed by deployed COMMAND/MEM/
EDIT regressions.

## S26 Closure Record

[T420 S26 BIOS capability closure](../etc/evidence/m0-t420-s26-bios-capability-baseline.md)
closes all fourteen selected BIOS bodies: safe real guest INT 11h/12h/15h,
Build-ID and control-BOP services pass; RTC is corroborated by S24; EMS is a
source-proven PIF-zero profile-null; error/IRQ/VDD endpoints retain their
original non-normal disposition.  Fresh formal x86 linkage, deployed hashes
and all 17 established COMMAND/MEM/EDIT routes passed.

## S27 Closure Record

[T420 S27 support capability closure](../etc/evidence/m0-t420-s27-support-capability-closure.md)
closes all six selected support bodies: real DOS time/RTC/IOS acceptance,
source-proven excluded PIG environment consumer, original startup/termination
ownership and formal deployed COMMAND/MEM/EDIT regression.  The sole product
change is none; the test harness now uses the declared runtime test directory
instead of a cross-process `SUBST` assumption.

## S28 Closure Record

[T420 S28 disks capability closure](../etc/evidence/m0-t420-s28-disks-capability-closure.md)
closes all ten selected original disks bodies under the original
no-attached-media profile.  It adds no product implementation or virtual
medium: real original startup, controller reset and worker teardown pass;
disk transfer and non-selected floppy providers are source-proven profile
exclusions.  Fresh five-artifact x86 deployment and all 17 COMMAND/MEM/EDIT
transcript-gated routes pass.

## S29 Closure Record

[T420 S29 communications capability closure](../etc/evidence/m0-t420-s29-comms-capability-closure.md)
closes all five selected original comms bodies under the original no-device-
medium profile.  It adds no serial/printer provider: real INT 14h failure and
Ignore recovery plus INT 17h initialization/status pass, all 526 x86 build
edges and the 17 established COMMAND/MEM/EDIT routes pass, and worker cleanup
leaves no product process.

## S30 Closure Record

[T420 S30 DOS profile and host-medium closure](../etc/evidence/m0-t420-s30-dos-profile-host-medium-closure.md)
closes default DOS, HIMEM/DOSX, pure-DOS PIF and EMS PIF acceptance; original
COM3 controller loopback; and original LPT1 byte output through a removed
test-only DOS-device mapping.  The unchanged `nt_com.c` success lifecycle is
also covered by a non-product direct-source mock.  The only deferred result is
real external COM peer transmit/receive: this host has no CTS-capable peer,
so its original failure behavior and a concrete retest condition remain in
the debt ledger.  Formal five-artifact x86 and all 17 established
COMMAND/MEM/EDIT routes passed.

## Current Technical Baseline

- Product package: Win32/x86 CCPU40 `run16.exe`, `basesrv.exe`, `ntvdm.exe`,
  and `dtmgr.exe`; latest selected artifacts are in `O:\winnt`.
- T419 is closed with its owner-accepted post-closure DTMgr record-projection
  correction: four EXE component directories are explicit, `dtmgr.exe`
  replaces `dtaskmgr.exe`, and the task display derives depth from original
  BaseSrv DOS records rather than a parallel stack.
- T420 S1--S32 are delivered: CCPU386, C-VID, video, system, keymouse, BIOS,
  support, disks, comms, DOS, debug, host and DEM are closed complete package
  units.  S14 COMMAND restored two empty x86 adapter carriers to their
  original OpenNT forms; S15 closes XMS, S16 closes SubAlloc, S17 closes
  OEMUNI, S18 closes DPMI32, S19 closes guest DPMI/DOSX, S20 closes VDMREDIR,
and S21 closes CCPU386 capability acceptance. S22 closes C-VID publication
capability; S23 closes video capability within the public-Console boundary;
S24 closes system capability within the selected no-floppy-media profile; S28
closes the selected original disks capability under the corresponding
no-attached-media profile.

## Prior Closure

[T419 Terminal task-manager closure](../history/m0-t419-terminal-task-manager-closure.md)
records the accepted task-manager delivery and its evidence boundary.

## Recent M0 Closures

| T419 | [Terminal task manager](../history/m0-t419-terminal-task-manager-closure.md) | Delivered `dtmgr.exe`, authenticated selected-worker management, compact first-option compatibility, and explicit executable component roots. |

## Recent Governance

No standalone governance delivery is active or pending.
