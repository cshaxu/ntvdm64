# T447 S2 fixed-scope closure audit

## Decision and evidence identity

Audit date: 2026-10-15. Baseline: published P15,
01cb95ad9c0339cde33d9a98feb61880c548b16e. Single implementer, followed by a
separate skeptical review pass. This is a documentation-only audit delivery,
not a runtime repair or S2 closure. The completed P15 delivery is the safe
pause requested by the owner; no build/test was interrupted or edits discarded.

**S2 cannot close yet. Its remaining boundary is A1--A4 below.** Two contain
confirmed unchecked failure edges; two require completion of specific
execution-context/return proofs. Those are not four demonstrated application
crashes. WRITE editing, complete controls and full message semantics are not
additional S2 exit criteria. No new feature is admitted by this freeze.

The scope denominator is F01--F12 and the callback-family table below, at the
P15 selected build surface. Discoveries must map to these contracts. An
unrelated feature goes to its named successor; a newly discovered violation
of the same contract is recorded under the existing blocker, with its actual
caller and acceptance case, not a new open-ended foundation project.

Authorities: [Current](../../states/CURRENT.md),
[execution rules](../../rules/EXECUTION.md),
[proposal](../../proposals/proposal-wow32-message-task-execution-001.md),
[plan](../operations/t447-wow-execution-plan.md),
[original boundary census](m0-t447-s2-message-boundary-audit.md), and
[graph evidence](m0-t447-s2-window-graph-audit.md).
Chronological entries in older evidence are not automatically current defects.

## Evidence keys and their limits

All following paths are under build/M0-T447/S2. The checked-in entrypoint for
F cases is tests/observation/verify-wow-task-lifecycle.ps1 -Case NAME. Its
assertions, not the supervisor exit alone, define coverage. CCPU fixtures
exercise the production provider/page domain but native witness callbacks are
not real guest callback execution.

| Key | Actual retained evidence | What it establishes / does not establish |
| --- | --- | --- |
| E1 | payload-r2/graph-snapshot.out; window-borrow.out; window-reclaim.out | Typed operands, numeric negatives, previous-window returns, nested borrows, retained readers, real native slot reuse, external observation, failed preparation and post-native fail-fast. Does not prove every native callback family. |
| E2 | payload-r2/native-creation.out; dialog-lifecycle.out; hook-order.out | Creation reports callbacks=51, nested=1, rejected=1, vetoed=1, destroyed=7, table_empty=1; paired dialog cache and Hook ordering. Hook-order is a mock ordering test, not guest identity proof. |
| E3 | payload-r2/menu-resource.out; menu-cleanup.out; process-retirement.out; task-lifecycle.out | Parser/ownership/lifecycle cases and startup rollback, cleanup atomicity, refusal/retry, events/native ownership. Refusal sentinels and same-owner retry do not prove terminal callers honor refusal. |
| E4 | payload-r2/thunk-scope.out; native-send.out; native-nested.out | Thunk calls=4, exception=1; ordinary send result=114 and nested callback scheduled=1, owner=1. Does not cover timeout, every native modal entry or mixed-message reply association. |
| E5 | payload-apps-r2/guest-discovery; guest-hook | Real Win16 discovery/property callbacks and CBT nested creation/veto/recovery/unhook. Property result includes signed -7. Does not cover timer/GDI/resource/exported callback families. |
| E6 | payload-apps-r2/sol-Off, sol-Win16, sol-All; mine-Off, mine-Win16, mine-All; frontiers | SOL deal/stock/close and WINMINE flag/reset/open/close pass in three styles. WRITE has visible MSWRITE_MENU/MSWRITE_DOC/MSWRITE_PAGEINFO only, not editing/saving acceptance. |
| E7 | payload-apps-r2 DOS summaries; payload-publish-r1/verified-manifest.json; payload-docs-r1 | Both DOS17 matrices pass; all thirteen host images verified in assets/release and configured system32; governance/diff passed. Packaging success is not callback completeness. |

