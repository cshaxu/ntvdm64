# M0 T374 — WOW16 service, UI and host integration matrix

## Purpose

Recover the next complete original WOW32 provider cohort required after T373's
source-shaped dynamic-ingress boundary.  This is whole-package recovery: it
uses the byte-identical `mvdm-host/wow32` mirror and public/same-shaped adapter
bindings where the historical NT4 product shell is unavailable.  It does not
replace `W32Init` or `W32Dispatch` with a synthetic command route.

## Boundaries

* `mvdm-host/wow32` retains original provider flow, generated dispatch-table
  ownership, task order, layouts and source-defined failure paths.
* `adapter-mvdm-host-out/win32` may provide only same-shaped modern public
  Win32/NT compatibility facades; it cannot recreate CSRSS or private USER
  transport under a different contract.
* `adapter-mvdm-host-out/softpc` retains original SoftPC spellings and owns
  checked machine/guest-memory access through the session mapping manager.
* `adapter-mvdm-host-out/monitor` owns typed VDM control and monitor seams;
  it cannot become a generic WOW dispatcher.
* `session` owns only neutral identities, mappings, waits and lifecycle.
* `app` selects immutable media and assembles one session; it cannot create a
  Win16 task, parse NE or substitute a WOW provider.

The package excludes CSRSS/kernel-VDM recreation, fast WOW,
`CurrentMonitorTeb`, a second broker, arbitrary USER/GDI emulation, guest
mutation, a static `W32Dispatch` route, CPU/device semantics, x64 runtime
closure and trace-selected individual service patches.

## Ordered subtasks

1. **S1 — Whole WOW32 provider matrix and admission cohort.** Reconcile all
   active original WOW32 provider bodies, generated dispatch rows, W32Init
   prerequisites, UI/USER/GDI, task, shell and public-capability imports
   against the T276/T277 ledgers.  Select the smallest complete original
   cohort capable of satisfying the T373 dynamic ingress, or record the
   precise earlier non-composable private boundary.  No provider is enabled.
2. **S2 — Original initialization and dispatch product cohort.** Compose the
   selected `W32Init`/`W32Dispatch` bodies and their required original task
   records through same-shaped adapter bindings.  Preserve initialization,
   cleanup and unavailable order; no static command substitute.
3. **S3 — Selected task/UI service cohort.** Recover one complete source-owned
   service subdomain required by the selected dispatch table, including its
   public UI/console capability facade and negative paths.  Keep private
   USER/GDI transport explicit where it remains unavailable.
4. **S4 — Formal CPU40/x86 link and local matrix.** Link the selected original
   provider cohort, run source-defined positive and unavailable focused tests,
   and verify all guest-memory access uses the shared mapping-manager instance.
5. **S5 — Fixed WOW16 integration observation.** Only if S1--S4 select the
   complete original provider cohort and immutable WOW16 media, run exactly
   one fixed workload observation.  Otherwise close the reached earlier
   source-shaped unavailable boundary without a substitute.

## Exit criteria

Every reached WOW32 provider body and its immediate external edge has one
source-first disposition.  The selected cohort is original-source shaped,
same-shaped at every adapter boundary, formally linked under CPU40/x86 and
locally verified; one fixed observation is made only when its original
provider/media prerequisites are selected.  A private NT4 product boundary may
close the task as exact unavailable, but cannot be replaced by a static route
or invented provider.
