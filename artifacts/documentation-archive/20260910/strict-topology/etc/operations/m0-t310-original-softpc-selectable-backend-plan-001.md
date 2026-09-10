# M0 T310 — original SoftPC/CCPU recovery and Bochs runtime retirement

## Product decision

The product has one imported MVDM host composition per session. It must select
one and only one machine backend for that session:

```text
app CLI selection
  -> session backend binding
     -> original MVDM/SoftPC call shapes
        -> original SoftPC/CCPU backend
```

This is not two concurrently executing VMs, and it does not permit rewriting
MVDM callers.  The prior Bochs branch is only retained as recovery evidence
until the original SoftPC profile has passed S8.  Original SoftPC/CCPU is the
sole intended runtime backend: this task must import, build, bind and
progressively verify its original owner closure before the historical Bochs
runtime material is retired in S9.

The application default is original SoftPC. Until S5 makes SoftPC machine
creation runnable, the default selection must produce an explicit unavailable
result rather than silently selecting another executor. S8 verifies the
SoftPC profile on both x86 and x64. S9 then removes the alternate Bochs runtime
route rather than preserving a permanent dual-backend matrix.

For the original SoftPC branch, the selected executor is the original
`i386 + CCPU` pure-software configuration. `MONITOR`, hardware V86 execution,
kernel VDM execution and a Bochs CPU call from an original SoftPC body are
excluded from that branch. This preserves the historical non-MONITOR CCPU
route rather than creating a new CPU or treating the historical `i386` build
label as proof of a V86 dependency. NTVDMx64 is evidence for the narrow
selection/ABI/SAS corrections needed to expose this existing route; its
injector, loader, system-DLL patching, registry policy, CSRSS and host-mutation
product shell remain excluded.

The selection belongs to `app` and neutral session lifetime binding. The MVDM
mirror does not parse CLI policy, identify a backend, include a Bochs type or
branch on product policy. Its historical spelling remains in original context;
`adapter-mvdm-host-out/softpc` binds it to the selected backend.

## Why this task is required

The current baseline proves the Bochs path below the MVDM machine-facing
facade, but older ledgers classified original CCPU execution/device bodies as
Bochs-replacement exclusions. That is insufficient after the owner selected
original SoftPC as an alternative backend. T310 replaces that provisional
planning disposition with an exact original-source disposition. It does not
erase, crop or modify the original source to retain a Bochs-only link.

T309 explicitly transfers `nt_reset.c`, `nt_error.c` and `nt_msscs.c` here.
They are whole source-owner roots and must not be decomposed into Bochs-only
helpers merely to close a provider link.

## S breakdown

### S1 — source roots and shared backend contract

Inventory exact original roots and `sources` membership, beginning with:

- host control: `softpc.new/host/src/nt_cprgs.c`, `nt_cpu.c`, `nt_aorc.c`,
  `nt_reset.c`, `nt_error.c`, `nt_msscs.c`, `sim32.c`, `nt_sas.c`, `nt_mem.c`;
- executor: selected `softpc.new/base/ccpu386` CCPU library plus each original
  support/library root required by the historical selected target;
- machine source: original `softpc.new/base/bios`, `base/keymouse`, system,
  device and host packages in `mvdm-host`; immutable
  `mvdm-softpc-firmware` inputs are only `softpc.new/{bios,roms,data}`;
- current Bochs binding: `adapter-mvdm-host-out/softpc`, `adapter-bochs` and
  neutral session binding.

For every root/direct external interface, record source identity, role,
x86/x64 composability, MVDM-visible shape, shared lifecycle requirement,
backend-specific behavior and one disposition: `direct`, `binding-only`,
`adapter-backed`, `firmware-only`, `unavailable`, or
`not-selected-for-this-backend`.

S1 defines, but does not write, a fixed-width copied selection/lifecycle
contract outside mirrors. It distinguishes common `create`, `reset`, bounded
`run`, copied stop result and teardown from backend-specific work.

### S2 — original SoftPC target import and isolated build closure

