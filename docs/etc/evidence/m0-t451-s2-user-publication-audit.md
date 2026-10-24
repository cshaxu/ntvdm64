# M0 T451 S2 — USER publication source audit and focused evidence

## Scope and method

This record applies the T451 S1 contract ledger to the bounded S2 rows:
desktop/system/external WND and CLS identity, copied graph edges, MENU/ITEM
trees, first native returned HWNDs, and their no-foreign-ownership boundary.
It does not claim S3 mutation/reuse or S4 full package acceptance.

The recovery order was reviewed before changing code:

1. The immutable `src/mvdm/wow16/user/usercli.asm` PMODE32 reader remains the
   direct consumer for the relevant desktop, WND graph and menu queries.
2. The selected original owners remain `src/mvdm/wow32/walias.c` (standard
   class table/classifier), `wumenu.c`/`wmdisp32.c` (menu algorithms and
   conversion), and `opennt-host/windows/core/ntuser/kernel/queue.c`
   (`xxxInitTask`).
3. The existing project adapter is limited to copied identities and lifetime
   coordination; it neither recreates a USER server nor treats host objects
   as NT4 private objects.

No guest source, binary or configuration was changed. No new recovery
exception, native pointer projection, guest ABI or release image is added.

## Producer/consumer conclusion

| S2 row | Original reader and selected producer chain | Authority and failure boundary |
| --- | --- | --- |
| R01 desktop | `usercli.asm` desktop branches → `mvdm_softpc_wow_page_domain_enter` → `page_domain_client_desktop` / copied desktop WND | Modern desktop existence is host authority; the page-domain relation is a copied original-layout view. There is no task ownership or native desktop creation. |
| R03/R04 external graph | PMODE32 WND relationship readers → `wow_user_window_refresh_graph` → `project_graph_attempt` → `project_window_graph` | A full native HWND generation is sampled and validated before graph commit. Parent, owner, child and next remain distinct copied edges. A failed required/local capture is rejected; an external row may retain only its last complete approved snapshot. |
| R06 menu/item | original `wumenu.c` walkers → `wow_menu_native_refresh_message` / `wow_menu_client_refresh_all` → `capture_tree` / `publish_tree` / `window_menu` | Original menu algorithm remains selected. Native USER owns HMENU lifetime; child-before-parent copied publication installs no native pointer in the guest view. |
| R07 standard/system classes | original `walias.c::stdClasses` and `GetStdClassNumber` → `wow_standard_classes_prepare/publish` → page-domain named CLS allocation | Native registration/procedure is host-owned. Only copied original CLS fields are published. Desktop/menu/switch/icon-title retain their original system-class exclusion from normal control admission. |
| R08 returned/callback HWND | original callback/dispatch → `wow_user_window_refresh_message`, hook/registration bridge and task return publishers → `project_graph` | A returned native HWND must bind to a verified full generation. Observed objects never receive project callback, property or cleanup ownership. |

`queue.c::xxxInitTask` was additionally traced because a proposed early
publication point would have run before the protected page domain. The
original body initializes TDB/scheduler state and returns; it does **not**
invoke USER16 callbacks or direct PMODE32 readers before return. The proposed
pre-`xxxInitTask` publication was therefore rejected and is absent from the
worktree.

The subsequent original handoff is the remaining startup edge: `wkman.c`
calls `pfnInitTask`, then returns through its normal task-ready path before it
enters `host_simulate` to run the guest application.  The existing adapter
only performed a complete graph/menu publication after a native call returned
or a later callback.  Consequently a first direct USER16 graph/menu query
could observe only the desktop root.  The active worktree corrects that finite
gap in `registered_init_task`: after successful original `xxxInitTask` has
created the task carrier, but before the callback returns to `W32Thread`, it
calls `wow_user_task_lifecycle_publish_initial_view`.  That function acquires
the existing runtime owner and delegates to `publish_guest_return`; it adds no
second object table, scheduler, native ownership or guest ABI. Notification
retry remains notification-only only after that copied view is complete.
`wkman.c` retries `pfnInitTask` after any failed handoff, including a failed
initial publication. The committed task carrier therefore retains one private
transaction-stage bit: retry resumes the incomplete publication first, then
the original notification; a later notification retry sees the completed bit
and neither re-runs `xxxInitTask` nor duplicates the initial publication. The
normal lifecycle fixture now exercises the publisher with an inactive
page-domain boundary; the desktop-capable graph/menu case remains mandatory
for the actual protected-view path.

