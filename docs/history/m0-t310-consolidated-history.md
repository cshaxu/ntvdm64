# m0-t310 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t310-s15-xms-owner-package-closure-20260830.md

# M0 T310 S15 closure — XMS.486 owner package

## Result

S15 closed the selected original XMS.486 package at the source-owner boundary
for the sole Win32/x86 CCPU40 profile. The result is deliberately narrower
than a guest runtime claim: the original XMS callers need the later SoftPC
firmware/system lifecycle and cannot be validly exercised through the removed
Bochs production route.

## Accepted evidence

- [P1 source closure](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s15-p1-xms-source-closure-001.md)
- [P2 CCPU40 profile boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s15-p2-x86-profile-boundary-001.md)
- [P3 original-owner assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s15-p3-xms-original-owner-assembly-001.md)
- [P4 contract dispositions](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s15-p4-xms-contract-disposition-001.md)
- [P5 package closure](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s15-p5-xms-package-closure-001.md)

## Deferred integration condition

The final T310 selected SoftPC lifecycle must later prove the original
`nt_msscs -> XMSInit` and `nt_bop -> XMSDispatch` routes after the relevant
firmware and controller owner packages close. This record neither replaces
that condition nor authorizes a Bochs fallback.


## m0-t310-s16-bios-firmware-bindings-closure-20260830.md

# M0 T310 S16 closure — BIOS / firmware bindings

## Result

S16 closes the selected original SoftPC BIOS/firmware package at the
source-owner boundary.  The complete BIOS manifest, original ROM/CMOS resource
route, app-selected firmware location binding, CMOS/profile write-root rule
and first-session system-file policy have one recorded disposition.

## Accepted evidence

- [P1 firmware owner audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p1-bios-firmware-owner-audit-001.md)
- [P2 formal assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p2-bios-firmware-formal-assembly-001.md)
- [P3 manifest owner ledger](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p3-bios-manifest-owner-ledger-001.tsv)
- [P3 reset/dispatch boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p3-bios-reset-dispatch-boundary-001.md)
- [P4 resource-write root](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p4-firmware-resource-write-root-001.md)
- [P5 startup system-file disposition](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p5-startup-system-file-disposition-001.md)
- [P6 final disposition](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s16-p6-bios-firmware-final-disposition-001.md)

## Deferred integration condition

S16 does not enable reset or BIOS dispatch by itself.  The original source
order transfers controller work to S17–S31.  S49 must later validate the
integrated selected SoftPC lifecycle without a Bochs fallback or fabricated
firmware success.


## m0-t310-s17-system-controller-callbacks-closure-20260830.md

# M0 T310 S17 closure — system controller callbacks

## Result

S17 closes the selected original system-controller source package boundary:
PIT, PIC/ICA, CMOS/RTC, DMA, quick-event and their selected host callback
forms remain original bodies or already-registered same-shaped bindings.
Every adjacent source has a single S17, later-owner or unselected disposition.

## Accepted evidence

- [P1 source boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s17-p1-system-controller-source-boundary-001.md)
- [P1 owner ledger](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s17-p1-system-controller-owner-ledger-001.tsv)
- [P2 contract dispositions](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s17-p2-system-controller-contract-disposition-001.tsv)
- [P2 formal assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s17-p2-system-controller-formal-assembly-001.md)

## Deferred integration condition

The original reset sequence and actual timer/IRQ delivery require later
endpoint and host binding packages.  S49 must validate their integrated
selected SoftPC behavior; this closure authorizes neither a Bochs fallback nor
a synthetic PIC/PIT success result.


## m0-t310-s18-host-system-interrupt-bindings-closure-20260830.md

# M0 T310 S18 closure — host system interrupt bindings

S18 closes the selected host-system interrupt source boundary. Original
heartbeat, public thread/event/wait/timing calls, local CCPU registration and
the sole same-shaped thread-alert binding have one disposition. Runtime IRQ
delivery remains S49 integration work.

- [boundary ledger](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s18-p1-host-system-interrupt-boundary-001.tsv)
- [formal assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s18-p1-host-system-interrupt-formal-assembly-001.md)