P15 worker SHA256:
A9290D99DD2B2E36DFE97856B35CB9B8E2036911FC76C3D3C45C41FDB654356A.
P15 WOW32 SHA256:
548670A8B417681B2E6152D114A41DD997E67699E9978C99D6E848A8847FA985.
No new source, mirror, guest or release bytes change in this audit.

Reproduction entrypoints for E2/E5/E6 (all under tests/observation):
verify-wow-creation-boundaries.ps1 -Case hook;
verify-win16-discovery.ps1; verify-win16-hook-probe.ps1;
verify-wow-sol-gameplay.ps1 and verify-wow-mine-gameplay.ps1 with
-WindowStyle Off, Win16, All; observe-wow-frontiers.ps1 -WaitTarget
-Guests WRITE.EXE with the same three styles. Use the sealed E5/E6 package
and the observer/compiler/NASM identities and exact arguments retained in
build/M0-T447/S2/observation-e-apps-r4.ps1 and its owned
returned-object-apps-stable-wait.ps1 invocation. These disposable drivers
compose checked-in tests; they are not new product dependencies. E7 summaries
are payload-apps-r2/Console-17-summary.json and Window-17-summary.json.

## Fixed entry and exit inventory

Source paths in this table are relative to src/wow32-dll/source unless
otherwise specified. PASS means the stated bounded contract has evidence;
PARTIAL is not closure. Every row retains an uncovered boundary explicitly.

| ID / entry contract | Actual implementation and reverse edge | Evidence and disposition | Uncovered boundary |
| --- | --- | --- | --- |
| F01 session bind, registration, init and retry | ntvdm-exe/wow/wow_user_session_binding.c and wow_user_runtime.c; wow_user_registration_bridge.c; wow_user_task_lifecycle.c retain original xxxInitTask and notify retry | E3: startup rollback and task lifecycle; E5/E6 real startup. PARTIAL A3 | Terminal refusal handling is not established by an init or retry pass. |
| F02 original thunk and guest frame | wow_user_thunk_scope.c wow_user_invoke_thunk; mvdm/wow32/wcall16.c CallBack16 uses original frame resolution, snapshots caller fields, releases aliases before host_simulate, resolves return frame | E4 exception scope; E5 real callbacks. PARTIAL A1/A2 | Do not revive the historical early-FALSE claim without checking current CallBack16: the selected body returns TRUE after original execution, not the obsolete candidate lease-failure branch. Complete nonpresent/remapped and remaining family failure evidence is not supplied by E4. |
| F03 WND and tagged CallWindowProc | wow_window_words_binding.c dispatch_bound; callback enter, borrow, message roots/menu publication, DispatchClientMessage, finally reverse cleanup | E1/E2/E5. PARTIAL A1 | restore_callback, borrow_leave and task_callback_leave results are ignored in this finally. Happy-path nested restoration is not a failure policy. |
| F04 dialog callback and creation | wow_dialog_creation_binding.c; original wudlg.c; creation transaction and callback cache/borrow pairing | E2 dialog nested destruction/cache and rollback; E6 dialogs used by apps. PARTIAL A1 | Sweep the same fallible finalizers and pre/post-callback failures as F03; dialog feature behavior belongs to S3. |
| F05 installed Hook callback | wow_hook_callback.c StdHookProc, original hook table type, creation enrollment/finish, typed payload/menu roots | E2 ordering plus E5 real CBT. PARTIAL A1/A2 | Non-CBT task-leave failure returns zero; CBT leave failure changes veto. Neither proves execution ownership restored. Other installed Hook families need context disposition, not new Hook features. |
| F06 window/property discovery | wow_user_discovery.c, original wuwind.c Enum/Find/GetClassName and wuser.c EnumProps callbacks | E1 held=0/1, signed/atom/string/stop/exception/failure/recovery; E5. PASS for these paths | Original static EnumProps guest callback state does not prove recursive/yielding property enumeration; retain that original limitation, do not claim general reentry. |
| F07 other selected callback entries | Selected original CallBack16 callers and registrations in family table below | Source census exists; E5 only proves its listed families. PARTIAL A2 | Timer/modal, GDI/print, common-dialog/word-break, resource/service and exported callback contexts must be accounted for individually. |
| F08 WND/CLS identity and mutation | wow_user_object_bindings.c, wow_window_words_binding.c, wow_user_borrow_scope.c; ntvdm-exe/softpc/mvdm_softpc_wow_page_domain.c | E1 promotion/same backing/stale rejection, create/update/dead withdrawal, reader retention, actual slot reuse. PASS bounded view contract | External relationships use approved E consistent observations, not live NT4 shared memory. No new semantic relaxation. Consumer restoration failures remain A1. |
| F09 MENU identity and lifecycle | wow_menu_native_bindings.c and wow_menu_client_bindings.c; retained original parser/cleanup owners | E1 typed menu operands; E3 resource/cleanup/rollback. PASS bounded ownership contract | All menu functionality is not certified; mutation/rendering/selection completeness belongs to S3. Callback/return failure safety stays A1/A4. |
| F10 native operation return | wow_user_task_lifecycle.c wow_user_native_call_begin/result/end and WOW_NATIVE_CALL_AFTER; creation/menu wrappers | E1 post-native consumed_once=1, worker_exit=c0000420; previous HWND/numeric discrimination; E4 ordinary/nested result. PARTIAL A4 | SendMessageTimeout return cause is not represented in call-end; receiver_id path can enter xxxSleepTask(TRUE,NULL). Timeout result, error and execution-owner restoration require a bounded witness. |
| F11 destruction and ordinary retirement | window terminal/collector; lifecycle cleanup and runtime unbind; ntvdm-exe/win32/thread_start_compat.c | E1 terminal retry; E2 table_empty; E3 refusal/retry; E6 app close. PARTIAL A3 | opennt_exit_thread and thread-return thunks ignore unbind rejection. host_ExitThread releases CCPU context before redirected ExitThread; guest-capable cleanup must not be moved after that release. |
| F12 Get/Peek/wait and return to execution | wow_user_message_bridge.c native_peek/receive; lifecycle message wait and native return scopes; original taskman | E4 ordinary/nested send, E6 interactive apps. PARTIAL A2/A4 | Receiving a callback inside native modal/wait paths must have the right owner. Full sender/receiver message identity, early reply and cross-task sequencing belong to S4; local ownership safety does not. |

