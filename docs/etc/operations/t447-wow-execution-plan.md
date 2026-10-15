# T447 WOW32 execution working plan

## Admission and boundaries

### Next S2 group after E: returned-object publication

The post-P13 source audit in the window-graph evidence identifies the bounded
next group: parent/owner-valued window-word queries, SetCapture/ReleaseCapture,
previous-HWND mutation results and EnumProps callback publication. Preserve
original thunks, native result/last-error and existing callback conversion.
Use the same graph transaction and native-call owner; do not add a second
capture manager, blanket integer-to-HWND conversion or query algorithm.
All affected positive/negative/reentrant/lifetime cases must be connected
before one grouped build and the full per-P release gate. Current P13 remains
the verified runtime baseline. This source-audit checkpoint is not runtime
completion and does not close S2 or admit S3.

### Approved E observation-view repair group

Owner explicitly approves route E and the external-window observation
semantics, and requests complete design, grouped implementation, build/test,
coherent publication, commit and push. This supersedes the pending-choice
record in the graph audit, not its evidence or the full S2/T447 obligations.
No new S is admitted. Guest images/code/data hot patches are prohibited;
the intended additional original-mirror diff is zero.

#### Retained owners and representation

Original USER16 local query algorithms and layouts, original WOW conversion,
callback execution and task scheduling remain unchanged. Modern USER owns
native windows and menus. The existing worker page-domain owns the copied
NT4-layout view, identities, references and delayed physical reclamation.
WOW32 owns observation and admission at existing execution boundaries. No
new scheduler, periodic publisher, global WinEvent hook, CPU interception,
native window manager or second persistent object registry is introduced.

The unavailable original dependency is desktop.c's server-owned desktop-heap
section and its lifetime contract. usercli.asm proves the relevant compiled
local readers have no runtime fallback. The preceding graph audit records
why original mapping, query-only refresh and memory-interception alternatives
are not the selected bounded route. This owner-approved view is an explicit
compatibility exception, not a claim to have recreated original live memory.

#### Observable contract

External existence, relationships, geometry/style and published menu data
are observations, not a synchronous live mirror. They can remain unchanged
between serviced host boundaries, with no millisecond freshness promise.
Native operations still use full native handles and native failure results;
copied-memory retention never grants ownership or pins a native HWND.
Actual PID/thread identity classifies observations; observed-versus-managed
storage ownership is not a process-identity test. Desktop and mixed-process
relations are observation boundaries, not a promise of native global atomicity.

Worker-owned creation, destruction, callback targets, menu attachments and
native mutations must publish their required state before guest resumption.
Preserve pre-link creation and post-unlink terminal windows, owner visibility,
class/WOW words and original task ownership. An observed native control in
the worker is not exempt from this synchronous requirement.

#### Capture, merge, validation and commit

1. At an existing serialized host/guest boundary, obtain a call-local copy
   of the already committed view through the page-domain owner. This is
   transaction scratch, not another retained cache. A cold view has the
   existing desktop root and required enrolled objects, not a fabricated
   successful full desktop enumeration.
2. An external refresh is optional observation work. A failed/disappearing
   external sample cannot reject an otherwise valid local callback. Retain
   the committed external view when a replacement cannot be formed; do not
   retry until the desktop becomes stable. Required local state is refreshed
   separately and must not fall back to stale local data.
3. Merge all live worker objects plus current creation/terminal/discovery
   roots into transaction scratch. Refresh affected local parent/child/next/
   owner links and properties, not only the callback WND. Newly referenced
   external identities require a readable observation before their address
   is exposed; vanished external identities are unavailable observations,
   not a successful native window. Do not alias a different full generation.
4. Validate the composed view structurally, independently of later native
   desktop changes: valid represented targets, unique low-word generations,
   terminating next/parent/owner chains and desktop termination. Do not
   impose symmetric child membership on unlinked creation/terminal nodes.
   A conflict involving stale external edges must discard/repair those
   observation edges in scratch, never change a required local edge or
   fabricate a native mutation. Replacing an observation list is one
   structural transaction; no half-old/half-new published chain is allowed.
5. Prepare all new WND/CLS backing and retention before commit. Commit full
   identities, links and copied fields together with guest execution excluded.
   The worker consumes captured external descriptors without re-querying
   every external object as a final global freshness gate. Required local
   object identity/state checks remain. Dispose of uncommitted scratch on
   failure; preserve prior publication and the original failure code.
6. Withdraw replaced/expired observations only after surviving incoming
   edges have been redirected or removed in that transaction. Managed
   objects retain their terminal owner. Retain borrowed backing until the
   last reader releases it; successful logical withdrawal is not undone by
   a deferred physical free. Preserve original dead-window terminal fields.

Structural consistency is not proof of one simultaneous native snapshot.
In a mixed relation, external edges may lag or be unavailable under this
approved model. Persistent local inconsistency is an actual adapter error,
not an external-observation excuse. Local-only cycles are rejected rather
than repaired by inventing hierarchy.