## m0-t310-s19-input-controller-callbacks-closure-20260830.md

# M0 T310 S19 closure — input controller callbacks

S19 closes the original SoftPC input-controller callback boundary for the
selected Win32/x86 CCPU40 profile. Keyboard initialization retains its
original callback assignments and EOI hook; INT 15h retains IVT validation
before its original BIOS table call; the mouse dispatcher retains its complete
range-checked 53-entry table and original unavailable branch.

The only retained mirror corrections are the paired scalar-pointer contract
and private SoftPC RAM-backing correction. Neither introduces an adapter,
mapping-manager identity, BOP route or host-input policy. Public host-console
input is deliberately S20 work and display/fullscreen behavior remains S27.
Integrated keyboard/mouse delivery is mandatory S49 validation, not claimed
by this source-boundary closure.

- [boundary audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s19-p1-input-controller-contract-boundary-001.md)
- [x86 assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s19-p2-input-controller-x86-assembly-001.md)
- [contract disposition](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s19-p3-input-controller-contract-disposition-001.tsv)


## m0-t310-s20-host-input-bindings-closure-20260830.md

# M0 T310 S20 — host-input bindings closure

S20 closed the selected original SoftPC host-input binding cluster for the
Win32/x86 CCPU40 recovery profile.

- `nt_keycd.c` remains byte-exact OpenNT source; its fixed-buffer RTL call
  uses the existing same-shaped public Win32 binding.
- `nt_event.c` now has same-shaped bindings for its original cdecl console
  wait handle, input record read/write, cursor sign and reached icon query.
- `nt_mouse.c` receives only its reached client-rectangle/client-to-screen
  coordinate binding.  System-menu, fullscreen and other display selectors
  are explicit S27 unavailable work.
- The obsolete non-width `key_history_count` type insertion was removed; the
  mirror again has the original spelling and the selected `/W4` x86 build
  keeps its warning visible.
- A selected CCPU40 formal archive and forced-link audit resolve all five S20
  input symbols.  The focused public failure-contract fixture passes.

This is source/binding closure, not a claim of live keyboard/mouse device
delivery.  That integration remains mandatory in S49.


## m0-t310-s21-storage-controller-callbacks-closure-20260830.md

# M0 T310 S21 closure — storage controller callbacks

S21 closes the original SoftPC storage-controller source contract for the
selected Win32/x86 CCPU40 profile.

- The original INT 13, WD2010, FDC, remote-floppy-client and EMS controller
  bodies remain selected mirror sources; no controller logic was rewritten.
- Callback tables and their declarations, initializers and indirect calls were
  reviewed together. No source ABI repair was necessary.
- One earlier mirror guard incorrectly suppressed original `host_delays` under
  CCPU. S21 restores that independent timing global while retaining the
  registered guard only for duplicate SAS-memory globals.
- The fresh 358-edge forced-link candidate contains no unresolved
  `original-softpc-disks.lib` entry. The remaining unresolved entries are
  named later owner packages, including S22 host-storage endpoints.

This is source and formal-assembly closure only. It does not claim that a
guest disk, floppy or EMS workload has executed. S22 owns host-media binding;
S49 remains the mandatory integrated machine verification.

Evidence: [source boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s21-p1-storage-controller-source-boundary-001.md),
[call contract](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s21-p2-storage-controller-call-contract-001.md), and
[formal assembly](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s21-p3-storage-controller-formal-assembly-001.md).


## m0-t310-s22-host-storage-bindings-closure-20260830.md

# M0 T310 S22 — host-storage bindings closure

S22 closed the selected Win32/x86 CCPU40 host-storage binding cluster.

- The original no-fixed-disk CCPU callbacks retain their zero-transfer
  failure direction; they were not redirected into DEM direct DASD.
- Original `nt_fdisk` and `nt_rflop` bodies retain their operation order.
  The existing OpenNT-shaped NT I/O facade and synchronous current-PDB lease
  are the only reached compatibility bindings.
- The historical `SLAVEPC` remote floppy protocol is not compiled in the
  selected profile and has no invented replacement.
- A clean S22 formal graph completed 369 selected compile/archive edges; the
  forced-link audit has no S22 owner unresolved symbol. A safe impossible-name
  fixture verified a non-success NT file-open result without touching media.