### F07 callback-family denominator

This is the finite selected-source census inherited from the boundary audit,
not a request to implement each subsystem. No subsystem feature or new
provider is admitted merely because it contains a callback. A2 must record
registration, inherited/acquired execution owner, original caller result
contract, alias lifetime and unwind for each row. A family unavailable in
the selected product needs proof of that boundary, not a fictitious pass.

| Family | Original selected owner / entry | Present proof / residual |
| --- | --- | --- |
| Window, dialog, Hook | wmdisp32.c, wudlg.c, wowhooks.c, wumsg.c | F03--F05; guest CBT and ordinary application callbacks, not every Hook type. |
| Window/property enumeration and gray string | wuwind.c, wuser.c | Enumeration E1/E5; gray-string context/return still A2. |
| Timer | wutmr.c W32TimerFunc and its registered timer wrappers | Direct CallBack16 does not traverse WND wrapper. Guest DispatchMessage and native modal dispatch are separate paths; modal ownership proof missing. |
| GDI/font/metafile/line and print abort | wgdi.c, wgfont.c, wgmeta.c, wdib.c | Original synchronous callback bodies selected; shared entry/return proof A2. Full graphics/printing features remain their owners. |
| Common dialog and EDIT word break | wcommdlg.c, wmdisp32.c | Shared callback owner/unwind A2; concrete dialog/EDIT behavior S3. |
| Resource/module/memory/subclass and exported callbacks | wow32.c, walias.c, wcall16.c, wcall32.c, wres16.c, wkman.c, wcuricon.c, wsubcls.c | Some exercised incidentally in applications, not per-entry proof; A2. Exported WOWCallback16/Ex must not assume WND entry. |
| DDE/message/foreground-idle/shell/network callbacks | wdde.c, wmsgem.c, wshell.c, wsext.c | Installed-provider/context reachability A2; full messaging S4, subsystem behavior successor owners. wddeml32.c is not selected by this census. |

## Four finite S2 blockers and one integrated remaining repair group