#### All consuming boundaries, menus and completed operations

Apply the same required-local/optional-external boundary to window and dialog
callbacks, W32InitDlg admission, WU32StdHookProc, tagged CallWindowProc,
discovery/enum callbacks and native-call returns. Do not leave a second
whole-desktop gate in an adjacent entrance. Original callback code executes
once; a real denied admission is distinct from a successful zero result.

Keep call-end responsible for restoring task/execution ownership. Keep native
result/error and a removed MSG as separate captured values until restoration
and required publication finish. Optional observation failure does not replace
them. No replay of PeekMessage, native mutation or application callback.

Menu trees require a complete prepare/commit transaction too: recursively
prepare referenced items/submenus, preserve attachment versus ownership,
publish before original direct readers and retire only after unlinking.
Current sequential publish_tree success on children is not whole-tree
atomicity. Refactor that path within this same group rather than silently
claiming the window transaction already covers menus. External menu refresh
may lag; own mutation and callback menu payloads remain required.

Menu responsibility is not inferred from cleanup ownership: public USER has
no HMENU-to-PID query. Creator enrollment, known local window attachment and
local GetSystemMenu establish required freshness; descendants inherit it.
The existing menu entry and page-domain index retain this responsibility,
without acquiring native destruction ownership. Observed native windows are
included, not only managed WW windows. Unavailable external incoming menu
references are cleared in the same logical retirement commit; required local
references instead block retirement until their unlink is published. Original
readers guard a null submenu. Failed physical MENU/ITEM frees retain a retry
obligation after logical withdrawal, never a half-cleared published object.

Pre-operation resource failure returns the existing failed admission/result.
After an irreversible native side effect, a required-local publication or
execution-restoration failure must not be reported as an ordinary native
failure and must not resume guest against unsafe state: use the existing
worker fatal-error/termination boundary with the captured operation/error
diagnostic. No automatic retry, native rollback guess, silent callback drop
or continuation with stale required-local state. Deferred physical freeing
alone is not this fatal condition. Tests must distinguish all these cases.

#### Files, ABI and replacement inventory

WOW32: wow_user_object_bindings, wow_window_words_binding,
wow_user_discovery, wow_user_task_lifecycle, wow_dialog_creation_binding,
wow_hook_callback, wow_user_registration_bridge, wow_menu_client_bindings
and wow_menu_native_bindings with their headers as required.
Worker: mvdm_softpc_wow_page_domain and its private header/export selection.
Extend only the synchronous copied view contract, never pass host pointers
into guest state. Rebuild worker/WOW32 together. Remove superseded global
stability rechecks and post-result failure coupling in the same delivery;
do not retain parallel legacy/new routing switches. Keep original algorithms
and existing ownership tables. No guest, broker wire or public CLI change.

#### Group acceptance and delivery

Checked-in fixture cases must cover fresh/cold view, external churn and
disappearance, mixed relations, local native controls, local mutation while
external refresh fails, generation replacement, structural cycle rejection,
creation veto/unlinked windows, nested destruction, deferred frees and
allocation failures before/after native side effects. Cover menu subtree
rollback/attachment/destroy, callback exactly-once/zero-result distinction,
and a removed message retained exactly once. Tests use the selected real
provider and CCPU backing; independent synthetic NE probes do not modify
original media. A separate native churn process proves the external boundary.

Only after the complete group is connected, run owned background incremental
build and grouped regressions. Then real Win16 discovery/CBT, independent
WINMINE/SOL/WRITE frontiers, three style modes where applicable and Console17/
Window17. Retain prior failed SOL/F2 evidence; do not treat later passes as
causal explanations. Build/runtime artifacts stay under build, isolated
execution stages under the approved O:/tmp owner. Preserve assets/winnt.zip.

Perform the second-role diff/source review, documentation/link gates and
guest/mirror-diff audit. Seal one tested thirteen-image package; preserve
recoverable previous binaries, publish to assets/release and configured
O:/winnt/system32, verify hashes/PE identities and unchanged guest/config
companions, then commit/push reviewed main changes. Do not publish a partial
or regressed package. This delivers the repair group, not the remaining
WRITE/control/message/task-termination obligations of the complete T447.

#### E implementation and verified delivery

The candidate now copies the committed WND view through its existing owner,
refreshes local rows and explicit roots, and resolves new references in
call-local scratch. PID/TID observation metadata is attached to the existing
backing-lifetime record, not a second identity registry. Dead observations
are withdrawn with incoming edges; a disappeared external endpoint is
unavailable rather than rebound to another generation. Boundary edges to
such endpoints can become null without mutating native USER. Only external
edges can be cut to resolve a mixed sampled cycle; local-only cycles fail.
The worker independently validates full identities and terminating chains,
then commits captured fields without a second native-desktop stability scan.