This is not guest-media execution proof. Real fixed/removable media integration
and machine lifecycle remain S49 acceptance work. The next sequential owner
cluster is S23, original communications controller callbacks.


## m0-t310-s23-comms-controller-callbacks-closure-20260830.md

# M0 T310 S23 — communications-controller callbacks closure

## Closed scope

S23 closes `SPC-COMMS-CONTROLLER-CALLBACKS` for the selected Win32/x86 CCPU40
SoftPC profile.  It covers the original serial UART controller, its INT 14
front end, printer/parallel/screen-print source selection, reset callers and
the existing host-local ioctl transport divergence.

## Evidence and result

- The [source boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s23-p1-comms-controller-source-boundary-001.md)
  records every selected controller and endpoint direction.
- The [formal assembly evidence](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s23-p2-comms-controller-formal-assembly-001.md)
  records a fresh 369-edge x86 CCPU40 graph, all five original communications
  objects in `original-softpc-comms.lib`, and no communications-owner
  unresolved forced-link symbol.
- Printer/parallel/screen-print behavior is retained as exact original
  `PRINTER`-unselected unavailability.  S23 did not fabricate a port response
  or activate a host endpoint.
- `MVDM-HOST-DIV-068` remains the sole relevant registered mirror change: it
  preserves original host ioctl request/result ordering through native-width
  process-local transport without crossing a guest ABI or creating a mapping
  identity.

## Explicit non-claims and successor

This is controller/source closure, not proof of a live COM/LPT endpoint or
guest communications execution.  S24 owns the original `nt_com`/`nt_wcom`/
`nt_lpt` endpoint binding, public Win32 device-policy disposition and any
host-resource identity boundary.  S49 remains responsible for integrated
machine behavior.


## m0-t310-s24-host-comms-bindings-closure-20260830.md

# M0 T310 S24 — host communications bindings closure

## Closed scope

S24 closes `SPC-HOST-COMMS-BINDINGS` for selected Win32/x86 CCPU40 SoftPC:
the original serial endpoint lifecycle and its fast IOCTL/wait helper binding,
with exact disposition of WOW and printer/MONITOR adjacent paths.

## Evidence and result

- The [source boundary](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s24-p1-host-comms-source-boundary-001.md)
  proves the original `nt_com` / `nt_ntfun` serial state and public-API
  boundary, private handle ownership, WOW callback ownership and unselected
  LPT conditions.
- The [formal assembly evidence](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s24-p2-host-comms-formal-assembly-001.md)
  records a fresh 369-edge x86 CCPU40 build, source archives and no S24-owner
  unresolved forced-link symbol.
- The tracked impossible-COM fixture proves only the safe public host-open
  failure result.  It never opens a configured COM device, changes host DCB
  state, starts an original worker or performs I/O.

## Explicit non-claims and successor

S24 does not claim live COM/LPT traffic, WOW callback delivery, MONITOR
direct-printer access or guest communication execution.  WOW-specific handle
callbacks remain S39; printer/MONITOR work remains unselected unless a later
profile admission changes that state.  S49 remains responsible for integrated
machine behavior.  The next sequential packet, S25, owns C-VID generated
dispatch ABI recovery.


## m0-t310-s25-cvidc-generated-dispatch-closure-20260830.md

# M0 T310 S25 — C-VID generated dispatch closure

## Closed scope

S25 closes `SPC-CVIDC-GENERATED-DISPATCH` for selected Win32/x86 CCPU40
SoftPC.  The original generated `VideoVector` and EVID tables are restored as
the production table owners; no per-slot typed-wrapper implementation remains.

## Evidence and result

- The [source-boundary record](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s25-p1-cvidc-generated-dispatch-source-boundary-001.md)
  identifies all 156 `VideoVector` entries and all 692 entries across the 14
  EVID tables, their original definition/caller headers, and the two minimal
  live declaration corrections.
- The [formal-assembly record](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s25-p2-cvidc-generated-dispatch-formal-assembly-001.md)
  records the direct original C-VID archive and complete 369-edge forced-link
  audit, with no selected C-VID table unresolved symbol.
