# T447 S3 control dependency closure

## Decision and scope

The final single-person second-role review closes S3's fixed basic
control/dialog/MENU-01 dependency scope, not T447 or the whole WOW provider.
The runtime baseline is P22 eba5fcfc7, with P23 e5d7d77bb's stronger actual
Win16 interaction proof. The closeout changes documentation only. No new
semantic exception, product binary, original guest or configuration is selected.
This is an implementation/evidence closure, not a claim of new manual owner
acceptance or universal UI compatibility.

The original C1--C5 inventory and six dependency groups are retained in the
[source ledger](../etc/evidence/m0-t447-s3-control-source-audit.md) and
[working plan](../etc/operations/t447-wow-execution-plan.md).
Historical staged statements in the [status snapshot](m0-t447-s3-pre-closure-status.md)
are superseded by the evidence below; they are not newly outstanding tasks.

## Fixed entry and responsibility audit

All run roots below are under build/M0-T447/S3. Existing test entrypoints
are under tests/observation; exact commands, owned processes and input hashes
remain in those run roots and the source ledger.

| Required chain | Actual production implementation / authority | Acceptance and boundary |
| --- | --- | --- |
| C1 direct/template/internal children and first callback | wow_window_creation_binding.c retains exact creation/class facts and original control targets; CBT enrollment precedes dispatch. Original comboini/dlgbegin create their children; rollback releases acquired references. | class-reference-acceptance-r1 native-creation, creation-unwind, standard-class; actual controls/dialog Off/Win16/All. No parent-derived encoding or separate native-control approximation. |
| C2 single/multiline EDIT and word-break | Seven original EDIT units are selected in production-cohort-x86/build.ninja. Original LOCAL*16/pfnWowEditNextWord consume registered MVDM callbacks; scoped edit entry restores task, borrow, metrics and error state. | Actual controls require LocalAlloc/Free callbacks before retirement, three word callbacks with three nested USER16 calls, selection/removal and CRLF. Original30 supplies focused allocation/drawing/failure/unwind tests. Not every EDIT command or font/IME behavior. |
| C3 shared/private dialog DS, creator/modal/default/cleanup | Original client.c GetEditDS/ReleaseEditDS, dlgbegin/dlgmgr/dlgend/dlgmgr2 own algorithms. Actual native dialog procedure/result slots are authoritative under the approved boundary; adapter has no competing current procedure/result store. | Actual dialog cases require4 shared/2 private instances,2 deferred modal ends, failed-child rollback, init destruction and live-on-task-exit cleanup. Keyboard assertions cover Tab, multiline self-dispatch, default and Escape. Native dialog-lifecycle and original30 cover A/W, exception and finalizer boundaries. Not all accelerators or S4 mixed-message ordering. |
| C4 MENU-01 resource/hierarchy/mutation/retirement | Original MVDM thunks and clmenu parser own conversion/parsing; native HMENU owns resources, existing page domain owns copied observations. No second parser. | Dedicated menu-resource case validates16 resource cases,7 parser cases,64 invalid repeats, owner-draw data and6 original thunk cases with allocation recovery. Actual controls test hierarchy/check/detach/remove-preserves-child/destroy. Full menu presentation and unrelated icon/accelerator families remain queued. |
| C5 three applications | Original source paths are selected; WOW/CCPU providers and runtime package are real product inputs, not fixture substitutes. | P22 product: WINMINE/SOL gameplay and normal exit in all3 policies; WRITE four initialization samples per policy. Installed ordinary-desktop WINMINE with complete logon environment also passes. WRITE edit/save/reopen/full exit is not proved and remains the separately planned usability scope. |
| Relationship/object identity, promotion, reuse and retirement | Modern USER owns HWND existence/geometry; existing page-domain owns exact-generation copied WND/CLS/menu views; retained original WND owns behavior-private state. | Current native20 includes window-borrow, graph-negative, class/query/procedure/lifetime, creation and task/process retirement. New32-class assertions retain live/retired borrowers, reject resurrection and recover allocation. Route E external freshness remains the already approved limit, not a new waiver. |
| Native/guest procedure identity and restoration | Native WND slots and immutable guest entries; original class records retain class behavior, native registration remains its creation resource. Original thunk/getset conversions remain selected. | Native window/class procedure and lifetime cases, actual saved-control procedure calls, real dialog and hook callbacks. Full/zero16/signed16 carriers canonicalize through validated bindings; stale-full rejection remains strict. No NT4 interpretation of opaque modern CPDs or install/query/restore transaction. |
| Nonclient and capture/scroll/dropdown | Ordinary native nonclient hit testing is delegated only at the approved boundary; NTW16S uses its drawing-consistent geometry. Original combo/list algorithms retain interaction. | Original default-native fixture and both-width style boundary, plus control-interaction-r3 all3 policies: line/thumb/top scrolling; two dropdown forms programmatic show/hide, capture, release with persistence and second-click close. Not physical pointer tracking, all scrollbar pixel hits or exact NT4 legacy border returns. |
| Callback/native-return/error/failure recovery | Existing task/borrow/native-call owners restore execution before required publication; irreversible restoration failure uses the existing terminal-adapter failure contract. | Native-dispatch-gate, native-send/nested/foreign, creation-unwind, original30 and actual nested EDIT/CBT callbacks. These prove the S3 prerequisite, not early/duplicate replies or complete native message identity. |
| Ordinary and task cleanup | Original control/dialog finalizers execute before identity withdrawal; NTW16S terminal disposal is resource-only. Normal exit and test cleanup are distinguished. | Actual ordinary destruction8/live-at-task-exit2 controls, exact-once task retirement; dialog and16-round common-dialog resource retirement. Owner-driven process termination is not counted as normal guest exit. Requested individual-task termination remains S5. |

