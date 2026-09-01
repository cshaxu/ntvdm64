# M0 T341 S1 — Original VDD owner-package admission

## Question

What is the complete original VDD host-service package, and does the current
single-session product need to select an external VDD provider before it can
truthfully retain that package?

## Inputs

- Original `mvdm-host/softpc.new/host/src/nt_vdd.c` and `nt_msscs.c`;
- original private declaration `nt_vddp.h`, export definition
  `softpc.new/obj.vdm/obj/i386/ntvdm.def`, VDD service document, and direct
  production callers;
- selected formal CPU40/x86 build graph
  `build/M0-T337/S3/formal-x86/build.ninja`; and
- the existing `MVDM-HOST-DIV-063` and `MVDM-HOST-DIV-148` registrations.

## Procedure

1. Enumerated every original VDD export and local helper in `nt_vdd.c` and
   `nt_msscs.c`, including memory, user-hook, re-entry, and DEM handle forms.
2. Located direct production callers, then separately classified `vdd/samples`
   and `ieuvddex` rather than treating them as runtime providers.
3. Inspected the selected formal build manifest for the original VDD source
   and the only current VDD-adjacent adapter files.
4. Recorded owner, ABI, direct dependency, selected disposition, failure
   direction and transfer for each surface in the accompanying ledger.

## Observations

- `nt_vdd.c` contains the original finite BOP callback table, VDD adapter
  allocation, IO hook, IRQ reservation and DMA services.  It is selected in
  the formal CPU40 host library.
- `nt_msscs.c` contains the same package's original installable-VDD selection,
  UMB memory hooks, user lifecycle callbacks, synchronous `VDDSimulate16`,
  and VDD termination forms.  It too is selected in that library.
- The current product's registered configuration boundary treats a missing
  retired NT4 registry key as an explicit selection of no installable VDD.
  It neither loads a sample nor claims that an unprovided device exists.
- The only non-sample production caller found for the user-hook family is
  original VDMREDIR.  That owner remains unselected.  Memory-fault delivery
  still has a kernel-monitor boundary, and printer interception is an original
  `MONITOR` conditional, neither of which S1 may recreate.
- Existing mapping is narrow: VDD's historical `PVOID` UMB carrier is decoded
  as an Intel physical numeric address.  No native pointer is exposed to a
  VDD contract, and no independent VDD mapping manager exists.

## Interpretation and confidence

The entire original VDD API owner package is already retained and selected as
source in the formal CPU40 graph.  The current product profile deliberately
selects zero installable external VDDs; that is an original-shaped selection
result, not missing source or a reason to invent a generic provider.

Therefore S1 selects no new VDD implementation cohort.  The correct next
step is a focused formal source/build closure for this already-selected
no-provider profile.  Any future request to load a particular VDD must be
admitted with that VDD's complete source package, guest stub, machine and
monitor contracts; it cannot be justified by this generic API audit.

Confidence is high for source ownership, current formal-build selection and
the no-provider configuration outcome.  It is intentionally not a claim that
an external VDD lifecycle, kernel fault path, or VDD sample runs.

## Follow-up

T341/S2 verifies the selected original source/build closure and focused
failure disposition without enabling an external VDD.  Redirector, debugger,
kernel-monitor and device-specific VDD work remain named owner transfers.