- The x64-only generated wrapper tool and its obsolete overlay registration
  were removed rather than retained as a second production implementation.

## Explicit non-claims and successor

S25 does not claim controller behavior, physical video-memory behavior,
host display or guest-visible video execution.  S26 owns original video
controller dispatch, S27 owns host video bindings, and S49 owns integrated
machine behavior.


## m0-t310-s26-video-controller-dispatch-closure-20260830.md

# M0 T310 S26 — video-controller dispatch closure

S26 closes the selected original SoftPC `base/video` controller package for
Win32/x86 CCPU40.  Nineteen original translation units now compile directly;
the discarded generated wrapper carrier is not part of production composition.

The sole state-carrier overlay preserves the original `Gdp`, `Cpu`, and
`Video` declarations while preventing a duplicate `Sas` global already owned
by selected CCPU40.  Direct controller selection keeps original table ordering
and uses a same-shaped `effective_addr` facade over the selected CCPU address
provider.  Formal forced-link review has no unresolved S26 controller, C-VID,
or effective-address edge.

This is not a host display or guest-video claim.  The unresolved original
`InvalidateConsoleDIBits` endpoint belongs to S27; end-to-end machine behavior
remains mandatory S49 work.  Evidence: [S26 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s26-p1-video-controller-direct-source-boundary-001.md)
and [S26 P2](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s26-p2-video-controller-formal-assembly-001.md).


## m0-t310-s27-host-video-bindings-closure-20260830.md

# M0 T310 S27 — host-video bindings closure

S27 closes the selected SoftPC host-video endpoint contract for Win32/x86
CCPU40.  Original controller and host caller bodies remain source-shaped.
Public GDI palette calls link directly; retired NT4 Console Server graphics
calls retain their original ABI through a small adapter/session presenter seam.

The seam reports an explicit unavailable result without a presenter and
delivers typed host-only dirty-rectangle, palette and activity notifications
when one is bound.  It owns neither a video controller nor a UI.  Formal
forced-link evidence has no S27 unresolved edge, and the focused contract test
passes.  Guest-visible graphics and display presentation remain S49 work.

Evidence: [S27 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s27-p1-host-video-source-boundary-001.md)
and [S27 P2](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s27-p2-host-video-formal-assembly-001.md).


## m0-t310-s28-host-platform-bindings-closure-20260830.md

# M0 T310 S28 — host-platform bindings closure

S28 closes the selected SoftPC host-platform bindings for Win32/x86 CCPU40.
It selects the original SoftPC trace provider, retains the documented patch
idle-link body without claiming scheduler recovery, binds the original Base
client exit route, and exposes Console Server registration as explicit
unavailable rather than a fake transaction.

All remaining forced-link edges have a non-S28 owner.  The closure does not
claim integrated execution, private Console Server recovery, or an idle-policy
implementation.

Evidence: [P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s28-p1-host-platform-source-boundary-001.md),
[P2](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s28-p2-host-platform-trace-and-idle-assembly-001.md),
and [P3](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s28-p3-host-platform-owner-disposition-001.md).


## m0-t310-s29-host-configuration-bindings-closure-20260830.md

# M0 T310 S29 — host-configuration bindings closure

S29 closes the selected SoftPC host configuration/PIF bindings for Win32/x86
CCPU40.  The original two-file OEM/Unicode support library is selected
directly, preserving its original conversion and public host API sequence.

The two private Console Server configuration calls retain their full source
shapes and explicit unavailable results.  No global shortcut hook, menu
injection, or private Console Server protocol has been invented.

Fresh formal assembly has no remaining selected OEM, PIF configuration,
shortcut, or console-menu unresolved symbol.  Integrated configuration
behaviour remains part of later machine validation.

Evidence: [S29 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s29-p1-host-configuration-source-assembly-001.md).


## m0-t310-s30-host-vdd-binding-closure-20260830.md

# M0 T310 S30 — host-VDD binding closure

S30 closes the selected original SoftPC host-VDD binding cluster for Win32/x86
CCPU40.  The original `nt_vdd.c` installation, IRQ and DMA service bodies are
in the formal source closure; no hand-written VDD provider replaces them.