Form independent x86/x64 candidate graphs using the exact selected original
CCPU/SoftPC `sources` closure. Preserve source body, names, ordering and build
membership. Classify every residual by original owner and bind only existing
or source-shaped named adapters. An archive is not a runnable claim. A
reviewed NTVDMx64-derived patch body, if source identity proves it is required,
lives only in `mvdm-softpc-patch`; its mirror caller keeps a minimal registered
`DIVERGENCE:` hook. No patch is admitted merely because an earlier experiment
happened to compile. The owner has specifically admitted the NTVDMx64
`patches/common/fmstubs.c` body as a byte-identical registered carrier: its
original `int 3` placeholders remain evidence and are not silently rewritten
or omitted during import.

### S3 — selected-backend binding and lifecycle implementation

Implement the S1-approved copied backend-selection binding in session and thin
app composition. `adapter-mvdm-host-out/softpc` is the sole source-shaped
binding: its Bochs branch reaches only `adapter-bochs`; its SoftPC branch
reaches only the selected original backend. A session selects one branch before
machine creation and never switches while active.

### S4 — original SoftPC machine-composition recovery

Recover the original selected SoftPC machine composition as a source-shaped
package, rather than treating the CCPU archive as a machine.  Audit and bind,
in original initialization/dependency order, the reached firmware inputs and
machine-control families: BIOS/ROM/CMOS, memory/SAS/A20/UMB, PIC/PIT, keyboard
and mouse, DMA, display/VGA, disk/floppy, and serial/parallel I/O.  Every
family must receive one disposition: direct original composition,
adapter-backed original composition, deliberately disabled with the original
unavailable/failure direction, or a named later owner.  A required native
pointer/VDM alias must use the existing session mapping manager; device logic
may not be replaced by a Bochs call or silently omitted.

The S creates a bounded original-source build and initialization workset per
family.  It does not require every historical optional device to be enabled,
but it does require every selected/reached device family to be explicitly
accounted for before a backend is called runnable.

S4 closes on source-first machine-family selection, bounded initialization
worksets and interface dispositions. It deliberately does not claim that the
selected CCPU executor has run guest instructions; that runtime recovery is
the next S rather than an implied result of archive or forced-link evidence.

### S5 — original i386 + CCPU pure-software execution recovery

Recover the selected original CCPU execution path from initialized SoftPC
state through one bounded execution interval and a typed controlled stop. The
active configuration is explicitly `i386 + CCPU`, never `MONITOR` or V86. Keep
the original `ccpu386` executor and its call shapes as the source owner; do
not replace it with a Bochs call or rewrite the executor.

Bind only the source-shaped lower seams required by this interval: SAS-backed
guest memory, register/state transfer, interrupt/stop observation and the
existing session mapping-manager lease for any historical native-pointer
alias. Evaluate the NTVDMx64 CCPU-selection, C-FPU layout and `nt_mem`/SAS
patch intent against the selected original source. Import a patch body only
under the registered source-policy exception and only when the original body
cannot express the same x86/x64 build contract through a smaller named
adapter. `fmstubs.c` remains an immediate-debugbreak default, not an implicit
runtime substitute.

S5 must prove on both x86 and x64 that a selected initialized SoftPC session
enters original CCPU execution, returns one bounded typed stop result, and
tears down without selecting MONITOR, V86 or Bochs. It does not yet claim the
full device/startup-media matrix, BOP completion or a runnable user product.
It also produces a source/path-level CCPU-profile removal audit: each reached
V86/monitor/scaffold/tool path is classified as delete-after-S5,
adapter-contract-retained, tool-only relocation, or still-required-with-owner.
S5 audits this disposition but does not physically delete the original mirror
paths.

### S6 — CCPU-profile V86/monitor implementation removal

Apply the S5 removal audit before full-profile verification. Physically remove
every original V86/Kernel-Monitor implementation path classified
`delete-after-S5` from the production mirror, move tool-only inputs to their
proper tool component, and remove `MONITOR`, `monitor.lib`, V86 scaffold and
kernel-VDM implementation inputs from all CCPU build manifests. Do not delete
an interface solely because its original implementation was V86-specific:
`NtVdmControl`, `VDM_TIB`, `CurrentMonitorTeb` and related reached call shapes
remain only where their named session/monitor adapter retains the same-shaped
modern contract or explicit unavailable result.

