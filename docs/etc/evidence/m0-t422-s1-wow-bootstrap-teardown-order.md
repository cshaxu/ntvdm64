# T422 S1 WOW bootstrap and teardown order

## Question

What is the earliest point at which the immutable WOW16 client needs a valid
desktop/client view, and which original task, module and worker exits may
retire that view without introducing a second USER server or treating a module
cleanup as task destruction?

## Inputs

- Selected original `src/mvdm/wow32/{wow32.c,wuman.c,wkman.c,wuser.c}`.
- The pinned immutable USER profile and direct-reader checks in
  [`verify-wow-user-profile.ps1`](../../../tests/observation/verify-wow-user-profile.ps1).
- Current bounded page-domain transport,
  `src/ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c`.
- Current provider registration and lifecycle carriers under `src/wow32-dll`.

## Procedure

1. Trace original registration from `W32Init` through
   `UserRegisterWowHandlers`, then trace `WU32NotifyWow(FUN_FINALUSERINIT)`.
2. Compare that order with the immutable USER's direct `FS:18h`,
   `TEB+5ch`, `TEB+60h`, `DESKTOPINFO+8` and WND reads.
3. Trace the two original `pfnWOWCleanup` call shapes and the task-destruction
   path in `W32DestroyTask`.
4. Locate all production callers of the current page-domain desktop transport
   and lifecycle cleanup/dispose carriers; distinguish fixtures from product
   callers.

## Observations

### Bootstrap precedes `InitTask`

`W32Init` fills the original 21-input table and calls
`UserRegisterWowHandlers` (`wow32.c:464-549`).  The provider currently returns
the guest-linear `SHAREDINFO` address.  During
`WU32NotifyWow(FUN_FINALUSERINIT)`, original `wuman.c:93-145` writes that
shared address and the CallCsr flag into guest structures before it returns to
USER16.

The pinned USER does not wait for `pfnInitTask` before its first desktop
query: its direct path is `FS:18h -> TEB+5ch -> DESKTOPINFO+8`, adjusted by
`TEB+60h`, then dereferenced as a WND.  The recorded all-zero view reads DOS
IVT bytes and faults at `0337:011F`.  Thus `pfnInitTask` is too late to create
the initial desktop representation.  This is an ordering fact, not a proposal
to alter USER.EXE.

The existing page-domain only allocates SHAREDINFO, SERVERINFO, the free
HANDLEENTRY chain, TEB and CallCsr backing.  Its `set_client_desktop` safely
publishes a *complete already-created* desktop pointer/delta pair and clears
the cached HWND/PWND fields, but has no product caller.  Its active-domain
`publish_handle` deliberately refuses a metadata-only entry because `phead`
still holds the free-list successor.  The refusal is correct, but confirms
there is no current desktop/WND producer.

### Cleanup has two distinct original meanings

Original `wuser.c:3441-3459` calls `pfnWOWCleanup` for module cleanup with a
zero task ID plus module selectors.  It must release module-owned classes and
subclass associations while retaining a live task/thread.

Original `wkman.c:1691-1738` calls `W32FreeTask`, performs task-completion
notification, then invokes `pfnWOWCleanup(instance, htask16, NULL, 0)` only
when task cleanup was not already performed, removes the task from the task
list, and closes its thread handle.  Therefore only the nonzero task-cleanup
shape may retire task-local guest client data; neither generic module cleanup
nor a failed cleanup may do so.

The current `registered_cleanup` always invokes the module-shaped
`wow_user_task_lifecycle_wow_cleanup`; the separate
`wow_user_task_lifecycle_cleanup(task_id)` and `dispose` routines are reached
only by native fixtures.  No production caller presently binds either to the
original W32 task-exit or worker-loss path.

The current helper shapes also rule out a tempting but incorrect wiring:
`wow_user_task_lifecycle_cleanup(task_id)` calls `wow_user_cleanup_bound`
itself before freeing its task record.  Calling it *after*
`wow_user_task_lifecycle_wow_cleanup` for the same nonzero task would run the
original cleanup operation twice.  S2 must instead factor one task branch
which receives the original `(instance, task_id, selectors, count)` arguments,
calls `wow_user_cleanup_bound` exactly once, then—only on success—removes the
task record, drops message/queue resources and invalidates its guest objects.
The zero-task module branch retains only module cleanup and never removes a
live task.  This is the required `registered_cleanup` dispatch; it is not
permission to call the current two helpers in sequence.

