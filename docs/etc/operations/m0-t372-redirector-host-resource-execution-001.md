# M0 T372 — Redirector and host-resource execution

## Purpose

Recover the next dependency-ordered original execution package: the complete
OpenNT `vdmredir` owner package and the original local host-resource paths it
uses. The task starts from the current byte-identical `mvdm-host/vdmredir`
mirror, the previous whole-package source/build closure, and existing
same-shaped session, SoftPC and public-Win32 facades. It proves original local
resource behavior first, then bounded public network bindings; it never turns
individual `57:xx` trace hits into a provider design.

## Boundaries

- Original `vdmredir` bodies own Redirector dispatch ordering, marshal layout,
  lifecycle and source-defined failure results.
- `adapter-mvdm-host-out/redir` owns only same-shaped external historical
  interfaces. It cannot become a second Redirector provider.
- `session` owns host-resource IDs, native handles, completion records and
  mapping managers. Guest memory is reachable only through a bounded,
  synchronous mapping-manager lease.
- `adapter-mvdm-host-out/softpc` owns only original machine-shaped calls;
  CPU40/SoftPC remains the selected machine route.
- The original `nt_bop.c` dynamic `LoadVdmRedir`/`GetProcAddress` boundary is
  retained. A shipped `VDMREDIR` image and its original exports can be added
  only after the corresponding owner group has passed local closure; this task
  does not create a replacement selector dispatcher.
- Guest media remain immutable. CSR/BaseSrv recreation, kernel VDM, private
  RPC, downlevel RAP, a second COMMAND broker, Bochs, CPU30 and x64 recovery
  are outside this task.

## Subtasks

1. **S1 — Original owner/runtime-topology revalidation.** Reconcile all 50
   original dispatcher entries, 15 provider bodies, `cmdredir`/DEM consumers,
   declarations and current CPU40/x86 build graph. Record source, formal link,
   local-test and runtime-reachability separately, with one immediate
   immutable-workload gate.
2. **S2 — Source-first interface closure.** Reconfirm each external call
   against the mandatory recovery ladder: direct original composition,
   same-shaped modern facade, registered external intrusion, then new logic
   only if no historical route remains. Freeze local named-pipe/lifecycle,
   mailslot/async, and public NetAPI/remote group boundaries.
3. **S3 — Original DLL host-import ABI closure.** First recover the one
   source-shaped `ntvdm.lib`-equivalent import surface required to compose the
   original `VDMREDIR.dll` without a second SoftPC instance. Then compose the
   lifecycle, local broker and synchronous named-pipe groups through those
   accepted imports; add focused original-owner tests and formal CPU40/x86
   link proof.
4. **S4 — Mailslot and completion owner groups.** Compose original mailslot
   and asynchronous-completion groups with copied completion records and
   checked leases; prove stale, cancellation and disconnect failure paths.
5. **S5 — Public network owner groups.** Compose original NetAPI/NetBIOS/DLC
   groups where public capability preserves their original contract; retain
   source-proven unavailable results for private/downlevel-only paths.
6. **S6 — Whole-family reconciliation and frozen observation.** Reconcile
   every `57:00..31` entry, brokered DEM/COMMAND consumers, mirror exceptions
   and adapter ownership; run the declared family matrix and exactly the
   allowed fixed integration observation.

## Exit criteria

Every original Redirector branch has one source-first owner/disposition; each
selected owner group passes focused proof and formally links into the CPU40/x86
product; a single immutable-workload observation is classified without
claiming unobserved network, COMMAND, DOS-child or WOW behavior.
