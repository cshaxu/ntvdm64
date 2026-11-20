# C-owned adapter placement decision

Baseline: `f2ccb177e`. Status: new-component option not admitted after owner review. Continue
under the existing six layers, separating independent B mechanisms from C
subsystem coordination and F product policy. S14 and its existing six-layer rules remain active.

## Concrete problem

P68 delivered independent TLS/lock/binding mechanics into B. P69 verified
that the 114 native-call/discovery/menu roots also reach original scheduler,
WND/CLS/menu projection, borrowed backing and canonical-control retirement.
Those operations are not independent B resources. They must not remain in F
or become callbacks from B to F/C. Their physical EXE/DLL linking does not
determine their source ownership.

The current architecture rule lists only `mvdm-host` and `non-mvdm` in C and
allows only those two same-layer components to depend on each other. Both are
mirror roots. Therefore an autonomous adapter cannot simply be renamed into
a new C component without owner approval. No such move has been made.

Examples from current code:

| Unit | Required knowledge | Correct responsibility |
| --- | --- | --- |
| `wow_user_runtime.c` | TLS, locks, opaque caller storage | B; delivered P68 |
| `native_call_begin/end` | Original task owner, execution scopes, scheduler waits and guest-view publication | C-side subsystem integration |
| `wow_user_object_bindings.c::refresh_graph` | Original object table and pending terminal windows before projection | C-side subsystem integration |
| `wow_window_words_terminal` | WND flags, FNID, retained backing and original control finalizers | C-side subsystem integration |
| `wow_user_task_lifecycle_notify_start` | Launcher request ID, notification result and retry ownership | F product composition; cannot migrate with the whole lifecycle file |
| Session hook admission/detach | Worker/session startup and quiescence | F; resource mechanism remains B |

Pinned OpenNT `windows/core/ntuser/kernel/createw.c::xxxFreeWindow`
(revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`, start1817,
canonical WM_FINALDESTROY1888, cleaned-up bit1914) establishes original
control-retirement responsibility. It does not make every modern property,
projection or native transport operation an original algorithm, and does not
authorize importing the unavailable kernel dependency closure.

## Two placements

### A. Keep the current component table

Split mixed units and place necessary integration bodies in individually
justified original carriers, such as task scheduling in taskman.c and selected
WOW conversion boundaries in their original thunk owners. A missing original
carrier requires the existing source-policy admission, not a random insertion.

This follows current authorization. It adds autonomous adaptation bodies to
mirror files. The complete non-error point/line increase has not been measured;
the three root files' 2,455 physical lines are **not** a mirror-delta estimate
and include product logic that must remain outside. Large increases still
require individual or bounded-batch approval before production edits.

### B. Add a C-layer autonomous companion (unapproved historical option)

Admit `src/mvdm-compat` at C alongside the two mirrors, and extend the existing
C-only same-layer dependency permission to these three components. It is
explicitly **not a mirror**. It owns finite adaptations that need original
MVDM/USER layouts or algorithms but are not product orchestration.

Allowed dependencies: A, B and the two C mirrors. Forbidden dependencies:
common, frontend-base, worker-base and every F component, including invoked
callbacks, TLS accessors, session accessors and copied product policy.
The two historical B-to-C exceptions are not broadened. The six layers stay
in the same order; no new executable, DLL or runtime state machine is added.

Retained original algorithms stay in their original mirror files. Existing
source-derived extracted helpers retain their pinned provenance and signatures.
Independent host mechanisms still belong in B. Products link/compose the C
adapter through the existing package; source relocation does not create a
second runtime instance.

The 44 reachable F objects are a **review closure, not a wholesale move list**.
In particular, launcher notifications and session admission must first be split
from the task integration body. The accompanying private declarations and
actual providers must move together. Moving only headers, or retaining outer
provider tables behind the new component, does not close any dependency.

This option avoids inserting autonomous source wholesale into original
mirror files. It does not promise zero mirror changes: actual includes and
remaining semantic bindings still require measured non-error accounting.

## Implementation and proof after decision

1. Finalize per-function/state ownership for the 114-root closure and its
   imported page-domain providers; separate F request/notification portions.
2. Move actual C-side implementations, matching declarations and tests as one
   batch, keeping one runtime, one scheduler and the existing failure ordering.
3. Reconcile source/include/link/callback evidence. Count remaining outer
   dependencies explicitly; relocating an exported wrapper alone is not
   complete-chain elimination.
4. Measure mirror points/lines and seek any required large-increase approval.
5. Cached full build, dual11 publication, affected scheduler/window/menu/
   retirement/CCPU and immutable Win16 tests, commit and push.

Owner review: the companion is not admitted. Continue under A, first extracting
independent B mechanisms and preserving C algorithms/F product policy. The
registry binding batch demonstrates an existing-B destination without a new
component. The 114-root scheduler/projection placement and any large mirror
cost remain separate unfinished obligations, not waived by this B migration.