S6 proves x86/x64 CCPU build selection has no physical V86/monitor executable
source, `MONITOR` define, `monitor.lib`, kernel-VDM execution or Bochs fallback.
It records every deletion/move and verifies no live source/build input retains
an obsolete path.

### S7 — original SoftPC physical-address and mapping-manager binding closure

Before controller composition can make the selected profile runnable, recover
the original `nt_mem.c`, `sim32.c`, `VdmSetPhysRecStructs` and
`PhysicalPageREC` crossings through the existing session-owned mapping-manager
instances. Preserve the original SoftPC function names, parameter shapes,
call order and failure direction. On both x86 and x64, every host backing
object that enters a source-shaped SoftPC physical-memory boundary must first
receive a stable session-owned 32-bit surrogate from the applicable mapping
manager; `adapter-mvdm-host-out/softpc` then resolves it to a checked physical
page operation.

No Intel `ULONG`/`DWORD` address may be cast to `PVOID`, and no native pointer
or handle may persist in `PhysicalPageREC`, SAS, a SoftPC global or a fixed-width
component ABI. `guest_memory_lease` remains a short-lived synchronous copied
byte-access facility: it may not serve as a persistent physical-page record,
SAS alias, mapping-manager substitute or asynchronous handoff. The existing
WOW, Redirector and VDD pointer-scope callers are not enabled or rewritten by
this S; they are separately owned future-package surfaces and may not be used
as a hidden SoftPC memory backend.

This S also inventories every current `guest_memory_lease` call site and
removes only a lease wrapper that is demonstrably dead or exclusively a
duplicate selected-SoftPC physical-memory route. It must retain the neutral
session lease facility and every independently owned DEM/WOW/Redirector/VDD
caller until that owner's package can preserve its source-shaped guest-pointer
contract. A lease caller is not made a mapping-manager caller merely to make
this cleanup appear complete.

S7 closes only with focused x86/x64 positive and negative evidence for the
same source-shaped physical-page binding: stable surrogate allocation and
lookup, checked span/overflow rejection, teardown invalidation, no raw native
alias, and no lease retained beyond its synchronous caller. It records any
minimal mirror divergence and adapter/overlay location in the relevant README
registers.

### S8 — original SoftPC machine/device composition recovery and verification

Starting from the S5 CCPU execution interval, recover and verify the selected
original SoftPC controller and peripheral composition as one integrated
machine profile. This S owns the enabled internal machine families and their
source-shaped host edges: BIOS/ROM/CMOS, SAS/A20/UMB, PIC/PIT, DMA,
keyboard/mouse, display/VGA, disk/floppy and serial/parallel I/O. It verifies
their original initialization order, interrupt and port behavior, failure
directions and teardown against the selected startup-media profile.

`softpc.new/host/src/nt_timer.c` is a required original host-heartbeat member
of this composition, not an optional replacement timer.  Its original
`host_timer_init`, `TimerInit`, `Win32_host_timer`, suspend/resume and drift
correction order remain selected.  Public modern Win32 thread/event/wait and
performance-counter facilities provide the same-shaped host binding.  The
historical `NtAlertThread` termination route becomes the owning session's
cancellation event; it must wake the heartbeat's source-shaped wait and lead
to a session controlled stop rather than process termination.  Its BIOS Data
Area tick/RTC writes must resolve through S7's session mapping-manager
physical binding and then the selected machine write operation; raw
`Start_of_M_area` pointer arithmetic is not a runnable route.  `time_tick`,
`cpu_interrupt`, ICA locking and delayed hardware interrupts remain original
SoftPC call shapes and original system/host source owners.  This requires
PIT/IRQ0/RTC behavior from the selected SoftPC machine profile, but no BOP
family admission.

S8 begins only after S7 has closed the shared physical-address binding. It
consumes that binding rather than reintroducing a raw alias or using a bounded
guest-memory lease as device backing. S8 does not broaden the product to optional devices merely because a source
file exists, and it does not use a Bochs substitution to satisfy a SoftPC
controller. Every enabled or unavailable family must retain its S4 source
disposition. Its result is a verified original SoftPC machine profile, not the
cross-backend product matrix.

### S9 — Bochs runtime retirement and original-source diff recovery