The startup transaction is not a desktop-only shortcut. `refresh_graph(NULL)`
starts with the actual desktop root; its native sampling captures the distinct
child, next, parent and owner edges, then `project_graph_attempt` closes every
full-identity edge before one copied graph commit. Thus a pre-existing ordinary
top-level window is included through the desktop child/next chain without a
second enumeration authority, synthetic WND, or host-window creation.

## Selected implementation authority audit

The active S2 change was reviewed by call chain rather than by the passing
fixture alone.  The result is deliberately narrow: it covers the selected
R01/R03/R04/R06/R07/R08 publication and lifetime paths, not the later S3
mutation/reuse scope.

| Concern | Authority and current path | Audit conclusion |
| --- | --- | --- |
| Task admission and retry | Original `mvdm/wow32/wkman.c::W32Thread` calls `pfnInitTask` and owns its bounded retry plus `W32DestroyTask` failure path.  `registered_init_task` only adapts that callback. | Preserved.  The adapter never invents a second task carrier; a retry resumes incomplete copied publication, then the pre-existing notification retry. |
| USER task/queue/class setup | `wow_user_task_lifecycle_init` calls original `xxxInitTask`; original USER establishes the task, current queue and compatibility state. | Preserved.  The adapter supplies finite host carriers and releases them only through the original cleanup/retirement boundary. |
| Window identity and graph | Modern USER is authoritative for actual `HWND`, hierarchy and liveness.  `refresh_graph` obtains a bounded native observation and `project_graph_attempt` validates full `(index,generation)` identities before the existing page-domain commit. | One copied projection, not an independent window manager or cache.  Reused low words cannot alias a former generation. |
| Menu identity and destruction | Modern USER owns `HMENU` and recursive destruction; original `_WOWCleanup` selects creator/task cleanup.  `wow_menu_client_bindings` captures a whole tree before committing copied page-domain rows. | One copied projection.  Retirement only withdraws copied rows after native destruction; it never selects or schedules native destruction. |
| Native-return publication | Existing `publish_guest_return` is the sole graph/menu publisher for a normal native return or scheduled return. | The new startup path calls that exact publisher once; it does not create a parallel startup serializer, object table or scheduler. |
| Failure and retry | The private `initial_publication_pending` bit is a transaction phase of an already committed original task carrier, not an object or lifecycle authority. | On publication failure it remains set; the next original callback retry cannot report notification success before publication succeeds.  On notification-only failure it is already clear, preventing duplicate initial publication. |
| Retirement | Original cleanup/task ordering and native window destruction remain in `retire_task` / `destroy_terminal_windows`; shared-page retirement retains exact handle generation checks. | No S2 change moved destruction ownership to the adapter or to the page-domain projection. |

No selected S2 route was found to substitute a host default control/menu
algorithm for an available original body, or to keep a second independently
mutable USER graph.  The later r13 private-desktop matrix exercised the
complete graph/menu path below; the remaining S2 delivery gate is a coherent
ten-image package and immutable-guest acceptance from finalized source.

## Corrected focused fixture contract

After removal of the project-owned Win16 Style policy, the task-lifecycle
fixture retained a stale assertion: it invoked the plain lifecycle entry and
expected the former invalid-style failure (`ERROR_NOT_FOUND`) before enabling
its class-publication failure injection. The fixture replaces the page-domain
class-storage leaf with a deliberate third-publication failure; without that
injection, a plain lifecycle initialization correctly succeeds. The stale
prelude made the next assertion cascade and did not represent a product
failure.

