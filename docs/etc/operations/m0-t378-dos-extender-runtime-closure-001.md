# M0 T378 — DOS extender runtime closure

## Purpose

Turn the already selected original CPU40/SoftPC XMS, DPMI32 and immutable DOSX
source/link baseline into one bounded, real DOS-extender workload result.  This
is an owner-package recovery: it begins with the complete original startup and
guest handoff cohorts, never with one `52:xx`/`53:xx` selector observed in a
trace.

## Fixed architecture

- The only execution machine is original SoftPC CPU40 on Win32/x86.  CPU30,
  Bochs, x64 runtime and a second XMS/DPMI implementation are out of scope.
- Original `mvdm-host/xms.486` owns XMS/A20/UMB/INT15 semantics; original
  `mvdm-host/dpmi32` owns the DPMI table and protected-mode transition logic;
  original DOSX remains immutable guest media.
- The mapping manager applies only at genuine host-object/guest-memory
  boundaries.  It must not replace numeric XMS addresses, DPMI selectors,
  service bytes or an in-machine `Sim32GetVDMPointer` lifetime.
- Existing selected media `system32/HIMEM.SYS` and `system32/DOSX.EXE` are
  provenance inputs.  S1 must re-establish their exact source/artifact
  identity before a workload is selected or the stage is changed.

## Ordered subtasks

1. **S1 — Whole extender workload and owner admission.** Reconcile T371 with
   the current T375--T377 fixed product; identify the exact original HIMEM and
   DOSX artifact/source pairs, the guest CONFIG.NT/device and DOSX handoff
   paths, and one smallest non-interactive workload that can prove the full
   chain.  Establish every immediate original owner and current source/build
   disposition.  No product run or source change.
2. **S2 — Original XMS boot and HMA/UMB lifecycle recovery.** Recover/bind the
   complete original `CONFIG.NT -> HIMEM.SYS -> XMSInit -> A20/UMB/INT15`
   cohort selected by S1.  Preserve original callback order, error direction,
   memory ownership and mapping-manager boundary.  Do not add a synthetic XMS
   dispatcher or individual BOP service.
3. **S3 — Original DOSX/DPMI handoff recovery.** Recover/bind the complete
   original DOSX guest-to-host DPMI transition, table initialization,
   protected-mode frame/stack and exit/return cohort selected by S1.  Kernel
   VDM, VDD/VCD and private CSR routes retain source-shaped unavailable
   behavior; do not fake a protected-mode success response.
4. **S4 — Formal extender matrix closure.** Add the complete selected original
   cohorts and immutable workload media to the formal CPU40/x86 graph/stage;
   run focused source-shaped positive and negative checks and link the final
   product.  No integration run.
5. **S5 — Frozen original extender observation.** Make the predeclared bounded
   no-diagnostic fixed-container observation.  Record the first original
   XMS/DOSX/DPMI marker, normal return or exact earlier source boundary.

## Completion standard

T378 closes only when its selected immutable workload has an original
source/media identity, a complete immediate-owner route, a formal
source/binding closure and one fixed-container observation.  A source-shaped
unavailable result may close only if the smaller original route is exhausted
and the result is attributed to its owner.  This does not claim arbitrary
DOS-extender compatibility, native-child execution, Redirector or WOW/Win16.