These are closure blockers, not a claim that each caused a particular past
timeout. Do not manufacture success by moving these common contracts to S3/S4.

### A1: fallible callback restoration has no enforced terminal outcome

- Cause: F03's finally discards callback-cache, borrow and task-leave results;
  F05 substitutes a callback result on task-leave failure. The helper can
  return FALSE when TLS/lock state is invalid. F02 also ignores borrow-leave.
- Scope: project callback wrappers and their common task/borrow finalizers,
  including F04 and discovery consumers. Preserve original callback results
  and last-error when restoration succeeds; pre-entry rollback must leave no
  partially acquired owner. Never resume guest/native callback continuation
  after ownership restoration fails. Reuse the existing post-native fatal
  boundary where state is irrecoverable; do not invent retries of consumed work.
- Dependencies: existing E view and task owner; no full SMS identity, new
  control, or guest change. Zero mirror diff is the target.
- Acceptance: held/unheld entry, nested callback, destruction within callback,
  exception, pre-entry failure, and injected failures of each fallible reverse
  edge. Assert original result/error on success, exact once-only release and
  no continuation on irrecoverable restoration. Separate expected fatal-child
  evidence from recoverable failures. Real guest nested callback retained.

### A2: callback-context and resolver proof is incomplete outside WND

- Cause: the WND wrapper is not the owner of every CallBack16 caller. Timer
  registration reaches W32TimerFunc directly; exported and synchronous
  enumeration/service callbacks have different inherited contexts. Existing
  tests do not establish their equivalence or absence from runtime selection.
  This is a coverage/design gap, not proof all these callbacks are broken.
- Scope: finish the fixed F07 registration/context table against selected
  build roots; repair only missing common entry/return ownership or frame
  lifetime at the smallest project seam. Preserve original resolver and
  callback conversion; no subsystem implementation sweep.
- Dependencies: A1's failure contract; original CallBack16/CCPU/taskman.
- Acceptance: every table row has a justified inherited-owner, explicit
  owner-acquisition, or proved unavailable classification with source callers.
  Each distinct reachable ownership mechanism gets normal/nested/failure
  evidence through its actual provider, including real guest timer/modal and
  a synchronous non-WND callback. Nonpresent/remapped callback resolution and
  failed frame acquisition must retain original exception/output semantics.
  Unchecked original caller results cannot be exposed by an added early return.

### A3: retirement refusal is lost at terminal callers

- Cause: wow_user_runtime_unbind preserves state and returns FALSE when
  retirement cannot finish, while opennt_exit_thread and ordinary thread
  wrappers discard session_thread_unbind's result and terminate anyway.
  A retry-capable callee is not a retry-capable terminal caller. Normal app
  close passing does not cover this rejection. Original host_ExitThread calls
  ccpu386exitthread before ExitThread, constraining cleanup placement.
- Scope: project thread/session terminal boundary and existing lifecycle
  disposal callers. Trace ordinary return, explicit exit and exception paths;
  keep callback-capable cleanup before CCPU release. Preserve original normal
  task cleanup and do not turn this into S5 targeted termination. Do not retry
  guest work after its CPU context is gone or silently abandon live bindings.
- Dependencies: A1/A2 entry safety and existing retirement owner; no NTMON or
  broker identity changes. Any unavoidable mirror hook needs recorded minimal
  diff justification before implementation; no guest changes.
- Acceptance: actual terminal callers with live-object/refusal injection,
  normal application exit, startup failure and exception exit; assert removal
  of task/window/class/input/wait references exactly once, no dangling TLS,
  no post-CCPU guest callback, preserved sibling operation where applicable.
  Fixture retry and observer force-cleanup are not normal-exit evidence.

### A4: native completion restoration needs bounded failure evidence

- Cause: the receiver_id return branch restores through original taskman,
  including xxxSleepTask(TRUE,NULL), without a native completion-cause field.
  Ordinary/nested send tests do not prove timeout/failure/receiver-loss return
  ownership. Source indicates a risk, not a demonstrated timeout deadlock.