A current-source recheck linked the formally selected provider lifecycle
object with `wow_user_task_lifecycle_fixture` and produced
`WOW_USER_TASK_LIFECYCLE errors=0`.  It proves the helper's native
failure/retry cleanup contract still works, but deliberately does **not** add
a product caller or establish a guest task-exit route.  It remains evidence
for B4's required rollback behavior only.

## Design conclusion

S2 has one finite, source-shaped lifecycle with five ordered phases:

| Phase | Required owner/boundary | Required operation | Retirement rule |
| --- | --- | --- | --- |
| B1 bootstrap | worker-local page domain, before `WU32NotifyWow` returns | Allocate source-pinned guest desktop, DESKTOPINFO, initial desktop WND/CLS and typed HANDLEENTRY backing; publish a valid `pDeskInfo/delta` pair atomically with cache invalidation. | On failure, return registration/final-init failure; do not publish a partial view. |
| B2 registration | original `W32Init`/`WU32NotifyWow` and the 21/20 ABI | Return the guest SHAREDINFO only after B1 backing is valid; retain original direct-data and callback shapes. | The output table may not advertise a successful slot whose backing is absent. |
| B3 task creation | original `WK32InitTask` plus `pfnInitTask` | Bind each real task/thread to the pre-existing desktop, then publish task-local class/message/callback state before creation callbacks. | Failure unwinds only objects created by that task; bootstrap desktop remains for the WOW process. |
| B4 module/task cleanup | original `wuser.c` versus `W32DestroyTask` | Keep module cleanup separate; use the task-ID branch only after original cleanup succeeds to invalidate task bindings, callbacks and task-owned handles. | Do not retire the shared desktop merely because a module unloads. |
| B5 worker loss | `ntvdm-exe` session teardown, after original task cleanup attempt | Cancel callback leases, clear TEB desktop/cache fields, retire all guest handles/backing, then release page-domain mappings. | Never free mapped backing while guest TEB or a callback still references it. |

The B1 producer is a bounded `ntvdm-exe` worker-local binding because it must
write CCPU-visible guest linear backing.  It is **not** a new USER server:
the representation uses the recovered original layouts and ordering, while
modern USER32 remains the native window/queue owner.  No host HWND, native
pointer, host TEB or USER server structure is written into guest memory.
Selected original desktop/client source is evidence for layout, publication
and withdrawal ordering; importing the recursive USER/CSRSS server remains a
stopping boundary.

`delta` must be nonzero: original `client.c::DispatchClientMessage` asserts
that condition, and `USER.EXE` treats desktop/handle pointers as server-view
addresses before subtracting it.  The page-domain setter rejects a zero delta
and its focused fixture proves that rejection preserves the prior context.

## Required S2 evidence

- **X01:** first immutable `GetDesktopWindow` after final USER init reads a
  valid guest WND through the source-pinned pointer/delta relation; a forced
  B1 failure leaves no partial pointer or typed handle.
- **X02:** creation callback observes the completed initial desktop/task
  relation; repeated direct reads without a WOW thunk retain a valid view.
- **X03:** task cleanup and module cleanup are exercised separately; module
  cleanup cannot withdraw the live task/desktop, and failed task cleanup
  preserves its backing for retry.
- **X04:** worker/broker loss clears the guest TEB and retires handles before
  page-domain disposal; no callback lease survives release.

S2 may implement only after it names each source layout, the exact bounded
producer and an end-to-end test for these four observations.  This record does
not claim that any current WOW16 application has passed.

## Confidence and follow-up

Confidence is high for the ordering and cleanup distinction because each is
directly present in selected original caller bodies and the current no-caller
scan.  A current-source native lifecycle fixture linked the selected provider
object and emitted `WOW_USER_TASK_LIFECYCLE errors=0`; it verifies its
failure/retry helper behavior but not guest reachability.  The exact minimal
initial desktop object set and the safe native-object relationship still
require S2's bounded feasibility implementation; they are not left as an
unassigned edge.