The only reached fixed-width host-object bridge is the previously admitted
SFT/JFT synchronous shadow.  Its identity facade delegates to the session's
single host-resource mapping-manager instance, preserving the project-wide
one-mapping-manager rule.

The selected CCPU40 candidate and forced-link closure completed with 381
edges, and the existing x86 VDD shadow fixture completed successfully.  S30
does not enable arbitrary VDD products, MONITOR/kernel VDM, Bochs fallback, or
integrated device behavior.

Evidence: [S30 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s30-p1-host-vdd-binding-closure-001.md).


## m0-t310-s31-host-bop-binding-closure-20260830.md

# M0 T310 S31 — host-BOP binding closure

S31 closes the original SoftPC host-BOP binding boundary for the selected
Win32/x86 CCPU40 profile.  `nt_bop.c`, `cntlbop.c`, and the VDD `ms_bop`
table carrier are selected original bodies.  Their historical call order,
fixed-width reads and register/IP updates were not replaced.

The source shows that `nt_bop.c` is an ingress aggregator.  Every dispatched
owner is explicitly retained or transferred to its complete owner packet;
MONITOR-only IRET behavior remains unselected.  No new host table, BOP
dispatcher, Bochs route, or service-specific substitute was added.

The formal selected CCPU40 graph compiles the complete carrier set.  Its
remaining `DpmiDispatch` and `DBGDispatch` unresolved references are correctly
owned by S37 and S42, rather than hidden by S31.

Evidence: [S31 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s31-host-bop-source-boundary-001.md).


## m0-t310-s32-other-machine-source-closure-20260830.md

# M0 T310 S32 — other-machine source closure

S32 closed the inherited residual-machine source bucket.  Its 110 static ABI
records are 20 original declaration/tool carriers, not a separate runtime
package.  Every selected callable carrier now has its original owner in an
earlier or later T310 cluster; unselected test, license and generator inputs
remain mirrors only.

No generic callback wrapper, adapter, overlay, warning suppression or source
rewrite was added.  The selected x86 CCPU40 formal graph remains the build
evidence; runtime behavior continues through each owning packet and S49.

Evidence: [S32 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s32-other-machine-source-boundary-001.md).


## m0-t310-s33-support-controller-callbacks-closure-20260830.md

# M0 T310 S33 — support-controller callbacks closure

S33 closes the original SoftPC support-controller source package.  The full
original `ios.c` port-routing tables and width-expansion algorithms are
selected for x86 CCPU40 with no replacement implementation.  Its function
pointers remain private machine state.

`terminat.c` is retained as original source but its historical process-exit
tail is explicitly a later S49 app/session lifecycle proof.  S33 did not hide
that behavior behind a controller shim or falsely claim a runnable teardown.

Evidence: [S33 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s33-support-controller-callbacks-closure-001.md).


## m0-t310-s34-mvdm-support-library-closure-20260830.md

# M0 T310 S34 — MVDM support-library closure

S34 closes the selected original MVDM support libraries for the Win32/x86
CCPU40 composition. The formal graph directly archives original
`suballoc.c` and original `oemuni/{file,process}.c`; their historical test
programs remain excluded from the product graph.

The source-level caller review retains the original XMS/DPMI allocator callback
contract and the DEM/PIF OEM file/environment contract. Existing named
machine-memory and Win32 bindings are reused; no generic support shim or
second mapping manager is introduced. The fresh 381-edge formal graph and
forced-link audit expose no S34-owner unresolved symbol.

Evidence: [S34 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s34-mvdm-support-library-closure-001.md).


## m0-t310-s35-mvdm-dos-owner-package-closure-20260830.md

# M0 T310 S35 — MVDM DOS owner-package closure

S35 closes the original source-package composition of the full DEM and
COMMAND libraries for the selected Win32/x86 CCPU40 graph. All 27 original
translation units remain direct selected bodies; the original DEM/COMMAND
dispatch tables remain authoritative.

The only new source binding is a same-shaped public Win32 implementation of
the reached Console Server keyboard-layout query. Existing mapping-manager,
guest-span, command-worker and checked-length boundaries are retained rather
than replaced. Formal 381-edge composition and the two current source-shaped
x86 checks pass. Monitor and debugger residuals are explicitly transferred to
S36 and S42; integrated guest behavior remains S49.