Menu capture prepares complete child-before-parent trees and their cleanup
records before publishing any child. A copied batch contract resolves all
submenus and prepares storage before a non-fallible commit. Deferred physical
free records retain failed release obligations. Required local attachments,
owned menus and explicit mutation/callback roots remain synchronous; failure
of an unattached borrowed observation can retain its preceding complete tree.

The post-native failure boundary is explicitly worker fail-fast, using the
existing product convention for broken ownership, not opennt_exit_thread:
that latter path only exits a task and cannot protect the shared worker view.
The diagnostic records the publication/restoration stage and error, with
STATUS_ASSERTION_FAILURE and the error parameter in the fail-fast record.
It does not replay or turn an already consumed MSG/native result into a
normal API failure. This is a project adaptation failure policy, not a claim
that original NT4 failed this way or an original-source algorithm change.

The completed E group is S2 P13. observation-e-r5 passes the worker/WOW32
build and all12 boundary cases; observation-e-apps-r4 passes independent
guest discovery/CBT, SOL/WINMINE three modes, WRITE's existing frontier and
both17-route DOS matrices. observation-e-publish-r1 synchronizes and verifies
the13-image package at both prescribed locations. Only ntvdm/WOW32 change;
the46 guest/config files remain identical. Earlier r1--r4 failures and the
second-role review remain in the window graph audit. This delivers E, not
full S2/T447 closure or a claim that WRITE editing is complete.

Current implementation disposition: the continuation recorded at the end of
this plan supersedes the older blanket-freeze chronology below. Dependency-
complete repairs may proceed after their source review. The InitTask group
has passed its lifecycle transaction fixture; its new production registration
entry fixture is not yet linked (unrelated callback-table dependencies pull
the full WOW startup closure). This is a test-harness gap, not a runtime pass
or proven runtime failure. Do not add production stubs to make it pass.
The callback resolver group now removes the autonomous current-mode lease
route and compiles successfully; see the boundary-design implementation
update for its exact budget and remaining acceptance. S2 P1 32876c415 now
publishes the bounded menu/lifetime baseline; this does not close S2. The
window-graph audit records WRITE's improved startup and remaining contracts.

The owner accepts/closes T446 and admits the former queue head. Current
owns the sole active S2 brief after S1 delivery f950ffd38. This plan inherits W01 from the
[candidate](../../proposals/proposal-wow32-message-task-execution-001.md)
and the [WOW program](../../proposals/proposal-wow32-production-completion-002.md),
without resetting stable capability IDs or declaring inherited work complete.

The released runtime still uses modern USER message/reply transport. Owner
now approves evaluating original same-worker WOW-to-WOW SMS delivery as a
bounded design alternative. Native destinations, external callers and implicit
native sends retain an explicit interoperability obligation. Original MVDM
thunks and composable OpenNT task algorithms remain their owners. No second
scheduler, guessed private USER identity, guest patch or broad USER-server
import is authorized; production implementation awaits the integrated review.

## Sequence

Latest owner clarification permits clean project implementation when original
integration would impose excessive architectural complexity or error risk,
while requiring equivalent semantics. Original reuse remains preferred.
This supersedes the pending implementation-policy decision, not the reply,
reentry, lifetime or single-executor acceptance contract. Record the exact
original behavior, integration cost, replacement boundary and equivalence
argument before implementation. Do not equate different internal structures
with different semantics, or claim equivalence merely from successful apps.
No semantic-waiver approval is inferred. The integrated design brief and
mirror budget still precede production edits.

The preceding owner approval admits the bounded original SMS design evaluation
requested after the minimal-subset review. This supersedes the earlier
native-only research limit below, not the implementation freeze. Audit the
complete send/receive/reply/timeout/death and parameter-lifetime dependencies,
including how original pending sends are serviced during native waits/modal
loops. Do not treat a new queue plus a wake notification as a complete design.

Owner approval following the message-identity decision request admits only
design evaluation of actual native callback/wait/reply handoff with retained
original task selection and native transport. See the
[native execution design](t447-native-execution-boundary-design.md).
It does not approve implementation, hidden ordering changes, a second
scheduler or the previously rejected local SMS transport.

Owner subsequently approves researching explicit association-bearing envelopes
over native USER delivery and grants standing authority for similar necessary
design-research decisions. Do not ask repeatedly for that research boundary.
Retain original-source priority, immutable guest and the single original
scheduler. Deliver the requested integrated design brief and mirror-diff
budget before production implementation; research approval does not certify
envelope transparency or permit replacing native transport wholesale.

Owner's subsequent correction requires a system-wide T447 execution audit
before further implementation, not a sequence of individually discovered
patches. S2 remains the only active packet; its first phase now reconciles
the complete C01--C03/C09/execution-C10 graph, including S3 dependencies.
Existing unverified callback edits remain candidates, not delivery inputs.
No further production edits or runtime discovery runs precede that review.

