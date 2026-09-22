# Project Status

## Current Work

## Active Packet

**Active: M0 T420 S45**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T420 S45, Ordinary Mode, single implementer/reviewer. |
| Candidate Proposal | [Runtime package completion](../proposals/proposal-mvdm-runtime-package-completion-001.md), S45 static-registry receiver matrix and final non-WOW32 acceptance. |
| Admission And Approval | Owner authorizes automatic sequential admission through the remaining non-WOW T420 packets unless manually interrupted. S44 is delivered; S45 begins with complete receiver audit before implementation. |
| Objective | Replace each selected original static registry read with one worker-local immutable `NTVDM.REG` same-shaped read provider, then complete final non-WOW32 reconciliation and acceptance. |
| Non-goals | No Windows registry access, mutable WOW registry emulation, WOW32/WRITE implementation, guest mutation, CPU30, private CSR server, or compile-only acceptance. |
| Reference Baseline | [S44 VDD ledger](../etc/evidence/m0-t420-s44-vdd-capability-ledger.md), S45 receiver matrix, pinned original callers and fresh x86 DOS regression. |
| Files And ABI Surface | Selected original static registry callers, worker-local shadow provider, source manifests, controlled configuration files and production-path tests. |
| Applicable Rules | Source-first complete package, finite public-host boundaries, immutable guest, exact mirror formatting, one active S. |
| Verification | Freeze every static reader; test configured, absent and malformed input through its original caller; distinguish hardware discovery and mutable API families; then fresh x86 and all 17 DOS routes. |
| Expected Markers | Every selected static caller observes a same-shaped configured read or its original fallback/failure; no formal product import reaches Windows registry APIs. |
| Asset Needs | Pinned OpenNT, controlled `NTVDM.REG` files and disposable probes; build/M0-T420/S45, O:/winnt/tests and O:/winnt/logs. |
| Reporting Requirements | Receiver/capability matrix, source/adapter ownership, configured/absent/malformed evidence, registry-import sweep, exact source/artifact hashes and mirror/non-mirror line accounting. |
| Stop Conditions | Guest modification, Windows registry access, recursive private server import, unexplained DOS regression or treating mutable/discovery behavior as a static file read. |
| Exit Criteria | Full selected receiver disposition, final non-WOW32 reconciliation, x86/regression/source gates, clean reviewed commit/push. Unavailable capabilities are not passing tests. |
| Original Owner Request | Finish the remaining non-WOW32 work in S44--S45 after moving WOW32 into the queue-head successor. |
| Similar-Issue Sweep | Every formal `Reg*`/`Nt*Key` consumer, static versus discovery versus mutable classification, source-shaped errors and worker teardown. |

## S44 Closure Record

The [S44 VDD ledger](../etc/evidence/m0-t420-s44-vdd-capability-ledger.md)
records complete selected VDD closure. A controlled provider reaches the
original load path from immutable package-local configuration, BOP 58 dispatch,
user/I/O hooks, memory/IRQ/DMA cycles, normal callback cleanup and precise
worker-loss reload. `VDDTerminateVDM` is separately source-proven to retain
the original non-WOW zero exit. The sole VxD instance-data boundary remains
explicitly unavailable rather than a false successful no-op. Fresh x86 and
all 17 established DOS routes pass; guest media remain unchanged.

## S43 Closure Record

The [S43 debugger capability ledger](../etc/evidence/m0-t420-s43-debugger-capability-ledger.md)
records that S43 replaces the retired local debugger implementation with the byte-identical
original `mvdm/dbg/dbg.c`, retaining only a bounded six-DWORD CCPU40
debug-register input copy and a dynamic original `DbgPrompt` binding.  Real
Windows DebugPort evidence proves original DOSX BOP 6 `DBG_SEGLOAD` plus DEM
CP437 OEM `MEM` module load/free notifications.  The source matrix transfers
WOW-only producer families to the WOW32 successor and records all otherwise
unproduced original no-op/default modes without inventing a replacement path.
Formal x86 product and focused debugger/DPMI/OEM fixtures pass; the deployed
package passes all 17 established DOS routes.  Guest media remain unchanged.

## S42 Closure Record

The [S42 handoff](../etc/evidence/m0-t420-s42-wow32-successor-handoff.md)
records owner-revised research/code preservation closure: fresh x86 four-EXE
and two-DLL builds, domain/layout/cleanup/source checks and all 17 text-gated
DOS routes pass. The same delivery preserves all outstanding code/research
and the queue-head successor proposal. Its commit/push is the S42 handoff P;
WOW32 desktop publication, production task teardown and WRITE remain unaccepted
and explicitly transferred. No guest media changed.

## WOW32 successor transfer