- Scope: common native begin/end and actual wrappers, including message peek,
  caller-held lock, result, last-error and returned-object publication. Retain
  post-native fail-fast rather than replay/suppress a completed native call.
  First establish exact contract for success, timeout, native error, exception
  and receiver loss; repair only proven local restoration defects.
- Dependencies: A1/A3 failure/lifetime outcomes. Full received-message identity,
  early/duplicate reply and mixed-send ordering are S4, not prerequisites for
  proving a safe local return. If a counterexample actually requires identity,
  isolate that exact minimal dependency rather than importing all S4.
- Acceptance: actual production wrappers preserve return/error and one
  execution owner after each completion class, with a finite test deadline;
  nested return objects remain valid, dead/reused identities do not revive.
  Add explicit native-timeout/receiver-loss assertions, not a green ordinary
  send as a substitute. Preserve E1 consumed-once fatal case.

Implementation order: establish A1 failure outcomes and A2 fixed context
dispositions, connect A3 terminal ownership and A4 return handling against
those same outcomes, then run the whole affected group once. Reuse existing
validated build caches; do not test every leaf as an independent delivery.
No runtime change is claimed by this audit. Standing execution authorization
continues; a new semantic waiver remains a stop condition, not presumed consent.

## Successor ownership and closure rule

| Class | Owner and retained obligation | Why not an extra S2 requirement |
| --- | --- | --- |
| A | S2 A1--A4, F01--F12 | Common entry/return/identity/lifetime safety. Must be resolved, not deferred. |
| B | S3: W03 EDIT storage/word break, W06 dialog/internal controls, W09 menu parsing/mutations and concrete behavior; WRITE functional progression and normal SOL/WRITE initialization | Object publication already needed by callbacks stays S2. Control-specific conversion, editing, menu selection and dialog features stay S3 even when they share files. |
| C | S4: C01/C02 complete message identity, explicit/implicit mixed traffic, Win16/Win16 and both native directions, early/duplicate replies, cross-task waits/reentry | F10/F12 local safe return is S2. Complete message association and ordering remain mandatory in S4, with no newly accepted semantic waiver. |
| D | S5: request/incarnation-to-HTASK binding, individual deletion and hung/cancel behavior; S6 final reconciled acceptance | Ordinary teardown/refusal safety stays A3. S6 cannot absorb unfinished implementation. Other graphics/printing/network features retain their existing queued owners. |

Earlier intermittent SOL readiness/WINMINE F2 failures remain preserved in
the graph audit. Subsequent P15 interaction passes do not establish their
cause. They are not an unbounded new feature list or evidence of guest defects.
A reproduced common-owner failure maps to A1--A4; no unsupported claim that
those failures were fixed or harmless is made here. Preserve the prior
frontiers and adverse evidence through the integrated acceptance run.

S2 exit requires all four blockers resolved with the row-specific evidence,
then required x86 /MT CCPU40 build, affected cases, both DOS17 matrices,
independent application frontiers, thirteen-image publication/hash equality,
reviewed commit/push and closure record. S3/S4 features do not become S2 exit
criteria just because they are useful. This audit itself requires only
documentation/link/diff checks, not rebuilding the unchanged P15 binaries.

## Second-role review

Reviewed the proposed closure claim against code, not just test exits:

- Rejected closure: ignored restoration/unbind failures are real source edges.
- Rejected obsolete evidence: current CallBack16 does not contain the old
  candidate early-FALSE path described in the chronological audit.
- Rejected broad pass claims: Hook mocks, retirement sentinels and WRITE visible
  windows do not prove guest Hook coverage, terminal refusal or editing.
- Rejected scope transfer: safe callback/native return and ordinary retirement
  remain S2 even when S4/S5 will also use them.
- Rejected unlimited growth: fixed entries and four blocker IDs replace
  "remaining callback/WRITE gaps" as the active remaining scope.

Documentation verification: owned hidden supervisor PID9092, run
build/M0-T447/S2/scope-freeze-audit-r1, completed exit0. Its process records
contain UTC start, command and deadline. Documentation governance (including
relative links), diff whitespace and zero mirror-change checks passed. Final
record/link additions receive the same gate in scope-freeze-audit-r2 before
commit. No runtime gate or new publication is claimed for this docs-only work.