The six admitted dependency groups (creation provenance, relationship/lifetime,
procedure ownership, nonclient closure, complete original source selection,
grouped real-guest delivery) are accounted for above. No identified defect in
those public execution prerequisites is transferred merely to close S3.

## Source reuse and minimum-diff review

The original EDIT/ComboBox/ListBox/Button and dialog collaboration bodies are
production-selected, not just compilable examples. The former native dialog
creator approximation, native combo notification bridge, independent default
message tables and duplicated brush service have been displaced and removed
by the original owners during S3; their removals and precise retained seams
are individually recorded in the source ledger. Native window/DC/timer
operations are resource leaves, not substitutes for those original algorithms.
The narrow approved color/default, dialog-procedure, nonclient and style-change
boundaries remain explicit; this closeout broadens none of them.

The selected mirror inventory is278 files:192 byte-exact and86 registered
content-different, zero format-only or missing-registration rows. This is the
selected total, not86 new S3 modifications and not proof of every dormant
function. P20 restored19 formatting-only files; P21 recovered the public SDK
declaration, restored wow32.h exactly, moved material callback diagnostics to
their existing adapter owner and registered the historical ABI/GDI/subset seams.
The fixed-chain review above is separate from that registration coverage.
P22 changes only the project-owned class collector, preserving strict final
retirement and discarding its call-local derived reference set. P23/closeout
add no runtime or mirror diff. Original guests remain immutable throughout.

## Delivery and retained verification

- class-reference-build-r2: Release MSVC /MT Win32 CCPU40 dependency-selected
  worker/provider outputs; unchanged components reuse proved input identity.
- class-reference-acceptance-r1: fresh uninterrupted native20/20, original30,
  MENU-01/resource/style guards and12 actual Win16 groups across3 policies.
- class-reference-product-r1: WINMINE/SOL gameplay/normal exit, WRITE startup
  only, Console17 and Window17 with exact Expected/Actual equality.
- class-reference-desktop-r1 and class-reference-logon-r1: ordinary desktop
  gameplay; complete current-token logon block, no runtime environment filter.
- published-class-reference-r1: recoverable coherent13 backup/replacement and
  candidate/assets/release/O:/winnt/system32 SHA-256 equality.
- class-reference-installed-logon-r1: actual installation all3 policies,
  gameplay and normal completion using its existing guest/configuration.
- class-reference-deployed-boundary-r1: deployed both-width DLL import,
  negative/lifetime/font checks and28-page demo normal exit.
- control-interaction-r3: strengthened real Win16 control group, all3 policies.
  control-interaction-identity-r1 rechecks872 production/build inputs,278
  selected mirror identities and both published13-image destinations unchanged.

These records are not promoted from source/hash evidence to behavior evidence:
only their actual assertions count. No unrelated fresh product rebuild or
runtime replay is needed for the documentation-only closeout. Its final audit
revalidates input identity, retained terminal results, explicit scope/risks,
documentation governance, links, diff, reviewed commit and synchronized push.

## Retained limits and successors

- The source-proven immutable COMMAND EndInit INIT/EnvSiz reuse with a large
  inherited environment remains in TODO. It is not repaired or hidden by an
  allowlist, guest patch or fabricated success; normal logon is separately tested.
- The owner-approved unexplained old common-dialog witness and older GDI16/19
  risks remain recorded. Current passing cases do not establish their cause/fix.
- S4 retains complete message identity, explicit/implicit and mixed traffic,
  early/duplicate replies, native/Win16 directions, waits and reentry. Native
  USER remains the delivery/reply authority; original WOW conversion/scheduling
  remain. No NT4 private-SMS identity is inferred from an outer native API.
- S5 retains per-Win16-task termination/HTASK identity/reuse and NT Monitor
  integration, separate from worker termination. S6 retains whole-T reconciliation.
- Broader queued controls/input/resource/GDI/usability obligations are not
  declared complete. WRITE edit/save/full exit is not an S3 accepted capability.

S3 has no remaining identified finite blocker in its fixed scope. This decision
does not complete the persistent WOW32 objective or waive a future reproduced
foundation regression; such a regression stays with its actual owner.