The fixture now begins with the actual third-publication injection. It proves
that the first two copied standard classes survive the failed admission, the
task binding/hook list rolls back, and the retry publishes the remaining
members exactly once. The isolated page-domain fixture now also emits its
exact desktop-creation error rather than leaving a silent exit code. The
observation runner consumes that marker before its ordinary postconditions and
reports the unavailable desktop as the failure cause; it does not recast an
unrun fixture as a message-replay defect.

## Executed evidence

| Command/case | Result | Contract proved |
| --- | --- | --- |
| `verify-wow-task-lifecycle.ps1 -Case task-lifecycle` | pass: `WOW_STANDARD_CLASS_ADMISSION ... projected=9 calls=10 retry=1 errors=0`; `WOW_USER_TASK_LIFECYCLE errors=0 native_direct=5` | Standard-class partial-publication rollback/retry, original task setup, unlocked native callout and queue/quit ownership. |
| `verify-wow-task-lifecycle.ps1 -Case class-query` | pass: `WOW_CLASS_QUERY errors=0 queries=6 logical=1 native=1`; `WOW_CLASS_COMMIT errors=0 publications=3 rollback=1 original_task=1` | Original class classifier/query and copied class commit/rollback boundary. |
| `verify-wow-task-lifecycle.ps1 -Case task-lifecycle` (active handoff worktree) | pass: 20 compiler/link children exit 0; `WOW_STANDARD_CLASS_ADMISSION ... errors=0`, `WOW_NATIVE_EXECUTION_SCOPES ... errors=0`, `WOW_NATIVE_QUEUE_PRECONDITION ... own_preserved=1`, `WOW_NATIVE_QUIT_ORDER ...`, `WOW_USER_TASK_LIFECYCLE errors=0 native_direct=5` | The initial-view publisher compiles into the selected lifecycle owner, returns with no runtime exclusive lock, and preserves the existing class rollback, native-call, queue and quit contracts. |
| `verify-wow-task-lifecycle.ps1 -Case init-notification` (active handoff worktree) | pass: 24 compiler/link children exit 0; `WOW_INIT_NOTIFICATION errors=0 registration/retry/exhaustion/exception/retirement checked` | The actual registration bridge retains original notification retry semantics. The fixture uses one renamed notification seam while retaining the real launch client for all other selected BaseClient imports. |
| `verify-wow-task-lifecycle.ps1 -Case window-borrow -InitialPublicationOnly` (r12) | pass: `WOW_INITIAL_TASK_PUBLICATION_CCPU errors=0 desktop=1 task_base=1`; `WOW_INITIAL_TASK_PUBLICATION_SCOPE real_ccpu=1 registered_owner=1 normal_cleanup=1`; `WOW_PAGE_DOMAIN_LIFECYCLE_OK` | The real registered `pfnInitTask` returns with the task-visible desktop/base copied view available before guest entry, then completes ordinary WOW cleanup, unbind and owner disposal. The fixture redirects only the authenticated BaseSrv-start notification import; that callout correctly runs outside the protected guest page domain, while the post-return check proves the committed view. This narrow startup row deliberately does not create or claim an unrelated native window; external graph publication remains its independently isolated `window-borrow` row. |
| r13 private-desktop `window-borrow` | pass: fixture exit `0`; `WOW_UNOWNED_VIEW_CCPU errors=0`, `WOW_RETURNED_WINDOWS_CCPU errors=0`, `WOW_PAGE_DOMAIN_LIFECYCLE_OK` | Existing external/native view acquisition, returned-window identity, release and domain cleanup assertions ran on the fixture-owned desktop. |
| r13 private-desktop `graph-snapshot` | pass: fixture exit `0`; `WOW_EXTERNAL_OBSERVATION_CCPU errors=0`, `WOW_EXTERNAL_REFRESH_BOUNDARY_CCPU errors=0`, `WOW_EXTERNAL_CAPTURE_CCPU errors=0` | Existing complete external observation, explicit refresh and terminal-failure assertions ran without a shared-desktop allocation assumption. |
| r13 private-desktop `menu-resource` | pass: fixture exit `0`; `WOW_MENU_PROJECTION_CCPU errors=0`, `WOW_MESSAGE_OPERANDS_CCPU errors=0`, `WOW_PAGE_DOMAIN_LIFECYCLE_OK` | Existing menu tree, item/operand typing and retirement assertions ran with the fixture's selected original menu conversion route. |
| r13 private-desktop `standard-class` | pass: fixture exit `0`; `WOW_STANDARD_CLASS errors=0`, `WOW_STANDARD_STORAGE errors=0`, `WOW_PAGE_DOMAIN_LIFECYCLE_OK` | Existing original standard-class table, storage precedence and retirement assertions ran against the isolated desktop. |

