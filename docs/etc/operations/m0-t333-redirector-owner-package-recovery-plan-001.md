# M0 T333 — Original Redirector owner-package recovery

## Objective

Recover the complete original OpenNT `mvdm-host/vdmredir` package as a
source-shaped provider family.  The package owns the original `57:00..31`
dispatch table, lifecycle, named-pipe, mailslot, async, NetAPI/NetBIOS/DLC
and mode algorithms.  Adapters only provide the historical external forms;
they never replace Redirector policy or create a second COMMAND broker.

## Current architecture boundary

- `mvdm-host/vdmredir` is the original provider mirror and remains the only
  Redirector policy owner.
- `adapter-mvdm-host-out/redir` owns only original-shaped external bindings:
  checked copied guest spans, opaque host-resource identity conversion and
  public Win32 endpoint mechanics.
- `adapter-mvdm-host-out/softpc` supplies the selected original CPU40
  register, Sim32, interrupt and CCPU mechanics.  No Bochs route exists.
- `session` owns the single mapping-manager implementation, its distinct
  guest-memory, host-resource and completion-callback instances, plus
  lifecycle/teardown ownership.
- `opennt-host/netapi/netlib/ntstatus.c` is the already selected original
  non-MVDM helper slice.  Private RAP, CSRSS, kernel-VDM and DLC drivers stay
  explicit non-invasive boundaries.

## Sequential subtasks

### S1 — Current original source, table and ABI admission

Reconcile the original `vdmredir/sources` manifest, every 32-entry dispatch
table row, direct COMMAND/DEM broker callers, current mirror differences,
adapter seams, selected `ntstatus.c` slice and existing build inputs.  Give
each entry one current disposition and one recovery group.  No Redirector
behavior changes.

### S2 — External forms and original failure admission

Audit the source's immediate public Win32, NetAPI/RAP, monitor, VDD,
NetBIOS/DLC and CPU40 forms.  Retain directly composable original calls;
bind the existing same-shaped adapters; identify exact unavailable results;
remove a current adapter path only if an original body supersedes it.  No new
Redirector provider policy.

### S3 — Original DLL lifecycle, synchronous named-pipe and local broker cohort

Compose the original `VDMREDIR.DLL` source package, including `vrinit`,
`vrmisc`, `vrnmpipe`, `vrputil`, `vrdll`, and every source unit selected by
the original `vdmredir/sources` manifest.  Preserve the original deferred
`LoadVdmRedir`/`GetProcAddress` contract, error ordering, handle identity and
no-second-broker rule.  First prove the complete source package compiles on
x86/x64, then separately admit its original DLL entry/link/load composition;
a static library is evidence only and is not a substitute for the DLL.

### S4 — Original mailslot and asynchronous completion cohort

Compose `vrmslot` and the source-owned async completion paths with session
teardown and opaque identities.  Preserve source failure where physical IRQ
completion cannot yet be delivered; do not inject guest callbacks or host
handles.

### S5 — NetAPI, NetBIOS, DLC and assignment-mode cohort

Compose public modern-capability paths while retaining original algorithms.
Keep RAP/private broker/DLC driver branches source-shaped unavailable unless a
finite same-shaped adapter is proven.  Run the whole cohort x86/x64 matrix.

### S6 — Whole package reconciliation and bounded observation

Reconcile all 32 table rows, direct DEM/COMMAND consumers, mirror
divergences, mapping lifetimes and build selection.  Run the declared
whole-package matrix and one fixed observation only after every owner cohort
has a result.  Trace cannot choose a new leaf repair.

## Exit

Every original table row and direct provider dependency has one source-shaped
implementation, exact unavailable result or named successor owner.  Both
MSVC x86 and x64 final product graphs link without `/FORCE`.  Runtime claims
are limited to observed complete cohorts; no raw host handle/pointer crosses a
guest boundary, no CPU30/Bochs route or `src.old` input exists, and no
Redirector-local mapping manager or duplicate COMMAND broker is added.