Evidence: [S35 P1](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t310-s35-mvdm-dos-owner-package-closure-001.md).


## m0-t310-s36-mvdm-sim32-owner-package-closure-20260830.md

# M0 T310 S36 closure — MVDM SIM32 owner package

S36 directly composed the selected original `softpc.new/host/src/sim32.c`
under the sole x86 CCPU40 profile.  It formally added the pre-existing
same-shaped `NtVdmControl(VdmQueryDir)` monitor binding to the original
SoftPC graph and selected the exact original `sas_overwrite_memory` facade as
a narrow mirror overlay because full `ntstubs.c` duplicates alternate CPU/FPU
state.  The fresh 384-edge formal graph contains no SIM32, SAS-overwrite or
`NtVdmControl` unresolved symbol.  The focused x86 monitor fixture passes.

No kernel VDM, MONITOR process route, Bochs fallback, generic memory shim or
new mapping manager was introduced.  DPMI32 is the next owner package; global
machine behavior remains S49.


## m0-t310-s37-dpmi-bochs-era-cleanup-20260830.md

# M0 T310 S37 — DPMI Bochs-era cleanup

S37 returned the selected DPMI32 mirror bodies to their original SoftPC
`sim32`/SAS/CCPU calling form and removed the nine private DPMI overlay files
that existed solely to stage Bochs-era protected-memory, descriptor, frame and
session-state transactions.  No BOP or DPMI algorithm was replaced: the
original pointer decoding, descriptor loop, interrupt/fault table order and
real-mode stack sequence are again in their original mirror files.

The sole retained DPMI identity divergence is `MVDM-HOST-DIV-018`: the
host-owned `VdmTib.PmStackInfo` address still crosses the original `CX:DX`
guest ABI through the session mapping manager.  That is a host-object identity
crossing, not a Bochs mechanism.

The original `dpmi32/sources` `i386_SOURCES` variant is retained in the mirror
but deliberately not selected by the CCPU40 product graph.  Its
`i386/dpmi386.c` body requires NT4 kernel-VDM operations
`NtSetLdtEntries`, `NtSetInformationProcess(ProcessLdtInformation)`, fixed
NTVDM V86 state and VDM feature bits.  Selecting it reproduced those concrete
missing declarations; it is evidence for the established no-kernel-VDM/V86
boundary, not an invitation to add a second machine route.

The selected portable `SOURCES` package compiles as
`original-mvdm-dpmi32.lib` in the formal Win32/x86 CCPU40 graph after the
narrow `MVDM-HOST-DIV-139` declaration guard prevents the identical modern
`winnt.h` `LDT_ENTRY` carrier from being declared twice.  This changes no
descriptor field or DPMI control flow.

The r10 formal graph also builds the original CCPU40 archive and the original
`cvidc` archive beside the selected DPMI archive.  `cvidc/accessfn.c` is
compiled with its historical non-`PROD` configuration carrier, because that
original conditional body is the source-defined `getXX`/`setXX`, segment,
control-register and SAS bridge.  Symbol inspection confirms exports including
`getEAX`/`setEAX`, `getEBP`/`setEBP`, and `getCR0`/`setCR0`; no replacement
adapter register facade is selected.

The independent r10 whole-graph forced-link audit completed all 154 graph
edges and produced `original-softpc-forced-closure.dll` plus its link log.
The previous DPMI virtual-memory, DPMI lifecycle/fast-BOP, and CCPU
register/segment/control-register bridge misses are absent.  The remaining
unresolved symbols belong to already-separate owners: EMS/LIM writes,
DEM module/debug facilities, host startup UI and VM allocation, and debugger
initialization/hard-error facilities.  They are recorded as their owners'
future boundaries rather than DPMI substitutes.

The production-source negative scan found no `adapter-bochs`, `bochs-core`,
`machine_facade`, or removed DPMI private-overlay reference beneath
`src/mvdm-host` (excluding documentation).  That scan is specific to the
selected DPMI cleanup; it does not authorize broad deletion outside this
owner package.

