# M0 T373 — WOW16 bootstrap, callback and Win16 NE lifecycle

## Purpose

Recover the first complete source-shaped WOW16 execution vertical: original
WOWEXEC command acquisition, non-fast `CallBack16`/monitor re-entry, one
single-session Win16 NE launch and its original return/teardown path. The task
starts from the byte-identical `mvdm-host/wow32` mirror, immutable
`mvdm-guest/wow16` media, already-closed Base VDM local command record route,
and selected original CPU40 SoftPC machine.

This is one owner package. It does not activate isolated `WOW32` service
exports because a trace reaches them.

## Boundaries

* Original `wow32` and WOWEXEC bodies retain startup ordering, task/NE
  lifecycle, callback frame layout, error returns and teardown order.
* `adapter-mvdm-host-out/basesrv` provides only the already-selected
  source-shaped `GetNextVDMCommand` local record contract. Its DOS and WOW
  operations remain distinct.
* `adapter-mvdm-host-out/monitor` owns only typed monitor/VDM-control and
  copied task projection boundaries; it cannot become a WOW provider.
* `adapter-mvdm-host-out/softpc` retains historical SoftPC spellings and
  supplies bounded guest-memory leases plus the original CPU40
  `host_simulate` mechanics. No raw guest pointer survives a recursive run.
* `session` owns only neutral session lifetime, mappings and completion
  identities. It does not own a second command broker or a WOW task model.
* `app` selects immutable guest media and creates the one session. It does
  not parse NE, fabricate a task, inject a callback or replace WOWEXEC.

The task excludes fast WOW, `CurrentMonitorTeb`, CSR/CSRSS transport, kernel
VDM, a multi-VDM broker, general USER/GDI/Win16 API recovery, private shell
behavior, new VDDs, CPU/device work, x64 functional runtime, guest mutation
and a synthetic Win16 loader.

## Ordered subtasks

1. **S1 — Whole-package bootstrap topology and build-selection audit.**
   Reconcile original WOWEXEC, `wkman`, `wow32`, `wcall16`, NE loader/task
   bodies, exports, callers, guest artifacts and all current product inputs.
   Classify direct original, binding-only, adapter-backed, unavailable or
   later WOW service-owner paths. Select the minimal complete non-fast,
   single-session bootstrap cohort; no source body changes or runtime run.
2. **S2 — Original command and task bootstrap binding.** Compose the selected
   original WOW command acquisition and task-start/lifecycle bodies through
   the source-shaped Base VDM and monitor boundaries, retaining original
   `MS_bop_1 → WOW32.dll → W32Init/W32Dispatch` dynamic ingress rather than a
   static parallel route. Preserve original `VDMINFO`, task identity, failure
   and cleanup order; prove positive and unavailable forms locally.
3. **S3 — Non-fast callback and CPU40 re-entry closure.** Complete the
   original `wcall16.c` transaction using the existing short mapping leases,
   original `SETVDMSTACK → host_simulate → VDMSTACK` order and typed task
   projection. Audit every retained callback entry point in the selected
   bootstrap cohort; fast-WOW and non-selected GUI providers remain explicit
   unavailable routes.
4. **S4 — Original Win16 NE launch/return cohort.** Compose the selected
   original NE bootstrap, module/task start and normal return teardown path.
   A modern public API may only appear behind a same-shaped required facade;
   it cannot replace the original NE/task algorithm.
5. **S5 — Formal link, focused matrix and one fixed observation.** Formally
   build/link the selected CPU40/x86 WOW cohort, run source-defined focused
   positive/negative tests, then make exactly one immutable guest observation
   with a selected original Win16 workload. Classify the earliest source owner
   without creating a trace-selected repair.

## Exit criteria

The selected original non-fast one-session WOWEXEC/NE lifecycle has one
source-first disposition per reached body and boundary; original algorithmic
bodies are directly composed wherever their dependencies permit; every
adapter is same-shaped and mapping-safe; the cohort formally links under
CPU40/x86; and one immutable observation distinguishes a proved lifecycle
result from a precise earlier original owner. No claim is made for fast WOW,
general Win16 UI, private shell/CSR behavior, multiple sessions or unselected
WOW provider families.
