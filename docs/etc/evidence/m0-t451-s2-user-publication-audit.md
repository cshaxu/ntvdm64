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
| `verify-wow-task-lifecycle.ps1 -Case window-borrow` | environment-blocked before fixture assertions: `WOW_PAGE_DOMAIN_DESKTOP_UNAVAILABLE error=5`, exit 2 | The background session lacks permission to create the fixture's isolated desktop. This is not a pass and not a product failure; it blocks the remaining actual page-domain graph/menu matrix in this session. |

`window-borrow`, `graph-snapshot`, `menu-resource` and `standard-class`
share this isolated-desktop prerequisite. Their prior fixture bodies remain
the S2 acceptance vehicle; no assertion was weakened or bypassed.

## S2 remaining bounded work

1. Run the four isolated-desktop page-domain cases in a desktop-capable test
   session and retain their markers: full external/system graph capture,
   menu attachment/tree publication, typed relation separation and retained
   generation withdrawal.
2. Build the selected x86 WOW32 inputs once against the finalized fixture
   source, then conduct the packet-required coherent ten-image deployment and
   immutable-guest acceptance. This record changes test diagnostics only; it
   does not authorize an unverified package replacement.

These are fixed acceptance gates, not an invitation to add a new object
family. R05/R09/R10 remain S3 exactly as assigned by the S1 ledger.

## Disposable output disposition

The focused run's `build/t451-s2/` tree was removed after this record retained
its commands, markers and environment diagnosis. The reviewed cleanup manifest
covered 282 generated files / 32,803,106 bytes at inventory hash
`80E8B190E8D1A50C3CCCCD4E192D7291F31E455DADA0547F6D3245F9EB06CC7B`.
The cleanup tool rejected no S2 item and did not traverse the stable
`.ninja`/`.ccache` roots. Its final global residual scan found older
unattributed build children; they were not within this packet's authority and
were left intact.