S9 begins only after S8 has verified the original SoftPC profile on both x86
and x64 through `create -> reset -> firmware/machine initialization -> bounded
execution -> typed controlled stop -> teardown`, including the enabled keyboard
input, timer/PIC delivery, basic video/port path and selected startup-media
path.

Then retire `bochs-core` and `adapter-bochs` from the live product tree into
the indexed historical-code record. Remove their build, link, session-selection
and CLI/composition routes; remove all live app and MVDM references to Bochs;
and simplify every OpenNT mirror divergence that existed only to bind a Bochs
mechanical path. The original MVDM/SoftPC spelling and control flow must be
restored wherever a same-shaped SoftPC binding now exists. Any remaining
non-SoftPC historical reference must be evidence-only and outside production
source/build inputs.

S9 verifies the final SoftPC-only x86/x64 product path and a negative source /
build scan proving no live `bochs-core`, `adapter-bochs`, Bochs type, global,
CLI option or backend-selection branch remains. It also records each removed
or retained divergence and why, and updates the mirror README registers. Only
after this closure may later XMS/DPMI packages use the SoftPC-only machine
contract.

## Invariants

1. Before S9, any retained Bochs recovery material remains isolated from
   original MVDM/SoftPC source; after S9 it is historical evidence only.
2. Original SoftPC/CCPU remains a mirror. Any divergence needs `DIVERGENCE:`
   and README registration; substantial added logic belongs in a private
   overlay or named adapter. The sole pre-approved divergence class is an
   original 32-bit native host pointer/handle/VDM alias replaced by the
   existing session mapping manager. A bounded lease is permitted only for an
   exact synchronous byte transfer, never durable page/object identity, so the
   same source builds correctly on both x86 and x64. It must preserve the original function
   spelling, parameters, ordering and observable failure result.
3. `adapter-mvdm-host-out/softpc` preserves historical call shapes and cannot
   invent an MVDM service result.
4. A session owns the SoftPC backend and mapping-manager instances. No
   pointer, CCPU state pointer or host handle crosses fixed-width
   component ABI.
5. Once original source is composable, any equivalent Bochs substitution is
   removed from the live product and retained only as indexed history.
6. `mvdm-softpc-patch` may contain only a registered NTVDMx64-provenance body.
   It retains original spelling, parameters and failure order; any x86/x64
   address conversion uses the session mapping manager through the
   source-shaped SoftPC adapter boundary.

## Explicit exclusions

No kernel VDM/CSRSS product shell, BOP expansion, trace-selected repair,
unapproved optional device enablement, XMS/DPMI/WOW/VDD provider recovery,
host mutation or `src.old` input.  This task does not claim a wholesale
historical product build; it does require the bounded original SoftPC machine
composition necessary for the selected backend's verified minimum execution
path.

## Required evidence

- `m0-t310-s1-softpc-source-root-ledger.tsv`: source root, original membership,
  selected edition/hash, target role and disposition.
- `m0-t310-s1-softpc-backend-interface-ledger.tsv`: caller/interface, original
  ABI, common contract, SoftPC/Bochs disposition, owner and next S.
- `m0-t310-s1-softpc-selectable-backend-audit-001.md`: method, exact commands,
  totals, exclusions, contract and approved S2/S3 workset.

## Completion standard

T310 closes only after S5 has recovered original `i386 + CCPU` bounded
execution, S6 has removed the excluded V86/monitor implementation profile,
S7 has closed the original SoftPC physical-address binding through the session
mapping manager, S8 has verified the selected original SoftPC machine/device
profile, and S9 has retired the Bochs runtime and verified the resulting SoftPC-only product
on x86/x64. The SoftPC backend must complete the
minimum
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` path.  The closure records one explicit
disposition for every reached machine family, proves keyboard input, timer/PIC,
basic video/port and startup-media behavior for the selected profile, and
proves that no live session, app or MVDM body can select or reach Bochs. Every
reached `nt_mem`/`sim32`/`VdmSetPhysRecStructs`/`PhysicalPageREC` crossing must
use the session mapping manager on both architectures, reject raw aliases, and
prove lease lifetime separation. The permitted mapping-manager changes must be
individually registered and tested.
S1 closes only the source/contract decision required to do that safely.
