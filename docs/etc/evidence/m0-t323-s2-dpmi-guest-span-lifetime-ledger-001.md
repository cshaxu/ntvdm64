# M0 T323 S2 — DPMI guest-span lifetime ledger

## Question

Which reached `Sim32GetVDMPointer` forms in the selected original DPMI32
provider are transient SoftPC-private accesses, which retain an address across
an original DPMI operation, and which require a distinct monitor or later
owner?  In particular, may `IntelBase` continue to be a `ULONG` when an x64
compile exists?

## Inputs

- Selected original provider: `src/mvdm-host/dpmi32`, including the x86
  comparison sources `i386/dpmi386.c` and `i386/dpmimem.c`.
- Original SoftPC implementation:
  `src/mvdm-host/softpc.new/host/src/sim32.c`.
- Existing monitor declaration/binding:
  `src/adapter-mvdm-host-out/monitor/source/vdm_control.c` and its README.
- Shared session lease and mapping rules in `src/session` and
  `docs/rules/CODING.md`.

## Procedure

1. Enumerated every selected DPMI32 file containing `Sim32GetVDMPointer`.
2. Read each use in its source context, including pointer arithmetic,
   retention and any call made while the pointer remains live.
3. Separately traced `IntelBase`, `FlatAddress`, `VdmSetInt21Handler`, and
   the original `Sim32pGetVDMPointer` definition.
4. Compared each category with the one allowed guest-memory rule: a native
   pointer may exist only as a synchronous, bounded, component-local lease;
   no native pointer may cross a guest ABI, durable record or asynchronous
   boundary.

## Observations

- `sim32.c` returns a process-local SoftPC-memory pointer.  The historical
  signature is itself correct for immediate host-side dereference; it is not
  a guest-visible value and is not a replacement for the session mapping
  manager.
- The ten reached provider files are `buffer.c`, `dpmi32.c`, `dpmiint.c`,
  `dpmiselr.c`, `dpmimemr.c`, `int21map.c`, `modesw.c`, `stack.c`,
  `i386/dpmi386.c`, and `i386/dpmimem.c`.  There are no reached
  `Sim32FlushVDMPointer` or `Sim32FreeVDMPointer` calls in this package.
- `buffer.c`, `modesw.c`, `stack.c`, `dpmimemr.c`, and the two x86 comparison
  files use returned pointers as immediate, same-thread SoftPC-private
  operands.  Their call form is source-shaped and has no guest/native ABI
  publication.
- `dpmiint.c` uses spans to edit stack, IVT and exception frames.  Its
  `VdmSetInt21Handler` call is a separate NT kernel-monitor service request;
  current `NtVdmControl` correctly returns `STATUS_NOT_IMPLEMENTED` for it.
  It must remain an explicit monitor-owner transfer until that operation has
  a source-shaped capability, rather than being reported as a successful
  interrupt registration.
- `int21map.c` contains the broad DOS/DEM re-entry cohort: many pointers stay
  relevant while the original code changes mode and calls `host_simulate` or
  the DOS execution path.  This is not a safely isolatable short-span cohort;
  it belongs to later DPMI/DOS re-entry integration.
- `dpmi32.c` initializes `SmallXlatBuffer`, `LargeXlatBuffer`,
  `DosxStackFramePointer`, `DosxDtaBuffer`, `CurrentDta`, and `Ldt` from
  SoftPC memory.  These are original host-private DPMI state, but their
  lifetimes extend across dispatcher calls.  Converting them to generic
  session mapping tokens would change the original pointer-bearing state
  model; retaining them requires the selected SoftPC memory backend to own
  their lifetime, not a DPMI-private mapper.
