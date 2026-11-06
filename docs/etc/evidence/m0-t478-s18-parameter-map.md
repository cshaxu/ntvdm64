# T478 S18 — parameter-map per-call and task owners

Baseline: 995780041. Finding 43 does not establish another valid-call defect.
The existing DIV-347 repairs remain effective; no product or test changes.
The hypothetical same-key cross-task forced-death case remains an explicit
original lifecycle risk, not a proven repaired or impossible case.

## Current paths and original boundaries

InitParamMap initializes the per-worker original cache. AddParamMap identifies
nodes by guest pointer, records the first task on new allocation, and increments
an existing reference before potentially reentrant selector refresh. DIV-347
rejects zero mappings and rolls back that acquired pin on failed or abnormal
refresh. Moving the increment after the callback would lose the required pin.
Find/Get/Delete preserve original matching, alias history and refcount policy.

Current WU32SetWindowText captures the guest key before reentry, records actual
acquisition and finally releases only that acquired reference. Text/CB_DIR
forward/reverse callers likewise pair their per-call acquisition with existing
unthunk/finalizer ownership. Optional map failure must not remove an outer equal
native-address mapping. Existing failure/exception cleanup is not redone.

WK32 task retirement calls FreeParamMap for the exiting task. Its original
comment restricts use to dead-application residual cleanup; it is not ordinary
per-call release. The selected WOW32 map binds Add/Delete/Free/Init to wparam.obj.
Cross-task same-key references retain the first task owner in both original and
current designs. Purging that owner can remove such a node despite references;
this source fact does not establish a legitimate supported callback that keeps
using the dead task's borrowed input. No such runtime case is demonstrated here.
A task owner list or new shared-ref survival protocol would change the original
model, not reconnect a missing original implementation. No such expansion.

MAPPARAM_EXTRA remains excluded and the original sixteen-alias bound remains.
No new tolerance for an application freeing/invalidating a borrowed input is
promised. These limits are distinct from the repaired per-call pin rollback.

## Existing tests rerun

[Receipt](../../../artifacts/mvdm-audit/shard-3/parameter-map-s18-results.json)
records the unchanged verify-wow-parameter-map.ps1 run. It extracts exact current
map/caller bodies and uses the actual finalizer with controlled lower services;
it is not an actual guest or compiled production-object execution. Current code
passes 54 scenarios, with reverse/buffer/forward sizes 148/84/72. Its historical
baseline negative controls fail in the expected named cases, and the selected
original release cache-body comparison passes. Nested refresh borrowing, false
and SEH rollback, nineteen relocations with bounded alias retention, normal and
aborted message handling, and independent-task key purge are covered.

The independent-task case uses different guest keys. It does not prove same-key
overlapping tasks survive forced owner death. Retain that unproved risk rather
than reporting a synthetic or untested case as a product defect or guarantee.

All ten worktree release hashes match S16 package
b4a13bec-806d-48d3-ad50-728178335129. S16's Full result applies to unchanged
product bytes; no new Full, build/publication, guest change, O:/winnt write or
push. S19/finding 47 follows.