The startup-handoff correction has selected x86 build and its focused CCPU
fixture evidence.  The r13 private-desktop matrix then ran its existing four
fixture bodies without weakening assertions: `window-borrow`,
`graph-snapshot`, `menu-resource`, and `standard-class` each exited zero.
Their retained stderr markers prove actual graph capture, external refresh,
menu projection, typed operand separation, terminal rollback and standard
class retirement.  The earlier ordinary-desktop allocation-invariance result
is not used as acceptance evidence: that fixture intentionally retains
third-party desktop classes, so its isolated-desktop free-page invariant does
not apply to a shared desktop.

## Final package and immutable-guest acceptance

The finalized source was built once as a complete ten-component x86/x64
package in `build/t451-s2/r25-full-component`. Its second incremental build
did not recompile an intermediate (`images=10`, `intermediates=1476`). The
staged package at `r26-candidate` independently validated every declared
machine type and SHA-256.

The immutable guest acceptance then ran from that exact package in a marked,
short physical stage, deleted by the test cleanup. Its retained evidence root
is `build/t451-s2/r29-wow-guest` and records these input hashes:

| Input | SHA-256 |
| --- | --- |
| `WINMINE.EXE` | `633C1328922B4092351E0019B1BA1C6D56C0F0FEA83824CC8B7E2E731B15E80B` |
| `ntvdm.exe` | `8333B6D92AAC64A753998DA10359FEB5AA7FC5729760599A25A6F931A34F2224` |
| `WOW32.DLL` | `475275C6B1D9F4976CC5371860D0EF19F072D07CE11D8884FCBA92B34AF62BF9` |
| `run16.exe` | `C482F54B93C3EC3F1F0DEC1D2D979ED7ACA7DBC1C22A6E7CF2F4343F8DD38C99` |

The result was `PASS flag/counter, reset, open cell, WM_CLOSE and run16
completion`. This is an actual immutable Win16 game run, rather than a host
fixture substitute: it covers the candidate's WOW startup, task publication,
game interaction, orderly close and Run16 completion. It did not edit,
rebuild or configure any guest image.

The final remaining delivery operation is mechanical: preserve the current
asset package, synchronize this exact ten-image candidate to
`assets/release` and `O:\\winnt\\system32`, compare all manifest hashes, then
commit and push. These are fixed acceptance gates, not an invitation to add a
new object family. R05/R09/R10 remain S3 exactly as assigned by the S1 ledger.

## Disposable output disposition

The active focused runs own `build/t451-s2/`. It contains fixture executables,
logs and compiler records only; it is neither a cache nor release evidence.
It must be inventoried and removed with the repository cleanup tool after S2's
remaining desktop-capable test and any resulting package work are complete.
Stable `.ninja`/`.ccache` roots remain outside this packet.