The audit must enumerate selected build roots and registered entrypoints,
trace forward calls and reverse release/unwind paths, and record original
owner, current provider, state ownership, unavailable dependencies and
superseded autonomous logic. Classify each edge as fully traced, confirmed
gap, unresolved dependency or explicit external-owner boundary. Do not treat
symbol presence, a successful fixture or one repaired path as coverage.
Before implementation resumes, reconcile the inventory with all inherited
checklist rows and produce dependency-ordered repair groups with deletion
sets and acceptance cases. Unresolved inputs must be explicitly isolated;
they cannot silently become presumed-correct prerequisites. Single-person
second-role review checks the whole graph, not only individual patches.

Owner clarification: find recovery gaps primarily by tracing source, not by
repeatedly running tests to discover the next patch. Trace the original
caller/owner contract through selected production bindings, implementations,
and return/cleanup paths; enumerate missing and divergent edges and their
dependencies before choosing repairs. Tests are acceptance after connecting
new code, not the gap-discovery method. Already-started baseline observations
are retained as auxiliary evidence only and do not dictate implementation.

The owner further requires zero original-source diff where composable, or
the minimum necessary original-source diff. Do not author substitutes for
original logic that can be connected. Each proposed repair must first name
the existing original function, its selected/unselected body, and exact
dependency preventing direct composition; prefer connecting that body through
the smallest same-shaped seam. This applies to teardown as well as execution.
When original logic is proved composable, connect it and delete the equivalent
project-authored replacement in that same repair; do not retain duplicate
behavior or defer the replacement as future debt. Finding an original symbol
alone is not proof of composition: account for its actual state and lifetime
dependencies before declaring that condition satisfied.

| S | Scope | Required result |
| --- | --- | --- |
| S1 | Source-first recovery-gap inventory and dependency design. | Finite C01--C03/C09/execution-C10 original-owner/production-binding/implementation/cleanup matrix; classified missing or divergent edges, dependency order and acceptance cases. Existing E91/E92/E93 and application observations are auxiliary evidence. No production repair. |
| S2 | Callback and object/shared-view foundations. | Production creation/first callback/update/rollback/normal retirement, original resolver and entry scopes; paired startup/teardown and real guest evidence. |
| S3 | Basic standard controls/dialogs/menu dependencies. | Internal children, original EDIT storage/word-break/dialog DS and MENU-01 parser/graph/mutations; normal SOL/WRITE initialization, retained WINMINE. Promote prerequisite message work before this exit. |
| S4 | Full message identity/reply/reentry. | Real Win16/Win16 and both native directions, explicit/implicit mixed messages, early/duplicate reply, waits and nested execution; no semantic waiver. |
| S5 | Task lifetime and NT Monitor per-task termination. | Original hung/exit ownership, request-to-HTASK identity and reuse protection, sibling remains usable; explicit worker termination stays separate. Earlier cleanup is not deferred here. |
| S6 | Final acceptance and handoff. | Reconciled owned matrix, source/minimal-diff audit, same-package regressions, coherent publication and delivery; no unfinished implementation backlog. |

Only S2 is currently active. Later stages require sequential admission with
exact files, source recovery decision and acceptance cases. Replanning must
preserve all obligations rather than move unfinished implementation to S6.

## S1 inherited inputs and proof levels

- [T422 S3 checklist](../evidence/m0-t422-s3-message-task-checklist.md):
  preserve every unchecked capability, original taskman handoff and cleanup.
- [T422 lifecycle ledger](../evidence/m0-t422-s2-user-client-lifecycle-ledger.md):
  E91 native tests, E92 early-reply failure, E93 mock ordering are distinct.
  Current auxiliary reproduction records exit95 after early native reply
  returns114 but WOW execution remains with the receiver; see S1 evidence.
- [Coverage ledger](wow32-recovery-coverage-ledger.md) and
  [run protocol](m0-t422-s2-run-evidence-protocol.md): reuse existing audited
  source slots and test infrastructure; new outputs use build/M0-T447/S1.