The [successor proposal](../proposals/proposal-wow32-complete-runtime-recovery-001.md)
inherits all unfinished WOW32 and WOW16/WRITE work. Historical receiver numbers
in evidence remain chronology; the new proposal and current T420 plan own
the live assignments. T420 S43 is debugger including OEM-DBG-PATH and S44 is
VDD; S45 is the admitted final `NTVDM.REG` shadow-registry completion plus
non-WOW32 acceptance. T420 remains open for owner review.
S42 handoff checks pass; the USER desktop failure remains unfixed and is
assigned to successor S1 audit and S2 implementation. S45 is the sole
admitted packet.

## S40 Closure Record

S40 reaches its owner-revised research/planning conclusion, not a runtime
capability pass. The [closure record](../etc/evidence/m0-t420-s40-wow32-research-replan.md)
assigns every earlier W1--W7 obligation and inherited implementation candidate.
The [proposal](../proposals/proposal-mvdm-runtime-package-completion-001.md)
owns the historical 48-S sequence (now superseded by the WOW32 transfer); the [checklist](../etc/operations/m0-t420-s40-wow32-closure-checklist.md)
retains detailed obligations with the new receivers.

The current source still calls modern UserRegisterWowHandlers. The recorded
native probe returns C0000022 and zero output slots; compiled recovered USER
bodies are not a complete registered provider. The pinned USER.EXE consumes
PMODE32 shared data directly. The exact live paging route remains to be
established: historical VCPI routines alone do not prove active DOSX paging.
S41 starts with this check before choosing a mapping implementation.

S40 research documentation passes governance, relative-link and pinned USER
profile checks; its delivery is the S40 research/replan documentation commit.
The extensive pre-existing source/test/build
changes remain preserved in the worktree and are explicitly unaccepted;
S42 now preserves them in its handoff delivery. Their remaining production
adoption/removal belongs to the WOW32 successor, not to T420's non-WOW32 tail.

## S41 Closure Record

[T420 S41 WOW registration frontier](../etc/evidence/m0-t420-s41-wow-registration-frontier.md)
records delivery `801ec6e39`: one CCPU worker domain, original DOSX page-table
entry, map/withdrawal evidence and the established 17-route regression. S42
inherits that published worker domain; it does not reopen S41's data-plane
closure.

## S38 Closure Record

Delivery `7b888b1a4` records S38 completion. The
[DPMI32 evidence](../etc/evidence/m0-t420-s38-dpmi32-capability-review.md)
retains the selected-package tests and original-guest limitations; its older
progress entries are chronology, not new S40 acceptance.

## S39 Closure Record

Delivery `90c873d44` records the
[DOSX capability acceptance](../etc/evidence/m0-t420-s39-dpmi-guest-capability-review.md),
including the 17-route product regression. Neither delivery proves WOW32.

## S37 Closure Record

The [OEMUNI evidence](../etc/evidence/m0-t420-s37-oemuni-capability-review.md)
closes the selected 28 compiled interfaces and real DOS/PIF workloads after
product delivery 6b80471d9. Formal x86, ten native fixtures, real OEM direct/
nested and ASCII/non-ASCII detached PIF probes, and all 17 established product
regressions pass. Guest media remain immutable. CP932 is mocked boundary
evidence, not a claim of real DBCS-host or full Win16 acceptance.

The owner explicitly transfers remaining real consumer acceptance to the
[proposal's mandatory receiver matrix](../proposals/proposal-mvdm-runtime-package-completion-001.md#mandatory-oemuni-consumer-acceptance-transferred-from-s37):
the WOW32 successor S6/S8 owns OEM-WOW-DIR/DELETE and
OEM-WIN16-INTEGRATION; T420 S43 owns OEM-DBG-PATH. These items are unverified, not excluded or passed;
each receiver must name them in its admission and closure evidence.

Final S37 mirror change versus admission is +192/-61 across six files;
existing non-mirror production bindings change +72/-3, with no new overlay.
These are corrective additions, not diff reduction. The evidence retains
source-first rationale, failure controls, exact hashes and the earlier S30
pure-profile false-positive correction. S37 does not reopen that immutable
guest limitation or conceal it as a passing profile.

## S36 Closure Record

The [S36 evidence](../etc/evidence/m0-t420-s36-suballoc-capability-review.md)
closes the selected original suballocator capability. Real XMS/DPMI forced
movement, failed growth preserving data, explicit free, same-worker implicit
task cleanup and broker-loss exit pass, alongside original-source branch,
fragmentation/exhaustion/coalescing fixtures and all 17 product regressions.
Original SAS callbacks replace local clearing; the allocator body remains
byte-exact. Required repairs cover explicit protected addressing, original
FLAGS restoration, segment indices and DPMI BOOL failure propagation. Guest
media are untouched. Dormant upstream rollback defects for a future fallible
provider remain in TODO, not misrepresented as repaired. S36 has no remaining
selected-provider acceptance gap; S37 does not inherit its implementation work.

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

## Retained S30 status context

[Historical status excerpts](../etc/evidence/m0-t420-prior-status-records.md)
preserve the S30 reopen chronology and corrected profile evidence; the
[S30 capability record](../etc/evidence/m0-t420-s30-dos-profile-host-medium-closure.md)
remains its evidence reference.

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