- `IntelBase` is the actual width defect.  It stores `Sim32GetVDMPointer(0,
  1, FALSE)` in a `ULONG`, then is used with `FlatAddress` in `buffer.c`,
  `dpmiselr.c`, `dpmiint.c`, `int21map.c`, and `xlathlp.h`.  On x64 it
  truncates a host pointer and makes both pointer subtraction and IVT access
  invalid.  `FlatAddress[LDT_SIZE]` is likewise a `ULONG` table in
  `dpmi32.c`/`dpmi32p.h`, so changing only `IntelBase` would be incomplete.

## Disposition

| Cohort | Source-shaped disposition | Reason |
| --- | --- | --- |
| Immediate local pointer users | Direct SoftPC-private pointer | They retain original signature and stay within one synchronous host call. |
| DPMI persistent state pointers | Selected SoftPC backing-owner dependency | No guest ABI publication occurs, but the state requires a stable machine-memory backing lifetime.  Do not invent a DPMI mapper. |
| `IntelBase` + `FlatAddress` address algebra | Required mirror-overlay width recovery | One ownership contract spans declaration, initialization, descriptor mapping and flat/segmented conversion.  `ULONG` is not a valid host pointer carrier on x64. |
| `VdmSetInt21Handler` | Exact later monitor-owner transfer | No kernel VDM monitor is present; current failure is truthful and must not be changed into synthetic success. |
| `int21map.c` DOS/DEM re-entry | Later DPMI/DOS integration transfer | Its pointers span mode switching and original DOS execution, outside this bounded shared-span cohort. |

## Interpretation and confidence

High confidence: the S2 recoverable cohort is not “all DPMI pointers.”  It is
the common host-local pointer contract plus the complete `IntelBase` /
`FlatAddress` width contract.  The former retains original `Sim32` call shape;
the latter needs one minimal, registered mirror-overlay change that uses
`uintptr_t` only for host-local address algebra.  Guest numeric addresses and
all cross-boundary identity remain fixed-width and session-owned as required.

No change is authorized here for `VdmSetInt21Handler`, protected fault
delivery, DOSX guest assembly, or `int21map.c` re-entry.  This prevents a
partial DPMI width edit from silently claiming a monitor or DOS runtime.

## Follow-up

The monitor and DOS re-entry transfers remain visible for later owner work.

## Recovery P2

`MVDM-HOST-DIV-140` changes only the private address carrier:

- `IntelBase` and `FlatAddress` use `ULONG_PTR`;
- DPMI initialisation retains the original `Sim32GetVDMPointer(0, 1, FALSE)`
  and conversion order;
- descriptor, IVT, DTA-range and flat/segmented arithmetic retain native
  width until the original code intentionally narrows a verified guest value;
- guest structures, register values, selector fields and mapping-token ABIs
  remain unchanged.

The changes are source-mirror modifications rather than an overlay because
they are small type/cast corrections inside the original declarations and
conversion expressions.  Each changed original location carries the same
`DIVERGENCE(MVDM-HOST-DIV-140)` marker and the mirror README registers it.

## Verification

With Node 22 supplied explicitly to the graph generator, the following
commands succeeded from the repository root:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 -BuildRoot build/M0-T323/S2/p2-x86
run-ninja-parallel.cmd original-mvdm-dpmi32.lib
run-ninja-parallel.cmd original-softpc-process.exe

New-T310OriginalSoftpcNinja.ps1 -Architecture x64 -BuildRoot build/M0-T323/S2/p2-x64
run-ninja-parallel.cmd original-mvdm-dpmi32.lib
run-ninja-parallel.cmd original-softpc-process.exe
```

Both rows built `original-mvdm-dpmi32.lib` and linked the selected original
SoftPC product.  The incremental x64 DPMI rebuild no longer produced the
`IntelBase`/`FlatAddress` pointer-width conversion warnings introduced by the
historical `ULONG` carrier.  Existing original warnings (for example
uninitialised historical locals and source macro redefinitions) remained
visible and were neither suppressed nor changed by this P.

This verification is a static source/link result.  It does not exercise a
protected-mode guest, install a real INT21 handler, or claim DOSX/DEM re-entry
completion.