- [T430 handoff](t430-non-wow-contract-plan.md#known-wow-handoff): G03 task,
  dispatch and callback-return proof belongs here. G03/G13 shared view stays
  W02; WRITE's initial provider investigation stays W03 until evidence proves
  a different owner. An OOM message does not establish heap failure.
- [T446 release evidence](../evidence/m0-t446-s1-carrier-visibility.md):
  accepted ca0abb7c4 package, independent prior frontiers and playable WINMINE.

The matrix records caller/provider, actual production wiring, exact test
entrypoint/input identities, normal and negative assertions, reentry,
cleanup, evidence level and remaining gap. Mock/native evidence cannot be
promoted into guest proof. Unavailable or unexplained outcomes remain open.
No runtime checks are claimed by this admission record.

## Phase and delivery constraints

### Approved individual WOW task termination

The owner explicitly approves both NT Monitor operations: select a WOW
worker to terminate the worker, or select a Win16 task and Delete that task.
S2 remains the only active packet. S5's implementation brief must include
the bounded NT Monitor/NTSRV/worker/WOW32 control and identity edges.
Suspension and individual DOS task termination are not included.

Reuse canonical VDMTerminateTaskWOW's task-targeted contract and
W32HungAppNotifyThread/WK32KillTask execution and cleanup, subject to the
selected CCPU40 dependency audit. Do not copy remote function pointers into
broker records, invent a guest exit algorithm, or use TerminateThread.
Original escalation can affect another current task or the whole worker;
targeted deletion must not do either without distinct confirmation.

The current MANAGEMENT_WOW_TASK projection contains WOWRECORD.iTask, a
BaseSrv request ID; the original terminator consumes TD.htask16. W32Thread
retains both as VDMInfoiTaskID and htask16. Bind at their actual owner with
worker/task incarnation checks; never cast the displayed number to HTASK16.
Audit guest-launched children, separate WOW and internal WOWEXEC explicitly;
the BaseSrv request list is not proof of an exhaustive guest task list.

Acceptance runs two real tasks in one worker, deletes either application,
observes task/thread/window/message cleanup and launcher completion, and
proves the sibling remains usable with the worker alive. Cover natural-exit
races, reused handles, duplicate deletion, nested sends, unresponsive tasks,
worker death and explicit worker termination. Request acceptance or the
global hung-event signal is not completion: W32DestroyTask signals that
event before resource/list cleanup. These are required cases, not passes.

### Integrated repair groups (design review, not implementation admission)

| Group | Original owner and change boundary | Required joint replacement / unresolved prerequisite |
| --- | --- | --- |
| Guest callback storage | Original protected-mode resolver and CallBack16; shared session lease mechanism | Remove mode-dependent callback address selection; resolve actual mapped storage, demand-load reentry, reserved lease slots and failure semantics together. No scattered unchecked-caller patches. |
| Callback execution and messages | Original taskman and native USER transport; callback registration/entry bindings | Replace fabricated outer-call association only with a proven receive/reply boundary. Acquire before the first guest access in native callback thunks, not just inside CallBack16. Exact receive association remains unresolved. |
| Task initialization and input membership | Original xxxInitTask/Recalc2; existing lifecycle and native input binding | Replace recalc-only attachment and embedded shared-queue assumptions together. Preserve original bounded retry; after init commit retry only notification. Partial native-link failure and full native-state equivalence remain design gates. |
| Task retirement and targeted deletion | Original W32DestroyTask/WK32KillTask/DestroyTask and hung-task path; existing native window retirement, worker and management boundaries | Complete guest-capable pre-exit cleanup before CCPU context release; release messages/input/task state once; add approved identity-bound task deletion without implicit worker kill. Depends on preceding groups. |

Each group must name superseded code in the implementation diff and pass its
real caller/guest acceptance; none is permission to implement an isolated
leaf while its group prerequisites remain unresolved. The detailed audit is
the source evidence, not a claim these groups are already fully designed.

### Runtime and publication

The investigation traces initialization and execution providers in source;
it does not repeatedly launch applications to choose the next repair.
SOL and WRITE must become normally initialized during basic functionality,
and display their usable normal interfaces during basic display; preserved
historical failures are only non-regression, never those phase exits.
An actual dependency on another queued owner is explicitly promoted in a
bounded packet, not implemented as a temporary local substitute.

S1 admission/closure documentation does not require a new runtime release.
Any subsequent production-code P requires original x86 /MT CCPU40 and affected
tests, all DOS17 routes, independent three-app frontiers and accepted package
regressions, thirteen-image publication to both prescribed locations, hashes,
commit/push and clean workspace before presenting it for owner acceptance.
# S2 implementation continuation: initialized-task notification

The owner's continuation removes the blanket all-groups freeze, not the
requirement for source-driven, dependency-complete repairs. The reviewed
registration/lifecycle group has no dependency on the unresolved SMS identity
design. Original `W32Thread` retries `pfnInitTask` seven times and destroys a
task on exhaustion. The project registration bridge previously committed
`xxxInitTask`, returned FALSE after a launcher-notification failure, then
rejected every retry because the thread was already bound.

Keep that original loop and destruction untouched. The existing lifecycle
carrier records the launch request and pending notification only after
successful original initialization. The registration retry resumes only that
pending notification, before style lookup or parent traversal. The callback
runs outside the USER lock. A call-frame token prevents recursive notification
and distinguishes task retirement/reallocation across the callout; completion
re-reads current binding rather than dereferencing a retained task pointer.
Completed or mismatched requests are rejected. No new registry or scheduler.

Source review of `OpenNtBaseServiceWowStarted` confirms generation, dispatched
task and parent-receipt checks; repeated notification sets the same event and
boolean, so replay after a lost RPC reply does not create another task.
The added mirror diff budget for this group is zero. Existing unverified
`wcall16.c` changes remain separate, not validated by this group's fixtures.
The new `start-notification` fixture links the production lifecycle and tests
identity rejection, recursive rejection, unlocked callback, seven failures,
subsequent success and rejection after commit. It does not establish guest
acceptance or registration-entry coverage; those remain required alongside
retirement and exception-path coverage before this group is considered done.

Local implementation result (2026-10-14): hidden build/fixture process 7828
finished with exit 0. Registration bridge compiled; the production-lifecycle
fixture emitted both `WOW_START_NOTIFICATION ... errors=0` and
`WOW_USER_TASK_LIFECYCLE errors=0 native_direct=4`, then
`WOW_FIXTURE_OK case=start-notification`. Evidence resides under
`build/M0-T447/S2/start-notification/` (`launcher-r3.log`, `stderr.txt`,
`build.log`, `result.exit`). The first attempt failed on conditional directives
inside the CHECK macro and was corrected. The second returned zero but lacked
the new marker on stdout; the third uses stderr and requires that marker.
The fixture retains existing /FORCE:multiple and fixture page-domain/RPC seams;
this proves the tested lifecycle transaction, not an actual guest startup or
an RPC failure injected through registered_init_task. No publication or S2
closure is claimed.

## Cross-workstream dependency disposition draft

Historical draft retained for traceability. The approved audit below replaces
its proposed numbering and transfer boundaries, including reached owner-draw
menu callbacks; it is not a second current plan.

This is a design response to the owner's question about moving prerequisites
from later T packages. It does not renumber S tasks, close S2, remove Queue
entries or admit additional production work. Current remains the authority
for already promoted repair groups. The existing W/C identifiers remain
stable; moving delivery responsibility must not create a second provider.

| Existing owner | Source-established dependency and proposed bounded transfer | Retained later work / acceptance boundary |
| --- | --- | --- |
| W02, C04--C07 and object C10 | WND/CLS identity, names and shared data consumed by original USER16; first-callback visibility, native standard-class association, creation rollback and destruction required by W01. Current already admits specific class/name/storage/projection prerequisites. | Do not claim all W02 complete: external/system/other-task objects, host-originated mutation, geometry, stale reuse and all direct readers still require reconciliation. Any row actually needed for the transferred paths moves with its failure/update/cleanup obligations, not merely its happy path. |
| W06, C08 | Original W32InitDlg/dialog callback ownership, template child WW/CLS, native standard controls and the original EDIT consumer dependencies needed for basic execution. Current admits callback-scope repairs; full dialog/control integration is still open. | Broader keyboard/caret/input operations, hook registration/removal and timers retain their owner unless an exact dependency is proved. The admitted common hook callback-entry repair does not complete all hook functionality. |
| W03 | Original EDIT LocalAlloc/ReAlloc/Lock/Unlock/Size/Free callbacks and dialog shared Edit DS lifecycle, only as reached by the original control/dialog implementation. Reuse original W32Local* and Global* callbacks; no second heap. | General module loading, alias families and module retirement remain W03. Its initial SOL/WRITE diagnostic assignment is not evidence of a heap defect; transfer a proven failed provider, not the OOM symptom. |
| W09 with W02 data publication | MENU/ITEM graph and window-menu association read locally by original USER16, including mutation and retirement. Current already promotes MENU-01 and the named direct-data dependencies; original resource-loading/conversion owners remain reused. | Complete icon/cursor/bitmap conversion, owner-draw/accelerator families and unrelated menu refinements remain W09. Any mutation reached in the transferred graph must be handled now, not deferred as a visual refinement. |
| W05 | Existing class-menu/template resource services are dependencies to reuse. No additional resource-loading defect is established by the latest class projection results. | No blanket transfer of find/load/lock/unlock/free. Promote a missing original boundary only with its caller, exact failure and lifetime evidence. |

W07/W08 display/font work and optional services remain outside this draft.
This does not waive already accepted display/gameplay or any earlier-stage
error handling. W19 cannot receive a failure cleanup path necessary to make
one of the transferred capabilities safe. Three-app startup results are
mandatory evidence, not a replacement for the API/message/lifetime matrix.

The proposed six-stage delivery shape is: retain delivered S1; execution and
object foundations; standard controls/dialog dependencies; full message and
reply handoff; task exit and individual termination; final integrated audit.
These are proposed stages, not newly admitted S identifiers. Existing S3's
approved single-task Delete requirement is retained if numbering changes.
Neither this draft nor a future split certifies local fixture results as a
deliverable. Every production P retains its runtime/publication gates; final
acceptance cannot become a bucket for unfinished implementation.

Before adopting the draft, reconcile each transferred capability with the
source inventory and mark its implementation owner, remaining producer/update/
release gaps, exact guest acceptance and deletion set. Update Current, this
sequence, the program ownership table and affected candidate proposals/Queue
descriptions together. Do not delete an entire candidate because one subset
has moved, or leave two documents assigning the same implementation twice.

## Owner-requested cross-T planning audit

### Audit status and evidence level

Owner approves this audit and its bounded transfers on 2026-10-14. This
section supersedes the preliminary transfer sketch and old S2--S4 sequence.
The active packet is still S2; S1 remains delivered. The owner additionally
requests an audited development checkpoint of all dirty code/evidence before
further implementation. That checkpoint is not a production P or release.
No residual queued package is removed or declared complete by this adoption.

Inputs reviewed: Current, Queue, the W01--W20 program ownership/priority
table, candidate scope/dependency sections, the existing T447 sequence,
the stable coverage ledger, and the S2 source-boundary evidence. The latter
traces original wuwind/walias/createw, dialog/Edit and menu readers through
their current bindings. This is a planning/dependency audit, not a new audit
of every API implementation or proof that all runtime gaps have been found.

### Findings

1. The ordered T split is not a valid strict runtime dependency chain. W01
   assumes W02 foundations, while W02 consumes W01 callback execution. That
   cycle is a planning dependency, not proof of a runtime deadlock. Missing
   W02 foundations must be delivered with the earliest execution consumer.
2. Current S2 includes object/menu/control prerequisites and initialization
   work absent from its short sequence row; S3 also names initialization and
   callback retirement. Admission detail and high-level scope have diverged.
3. Original direct USER16 readers prevent treating shared data as a later
   visual improvement. GetClassName/GetMenu/GetDlgItem require valid producers,
   updates and retirement before ordinary guest code can use those objects.
4. W06 and W09 mix foundational consumers with broader interaction/resource
   completion. Deferring their entire packages delays basic execution; moving
   their entire packages into T447 would unnecessarily expand it.
5. Generic SOL/WRITE OOM diagnostics do not select W03. Transfer the diagnosis
   coordination to the basic-execution milestone, then assign each demonstrated
   failing provider explicitly. No speculative memory-manager rewrite.
6. Every-P regression, phase acceptance and whole-provider acceptance are
   different gates. Requiring all later capabilities before every early P
   makes staged delivery impossible; conversely an inherited error frontier
   does not satisfy the basic-functionality phase. Preserve per-P accepted
   behaviour and require normal SOL/WRITE initialization before closing that
   phase. A replan must state these gates, not silently weaken Current.
7. Whole-closure/minimization work is not a destination for duplicates found
   here. Composable originals displace their autonomous substitutes in the
   same repair. Those later global audits retain only their broader coverage.

### Complete workstream disposition recommendation

Transfer means delivery responsibility, not moving source into another
component or copying an implementation. W keys remain provenance/ownership
identifiers. No row below asserts runtime completion.

| Owner | Recommendation | Exact boundary retained outside T447 |
| --- | --- | --- |
| W01 | Retain full execution/message/reply/reentry and task obligations in T447. | None of the existing W01 obligations is dropped. |
| W02 | Move the dependency-complete object foundation used by basic execution: WND/CLS/desktop/handle identity, direct reads, first-callback publication, updates reached by those readers, stale reuse, rollback and destruction. | Keep residual system/native/external-object and geometry cases only if source review proves them independent of the transferred paths. Do not claim a whole C04--C07 row closed by a narrow test. Retain the candidate until all residual rows are explicitly classified; do not delete it merely to simplify Queue. |
| W03 | Move the reached EDIT local-memory/dialog-data-segment integration with W06 controls, using original callbacks and existing allocation owners. Coordinate SOL/WRITE failure attribution in T447. | General module/loader and unrelated alias families remain; an unknown startup failure is unresolved, not automatically moved here or accepted. |
| W04 | Reuse existing file/environment/OEM behaviour. No new transfer proved. | Complete file/directory, non-ASCII/OEM and rollback coverage remains. Promote a concrete startup dependency only if traced. |
| W05 | Reuse original resource loading for classes/dialogs/menus; repair only an exact missing prerequisite if demonstrated. | General lookup/load/lock/unlock/free and format-independent resource coverage remains. |
| W06 | Move basic dialog lifecycle, standard-control/internal-child integration, EDIT callbacks and their cancellation/release. Keep admitted hook callback-entry repair in T447. | Broader input/caret/keyboard, full hook registration/removal and timers remain; callback entry alone does not complete these families. |
| W07 | Reuse verified GDI services and preserve existing accepted drawing. | DC/drawing/bitmap/palette completeness stays in basic/comprehensive display; no entire-family transfer from an empty/error window symptom. |
| W08 | Retain later font/text/metafile completion. | All selected typography/metafile obligations remain; do not confuse Win16-style font styling with WOW font semantics. |
| W09 | Move MENU-01 graph/direct-reader creation, mutation, callback visibility, failure coherence and retirement required by execution. | Unrelated icon/cursor/bitmap conversion, accelerator and display refinements remain. Owner-draw callback/data reached by transferred menu operations is not deferred merely as visual work. |
| W10 | Retain clipboard package. | Conversion, delayed rendering, ownership and task-loss obligations remain. |
| W11 | Retain DDE package. | Conversation/reply/data and peer-death obligations remain; W01 generic messaging is a dependency, not DDE completion. |
| W12 | Retain Shell/layered Registry package. | No global Registry mutation or duplicate Shell provider is admitted. |
| W13 | Retain Winsock package. | Async/network/cancellation coverage remains. |
| W14 | Retain COMM package. | Serial/device and unavailable-hardware dispositions remain. |
| W15 | Retain printing/spool package. | Loader dependence on a companion module, if proved, does not imply full printing must be implemented before basic execution. |
| W16 | Retain multimedia package. | Original SOUND limitation and remaining device/callback coverage remain. |
| W17 | Retain ToolHelp/debugger package. | NT Monitor task deletion is an explicitly approved W01 product operation, not wholesale ToolHelp/debugger completion. Reuse verified identity mechanisms without duplicating them. |
| W18 | Retain ordinary common dialogs and OLE, still separable from one another. | Basic dialog-template/control support moves via W06, not the complete file-picker/OLE services. |
| W19 | Bring only failure/terminal handling required by the current executing paths with those paths. | Remaining WOW hard-error responses stay here; share the original hung-task mechanism with T447, not a second terminator. |
| W20 | Retain whole-provider acceptance. | It receives completed owner evidence, never missing implementations. Three-app success alone does not satisfy its nineteen-family prerequisite. |

The window-residue investigation remains separately queued unless its exact
cause is traced to a transferred object/execution defect. The everyday
usability checkpoint remains distinct from T447's startup/execution gates:
editing, save/reopen, clipboard and full display are not all moved into T447.
The general error-response proposal must reuse any WOW exit mechanism proved
here and keep its DOS/other response work. Global closure recovery and
minimization reuse this audit's evidence rather than repeating completed
research. Windows3.1 Setup, external launch-loader and DOS-hosted Win3.1
performance proposals have no newly proved dependency and remain unchanged.

### Recommended S boundaries and prerequisite gates

These are approved replacement boundaries, not concurrent active packets.
The six-stage shape remains useful only with the following explicit exits.

| S | Complete delivery boundary | Required exit and limits |
| --- | --- | --- |
| S1 | Preserve delivered source audit. | Keep f950ffd38 and all later corrections; do not rewrite its historical evidence into a full-runtime pass. |
| S2 | Callback execution foundations plus required object/shared-view ownership. | Original resolver/entry scope, class/window creation, first callbacks, mutations, failure unwind and normal retirement connected in production; real guest boundary evidence, not merely nine class projections. Startup and teardown must be paired. |
| S3 | Basic standard controls/dialogs and menu dependencies. | Native/internal children, original EDIT storage/word-break/dialog DS, MENU-01 graph and original parser/mutations connected; normal SOL/WRITE initialization and retained WINMINE behaviour on the ordinary package. Any message dependency needed to achieve that milestone is scheduled before this exit, not deferred because of the table order. |
| S4 | Full message identity/reply/reentry closure. | Real Win16/Win16 and both native directions; explicit/implicit mixed messages, early/duplicate reply, waits and nested execution with the agreed semantic contract. No inferred ordering waiver. |
| S5 | Task lifetime and approved individual termination. | Reuse original task/hung/exit logic, identity/handle-reuse protection and sender/window/resource release; deleting either of two real tasks leaves the sibling usable, with separate explicit worker termination. Basic cleanup from earlier S stages is a prerequisite, not postponed here. |
| S6 | Final T acceptance and handoff. | Reconcile owned capability rows, remaining candidate scopes, source/minimal-diff deletion audit, same-package regressions, publication/hashes and delivery. No new runtime repair backlog is assigned here. |

Internal repair groups are not new S tasks: their tests do not independently
authorize publication or reset the baseline. Split a stage further only when
its independent production contract, exact acceptance and delivery boundary
are demonstrable. Merge adjacent work when neither can be correctly accepted
without the other. This avoids both the current unbounded S2 and a sequence
of one-helper S tasks. Current now carries the adopted S2 exit; no
retroactive closure or runtime acceptance is implied.

### Adoption checklist and independent review

The single-person second-role review checks this recommendation against the
original request: original code first, minimum mirror diff, source-led gap
identification, no guest change, no hidden second scheduler, and no discarded
unresolved message or task obligation. The audit changes no mirror or runtime
code; implementation diff budgets remain group-specific and are not zeroed
by moving a workstream. Preserve the current uncommitted implementation and
its proof levels, including staged helpers not yet production-wired.

Adoption updates Current's one active packet and per-stage exit, the sequence
above, program ownership and phase tables, and affected proposals plus Queue
descriptions in one planning change. For each transferred stable row, record
one implementation owner and test entrypoint; residual broad rows remain open
with their remaining cases. Reconcile the general error-response overlap and
NT Monitor versus ToolHelp boundary. Only delete a candidate when no remaining
capability is assigned to it; no deletion is justified by this audit alone.
Do not announce S2/T447 delivered or publish the staged runtime as a planning
side effect. Approval to audit is not acceptance of the new runtime or of a
relaxed message-ordering contract.
