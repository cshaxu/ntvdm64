# M0 T310 — original SoftPC/CCPU backend import and selectable Bochs backend

## Product decision

The product has one imported MVDM host composition per session. It must select
one and only one machine backend for that session:

```text
app CLI selection
  -> session backend binding
     -> original MVDM/SoftPC call shapes
        -> original SoftPC/CCPU backend, or
        -> same-shaped adapter-mvdm-host-out/softpc -> adapter-bochs backend
```

This is not two concurrently executing VMs, and it does not permit rewriting
MVDM callers. Bochs is the presently demonstrated mechanical backend. Original
SoftPC/CCPU is the source-first selectable backend: this task must import,
build, bind and progressively verify its original owner closure before runtime
selection is enabled.

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
- machine/firmware: only immutable `mvdm-softpc-firmware` inputs;
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

### S5 — focused dual-backend verification and transfer

Run x86/x64 selection, rejection, create/reset/run/stop/teardown tests. Test
the original SoftPC call form where actually linked.  The minimum original
SoftPC acceptance path is `create -> reset -> firmware/machine initialization
-> bounded execution -> typed controlled stop -> teardown`.  Its focused
evidence must exercise the enabled keyboard input, timer/PIC delivery, basic
video/port path and selected startup-media path.  Verify that unavailable
devices fail by their recorded source-shaped direction rather than disappearing.
Only after this closure may later XMS/DPMI packages use the selected backend
contract.

## Invariants

1. `bochs-core` remains private to `adapter-bochs`; neither MVDM nor SoftPC
   includes a Bochs type, object or global.
2. `adapter-bochs` remains Bochs-only and receives no SoftPC, BOP, DOS, WOW,
   COMMAND, DPMI or Win32 semantics.
3. Original SoftPC/CCPU remains a mirror. Any divergence needs `DIVERGENCE:`
   and README registration; substantial added logic belongs in a private
   overlay or named adapter. The sole pre-approved divergence class is an
   original 32-bit native host pointer/handle/VDM alias replaced by the
   existing session mapping manager and bounded leases, so the same source
   builds correctly on both x86 and x64. It must preserve the original function
   spelling, parameters, ordering and observable failure result.
4. `adapter-mvdm-host-out/softpc` preserves historical call shapes and cannot
   invent an MVDM service result.
5. A session owns the selected backend and mapping-manager instances. No
   pointer, Bochs object, CCPU state pointer or host handle crosses fixed-width
   component ABI.
6. Once original source is composable, an equivalent Bochs substitution is
   deleted or reduced to its binding seam.
7. `mvdm-softpc-patch` may contain only a registered NTVDMx64-provenance body.
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

T310 closes only after original SoftPC CPU/CCPU and the selected original
machine composition are imported, source-shaped bindings are formally tested
on x86/x64, and the SoftPC backend completes the minimum
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` path.  The closure records one explicit
disposition for every reached machine family, proves keyboard input, timer/PIC,
basic video/port and startup-media behavior for the selected profile, and
proves that a session never runs SoftPC and Bochs concurrently.  The permitted
mapping-manager changes must be individually registered and tested. S1 closes
only the source/contract decision required to do that safely.
