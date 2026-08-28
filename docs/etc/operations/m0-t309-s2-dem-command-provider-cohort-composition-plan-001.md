# M0 T309 S2 — original DEM/COMMAND provider-cohort composition

## Objective

Compose the complete original DEM and COMMAND provider cohorts selected by
their unchanged dispatcher tables. The work establishes the smallest current
x86/x64 source and link closure that retains original source ordering, handler
identity, and failure behavior through the already closed Base VDM, monitor,
SoftPC-shaped, Win32, session, and Bochs-facing forms.

## Baseline

S1 closed the sole ingress boundary: source-shaped `MS_bop_0` and `MS_bop_4`
preserve the historical byte-read → dispatcher → IP order, while the exact
original `demdisp.c` and `cmddisp.c` tables compile under the current
`opennt-abi` root on x86 and x64. The selected tables respectively reference
the complete 73-service DEM and 17-service COMMAND provider cohorts.

## Admitted work

1. Enumerate the original C translation-unit cohorts from their original
   `sources` files and build them as source-owned packages, not service
   slices.
2. Compile and link the cohorts on x86 and x64 through already named adapter
   forms, including the existing `adapter-bochs` mechanical archive as the
   lower machine boundary. Classify each unresolved external symbol by
   original owner and existing adapter or exact next owner.
3. Recover only same-shaped bindings that are required to link selected
   original source. Preserve the original provider body, function spelling,
   call order, data layout, and failure behavior; register every divergence.
4. Add focused cohort tests only after an original provider route is actually
   linked. Tests may validate original source contracts but may not substitute
   service bodies.

## Exclusions

No trace-selected BOP implementation, invented provider, replacement
dispatcher, full historical PIC/CCPU executor, kernel/CSRSS reconstruction,
XMS/DPMI/Redirector/WOW/VDD/debugger recovery, guest execution repair, or
app-owned service policy.

## Exit

Both cohorts either formally link on x86/x64 through source-shaped bindings,
or each remaining original external form has one exact owner/disposition and
the complete original cohort remains selected. No table entry or provider body
may be replaced merely to obtain a link.

## P3 — residual original-owner selection

P3 starts with the forced-link caller/symbol pairs, not bare symbol spellings.
For each residual it first asks whether the selected `mvdm-host` mirror already
contains the original body.  If so, that body is an original-source selection
candidate, not an excuse to author an adapter replacement.  The initial
inspection proves this distinction is material: `nt_rflop.c`, `nt_msscs.c`,
`nt_error.c`, `nt_event.c`, `nt_lpt.c`, `nt_pif.c`, `nt_hosts.c`, and the
original debugger/firmware paths own reached forms from DEM or COMMAND.

Only after that pass may a row select an existing named adapter, a lower
Bochs/firmware boundary, public CRT/Win32 linkage, or a later owner package.
P3 does not add all of those source files blindly: each candidate retains its
original caller, full translation-unit boundary, direct composition result,
and failure/product-shell disposition.

The selected source candidates are grouped for whole-package treatment in this
order: (1) `nt_pif.c` configuration parsing; (2) `nt_error.c` error
interaction; (3) `nt_event.c`, `nt_hosts.c`, and `nt_lpt.c`
console/standard-stream/device endpoint state; and (4) `nt_msscs.c`
cross-owner startup/VDD composition.  `nt_reset.c` is recorded separately:
its `CpuH -> CpuInt_c.h` dependency belongs to the queued original-SoftPC
backend, where the CLI will select SoftPC or Bochs under one MVDM host
composition.  The current Bochs-only T309 audit may not crop or rewrite it to
make that backend disappear.  A group may be admitted to a formal composition
graph only after its whole original translation-unit boundary is recorded.
This order is a dependency grouping, not a trace-derived service list.
