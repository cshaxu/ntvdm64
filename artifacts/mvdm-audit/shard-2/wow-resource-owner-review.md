# Original WOW16 resource conversion and record owner

This frozen shard-2 group covers the pending `wow32/wres16.c` and `wres16.h`
file roles. The [observer](wow-resource-owner-observer.py) independently
checks current and both original edition hashes, the two audit catalogs,
selected WOW32 compile/link and the valid header dependency. Its [receipt](wow-resource-owner-observations.json)
binds ten original function bodies to the current release image, ignoring only
COFF relocation words, and resolves nine caller/target edges (eleven call
sites) to actual released addresses. This proves source and initial code
ownership, not guest execution or complete resource behavior.

| Pending path | Proposed status | Evidence and unproved contract |
| --- | --- | --- |
| `wow32/wres16.c` | partial candidate | Original resource-list, guest callback and menu/dialog conversion owner. Six W32 resource wrappers call its find, load, lock, unlock, size and free routines; original task teardown calls `DestroyRes16`; modal dialog and common-dialog template paths call `ConvertDialog16`. Image contains ten corresponding bodies. Prove module/task identity, reentrancy, repeated lock/unlock, callback failures, corrupt/truncated resource boundaries and allocation cleanup. |
| `wow32/wres16.h` | partial candidate | Original `RES` structure and conversion/resource declarations are an actual selected dependency of `wres16.obj`. Prove guest/host data representation, lifetime and all other callers; a dependency is not evidence of runtime ABI equivalence. |

Original source text retention is 100% for both files. A whole behavioral
restoration percentage is not measurable from linked objects and bounded
historical fixtures. The prior [template review](wow-resource-template-lifecycle-review.md)
records a separate uninitialized-result candidate in `wudlg.c`; that candidate
is not reproduced or fixed by this review. Historical menu conversion tests
use controlled inputs and do not prove real guest resource/task lifecycle.

`FindResource16` stores the module value and resource type in a process-owned
list; failed `LoadResource16` retains the entry until task cleanup according
to the original source comment. `LockResource16` either retains a converted
menu/dialog allocation or a guest alias, and `UnlockResource16` handles these
differently. `DestroyRes16` is invoked by original `WowOriginalFreeTask` with
a task handle; whether this value always matches the resource's module owner
requires a real lifecycle witness. Callback mapping, failure, reentrancy and
ownership must not be inferred from static wiring.

No shared ledger, product binary, installed package, user session, or guest
file was changed. The coordinator must replay the observer before merging;
these are partial candidates, never a claim that DDE/menu/dialog or resource
capabilities are fully restored.
