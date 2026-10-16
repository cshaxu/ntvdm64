# T447 S2 fixed-scope closure audit

Current decision after idle-shared-r10 and idle-delivery-group-r2:
**Owner approves closure with the unexplained initial common-dialog witness
retained as risk, not repaired. A1, A3, A4 and A2-P/A2-S/A2-I have bounded
local proof; P16 publication is complete and closure is recorded in the
[closure record](../../history/m0-t447-s2-foundation-closure.md).**
See the current candidate overlay below. Historical findings and failed runs
are retained, not presented as unresolved after their demonstrated repairs.
No candidate publication or P delivery is claimed.

The preceding sentence describes the pre-closure checkpoint only. Owner now
accepts the isolated unlocated witness; s2-final-publish-r1 exits0 with both
Ninja outputs unchanged and all13 published hashes equal to the tested package.
The closure commit includes the original guest archive and manifest. Historical
blocker statements below are retained evidence, superseded by this disposition.

Historical bounded delta (superseded by the final A2-S section): shell-guest-r2 proves actual guest Shell success,
missing-target failure, child execution and same-task RET_WINEXEC return.
A2-S still retains exception reachability/failure-disposition proof. See the
real Shell witness and direct-provider negative control at the end of this record.
shell-closure-r1 subsequently fails its injected native-exception case;
ordinary guest success is not revoked, but A2-S exception safety stays open.
frozen-exception-r2 now proves A3-X's real original exception Close path,
exact-once retirement, no later guest callbacks and same-worker gameplay.
frozen-startup-guest-r2 now proves A3-S's reached allocation-failure exit.
At that historical checkpoint A2-I/A2-S remained; both now have local proof.
The later failed common-dialog witness has not been explained by subsequent
successful runs. See the final delivery-review sections for its bounded gap.

## Safe-pause review after the current repair group

This review supersedes older pending claims only where the evidence below
discharges them. The fixed denominator remains twelve entries F01--F12 and
seven F07 families. No build/test was interrupted and no edits were discarded.
frozen-shell-r1/exit.txt and frozen-shell-r1-guests/exit.txt are both zero;
the group has ended, rather than merely timed out at the tool boundary.

Actual adapter proof is frozen-shell-r1/adapter/probe.out: original two-argument
callback returns failure2 and success42 through the modern three-argument ABI,
once on caller thread25392, with nested invocation restored. Adapter supervisor
35972 started2026-10-15T23:06:01.0972065Z; build supervisor35004 started
23:06:17.6533962Z. Both completed successfully. The new sealed guest package
contains worker SHA256
04A6DC62A3D70ED8FF39D0A07E1DF3D1EB4A096E0409D3E448228059C5B30876
and WOW32 SHA256
38D492F1A36FADC6CF9ACBB491FAD2484788ED209FCA2061E35170C80DE51050.
These are candidate hashes, not installed P15 hashes.

Read the actual guest-report.json files, not only the outer exit code:
generic reports errors0, Pascal3, Cdecl1, nested1, freed3; timer reports
errors0, ordinary1, modal1, killed2, fonts2/font_reads2, freed1;
common-dialog reports errors0, created2, closed1, hooks2, leaving1.
This confirms retained generic/modal/resource execution, not a guest
WS32ShellExecute invocation. The native Shell witness is not that invocation.

The earlier source/evidence tables remain the implementation cross-reference.
At this pause their current overlay is amended as follows:

| Fixed entries | Actual added evidence | Still not proved |
| --- | --- | --- |
| F01, F11 | terminal-entry-r2: four real host thread entry ABIs reject binding; actual pre-CCPU adapter unbinds before the controlled CPU-release witness. frozen-sibling-r1: real modeless task retires once and same-worker WINMINE remains playable. | Real guest startup failure and guest exception terminal paths; controlled CPU witness is not original CCPU release. |
| F02 | original-frame-r3 links the selected whole wcall16.c; inaccessible old mappings, changed return mapping and all three resolver exceptions are asserted. frozen-shell-r1-guests retains real generic/nested execution. | No physical guest-stack-relocation claim. The controlled alias proof and real nonpresent-code-segment proof are complementary, not a demand to implement new memory features. |
| F03--F06 | r13/early-exit-r3 retain nested creation, WND/dialog/Hook/discovery result/unwind and fatal restoration evidence; latest common-dialog regression passes. | Full control behavior and full mixed-message identity remain successor obligations, not failed foundation cases. |
| F07, F12 | Actual timer/modal/font, generic, resource and original-taskman idle paths; Shell adapter native return/nesting now passes. | Three bounded A2 cases below; no claim that all GDI, network or Shell features work. |
| F08--F09 | Existing identity, reuse, mutation, retained-reader, rollback and retirement assertions in r10/r13 remain bounded foundation evidence. | Whole menu features belong to S3; final coherent-package regression still required. |
| F10 | r13 proves held/unheld receiver death, timeout, exception, result/error and owner restoration. | S4 complete sender/reply association is not included in this pass. |

### Finite remaining acceptance units

These five tracked units refine A2/A3; A3-X, A3-S and A2-P have local acceptance, leaving
two open units. They do not add new blockers or reopen
A1/A4. An uncovered boundary is not automatically a demonstrated code defect.
Repair only a reproduced project integration violation within these owners.

| Unit | Root cause or exact evidence gap | Repair/proof scope and dependency | Exit condition |
| --- | --- | --- | --- |
| A2-I idle isolation | The local guard does not identify the global Hook's registering process. idle-workers-r2 now proves both actual worker exits and subsequent surviving-peer idle on the original separate-process path. | wow_idle_hook_binding.c, original wkman/taskman and ExitKernel, existing worker-local runtime binding. Depends on A1 unwind; no new Hook registry or scheduler. | Prove original foreign/unbound filtering; retain the actual process-handle exit/surviving-peer evidence. Do not substitute launcher exit for cleanup proof. |
| A2-S Shell entry/return | ABI and actual guest ordinary return pass. Outer native exception contrast passes, but shell-exception-r1 fails to retire the actual guest task after original callback return and Close; adapter TLS restored. | Existing wow_shell_callback.c, original wshell.c/RET_WINEXEC, callback stack and W32Exception disposition; establish CPU/logical frame agreement before attributing the defect. | Prove safe reached exception/return and task retirement. No silent swallowing, provider replay, unsupported original-host correction or shell feature sweep. |
| A2-P provider dispositions | Local pass: actual original print normal/cancel/cleanup in frozen-print-guest-r6 and Winsock normal/cancel/restore/free in frozen-winsock-guest-r2. | Original imports, registration and initiating-thunk context retained; project observer error transparency repaired. No printing/network subsystem implementation. | Retain these bounded callback results in coherent-package regression. General printing/network features remain queued, not claimed complete. |
| A3-S startup failure — locally accepted | frozen-startup-guest-r2 reaches seven original InitTask allocation failures, successful registered cleanup and actual original host-thread exit1; same-worker WINMINE remains playable. Native rollback/identity witnesses cover resource and rejection details. | Existing registration bridge, session binding, thread_start_compat and original W32DestroyTask; original failure policy retained. Test-only allocator, no guest change. | Retain these bounded assertions in final coherent-package acceptance. This does not prove every allocation site or general forced termination. |
| A3-X exception exit — locally accepted | frozen-exception-r2 reaches original exception Close and retires actual task1737 once; no later callback, same-worker WINMINE remains playable. | Original exception/RET_FORCETASKEXIT and existing early cleanup/pre-CCPU adapter; terminal-entry-r2 supplies the explicitly controlled pre-CPU/refusal evidence. | Retain this acceptance in the coherent final package; it does not prove InitTask failure or arbitrary Shell exception escape. |

After those units, run the already required coherent-package foundation and
application/DOS regressions, selected mirror comparison, governance, publication
hash verification, commit and push. These are delivery gates, not additional
feature scope. No production delivery is claimed before they pass.

Second-role review: A1/A4 local proof and normal sibling survival are real
progress and must not be relabelled unresolved. Conversely native Shell and
controlled terminal/frame witnesses cannot stand in for the named real guest
paths. Thus S2 remains open for two of the five tracked units above, not for a vague
"incomplete foundation". Source defects discovered outside these contracts
must be classified, not automatically absorbed.

### Successor ownership without shrinking T447

- A: the three remaining units and common restoration/identity/lifecycle regressions stay
  in S2; no transfer of an unsafe foundation to a later stage.
- B / S3: concrete controls, EDIT storage/word-break, dialogs and menu feature
  behavior; normal SOL/WRITE initialization and retained WINMINE capability.
- C / S4: full message identities, reply/early-reply, cross-task/native
  interaction and reentry. Existing approved semantic boundary is unchanged.
- D / S5: original per-task termination and NT Monitor operation; S2's normal
  and failure cleanup is a prerequisite, not postponed to this stage.
- S6 reconciles delivered rows, final T acceptance and residual queue owners;
  it is not a destination for unfinished implementation. General printing,
  Winsock and Shell features retain their existing queued owners.

Publication stays at P15. Candidate source and all existing edits remain in
the worktree; this audit is not a claim of a clean or synchronized delivery.

## Decision and evidence identity

Audit date: 2026-10-15. Baseline: published P15,
01cb95ad9c0339cde33d9a98feb61880c548b16e. Single implementer, followed by a
separate skeptical review pass. This is a documentation-only audit delivery,
not a runtime repair or S2 closure. The completed P15 delivery is the safe
pause requested by the owner; no build/test was interrupted or edits discarded.

**At the initial freeze, S2 could not close; its boundary was A1--A4 below.** Two contained
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

The F01--F12 table describes the published P15 evidence baseline. Later
candidate changes do not upgrade a PARTIAL row without its provider/guest
assertions. A repaired source edge is not an accepted runtime capability.

## Evidence keys and their limits

### Current candidate overlay (does not replace the fixed P15 denominator)

This overlay distinguishes repaired edges from the original audit findings.
Paths below are under build/M0-T447/S2; source owners are the F01--F12 rows
below. r13 and early-exit-r3 both completed naturally with exit.txt=0:
four ordering, twelve foundation and four real guest cases passed. No runtime
input was changed during those runs. result.exit from the build driver alone
was not counted as suite completion. Original files/edits are retained.
The subsequent TD initialization guard has its own focused run, not r13's
artifact identity. Full delivery gates remain outstanding.

| Fixed entry | Current actual implementation/evidence | Remaining boundary |
| --- | --- | --- |
| F01 bind/init/rollback | Existing registration and lifecycle owners; r10 process-retirement/task-lifecycle and terminal-entry-r1 actual thread entrypoints | A3 real startup/exception exit and sibling survival; retry fixtures alone cannot close it. |
| F02 thunk/frame | Required unwind in wow_user_thunk_scope; original CallBack16 resolver retained; r10 thunk-scope, real timer/font and frozen-generic-r2 actual generic PASCAL/CDECL/nested return | A2 nonpresent/remapped/failed-frame semantics, not missing normal generic execution. |
| F03 WND entry/return | Required cache/borrow/task restoration in window bindings; r10 graph-snapshot six fatal-child assertions, creation nesting, real CBT in early-exit-r2 | A1 local proof complete, delivery pending; no general message-identity claim. |
| F04 dialog entry/return | Existing creation scope/cache pairing plus three original COMMDLG wrappers; r10 dialog-lifecycle; early-exit-r2 two real resource unlock/free pairs before retirement | A3 remaining terminal cases, not a claim of complete dialog features. |
| F05 Hook entry/return | Original installed Hook body behind task/borrow scope; r10 hook-order and early-exit-r2 real CBT recovery | Other callback-family contexts stay in F07; no wholesale Hook feature acceptance. |
| F06 discovery | Original enumeration/property conversion and existing scoped gateway; r10 graph-snapshot held/unheld signed, nested, exception and recovery assertions | Retain original EnumProps recursion limitation; do not silently promise new reentry. |
| F07 other callback families | Timer/font ordinary/modal, modeless resources and real original-taskman idle proof in early-exit-r3; initialization guard in frozen-idle-init-r1; generic callback in frozen-generic-r2 | A2 two-worker isolation, resolver failure proof, remaining fixed-family provider dispositions. No new printer/network implementation. |
| F08 WND/CLS identities | Existing words/borrow/page-domain owners; r10 graph-snapshot/window-borrow/window-reclaim actual reuse, readers and mutations | Approved external snapshot boundary retained; final changed-package regression pending. |
| F09 MENU identities | Existing parser/client/native ownership; r10 resource/cleanup, r12 unchanged invalid-window assertion restored to 1400 | Concrete menu functionality S3; no assertion that all menus work. |
| F10 native return | Existing begin/result/end; r13 native-nested success, timeout1460, exception, destruction116, dead-before-entry1400 and active receiver-thread exit1400 held/unheld with owner restored | A4 local proof complete; delivery pending. Full message association is not claimed. |
| F11 ordinary retirement | Early task-specific cleanup before guest free; pre-CCPU session finish; terminal-entry-r1 five ABIs and fatal refusal; early-exit-r2 real modeless cleanup | A3 exact-once state removal and no post-CPU callback for remaining exit/sibling cases. |
| F12 wait/modal return | Original taskman plus existing native scopes; early-exit-r3 modal timer/nested fonts/idle; r13 held/unheld return and receiver-loss ownership | A2 remaining context proof; complete association/reply sequencing remains S4. |

Finite remaining implementation/proof groups (not new S tasks):

1. A2: finish only the seven F07 families' inherited/acquired/unavailable
   dispositions. The idle query now requires original hThread publication
   before hIdleHook access; incomplete TD rejection and retained guest delivery
   pass in frozen-idle-init-r1. Complete foreign-worker isolation and
   original callback-frame alias/remapping/failure evidence. Generic
   normal/nested PASCAL/CDECL and actual code-segment nonpresent discard/reload
   now pass in frozen-generic-r5; that is not callback-stack lifetime proof.
2. A3: retain the now-proved early resource fix; prove ordinary/startup-failure/
   exception retirement and surviving sibling, exact-once references and
   pre-CCPU ordering at the existing terminal owners. No new task-deletion UI.

A4 is no longer a local proof blocker: r13 requires active receiver-thread
loss in both lock states with actual runtime retirement, error1400, correct
caller owner/psmsSent, dead HWND and absent receiver identity. The fixture
does not claim real guest forced termination or complete S4 message semantics.

Dependency direction remains A1 -> A2/A3 -> A4; passing A4 does not erase
A2/A3 gaps. A source disposition can justify an
inherited context or an unavailable provider; it cannot substitute for the
distinct reachable mechanism's acceptance. Missing evidence is a blocker,
not automatically a product bug. After these groups pass, the already required
application/DOS/publication/hash/mirror/governance/commit/push gates remain.
No other feature may be absorbed into this S2 closure denominator.

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

## Integrated group implementation checkpoint (unverified candidate)

After scope-freeze delivery 9d0224fde, source follow-through distinguishes
two different FALSE outcomes in wow_user_borrow_leave: invalid LIFO scope
does not pop; deferred physical reclamation does pop and preserves pending
allocations under the existing collector. Do not make the latter fatal or
retry the consumed scope. The original lifetime fixture explicitly tests
rejection of an out-of-order leave, so changing the low-level BOOL contract
wholesale would regress a valid diagnostic interface. The candidate adds a
required-pop wrapper for production finally blocks, preserving the original
diagnostic API and deferred collection. It checks LIFO before consuming the
scope and preserves last-error afterward. All seven production leave sites
now use it; the Hook fixture has an explicit ordering-only wrapper mock.
Thunk entry also checks admission before invoking the original thunk. A1
remains open for the complete failure-injection and production-context proof.

Candidate implementation now makes task-scope restoration failure terminal
at the existing wow_user_native_call_failed boundary. Successful enter/leave
preserves last-error. An entry lock-release failure after acquiring execution
can no longer return ordinary FALSE to a caller that has not armed its finally.
WND/dialog callback-cache restoration failure also cannot continue with a
stale outer cache. These failures indicate broken owner/domain invariants,
not recoverable native operation failure or allocation pressure. Original
taskman, callback conversion and guest remain unchanged. The real lifecycle
fixture's send/nested callback entry now asserts successful last-error pairing.
No failure-injection coverage or successful build is claimed yet.

A3 follow-through confirms original wkman.c W32DestroyTask invokes
pfnWOWCleanup before WK32DeleteTask and before its callers invoke
host_ExitThread. Thus guest-capable task cleanup already has an original
earlier entry; do not add guest callbacks to late runtime unbind after
ccpu386exitthread. The registered_cleanup implementation and its original
callers' ignored BOOL must be reconciled with normal resource cleanup and
terminal refusal as one group, rather than adding a second exit algorithm.

No compiler/test/publication has run for these partial source edits. P15
remains the runnable baseline; A1--A4 remain open. Complete the shared
failure, callback-family and terminal/return connections and their tests
before the grouped build and release gate. This is not a delivered repair P.

### Terminal-owner follow-through and pending executable negatives

Original-source review (read-only OpenNT revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c) establishes a more precise A3
dependency than "call cleanup earlier":

- wkman.c W32FreeTask already destroys timers and frees task-owned Hooks;
  W32DestroyTask calls it, then pfnWOWCleanup, then WK32DeleteTask. Do not
  reproduce those original mechanisms in a project exit helper.
- cleanup.c _WOWCleanup's task branch pseudo-destroys windows by replacing
  client procedures; it does not call DestroyWindow. The project WND gateway
  reads cleanup.lpfnWndProc, so that replacement already affects its dispatch.
- Original queue.c xxxDestroyThreadInfo calls PatchThreadWindows, destroys
  timer/Hook/object ownership, performs SendMsgCleanup AFTER windows, then
  DestroyTask. Modern native ExitThread owns native teardown, but our unbind
  currently precedes it and refuses retained windows. This is the ordering
  gap; retrying the same early unbind or immediately killing a worker does not
  supply that missing normal cleanup.
- createw.c PatchThreadWindows selects the retained native standard procedure
  from fnid, otherwise server DefWindowProc; it does not blindly substitute
  one default procedure for every control. Dialog's separate project DLGPROC
  and borrowed-native class targets need explicit neutralization/lifetime
  disposition before any project-side early destruction is safe.
- handtabl.c DestroyThreadsObjects relies on kernel object/thread locks and
  HMDestroyUnlockedObject. Whole queue/createw/handle-table import is not a
  finite public-USER binding. No source has been imported by this research;
  preserve the existing original cleanup and audit the missing native-retire
  seam rather than importing a USER server or inventing another task killer.

Source identities (windows/core/ntuser/kernel, SHA256):

| File | Hash |
| --- | --- |
| queue.c | 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8 |
| createw.c | DA4A0C0E0A85DBE88130F01CCCEECF9DA08A0FEBF3F841458D2A3CBD707A57D7 |
| handtabl.c | F0B56314A93423599F06645D17104391F19A0CA07C3944853B065D8866493F7C |

Candidate tests now extend the existing real-provider/page-domain fixture's
owned fatal-child mechanism with --callback-owner-failure (double-held lock
at production task return) and --callback-borrow-failure (invalid LIFO at
production required-pop). The child signals after reaching its injected
condition; parent requires c0000420, rejects normal continuation and enforces
the existing 20-second child deadline. The graph-snapshot runner requires
both distinct zero-error markers. These assert restoration rejection, not
guest scheduling or every callback-cache failure; they have NOT run yet.

A4 source review also separates the native API timeout from original WOW
cooperative execution reacquisition. xxxSleepTask has its own original event,
task-lock and scheduling rules; an INFINITE internal wait alone is not proof
of a defect or permission to impose a new guest preemption policy. The
pending timeout/receiver-loss witnesses must assert correct local return
ownership under the actual cooperative prerequisites, not silently promise
hard wall-clock guest resumption when another guest task never yields.

Further A1 negatives now inject a mismatched page-directory identity inside
the existing native witness WND callback and dialog callback, then return
through the actual production finally blocks. Separate child options
--window-cache-failure and --dialog-cache-failure must reach the named event
and exit c0000420. No original guest image is loaded or patched by these
authored CCPU-fixture injections. All four new callback negatives remain
unrun; the parent asserts them independently, not via an aggregate exit alone.

A3 adjacent-source finding: wow_cleanup_window.serverSideProc has a reader
in the original cleanup facade but no production assignment (only test
fixtures set it). Consequently original task cleanup cannot distinguish
modern native standard-control targets from guest procedures through that
field. A new DestroyWindow loop after pseudo-destruction would therefore risk
skipping native control cleanup. It also would not neutralize dialog_proc's
separate creation->procedure callback. These are prerequisites within F11,
not new control-feature scope. Reject a blanket serverSideProc=TRUE fix:
subclassing a native control with a guest WNDPROC must still be neutralized.
Reject restoring by class name alone: original PatchThreadWindows explicitly
uses the procedure actually exercised, since arbitrary classes can call a
standard control procedure. The existing native_callproc/original class target
and original WOW standard-thunk metadata must be reconciled before choosing
the finite retirement binding; no new native-window teardown is implemented
at this checkpoint.

A2 source distinction is also confirmed: W32TimerFunc selects native timer
metadata and calls CallBack16 directly, whereas W32EnumFontFunc allocates and
writes guest callback arguments BEFORE CallBack16. A blanket guard added only
inside CallBack16 would be too late for that family. Synchronous enumeration
must prove its inherited caller execution owner; independently dispatched
timer/modal callbacks need an entry gate before their first guest access.
Original resource callbacks stay under their synchronous caller; subsystem
availability and callback entry are separate questions in the fixed F07 table.

### A2 timer entry selection (implementation admission)

Selected original wow32/wutmr.c already owns timer allocation, 27 callback
slots, cancellation and queued-message fallback. wmsg16.c's WM_TIMER branch
also restores TMR.dwTimerProc32 directly into the native message; wrapping
only SetTimer registration would miss this path. Neither a new timer-ID map
nor 27 replacement timer identities is justified. The original W32Timer1--27
all call W32TimerFunc before any guest argument access. Preserve this common
entry and rename only its definition to WowOriginalTimerFunc, as already
done for the central Hook body. A project wrapper pairs existing callback
execution and USER borrow scopes around the unchanged original body.

Recovery ladder: (1) the original translation unit is already selected, but
its NT4 USER callback ownership is unavailable in modern native modal loops;
(2) registration-only public API binding cannot cover the original message
conversion path, and a compiler-wide rename would rename callers as well;
(3) DIV-333 is one definition rename plus its explanatory comment, allowing
the existing original callers to enter the owned wrapper; (4) no replacement
timer algorithm, timer registry or scheduler is admitted. There is no guest
change. Rejected earlier registration-trampoline design is not implemented.

Acceptance must include ordinary guest DispatchMessage and native modal timer
delivery, nested entry, callback exception and pre-entry rejection, preserving
original slot/cancellation behavior and native return error. Wrapper-only
tests supplement but cannot replace real provider/guest evidence. This is an
unbuilt candidate within A2, not a completed callback-family proof.

Candidate wiring is now in wow32-dll/source/wow_timer_callback.c and the
formal New-T404S5Wow32ProviderNinja.ps1 support-source list. The only executable
mirror delta is wutmr.c +2/-1; original line endings are preserved. All 27
original stub calls and TMR.dwTimerProc32 assignments are unchanged. The
production wrapper unwinds borrow and task scopes with nested finally blocks,
including an original-body exception; it never catches or translates that
exception and introduces no timer retry. No timer policy implementation was
copied or left alongside the original.

The checked-in supplementary entrypoint is
tests/observation/verify-wow-creation-boundaries.ps1 -Case timer with the
production provider and a build-owned output root. Its fixture links the
actual wrapper object and explicitly mocks only scope owners/original body;
it checks all 27 index/argument tuples, nested call, exception and both entry
denials. Expected marker: WOW_TIMER_CALLBACK_GUARD errors=0 calls=30 nested=1
exception=1 denied=2. This is authored, not executed evidence. Real callback
ownership and cancellation/queued-message semantics remain required A2 tests.
Second-role source review rejected declaring A2 complete from that fixture,
and verified no SetTimer replacement or alternate timer registry was added.

### A4 completion witnesses added to the existing integrated fixture

The native-nested case now also invokes the actual
wow_native_SendMessageTimeoutA wrapper against the existing real receiver
thread, with caller USER lock absent and present. A separate receiver message
enters the actual callback scope and deliberately delays 500ms; the native
API deadline is 250ms. The fixture requires that callback to have run and
finished, a failed native send, retained native error (0 or ERROR_TIMEOUT),
the original caller lock state, original psmsSent and restored execution
owner. Its existing 15-second watchdog bounds the entire cross-thread case.
This is not a hard real-time guest deadline or a product Sleep/pacing change.
Failure to reach the receiver is a failed witness, not timeout acceptance.

After the existing receiver's normal shutdown, the same wrapper must reject
its destroyed HWND with ERROR_INVALID_WINDOW_HANDLE and leave sender ownership
intact. The observer now requires both timeout markers and the dead-target
marker, in addition to the fixture exit/result assertions. Neither witness
has run. A dead HWND before call entry does NOT prove receiver disappearance
during a suspended native call; the latter and native exception recovery
remain separate A4 obligations. No production timeout policy changed here.

### A2 synchronous-family source disposition

The selected wgfont.c W32EnumFontHandler calls native EnumFonts or
EnumFontFamilies with a stack FNTDATA and W32EnumFontFunc. The latter writes
guest font/metric arguments before CallBack16 and returns to that synchronous
enumerator; WG32EnumFonts enters through the original thunk. No private USER
native-call wrapper replaces those GDI APIs. This route inherits its thunk's
execution owner, rather than needing the timer's independently delivered
entry. wgmeta.c WG32EnumMetaFile similarly supplies stack METADATA to native
EnumMetaFile and its WG32EnumMetaFileCallBack. Retain these original bodies;
do not add duplicate ownership or independent callback registries merely
because they call CallBack16. Real non-WND guest execution, nesting and frame
failure remain required evidence; this source disposition is not a pass.

wgdi.c W32AbortProc is different: it reads CURRENTPTD/TDB before CallBack16
and is registered for later printing callbacks, not just a stack enumeration.
It cannot inherit the font-enumeration conclusion without tracing the actual
printing callout/registration boundary. Common-dialog and EDIT word-break
callbacks likewise remain in the fixed F07 census; no subsystem implementation
is admitted by identifying their callback entry requirements.

### Exported generic callback contract and A3 cleanup distinction

The retained original SDK wownt32.h's Generic Callbacks section expressly
defines WOWCallback16 for Win32 code called FROM 16-bit code (for example,
generic thunks). wkgthunk.c WK32ICallProc32 invokes the native routine through
WK32ICallProc32MakeCall on x86 within that original thunk. wcall16.c retains
WOWCallback16/Ex and their CallBack16 dispatch. Thus these exports do not
authorize a new arbitrary-native-thread guest entry or require a replacement
thread registry. Preserve the inherited execution contract; positive generic
thunk/callback execution still needs its own witness. The original Ex size
check precedes frame access and output writes. The graph-snapshot fixture now
calls that actual original with MAXDWORD arguments and NULL input, requiring
ERROR_INVALID_PARAMETER and unchanged output. This candidate negative is
unrun and proves neither normal guest execution nor CDECL/PASCAL stack return.

A3 further review confirms the distinction between _WOWCleanup and
PatchThreadWindows: the former can zombie an individual unloaded module's
window while other guest code continues; the latter runs only at terminal
thread cleanup and uses the procedure actually exercised (fnid), not its
creation class. Do not set serverSideProc on every native-looking class to
avoid pseudo-destruction. The current private CallWindowProc seam is a
potential place to retain an actually exercised standard target, but only
after matching it to original standard-procedure metadata; an arbitrary
native application WNDPROC must not be kept alive as a cleanup procedure.
The current native_callproc field alone is insufficient: it preserves opaque
CPD forwarding identity, not the original fnid classification. No blanket
DestroyWindow loop or late guest callback has been introduced.

### Real timer witness and terminal-classification review

tests/observation/win16_timer_probe.asm is an independently authored NE test,
not modified original media. Its imports use retained USER ordinals 10/12
(SetTimer/KillTimer), 108/114 (GetMessage/DispatchMessage), 1/60/88
(MessageBox/GetActiveWindow/EndDialog), plus the existing probe's KERNEL
initialization/file/MakeProcInstance ABI. It executes one callback through
ordinary guest dispatch, cancels that timer, then registers a second callback
while MessageBox runs its native modal loop. The second callback cancels its
timer and closes the dialog with IDOK. It frees its procedure instance and
exits normally, recording three22-byte phase records with exact callback,
cancellation and cleanup counts. The modal case deliberately does not depend on a peer
click, private-desktop activation or computer use.

verify-win16-hook-probe.ps1 now accepts -Case timer (default hook is unchanged)
and reuses its existing package isolation, input hashes, owned observer,
30-second observation/35-second process deadline and cleanup. Timer acceptance
requires all three records, errors=0, ordinary=1, modal=1, killed=2,
modal_result=1, freed=1, fonts=2 and font_reads=2, plus normal launcher exit. Missing modal delivery
is failure, not permission to click through or substitute a window callback.
This new witness has not been assembled or executed; it will join the integrated
group, and does not establish queued-after-KillTimer or timer-ID reuse behavior.

A3 source review found an additional constraint on reusing the standard-proc
resolver: walias.c GetStdClassWndProc can lazily call GetStdClassThunkProc and
map/flush guest thunk metadata. wsubcls.c IsThunkWindowProc also reads guest
memory before returning its native target/class. Neither may be newly invoked
after CCPU teardown merely to classify a cleanup target. Retain original
classification during live execution, or use already established native
metadata at retirement. Existing native_callproc is still not sufficient
proof of a standard control target. This rejects a late resolver shortcut;
it does not justify another timer/control map or change the fixed A3 scope.

The same timer witness now nests original GDI EnumFonts (ordinal70 from
wow16/gdi/gdi.def) inside each actual timer callback. It obtains/releases one
desktop DC through original USER66/68, retains a second MakeProcInstance,
reads both LOGFONT16/TEXTMETRIC16 callback buffers, verifies the exact DWORD
user parameter, returns zero to stop enumeration and checks SP restoration
after EnumFonts. The intermediate phase must show exactly one completed font
callback/read; final phase exactly two. Both procedure instances are freed.
GDI.EXE is included in the runner's immutable input hashes. This connects the
previously planned synchronous non-WND/nested guest acceptance to the same
isolated process, without adding another provider, registry or product shim.
No assembly, build or test has run yet; this is executable test source, not
evidence that native modal dispatch or the GDI callback is already restored.

### A3 selected terminal seam

Current source proves the only session thread hook registration is
wow_user_session_binding.c. session_thread_unbind invokes its reverse hook
before dropping session TLS/counts. Original ccpu386/ntthread.c
ccpu386exitthread only frees the current simulation stack and unlinks its
thread node; it neither owns WOW window cleanup nor reads session TLS.
Therefore the selected terminal ordering is existing session/WOW unbind
before that original CPU release, then original native ExitThread. An
nt_thred.c-only build alias can route ccpu386exitthread to a thin owned
wrapper that performs the former and calls the unchanged latter. It must
not rename the definition in ntthread.c. No additional mirror body change,
exit scheduler or asynchronous reaper is needed for this edge.

The terminal session helper must fail closed if an existing unbind hook
rejects retirement, including ordinary native start-routine returns. This
prevents dropping a live TLS owner merely because a BOOL was ignored; it is
NOT the normal-live-window fix or evidence of S5 targeted termination. A3
remains open until existing retirement safely neutralizes/destroys its owned
windows and passes ordinary exit, sibling and failure cases. Preparation
before CPU release preserves the ability to complete the original cleanup;
do not use the new terminal rejection as a substitute for doing that cleanup.

Candidate code now adds opennt_prepare_thread_exit to the existing thread
adapter and uses it at all four ordinary start-routine returns and explicit
opennt_exit_thread. A rejected hook reaches the existing noncontinuable
c0000420 fail-fast convention instead of abandoning session TLS. The thin
ccpu_thread_exit_compat.c calls this helper before unchanged ccpu386exitthread;
the formal graph aliases only nt_thred.c's call/prototype. The original
ntthread.c definition is not renamed and no extra mirror source is changed.
The existing thread-start fixture now adds an owned child whose unbind hook
rejects release; it requires c0000420 within five seconds, not normal return.
The child does not impersonate a real WOW retirement pass. These changes and
tests remain unbuilt; live-window cleanup and full terminal-path coverage are
still mandatory A3 work before publication.

Second-role terminal review found the four adapter start routines only called
cleanup after a normal return. Candidate wrappers now place the terminal
helper in finally; they do not catch/translate the original exception. The
original host ThreadStartupRoutine still owns VdmUnhandledExceptionFilter.
Explicit native ExitThread remains separately covered by its pre-exit helper.
The thread-start fixture now executes cdecl, WINAPI, void-cdecl,
void-cdecl-with-parameter and explicit-exit workers, requiring one unbind per
child and an intact parent binding. The rejected-unbind child remains a
separate expected fatal outcome. No test has run; exception-path production
coverage and normal live-window retirement remain open.

The existing prepared standard-class object already stores immutable
original_native_procedure and entry.fnid (wow_class_words_prepare_native).
Those are candidates for matching an actually exercised native target while
execution is live; they are not grounds to classify a window from its class
alone. No new standard-procedure table or lazy guest resolver is required.
Do not confuse the callable wow_class_call_original_native bridge (which
selects a window's creation class) with PatchThreadWindows' actual-procedure
rule for an arbitrary class that called a standard procedure.

A3 live-identity admission: match the actual native call target against the
existing borrowed standard-class records, recording their fnid and canonical
native procedure in the already-owned window. A wide call can compare the
same original standard atom's native wide descriptor, retaining the canonical
ANSI target for terminal messages. Never resolve guest thunk metadata at this
point, create a second class registry, or clear an established fnid on an
unrecognized application subclass call. Record before native dispatch (which
can reenter/destroy); keep the existing USER lock only for the metadata pass,
not the native call. This supplies PatchThreadWindows' actual-procedure input,
not a new dispatch decision. Matching uses borrowed native classes only, not
private application classes that shadow a standard name. Mirror budget zero.

Live identity recording is now connected at native DispatchClientMessage,
the retained CPD forwarder, the class cleanup forwarder and native A/W
CallWindowProc. It stores fnid/native_cleanup in the existing WW-owned cleanup
view before calling out, under the existing runtime domain lock, restoring
last-error. Unknown targets leave prior actual-procedure identity untouched;
ambiguous matches refuse classification rather than choosing the first class.
Only the existing borrowed standard class list is scanned. No guest pointer,
new native registration or second process table is introduced. The actual
dispatch procedure/result remains unchanged. A test in the existing real
class-projection fixture checks all prepared A/W targets, canonical identity,
unchanged last-error and unmatched-output preservation. It is unrun.

The new retained fields are not yet consumed by terminal window preparation;
this checkpoint cannot close A3 or be published as normal-retirement repair.
The remaining connection must neutralize all guest WND/dialog entrances
before native destruction, preserve standard cleanup targets and original
task/class cleanup, then prove ordinary/sibling exit and failure behavior.

### A3 terminal intent versus retryable detach

Review of session_thread_unbind and wow_user_runtime_unbind confirms both
also serve retryable detach/rollback. Turning every unbind into native window
destruction would violate that existing contract. The candidate now adds
session_thread_finish at the existing terminal adapter only. It runs the same
reverse hooks with a thread-local terminal intent, cleared in finally on
success, refusal and exception. Recursive finish is rejected before invoking
hooks. No new hook registry or task dispatcher is added.

The existing WOW session hook carries that intent into its owned runtime
thread record only while unbind runs, again clearing it in finally. Ordinary
detach and bind rollback remain unmarked. This appended private carrier field
requires rebuilding both worker and provider; it is not a broker/guest ABI.
The lifecycle consumer still needs the terminal-only window preparation and
destruction connection. Merely delivering this intent is NOT normal-exit
acceptance, and must not be published by itself.

The existing thread-start fixture now requires five terminal releases, one
ordinary parent release, retained binding after refusal, and cleared intent
after an injected hook exception. These assertions are authored but unrun;
the expected new marker is THREAD_TERMINAL_INTENT terminal=5 ordinary=1
rejected_reset=1 exception_reset=1. Second-role review preserves the prior
reject/retry contract and identifies no additional mirror or guest change.
The affected group remains unbuilt and unpublished pending A1--A4 completion.

### A3 terminal window connection (unverified)

Read-only createw.c PatchThreadWindows at the pinned OpenNT revision confirms
its two essential constraints: patch every owned window before destruction;
choose the actually exercised standard procedure, otherwise DefWindowProc,
never an arbitrary application subclass or merely its creation class. Native
USER retains its internal desktop/menu windows; these are not adopted by the
managed WOW window table. Its private server procedure table and native
destruction closure remain the declared public-USER stopping boundary.

Candidate destroy_terminal_windows now runs only for terminal_exit. Under
the existing USER lock it marks original TIF_INCLEANUP, collects already
pending retirement, and prepares every existing table-owned window. The
window carrier forwards to its retained canonical native cleanup procedure
or DefWindowProc; the separate dialog carrier disables guest DLGPROC while
retaining its native dialog and terminal property release. No guest resolver
is invoked here. Callback and explicit/implicit creation admission reject
INCLEANUP, preventing new guest entries or owned windows during destruction.

The second pass retains each selected window, drops the USER lock and calls
native DestroyWindow, so native USER owns child ordering and control teardown.
Parent destruction may already have removed later table entries. The existing
collector then resolves deferred storage and the pre-existing live-window
refusal check remains mandatory before original cleanup/DestroyTask. No
failure is converted into successful retirement. Ordinary unbind never sets
INCLEANUP and keeps its existing refusal/retry semantics.

This is source wiring, not acceptance: actual normal/live-window/dialog exit,
native standard-control cleanup, sibling operation and failure cases still
need the integrated fixture/real-package gate. Second-role review must also
check canonical-procedure recognition and every terminal property release;
no empty-table pass can substitute for those cases. No additional original
mirror body change or guest mutation was introduced by this connection.

The terminal preparation witness now extends the existing real CCPU
window-borrow/native-control fixture, retaining the ordinary dialog destruction
case first. Its second dialog contains an enrolled native BUTTON and holds
both storage references while preparing terminal cleanup. It asserts the
button's actual native cleanup target exists, new callback/creation admission
is rejected, the dialog witness receives no terminal callbacks, and both
managed identities retire after native destruction. The observer requires
WOW_TERMINAL_PREPARE_CCPU errors=0 with each named assertion. This is unrun
test source and a preparation-only witness, not the full lifecycle/sibling
exit proof. Ordinary destruction must still deliver its two terminal messages.

Second-role review corrected the terminal destruction call to reuse the
existing wow_native_DestroyWindow execution boundary instead of raw USER32.
Dropping the USER lock alone is not relinquishing cooperative execution:
native destruction may notify a sibling task, whose callback needs the
existing native begin/end handoff. This reuses the established owner rather
than adding a cleanup scheduler. Its result/return restoration remains under
A4's integrated gate; terminal preparation still precedes all callouts.

### A2 callback-family trace: retained contexts versus deferred hooks

This pass reads the selected wow32 sources, the registration bridge and the
pinned original ntgdi/client/output.c. It changes the remaining entry design,
not the feature denominator or any runtime pass status:

| Entry | Registration/caller and first guest use | Source disposition / remaining proof |
| --- | --- | --- |
| W32GrayStringProc | WU32GrayString supplies stack GRAYSTRINGDATA to native GrayString; callback invokes RET_GRAYSTRINGPROC | Synchronous original thunk-owned call, like font enumeration. No selected native-call wrapper releases ownership around GrayString. Preserve original conversion, no duplicate callback registry. |
| W32AbortProc | WG32SetAbortProc and WG32Escape SETABORTPROC register it; callback reads CURRENTPTD/TDB before CallBack16 | Original GDI output.c SetAbortProc stores the procedure in LDC; vSAPCallback invokes it synchronously from printing/output operations. Registration lifetime alone does not imply a background-thread callback. Selected original GDI thunks call native GDI directly. Native-provider/context acceptance remains required, not a new printing feature or an unconditional late CallBack16 guard. |
| WCD32DialogProc / WCD32PrintSetupDialogProc | Native COMMDLG hook pointers; GetCommdlgTd reads host PTD chain, then WCD32CommonDialogProc reads/thunks guest arguments | Modal calls and later native delivery must be distinguished. CommonDialogProc alone is not the entire family boundary. |
| WCD32FindReplaceDialogProc | Find/Replace installs this hook and returns a modeless HWND; future messages reach CommonDialogProc, while WM_DESTROY independently reads guest FINDREPLACE16 and releases original resources | Confirmed independent-delivery boundary. A blanket refusal during terminal cleanup would skip its original resource cleanup. Admission/recovery and original cleanup must be designed together within A2/A3; do not count modal/font evidence as proof. |
| W32EditNextWord | wow32.c puts it in pfnIn.pfnWowEditNextWord; the current bridge stores the input record but has no consumer of this field | Not a currently connected native callback path. The bridge's actual input consumers are WndProcEx and expected-version callbacks. Connecting native EDIT word-break behavior remains S3; the original callback body is retained, not replaced. Do not claim EDIT support from this availability disposition. |
| WWS32CallBackHandler | WSASetBlockingHook registration in wsext.c; original default path calls PeekMessage/DispatchMessage, custom path invokes RET_WINSOCKBLOCKHOOK | Preserve the original blocking-hook implementation. The default queue operations already use existing USER boundaries; custom-hook inherited context/provider reachability still needs its explicit row, not an invented asynchronous networking dispatcher. |

The modeless COMMDLG finding is an existing F07/A2 gap, not new S3 menu or
dialog-feature admission. Its resource-destruction dependency is likewise
within the existing A3 ordinary-retirement contract. No new mirror patch was
made in this pass: first select an entry that precedes guest access and keeps
the original WM_DESTROY cleanup, rather than renaming only the convenient
CommonDialogProc body or suppressing the entire modeless hook during exit.

### A2/A3 integration review: resource ordering corrects late-only teardown

The actual selected Ninja graph includes wcommdlg.obj and links comdlg32.lib;
FindText/ReplaceText are native public calls in that original body. This is
not an unselected-source concern. Source follow-through also disproves the
sufficiency of the current late-only terminal-window candidate:

1. wkman.c W32FreeTask calls DestroyRes16 before W32DestroyTask returns to
   host_ExitThread. Its only selected calls are W32DestroyTask and the
   existing WK32KillRemoteTask exit path.
2. wres16.c DestroyRes16 unlinks and frees PRES, calling UnlockResource16
   first when needed. UnlockResource16 itself uses original RET_UNLOCKRESOURCE;
   original cleanup therefore already requires a live guest callback context.
3. wcommdlg.c WCD32FindReplaceDialogProc WM_DESTROY still reads FINDREPLACE16
   and, for FR_ENABLETEMPLATE, uses ptdDlg->pRes. That pointer cannot be assumed
   live after DestroyRes16. Allowing its resource callback only at late session
   finish would not repair the ordering and can expose freed storage.

Implementation direction is now two existing lifecycle phases, not a second
cleanup algorithm: run the same terminal window preparation/destruction before
the original W32FreeTask resource body; leave final task/session unbinding at
the pre-CCPU terminal edge. Retain original timer/Hook/resource frees in place.
The late edge remains a rejection/retention safety net, not a substitute for
early cleanup. The candidate must not publish until this connection replaces
late-only ordering and the actual original modeless cleanup is exercised.

The smallest candidate seam is W32FreeTask's definition only, preserving its
two original callers and unchanged original body behind the existing-provider
wrapper. A translation-unit-wide rename alone cannot interpose because it
also renames both callers. Final source intrusion registration precedes that
edit. The COMMDLG boundary must distinguish application Hook entry from its
original WM_DESTROY resource-only branch: termination blocks app callbacks,
not the original resource release while its context is still alive. No broad
guest-entry exemption, new resource registry, replacement COMMDLG teardown,
or silently leaked modeless state is accepted. This ordering repair stays
within frozen A2/A3; concrete dialog/EDIT behavior remains S3.

### Early exit and COMMDLG entries connected (unbuilt)

DIV-334 now renames only W32FreeTask's definition. The owned
wow_task_cleanup_entry.c invokes existing lifecycle window preparation before
WowOriginalFreeTask, retaining both original callers and the entire original
cleanup body. Late retire_current_thread no longer destroys windows: it keeps
the mandatory surviving-owner refusal before original task retirement. Failed
startup without a published USER task leaves original cleanup unchanged.

DIV-335 renames only the three native COMMDLG Hook definitions, leaving all
original prototypes and registrations at WCD32*. The owned
wow_common_dialog_callback.c pairs existing task/borrow scopes and typed HWND
publication around the original bodies. It adds no dialog conversion, template
parser or resource registry. FindReplace WM_DESTROY alone enters the explicit
resource-cleanup scope; original code proves this branch never calls the app
Hook. During INCLEANUP that scope requires resource_cleanup_live, set only
around early window destruction and cleared in finally. Other Hook messages
remain rejected during cleanup, and the late session edge cannot enable the
resource exception. Recursive early cleanup is refused rather than restarting
partially consumed cleanup. Normal resource cleanup uses the same task owner.

Formal provider support-source selection includes both new carriers. Raw
mirror diff is wutmr.c +2/-1, wkman.c +2/-1, wcommdlg.c +6/-3: five definition
renames plus registered comments, with original LF restored in wcommdlg.c.
No guest bytes or original cleanup/conversion bodies changed. Diff whitespace
check passes; no compilation, runtime verification or new release is claimed.
Ordering/mock negatives must supplement actual modeless resource lifetime,
ordinary/terminal destruction and sibling execution tests before delivery.

### Integrated candidate gate started

The connected recovery/timer/COMMDLG/early-exit group now enters one grouped
build and foundation gate in build/M0-T447/S2/frozen-contract-r1. Hidden
supervisor PID13560 started 2026-10-15 13:08:44 local; its process JSON records
UTC time/command and child ownership. Build deadline is1800s, each fixture600s.
The existing worker/provider generators reuse their validated S6 caches;
the suite runs Hook, timer and COMMDLG ordering fixtures followed by the same
twelve foundation cases as P15. No compiler was live at launch (Get-Process
ninja/cl/link returned none); CIM process inspection was unavailable, not
treated as a successful inspection. This is an active run, not a pass.

The new common-dialog fixture links actual COMMDLG and early-exit wrappers
against explicit mock originals/owners. It asserts argument/result forwarding,
resource-branch selection, nested/exception unwind, task/borrow/publication
denial, prepare-before-original-free and no original free after preparation
failure. The real window-borrow fixture separately verifies that INCLEANUP
rejects resource entry outside the early phase, allows its explicit resource
entry within that phase, and still rejects ordinary callbacks there. These
are necessary boundary witnesses, not actual modeless resource lifetime or
complete normal/sibling task-exit acceptance. Real timer/font, COMMDLG guest,
terminal-thread and unchanged application/DOS gates still remain.

Launch follow-up: r1 produced no child/build log; PID13560 was subsequently
confirmed absent with an unrestricted read as well. The replacement is
frozen-contract-r2, supervisor PID26312 started13:09:30 local and build child
PID24540 started13:09:31, with deadline20:39:31 UTC. Both were confirmed live
using the same permitted process visibility as their launch; restricted
Get-Process could not see these processes and must not be used to infer their
termination. Worker graph generation completed. r1 is not a build failure or
passing test; r2 was then the one owned active run. No publication occurred.

### Safe-pause audit after the integrated build gate

r2 subsequently ended exit1: worker build completed, but the provider reported
C2011 duplicate wowuserp.h declarations in borrow/window carriers. r3 exposed
the same new dependency through precomp.h in the thunk carrier. Both ended
naturally; no build/test was interrupted. Removed redundant includes and
exposed the existing fatal-boundary declaration through the smaller task-order
header. W32FreeTask also receives its opaque /Gz declaration at the existing
forced wkman compile boundary. No original ABI definition changed.

r4 built worker and provider successfully (build.exit=0/result.exit=0).
Hook ordering passed. Timer ordering did not execute: fixture linking failed
on printf under the production _NO_CRT_STDIO_INLINE profile. Both new fixtures
now use the established Hook fixture's fprintf/explicit __cdecl main pattern.
This is fixture repair, not timer runtime acceptance. r5 runs the same group;
supervisor PID23292 started 2026-10-15 13:19:13 local. Its owned child records
retain deadlines/logs. Unrelated Ninja processes were identified and untouched.

The reviewer again rejects S2 closure. This finite matrix governs remaining
work without creating new families or moving unsafe foundations to successors:

| Blocker | Candidate and missing evidence | Dependency and completion condition |
| --- | --- | --- |
| A1 / F02--F05 | Required borrow pop, cache/owner restoration and fatal-child assertions are connected, but the complete candidate foundation group has not passed. | Existing owner/borrow only. Pass normal/nested/exception/denied-entry and every injected reverse-edge failure, retaining real guest CBT recovery. Fatal recovery must prove no continuation. |
| A2 / F07/F12 | Timer and three COMMDLG entries are wrapped, retaining original conversion. Synchronous GDI and unconnected EDIT word-break have source dispositions. Mocks are not provider proof. | A1. Finish the fixed family table's inherited/acquired/unavailable dispositions; run real guest ordinary/modal timer with nested font enumeration, modeless COMMDLG resource cleanup and remaining generic callback/frame-resolution negatives. No new subsystem feature is required. |
| A3 / F01/F11 | Early window cleanup precedes original resource free; final unbind precedes CCPU release and cannot discard refusal. Native control cleanup targets are retained. | A1/A2. Verify actual terminal-thread paths/refusal, modeless resource lifetime, ordinary/startup-failure/exception exit, exact-once retirement and surviving sibling operation. Preparation-only/mock-free tests cannot close this row. |
| A4 / F10/F12 | Actual timeout/dead-target assertions added; consumed-once post-native failure policy retained. | A1/A3. Pass held/unheld ordinary/nested/timeout/error returns with result/LastError/owner assertions; finish exception and receiver-loss-during-call coverage. A target dead before entry is not loss during a call. |

After A1--A4 pass, remaining delivery gates are the existing independent
application frontiers, both DOS17 matrices, coherent13 publication/hash checks,
mirror/guest audit, governance and reviewed commit/push. S3 retains concrete
controls/EDIT/dialog/menu functionality; S4 complete message identity/replies/
cross-task reentry; S5 targeted termination; S6 reconciliation, not new repairs.
No semantic waiver is inferred. All current modifications are retained and
remain unpublished until their gates pass.

### r5 completed: foundation counterexample, not closure

r5 ended naturally with exit1 at graph-snapshot. Worker/provider build and
all three ordering fixtures passed: Hook enrollment=6/accepted=1/rollback=4;
timer calls=30/nested/exception/denied=2; COMMDLG entries=3/resource branch/
nested/exception/denied=3/early-order/rejected-free, each errors=0.
These latter two use production wrappers with mocked owners/originals, not
actual guest resource cleanup. No subsequent foundation case ran.

graph-snapshot/stderr.txt records the first failed assertion at
tests/mvdm-host/wow_page_domain_fixture.c:952: the enrolled native BUTTON's
native_cleanup is NULL before terminal preparation. The current recognition
path matches invoked procedures against registered standard targets;
enroll_created_window instead captures the actual GWL_WNDPROC, which can be
an opaque CallProc value. Recognition/retention at this boundary has not been
proved complete. This is an A3 blocker; do not replace the failed assertion
with a class-name assumption or call DefWindowProc a verified control cleanup.
Precise token/dispatch attribution remains to be established before correction.

The same run also fails the class/window allocation restoration checks at
lines2097,1203,1372 (first reported free-memory delta is16384bytes).
Those are actual failures, not confirmed independent leaks or proven effects
of the first failure. Isolate retained object/alias ownership and restore exact
pre-case free memory before accepting A3. Preserve the successful normal
destruction case and failed terminal case in the integrated regression.
This adds concrete evidence under A3, not an additional subsystem or blocker ID.

Documentation governance including relative links and diff whitespace checks
passed in frozen-scope-docs-r1 (exit0). This final result entry is verified
again before handoff. No candidate publication, product commit/push or S2
closure is claimed. P15 remains the last published runtime baseline.

### Native procedure identity follow-through after r5

Source comparison (pinned OpenNT windows/core/ntuser/client/classc.c GCL_WNDPROC,
rtl/getset.c GWL_WNDPROC and client/clmsg.c CallWindowProcAorW) establishes that
class and window queries may wrap the same target as distinct CPD_WNDTOCLS
and CPD_WND handles. Direct token equality is not target equivalence. Public
CallWindowProc retains conversion; no private CPD decoder is admitted.
The candidate now observes the actual per-window procedure in the window's
own A/W encoding before installing the ANSI entrance, using the existing
standard-target recognition and cleanup fields. Later recognized procedure
calls still update the cleanup target. It does not infer a target from class
name or replace the retained native dispatch token used by ordinary calls.
This correction is unverified until the next integrated run.

The fixture already creates an isolated desktop through run_graph_desktop;
the earlier tentative interactive-desktop explanation is rejected. The four
free-memory failures remain failures. Added read-only before/after committed
view diagnostics identify each retained HWND/class and its live/dead status;
no free-memory assertion, retirement gate or failure count was weakened.
Distinguish process-owned native helper windows from retired task bindings
before selecting a correction. No guest or additional mirror edits.

r6 (supervisor30272, start2026-10-15 13:29:50 local) completed exit1 at the
same graph case. Build and the three ordering cases pass, but the A/W capture
candidate does not yet establish the BUTTON cleanup target. Retain this as
a failed candidate, not a solved CPD attribution. The next witness records
the actual retained dispatch target and both public class targets before the
unchanged assertion; no private CPD dereference or class-name fallback.

The view witness changes the memory diagnosis: before the case only desktop
#32769 is present; afterwards an IME observation is dead and the desktop
still names the destroyed dialog. Source follow-through finds the new terminal
fixture still calls raw DestroyWindow, while production early teardown calls
wow_native_DestroyWindow and performs its post-return view refresh. Corrected
that fixture to exercise the actual production boundary (the normal case
already did). Exact free-memory and empty-table assertions remain mandatory;
this source mismatch is not proof that all four failures are resolved.

Second-role review also found a real A1/A3 candidate defect: the modeless
FindReplace WM_DESTROY branch releases original resource/COMMDLG records,
but the wrapper returned FALSE if task/borrow/view preparation failed. That
would silently skip irrevocable native destruction cleanup. Those three
resource-only failure edges now use the existing fatal outcome; ordinary app
callback denial remains FALSE. The ordering fixture adds all three rejected
resource edges and requires no original-body execution, no ordinary return
and complete mocked unwind. Real fatal-child/guest-resource acceptance is
still separate. Original COMMDLG conversion/resource bodies remain unchanged.

### r7 return-boundary evidence and remaining native provider identity

r7 (supervisor33956, start2026-10-15 13:35:19 local) ended exit1 at graph.
Build and all three ordering cases passed, including the new COMMDLG
resource-failure marker (errors=0, fatal_edges=3). Actual graph evidence now
removes the dead IME/desktop links and passes view-retirement/dead-graph cases.
The initial memory difference falls from16384 to4096 bytes; the following
unowned-view allocation check also fails. These are not complete passes.

BUTTON's retained target was76a28f00 while public class A/W targets were
77b15670/77b15680. The A/W enrollment candidate still leaves fnid/cleanup zero,
so it is withdrawn rather than kept as a claimed repair. The actual provider
must be identified; neither a class-name fallback nor private CPD decoding is
accepted. The fixture now identifies its module/RVA through public APIs at
the failed assertion without changing the dispatch or accepting unknown targets.

Source review of the residual allocation ordering finds that native return
publication precedes releasing the explicit WND borrow. That last release
can remove a retired WND, leaving its now-unreferenced observed CLS until the
next collect_observed_classes call. An exiting thread may have no later borrow
scope. The existing terminal final collection now drains observed classes too;
the preparation fixture exercises that same post-final-borrow collection.
All exact allocation/table assertions remain. No new collector or class table.

A4 review found the same acquired-state rejection pattern at native-call begin:
two runtime_leave failures returned FALSE after assigning call->binding, and
one after publishing psmsSent/descheduling. Callers then omit the end scope.
Both now use the existing fatal outcome, matching callback/post-native policy.
An actual entry-boundary fatal child injects inconsistent lock ownership;
the observer requires its signal and noncontinuable failure exit. Recoverable
pre-acquisition failures retain their FALSE/error result. This remains A4,
not new message identity/scheduling scope, and is unverified pending the group.

### r8 collection acceptance and HWND-bound native-class candidate

r8 (supervisor22344, start2026-10-15 13:41:27 local) ended exit1 at graph.
Build and ordering gates pass. The final-borrow collection restores exact
allocation equality (6139904 before/after); unowned, promotion, retirement,
dead-graph, preparation and words cases report errors=0. The remaining native
class aggregate error is the BUTTON target assertion. Later graph fatal-child
checks, including the added native-entry negative, have not run.

The retained target76a28f00 is USER32.dll RVA78f00, while class-query targets
remain77b15670/77b15680. Link-map review confirms GetClassInfoExA/W are public
USER32 imports, not project replacements. No claim about those latter targets'
provider module is made without evidence. The global/native-class query value
is not enough to identify this HWND's actual procedure.

The revised bounded candidate retains global standard-target recognition for
explicit cross-class calls, then compares an otherwise unrecognized called
procedure with GetClassLongA/W on the actual HWND. This fallback applies only
to the already imported native registration, never a project/application
class, and requires exact procedure equality. It retains that window's public
ANSI class target for CallWindowProc cleanup. No class-name/DLL-name matching,
CPD decoder, extra registry or new mirror diff. An unknown procedure leaves
the prior target unchanged; the fixture asserts that and lock/LastError
preservation. This candidate still needs integrated acceptance before any
runtime publication; complete A1--A4 remain the unchanged S2 exit gates.

### r9 safe-pause audit: actual assertions versus observer completion

r9 (supervisor32252, start2026-10-15T20:46:42.3671156Z) finished exit1.
Build and Hook/timer/common-dialog ordering passed. The actual CCPU graph
fixture finished exit0 with WOW_PAGE_DOMAIN_LIFECYCLE_OK. Native terminal
preparation now records fnid029f/cleanup76a28f00, native_button=1 and errors=0;
allocation is equal at6139904. All six fatal-child markers (post-native,
native-entry, callback-owner, callback-borrow, WND cache, dialog cache) report
errors=0 and worker_exit=c0000420. Held/unheld discovery cases also pass.
These are bounded provider assertions, not real guest terminal/timer proof.

The observer still correctly rejects the run: WOW_EXPORTED_CALLBACK_REJECT
is absent. Source shows that this new witness uses puts, which the selected
original fprt.c PROD implementation discards. The same fixture already uses
fprintf(stderr) for that reason. Changed only the witness output channel to
that established route; invalid-size/output/error assertions are unchanged.
Do not silently accept a missing marker or mark the whole graph gate passed.
The remaining eleven foundation groups were not reached by r9.

r10 is the integrated rerun, supervisor33324, start2026-10-15T20:50:08.6052401Z,
with its own logs, child identities and deadlines. No previous run is still
using the shared build roots. No new feature, guest change, publication or
delivery is implied. F01--F12 remain the fixed denominator. Residual acceptance
still consists of A1 full nested/negative restoration; A2 selected-family
context and real guest timer/modal/resolver proof; A3 actual terminal caller
and resource/sibling cleanup proof; A4 completion-class return proof. The
S3/S4/S5/S6 ownership table remains unchanged.

### Terminal caller acceptance, separate from guest resource lifetime

The checked-in tests/observation/verify-thread-start-boundary.ps1 runs the
actual session/thread-start sources with the expanded existing fixture,
MSVC x86 /O2 /MT and no substituted session owner. All compiler/object/link
outputs stay in the supplied fresh build root. Each child records its exact
PID, UTC start, command, deadline and actual exit, and the runner requires
both assertion markers rather than treating process exit as sufficient.

terminal-entry-r1 (supervisor32972, start2026-10-15T20:52:16.8299088Z) passed:
THREAD_TERMINAL_UNBIND normal=5 once=1 rejected_exit=c0000420;
THREAD_TERMINAL_INTENT terminal=5 ordinary=1 rejected_reset=1 exception_reset=1.
This covers the four supported thread routine ABIs, explicit native exit,
fatal refusal, retryable ordinary detach and exception reset of terminal
intent. It does not prove guest modeless-resource cleanup, surviving sibling
operation or actual CCPU release ordering. A3 therefore remains open.

frozen-guest-r1 (supervisor724, start2026-10-15T20:54:22.7389122Z) exercises
the real timer/modal/font and retained CBT probes serially against an isolated
copy of the P15 package with the just-built worker/provider. Its package hash
inventory distinguishes the candidate from published P15. No original guest
bytes are rebuilt or patched; the independently authored NE probe is a test
input. Results remain pending until the recorded children finish.

frozen-guest-r1 ended before execution: NASM interpreted adjacent `%512` as
a macro token. Corrected the independently authored probe's modulo spacing,
matching the existing CBT probe. frozen-guest-r2 then ran and wrote all three
phase records, but errors=1 rejected acceptance. Dispatch evidence identifies
WU32EndDialog returning zero; original wow16/inc/windows.h declares EndDialog
void, and unchanged wow32/wudlg.c WU32EndDialog explicitly RETURN(0).
The probe had incorrectly asserted Win32 BOOL semantics. Removed that invalid
AX assertion; retained actual outer MessageBox result=IDOK and modal completion.
No runtime or guest-media change was made to accommodate the test.

frozen-guest-r3 (supervisor7260, start2026-10-15T20:56:02.1634557Z) finished
exit0. timer/guest-report.json has phases0,1,2 with errors0; final ordinary=1,
modal=1, killed=2, modal_result=1, freed=1, fonts=2 and font_reads=2. Thus the
actual original timer/modal and nested font guest-buffer paths now have
provider evidence, not only mocks. The subsequent actual CBT probe also
passes query/nested creation/veto/recovery/unhook and launcher completion.
The runner's cleanup is test containment, not proof that all worker objects
retired naturally. COMMDLG resource lifetime, resolver edge cases and other
unproved fixed-family contexts remain A2/A3, not inferred passes.

## Safe-pause result after r10: fixed scope retained, closure refused

The owned r10 supervisor ended naturally with exit0 after its worker/provider
build, three ordering fixtures and all twelve foundation cases. No running
build/test was interrupted. Exact checked-in cases are graph-snapshot,
window-borrow, window-reclaim, process-retirement, dialog-lifecycle,
menu-resource, thunk-scope, menu-cleanup, native-send, native-nested,
task-lifecycle and native-creation. Their .exit and assertion output are
retained separately, so a later case cannot conceal an earlier failure.

Particular new evidence: graph includes the exported invalid-argument marker
and all six expected fatal-child exits; thunk has calls4/exception1; dialog
has rollback4 and paired exception restoration; native creation has callbacks51,
nested1, rejected1, vetoed1, destroyed7 and table_empty1. Native nested returns
114, both held/unheld timeout paths retain1460 and their original ownership,
and a receiver dead before entry retains1400. None of these asserts receiver
loss during an active call or general mixed-message identity.

Single-person second-role conclusion: A1's admitted restoration edges now
have candidate implementation and the listed normal/nested/exception/fatal
evidence, including real CBT recovery. This removes its known local proof
gap; it does not turn the unpublished candidate into a delivered P. No entry
is removed from the F01--F12 denominator. Remaining S2 blockers are finite:

| ID | Exact residual cause/boundary | Repair or proof scope and dependency | Required acceptance |
| --- | --- | --- | --- |
| A2 | Non-WND families do not share one implicit WND owner. Timer/font and CBT are now proven, but exported/general callback resolver failures, modeless COMMDLG, print-abort and custom Winsock context/provider dispositions are not all complete. | Finish only the existing F07 table. Retain original conversion/resolver; repair a demonstrated missing common owner at its admitted seam. Depends on A1; modeless resource coverage shares A3. No new printer/network/control subsystem. | Every fixed family gets source-backed inherited/acquired/unavailable disposition; actual distinct reachable ownership mechanisms and nonpresent/remapped/failed-frame edges have result, alias-lifetime and unwind assertions. Keep real timer/modal/font and CBT passes. |
| A3 | Terminal entry is tested, but mock resource cleanup and native preparation cannot prove original modeless-resource lifetime, all normal/startup/exception exit paths or sibling survival. | Early W32FreeTask preparation plus existing pre-CCPU terminal boundary only; original frees retained. Depends on A1/A2. No targeted task deletion or broker expansion. | Actual modeless cleanup before original PRES free, no guest callback after CCPU release, exact-once task/window/class/input/wait retirement; ordinary/startup-failure/exception paths and surviving sibling operation. Retain five terminal-entry and fatal-refusal assertions. |
| A4 | Timeout and already-dead target now pass, but native exception and receiver disappearance during the call are separate completion classes without complete evidence. | Common begin/end wrappers and returned-object publication only. Depends on A1/A3; do not implement full S4 association as a precaution. | Actual call wrappers preserve result/error, one execution owner, caller lock and psmsSent after those two completion classes; stale identities cannot revive and failure cannot replay a consumed native operation. |

After these pass, the unchanged delivery gates remain independent application
frontiers, both DOS17 matrices, thirteen-image publication/hash equality,
mirror/guest and documentation review, commit/push. These are delivery work,
not authority to add capabilities. S3 controls/EDIT/dialog/menu, S4 full message
identity/reply/cross-task reentry, S5 individual termination and S6 reconciled
acceptance retain the full T scope. This audit does not move an A defect there.

P15 remains published: assets/release worker/provider hashes still match the
baseline above and no assets diff exists. Current candidate edits are retained,
not discarded, submitted as a delivered P, or silently installed for the owner.
Mirror delta remains only DIV-333--335: five host definition renames across
three files, with original bodies intact. No guest-media change/recompile.

Documentation run frozen-scope-docs-r3 did not reach its child gate: its
supervisor was confirmed absent and no exit/child record exists. It is not a
pass. A fresh owned documentation gate validates this final audit record;
its terminal result, not that failed launch, determines the document outcome.

## Remaining lifetime group: source-led acceptance additions

The preceding goal turn made progress: r10 and real timer/CBT proofs changed
the residual matrix. Documentation/link/diff gate frozen-scope-docs-r4 ended
exit0; S2 stayed active, not delivered. Current work rechecks the dirty tree
and retains all prior edits. No new S or feature is admitted.

Source tracing of WCD32FindReplaceText/GetCommdlgTd/FindReplaceDialogProc and
wres16.c confirms the normal and terminal resource path: native FindText
stores two COMMDLGTD records; FR_ENABLETEMPLATE loads an original PRES;
WM_DESTROY invokes original UnlockResource16/FreeResource16 and frees those
records. The native dialog remains modeless after its creation thunk returns.
The task's existing creation observer enrolls native children of an enrolled
owner; there is no need for a new COMMDLG window registry. The resource-only
callback must run before W32FreeTask's DestroyRes16, not after CCPU release.

Added an independently authored NE witness with a minimal RT_DIALOG template
and explicit FR_ENABLETEMPLATE/FR_ENABLEHOOK. It creates/destroys one modeless
dialog normally, creates another and exits with it alive. Three guest phase
records assert creation, hook delivery, explicit destruction and the live-exit
condition. Existing default-off callback/retirement traces share one output
stream; the observer requires original resource unlock/free callbacks for
the reported task before exactly one successful retirement. This proves a
bounded resource path, not all COMMDLG/control features or heap leak freedom.
Original guest binaries and templates are not modified or recompiled.

A4 adds actual production SendMessage wrappers with same-thread exception
propagation in held/unheld cases, plus cross-thread destruction of the target
window during its callback. Both retain owner/psmsSent assertions; destruction
must return116 and leave the HWND dead. The receiver task stays alive for
normal exit, so this is not falsely called receiver-thread death. No forced
task termination or full S4 message identity is introduced. These additions
are unverified until the grouped lifetime run completes.

The group reuses r10's unchanged worker/provider identities, checked against
the accepted frozen-guest-r3 package; only the changed native fixture is
rebuilt. Its followed guest group is common-dialog, timer and CBT. Twelve
unchanged foundation cases retain r10 evidence rather than being rebuilt for
each new assertion. Runtime delivery gates remain unchanged and still pending.

## Latest safe-pause verdict: lifetime-r1

This section supersedes earlier pending results, not the fixed denominator.
The owned lifetime-r1 supervisor34624, start2026-10-15T21:13:38.4349782Z,
ended naturally with exit1. No build/test was interrupted. native-return
passed; common-dialog failed; its following timer/Hook cases were not reached.
Their earlier frozen-guest-r3 pass remains valid for the unchanged candidate,
not a claim that lifetime-r1 reran them. Existing edits are preserved. No new
runtime edits, guest changes, publication or source delivery follow this audit.

### Confirmed A3 counterexample and bounded repair

common-dialog/guest-report.json reports phases0/1/2, errors0, final task6079
(17BF), created2, closed1, hooks2, leaving1. These guest facts alone pass.
The chronological lifetime.txt records original RET_UNLOCKRESOURCE0014 and
RET_FREERESOURCE0012 for the first dialog. It does not record the second pair
for the dialog left alive at task exit. Instead, late WM_DESTROY reaches
RET_FORCESEGMENTFAULT0024 for guest address1797:0038, then task retirement.
The observer correctly rejects missing resource cleanup despite normal exit0
from the application. This is a foundation lifetime defect, not S3 dialog UI.

Source establishes the ordering: wow16/kernel31/i21task.asm sends SG_EXIT
before GlobalFreeAll; wow32/wuser.c WU32SignalProc calls ModuleUnload, whose
task-specific pfnWOWCleanup precedes guest resource release. Project
wow_user_registration_bridge.c registered_cleanup currently calls
wow_user_task_lifecycle_exit without native-window preparation. Candidate
W32FreeTask preparation is later than GlobalFreeAll and therefore insufficient.
No change to those guest instructions is permitted or necessary.

The repair boundary is the existing nonzero-task pfnWOWCleanup callback:
validate matching task, prepare/destroy its native windows outside the USER
lock while guest resource callbacks remain usable, then existing original
cleanup. Preserve task==0 module cleanup and the thread carrier. Failure must
not silently continue into guest frees merely because the original caller
ignores the BOOL. Review W32DestroyTask and WK32KillRemoteTask late paths;
remove a superseded wrapper/rename if unnecessary, or retain only an explicitly
proved fallback. Do not add another registry, guest patch or target-termination
feature. Acceptance still requires both real unlock/free pairs before the
single retirement, plus startup/exception exit and surviving-sibling safety.

### Per-entry candidate overlay

The original table above remains the P15 baseline; this overlay associates
every fixed entry with the current proof and residual, avoiding aggregate-pass
substitution. Implementation paths remain those of the baseline table plus
the explicitly described timer/COMMDLG/terminal adapters above.

| Fixed entry | Candidate evidence | Remaining closure boundary |
| --- | --- | --- |
| F01 initialization/session | r10 task-lifecycle and terminal-entry-r1 five ABIs | A3 actual startup-failure/exception retirement and sibling survival |
| F02 thunk/frame | r10 thunk-scope and exported invalid-argument assertion; guest-r3 real frames | A2 nonpresent/remapped/failed-frame and exported positive ownership |
| F03 WND callback | r10 graph/borrow/reclaim and six fatal children; guest-r3 CBT | A1 local gap discharged; retain regression after A3 repair |
| F04 dialog callback | r10 dialog-lifecycle/native-creation rollback/nesting | A3 real modeless resource counterexample above; feature completeness is S3 |
| F05 Hook | r10 hook-order; guest-r3 CBT veto/recovery/unhook | A2 remaining selected-family context disposition, not new Hook features |
| F06 discovery | retained E1/E5 and r10 identity tests | Original recursive EnumProps limitation remains explicit |
| F07 other callbacks | r10 timer/common-dialog fixtures; guest-r3 timer/modal/font; lifetime-r1 real COMMDLG | A2 fixed family/resolver ledger and A3 resource repair |
| F08 WND/CLS identity | r10 graph/borrow/reclaim/native-creation | A3 terminal exact-once retirement; external view stays approved E |
| F09 MENU lifecycle | r10 menu-resource/menu-cleanup/process-retirement | Retain regression; actual menu features remain S3 |
| F10 native return | r10 return/timeout/dead-before-entry; lifetime-r1 exception/destruction | A4 receiver-thread loss during active operation remains unproved |
| F11 normal teardown | terminal-entry-r1 and r10 retirement fixtures | A3 real resource-before-free, CCPU ordering and sibling proof |
| F12 wait/resume | r10 native-nested; guest-r3 modal timer | A4 active receiver loss; full association/reply semantics stay S4 |

Native-return now proves held/unheld native exception propagation, preserved
447a4002 error and execution/psmsSent ownership. Cross-thread callback destroys
its HWND, returns116 and leaves it dead with caller ownership restored. The
receiver thread remains alive: this is explicitly not proof of receiver-thread
loss. A4 is smaller, not closed.

Remaining work is A2 fixed context/resolver proof, A3 the confirmed early
cleanup repair and finite exit cases, and A4 active receiver-loss completion.
Dependencies and acceptance remain the residual table above, except that its
W32FreeTask-only A3 location is superseded by the earlier original signal
boundary proved here. After this group, run application/DOS/publication gates.
S3 retains controls/EDIT/dialog/menu features; S4 full message identity/reply/
cross-task interactions; S5 per-task termination; S6 final reconciliation.
None receives an unresolved S2 foundation defect. S2 cannot close now.

## Early signal-boundary implementation, acceptance pending

The previous goal turn changed the authoritative audit and confirmed the
resource counterexample, so it was progress rather than an idle wait. Current
source reinspection confirms the signal and the second original exit path.
Task-specific lifecycle_exit now prepares native windows before acquiring
the cleanup lock; module-only task0 cleanup does not. registered_cleanup
enforces the existing fatal boundary on rejection because original ModuleUnload
ignores its BOOL. Original SG_EXIT/SG_GP_FAULT and resource algorithms stay
unchanged. WK32KillRemoteTask directly invokes W32FreeTask without this signal,
so DIV-334 remains a bounded fallback, not the normal modeless cleanup owner.
No additional mirror rename or guest change is introduced by this repair.

frozen-contract-r11 supervisor28496, start2026-10-15T21:24:38Z, runs the
unchanged grouped build/three ordering/twelve
foundation gate with the expanded native-return assertions. Its recorded
child34244 is live at recheck; no duplicate build is started. The separate
early-exit guest runner requires successful r11 before testing modeless
resource, modal timer/font and CBT against a fresh sealed candidate copy.
No result or publication is claimed until these assertions finish.

### F07 source dispositions refined without adding wrappers

The selected synchronous paths are not missing a WND wrapper merely because
they call CallBack16 directly. wow_user_invoke_thunk retains the existing
borrow scope through the original thunk and unwinds it in finally. Original
W32GrayStringProc (wuser.c) is invoked inside WU32GrayString's native call;
W32EnumFontHandler (wgfont.c), WG32EnumMetaFile (wgmeta.c), and WG32LineDDA
(wgdi.c) likewise pass original stack callback state to their synchronous
native enumerators. They inherit the current guest task, not a new native
background owner. Their original signed/BOOL/void result conversions remain.
Timer/modal dispatch is different and retains its proved explicit adapter.

Generic thunk WK32ICallProc32 (wkgthunk.c) directly invokes the native target
on the current VDM thread; its native-to-guest WOWCallback16/Ex reentry thus
inherits that caller. wcall16.c rejects oversized cbArgs before writing the
return output (r10 proof), otherwise retains its original Pascal/cdecl tail
handling and CallBack16 frame resolver. Positive generic callback and frame
failure/remapping assertions remain required: source disposition is not a
substitute for these distinct argument/frame mechanisms. This does not admit
arbitrary foreign-thread callback invocation with no VDM task.

Print abort is registered by WG32SetAbortProc (wgdi31.c), and the older Escape
path (wgdi.c), to original W32AbortProc. It reads the current task's TDB abort
procedure, invokes RET_SETABORTPROC and converts LOWORD to BOOL; no WND owner
can supply that context. Provider disposition remains explicit acceptance
work, not permission to build a printing subsystem. Original Winsock startup
installs WWS32CallBackHandler and its per-thread blocking-hook state; that
handler returns the low byte of RET_WINSOCKBLOCKHOOK, or the original default
blocking hook. Do not replace these original bodies with one blanket callback
framework merely to make the family table uniform.

### Foreground-idle is not covered by CBT acceptance

Continued F07 source audit identifies a concrete registration/binding
inconsistency, not a new subsystem admission. Original kernel31/i21task.asm
calls WOWSetIdleHook when an application sets interrupt2F. Selected wkman.c
WK32WowSetIdleHook registers WK32ForegroundIdleHook as WH_FOREGROUNDIDLE,
thread0, and retains hIdleHook in the original TD; the callback directly
invokes RET_FOREGROUNDIDLE. It does not traverse WU32StdHookProc.

At the retained original-source pin, OpenNT windows/core/ntuser/kernel/hooks.c
xxxCallHook filters a 16-bit installer's non-intersend hook to 16-bit threads
in the same process. taskman.c selects the current task before calling this
hook when all tasks are idle, then clears that selection and rescans. These
are source-owned context rules, not a license to invoke guest code in an
arbitrary modern native thread or to import the USER hook server.

Current project wow_user_task_lifecycle.c idle_hooked unconditionally returns
FALSE with an absence comment, while the original registration above is
selected and reachable. idle_hook terminates if reached. Thus the absence
comment is not proof that this family is unavailable, and native global
registration alone does not connect the retained taskman's callback path.
No new idle Hook was installed to investigate this; the finding is source
based. This remains under A2/F07: reconcile existing registration/context
binding and explicitly prove ownership/return/unwind. Ordinary CBT evidence
cannot discharge it. Do not silently convert the original process-restricted
contract to unrestricted global delivery or a merely thread-local substitute.
Full application Hook/message interaction stays with S4; this existing entry's
safe execution contract does not. No runtime patch for this edge is claimed.

The in-flight r11 and dependent real-guest job retain their frozen inputs;
this audit does not restart them or modify their source during execution.

## r11 terminal result and bounded error-contract correction

r11 ended naturally with exit1 at menu-cleanup. Build, the three ordering
fixtures and the first seven foundation cases passed. The failure is exact:
wow_user_task_lifecycle_fixture.c line965 expected
ERROR_INVALID_WINDOW_HANDLE for its already-destroyed native window, but
early preparation returned ERROR_INVALID_STATE(5023). Task retention and the
remaining menu assertions ran; the result is not accepted. The dependent
early-exit-r1 job stopped at its prerequisite, before guest execution.

Source review locates the mismatch in wow_window_words_prepare_exit's combined
owner/property validation. Added an explicit IsWindow rejection before that
validation, retaining ERROR_INVALID_STATE for actual owner/lock/property
mismatches. No test assertion is weakened; no native operation is retried.
Updated adapter comments to distinguish SG_EXIT/GlobalFreeAll from the later
host resource teardown. No new mirror change or guest modification.

r12 supervisor31336, start2026-10-15T21:37:39.1577530Z, rebuilds the candidate
and runs the five terminal/native cases beginning with menu-cleanup. Retain
r11's seven preceding cases and three ordering results as their bounded
evidence; do not spend another whole rebuild on each assertion. The changed
guard is in terminal preparation, not those callback/conversion algorithms.
This does not waive the final integrated delivery gates after A2--A4 repair.
Dependent early-exit-r2 supervisor12048, start2026-10-15T21:37:39.2232899Z,
waits for that exact supervisor identity and successful result before the
fresh-package modeless-resource, timer/font and CBT group. Both jobs retain
separate results and deadlines. No new runtime publication or closure yet.

## Idle binding batch prepared without disturbing r12 inputs

The added wow_idle_hook_binding.c/.h is not yet selected by the generator;
r12 continues using its frozen linked images. The adapter queries original
gptdTaskHead/TD.hIdleHook under original gcsWOW, retaining list lifetime and
avoiding a parallel registration table. An empty pre-initialization list
does not enter the not-yet-initialized lock. No lock spans native/guest entry.
The callback gate rejects missing provider task context before touching TD,
chains native negative/foreign callbacks, and otherwise uses existing task
and borrow scopes around the unchanged original foreground-idle body.
The scheduler dispatch helper leaves selection/rescan to original taskman.

The bounded ordering fixture is also added but not run: it checks registration
on the existing list, last-error, nested/exception unwind, admission rejection,
foreign/missing-task rejection and retirement. It installs no global native
hook and cannot prove real scheduler/guest delivery. Subsequent coherent
selection must replace constant-FALSE idle_hooked and the terminating idle_hook,
rename only the original callback definition (not its body/registration),
add the adapter to the provider, and include ordering plus real taskman/guest
acceptance. This would add one host definition rename, not another scheduler
or registry. Current actual mirror delta remains the prior five renames.
Do not count these unselected sources as a repaired runtime capability.

Second-role source review tightened the native gate: a valid local WOW task
alone is insufficient, because another worker may have installed the same
global procedure. Require this process's original TD list to contain its own
live hIdleHook too. All these registrations name the same original procedure,
which ignores installer parameters and returns0 without chaining after its
one current-task INT2F callback. A process with no own registration must chain
without entering guest; multiple own/foreign identical registrations must
not create multiple guest callbacks. Preserve this explicit two-worker case
in acceptance rather than claiming that a nonnull runtime proves isolation.

## r12 real-resource outcome and next frozen batch

r12 finished exit0: menu-cleanup, native-send, native-nested, task-lifecycle
and native-creation all pass. The original invalid-window assertion remains
unchanged and passes. Native creation retains callbacks51/nested1/rejected1/
vetoed1/destroyed7/table_empty1; no terminal object is hidden to pass the test.

early-exit-r2 finished exit0 with common-dialog, timer and Hook all passing.
For real task17BF, chronological lifetime.txt now contains two completed
RET_UNLOCKRESOURCE0014 and two RET_FREERESOURCE0012 callbacks before its
single TaskRetirement complete/status0. Guest report errors0, created2,
closed1, hooks2, leaving1 proves the second dialog was still alive when
guest initiated exit. This directly repairs the lifetime-r1 counterexample;
it is not inferred from process disappearance. Timer phases0/1/2 all report
errors0, with ordinary1/modal1/killed2/modal_result1/freed1/fonts2/font_reads2;
CBT also passes. No guest file is patched/rebuilt. A3 still needs its other
fixed exit/sibling cases; A2/A4 are not discharged by modeless cleanup.

After these frozen package tests completed, the idle batch is selected:
lifecycle idle_hooked and idle_hook now validate the current task and use the
existing TD query/original dispatcher, deleting constant absence and the
unconditional termination substitute. Provider generator selects the adapter;
DIV-336 renames only WK32ForegroundIdleHook's definition. Actual mirror delta
is six definition renames across the same three host files, original bodies
unchanged. The idle ordering observer is wired with its required assertions.
No build/test of this newer batch is claimed yet; r12 evidence belongs to its
sealed package, not automatically to the changed working tree. Finish the
remaining coherent acceptance additions before another product build.

## Idle/receiver-exit acceptance batch

Added an independently authored idle NE input: save INT2F vector, install its
own handler through the unchanged guest INT21 service, wait for two bounded
timer messages, count INT2F:1689, restore and verify the prior vector, exit.
Its own ordinary NE selector relocation gives the interrupt handler its data
segment without patching guest kernel code or external memory. Non-idle
interrupts chain with the original frame and registers. The observer requires
three phase records, zero errors, actual taskman's default-off idle-dispatch
trace and exactly one task retirement; a native hook hit alone is insufficient.
The existing trace records only entry, never alters scheduling.

A4's native-nested case now creates two further real registered receivers,
for held/unheld callers. The receiver destroys its window and releases the
fixture's borrow, performs actual runtime/session retirement and calls native
ExitThread while the sender is inside production SendMessageTimeout with
SMTO_ERRORONEXIT. No TerminateThread or scheduler-field fabrication is used.
Assertions require failure return, native error retained within the native
failure classes, dead window/thread, missing receiver identity, original
caller lock/psmsSent and one execution owner. This is a native boundary test,
not proof of guest forced termination or S5. Existing exception/timeout/
window-destruction/dead-before-entry cases stay required.

Both changes are assembled into the next single product build, four ordering
fixtures and twelve foundation cases, followed by sealed-package modeless,
timer/font, CBT and idle guest probes. Do not mark any new case passed before
its actual assertion output. Previous r12 proof remains attached to r12 only.

The batch is r13, supervisor31260, start2026-10-15T21:53:25.4042899Z;
its dependent frozen-early-exit-r3 supervisor19796 starts at
2026-10-15T21:53:25.4657851Z and waits for that exact successful prerequisite.

Further original-source review places a hold on final idle acceptance even
if r13's mock ordering passes: W32Thread publishes its stack TD in
gptdTaskHead before assigning hIdleHook=NULL. Its hThread, however, is set
NULL before publication and assigned the real task thread handle only after
InitTask succeeds, before guest resumes. Querying hIdleHook on an arbitrary
published TD without that initialization boundary may read an uninitialized
field. The minimal correction is to require original hThread publication
before reading hIdleHook; failed startup cannot have registered a guest hook.
No new flag/registry or mirror reordering is needed. This is a source-found
adapter defect within A2, not a reason to loosen the test or close S2.
Do not edit r13's running build inputs or interrupt it; retain its results as
bounded evidence, then incorporate the guard with the remaining coherent
acceptance batch. Its native/global mock must include an incompletely
initialized TD whose hIdleHook bytes are nonzero and prove it is ignored.

## Completed safe-pause review after r13 and initialization guard

Both original supervisors completed naturally: frozen-contract-r13 and
frozen-early-exit-r3 each have exit.txt=0. The former built the candidate,
passed four ordering fixtures and all twelve fixed foundation cases. The
latter sealed its package/hash manifest and passed common-dialog, timer,
Hook and idle. Idle records phases0/1/2, errors0, idles1, ticks2, restored1,
task6079; its observer also requires original-taskman idle-dispatch and exactly
one retirement. Native-nested now records both held0 and held1 receiver exits,
error1400/retired1/owner1. The receiver actually exits during the native call;
it is not the old already-dead target case. A4's local gap is discharged.

Only after both suites finished, changed the idle query to require original
TD.hThread before reading hIdleHook. The ordering fixture supplies nonzero
hIdleHook with NULL hThread and requires no registration, then publishes the
thread and requires registration. Added the missing owner-local DIV-336
comment beside the already registered definition rename. No original function
body, guest binary or guest-source build changes. This adds no new capability.

Focused supervisor9920, start2026-10-15T22:11:00.0215297Z, runs
build/M0-T447/S2/frozen-idle-init.ps1. frozen-idle-init-r1 finishes exit0:
incremental worker/provider build, production idle-object ordering fixture,
and frozen-idle-init-r1-guests' fresh sealed common-dialog/timer/Hook/idle
package all pass. Input identity is distinct from r13. The broader twelve
foundation results remain r13 evidence, not a claim they reran after the guard.
No release gate is waived by this focused check.

Second-role conclusion: S2 cannot close. The fixed F01--F12 matrix above and
seven-family F07 census are the denominator. A1 restoration and A4 bounded
native-return proof are complete locally, not published. Exactly two local
blocker groups remain:

| Blocker | Root / bounded correction or proof | Dependency | Exit assertions |
| --- | --- | --- | --- |
| A2 | Distinct non-WND ownership mechanisms cannot be inferred from WND success. Finish existing selected-family source dispositions (including print-abort/custom Winsock), actual generic callback normal/stack-return proof and original nonpresent/remapped/failed-frame edges; verify two-worker idle isolation. Preserve original conversions/resolver, repair only a demonstrated missing common seam. No subsystem feature implementation. | A1 and unchanged original CallBack16/taskman/CCPU; reuse the now-passing timer/font/COMMDLG/idle routes. | Each family has inherited/acquired/proved-unavailable disposition. Each distinct reachable mechanism has provider evidence for result/error, alias lifetime, nested and failed restoration. Foreign-worker registration cannot enter the wrong guest. |
| A3 | Five real host terminal entry ABIs and real modeless cleanup do not prove all original guest terminal transitions. Complete ordinary/startup-failure/exception exit and surviving-sibling cases at existing early cleanup and pre-CCPU finish. Do not replace original resource frees or add targeted deletion. | A1/A2 and existing runtime/session lifetime owners. | Exactly-once task/window/class/input/wait removal; rejection cannot silently terminate with bindings; no guest callback after CPU release; surviving sibling remains usable. Observer forced cleanup is not success. |

No demonstrated unresolved defect is relabelled as S3 merely because it
involves a dialog. S3 still owns concrete controls/EDIT/dialog/menu functions;
S4 full message identity/reply/cross-task interaction; S5 targeted termination;
S6 completed-scope reconciliation. The T objective and C/W obligations remain.
After A2/A3, retain existing DOS17/application/mirror/guest/publication/hash/
governance/commit/push gates. P15 remains installed; no new runtime P, S closure
or final owner-acceptance invitation is claimed. Dirty implementation is
preserved. This is a finite remaining repair group, not a request to admit more
S2 features or a new semantic concession.

## Original generic callback mechanism: real guest proof

The next goal turn is progress, not a restatement: it adds checked-in two-sided
acceptance of an existing production mechanism without changing production.
Source trace: SDK wownt32.h's generic callback contract; original KERNEL
kthunks.asm CallProc32W and ordinals513/514/515/517; wkgthunk.c
WK32ICallProc32/MakeCall; wcall16.c WOWCallback16/Ex and RET_WOWCALLBACK16=56.
The entry inherits the original generic thunk's task. It is not an arbitrary
native-thread entry and requires no new task registry or parallel dispatcher.

Checked-in inputs are tests/observation/win16_generic_probe.asm and
wow_generic_callback_probe.c. verify-win16-hook-probe.ps1 -Case generic
assembles the independent NE, builds the native witness DLL entirely under
its evidence root, stages both only in the isolated package's tests directory,
and records source/helper/host/guest identities. Original guest binaries are
unchanged. Native witness obtains actual selected WOW32 exports, rather than
mocking callbacks or modifying task/CPU state. It verifies same-thread return,
PASCAL and CDECL results, nested depth, and the original oversized-argument
failure ERROR_INVALID_PARAMETER with output unchanged. The NE also verifies
CallProc32W stack restoration, callback counts and three resource releases.

frozen-generic-r1 supervisor18984 start2026-10-15T22:17:28.1927130Z fails at
the helper compilation: parameter names pascal/cdecl collide with Windows
header calling-convention macros. No guest ran and no product regression is
inferred. Rename those parameters only; preserve the failed record.
frozen-generic-r2 supervisor31608 start2026-10-15T22:18:19.0175309Z completes
exit0. Its generic, timer and common-dialog cases all pass. Generic report
phases0/1/2 have errors0; phases1/2 pascal3/cdecl1/nested1; final freed3.
callbacks.txt records actual before/after RET0038 and original guest stacks.
The native helper's rejected cbArgs17 does not enter a fifth guest callback.
This is real original generic execution, not a host-only order fixture.

Remaining A2 frame proof is narrower: GetPModeVDMPointerAssert in wow32.c
checks ExpLdt, invokes original RET_FORCESEGMENTFAULT for a valid nonpresent
selector, then consumes FlatAddress. CallBack16 snapshots caller fields before
its next resolution and re-resolves after guest return. Do not equate old
t335_s3_wow_callback_frame_lease_fixture with this path: its own comment says
it is no longer a CallBack16 mapping test. The old real-mode CCPU witness also
does not prove protected-mode nonpresent/remapped frames. Preserve original
exception/output semantics; do not introduce an early FALSE return to make
these original unchecked callers appear safe. This remains finite A2 work,
along with two-worker idle isolation and the fixed provider dispositions.
A3 retirement/sibling cases are unchanged. S2 and the full goal remain active;
P15 stays published, with no new mirror or production change in this group.

## Real original nonpresent resolver and discard/reload witness

Source-led extension follows wow32.c GetPModeVDMPointerAssert through
RET_FORCESEGMENTFAULT, and kernel31 ldreloc.asm's internal selector relocation,
ldutil.asm MyAlloc flag conversion, 3ginterf.asm GlobalHandle/GlobalReAlloc,
3gmem.asm grealloc(size0,MOVEABLE), 3gmoreme.asm gnotify and mark_sel_NP.
No host LDT/FlatAddress writes, injected guest state, or original guest patch
is used. The independent NE owns a third segment; the native witness obtains
the actual WOWGetVDMPointer export, resolves its bytes, and checks null input.
The NE uses original GlobalReAlloc to discard its own segment, requires
GlobalFlags GMEM_DISCARDED, then calls the same resolver again. The original
kernel owns its allocation, nonpresent transition and reload from the NE.

Preserved negative evidence: frozen-generic-r3 supervisor16292,
start2026-10-15T22:22:46.7748316Z, exits1 because merely omitting PRELOAD from
the data segment did not produce the required resolver callback. The program
completed normal generic calls; this is incomplete test coverage, not a
claimed production failure. r4 supervisor24304,
start2026-10-15T22:24:41.0069736Z, fails the complete report assertion before
resolver acceptance after adding discard to that data segment. Its18-byte
report is not a pass despite observer exit0. Source inspection identifies
MyAlloc's data-segment GA_DGROUP path, distinct from discardable code.
No exact failing internal data-segment branch is claimed from that report.

Use a readable moveable discardable code segment owned solely by the test,
first resolve it to establish resident contents, then discard/re-resolve via
the original APIs. r5 supervisor7456 start2026-10-15T22:27:31.2654599Z completes
exit0. Generic, timer and common-dialog cases pass. All three generic records
have errors0; final pascal3/cdecl1/nested1/freed3, selector178F. callbacks.txt
contains two before/simulate/after RET0024 pairs for address178F0000, task17BF,
with caller1797:53BC and callback1797:5396. This proves actual original
nonpresent entry and re-resolution after original discard/reload, plus normal
null resolution, not just mocked resolver invocation. Source/export identities
are retained in the sealed package and native-inputs manifest.

This does not prove that physical relocation changed the segment base, nor
that a nonpresent/invalid callback STACK is recoverable. Those cannot be
inferred from a nonpresent application-code segment. Retain the remaining
CallBack16 caller-alias/remapped-frame and failed-frame exception/output
boundaries in A2; do not introduce an early FALSE or patch guest to force them.
The generic entry, PASCAL/CDECL/nesting and normal code-segment resolver are
no longer missing proofs. A3 and two-worker idle isolation remain unchanged.
No new production or mirror diff, no publication, and no S2 closure.

The adjacent fixed-family source pass follows Winsock registration from
wsext.c WSAStartup to WWS32CallBackHandler, with the custom procedure in
WINSOCK_THREAD_DATA TLS (winsockp.h). Original winsock2/dll/winsock2/dthread.cpp
BlockingCallback obtains the same thread's TLS and invokes its blocking hook
synchronously. The custom WOW path directly calls RET_WINSOCKBLOCKHOOK and
returns its low byte; default Peek/Dispatch uses existing USER gateways.
This is an inherited synchronous context, not a new arbitrary worker-thread
registration. It does not prove network operation or cancellation success.
The source pass also finds the original WSAUnhookBlockingHook pointer-test
expression (missing invocation), identical in the pinned source. It is handed
to the existing Winsock proposal as a concrete operation-level concern,
without changing original host code or claiming it caused a runtime failure.
That handoff does not discharge any A2 common-entry proof.

## Normal task retirement with a real surviving sibling

Source trace retains original wkman.c W32DestroyTask -> W32FreeTask resource
cleanup, ExitVDM(task) when nWOWTasks is nonzero, task-specific pfnWOWCleanup,
WK32DeleteTask and original host thread exit. The admitted adapter's early
SG_EXIT cleanup still runs before guest resource destruction; its later
W32FreeTask fallback does not replace original timer/resource/Hook frees.
The InitTask-failure branch calls W32DestroyTask then host_ExitThread;
that branch remains a separate unproved acceptance case, not inferred from
ordinary exit. No product source or original guest bytes change in this group.

Checked-in verify-wow-mine-gameplay.ps1 -RetiringSibling and its native
wow_mine_gameplay_probe.c now start ordinary run16 --wait CDLPROBE.EXE on the
same unswitched observer desktop after WINMINE's patterned board is ready.
The independently authored existing COMMDLG probe creates two modeless
dialogs, explicitly closes one and exits normally leaving the other. Native
witness waits for its launcher, then performs the established actual
WINMINE flag/counter/reset/open-cell/close assertions. The script separately
requires the COMMDLG report's task to retire exactly once in WINMINE's worker
PID, with both completed original resource-unlock/free pairs before retirement.
Thus two independent workers, a stale visible window, launcher exit alone,
or forced observer cleanup cannot satisfy this case.

frozen-sibling-r1 supervisor25564 starts2026-10-15 at15:38:01 local time;
child7736 has exact UTC start2026-10-15T22:38:01.8827211Z and
180-second deadline in process.json. The group completes naturally exit0.
Sealed input is frozen-generic-r5/package; compile/source and guest identities
are retained under game/. Worker35948 owns WINMINE window032F0B08 and the
retiring COMMDLG task1737. All three COMMDLG records report errors0; final
created2/closed1/hooks2/leaving1. Task1737 retirement completes once, followed
by successful survivor gameplay; WINMINE task17B7 then retires normally.
The probe confirms child run16 PID34576 exit0 before the survivor operations.

This discharges A3's ordinary modeless-resource exit with a usable same-worker
sibling, not its startup-failure/exception cases or S5 forced termination.
There is no new CPU-release trace assertion; the existing pre-CCPU entry
evidence remains distinct. A2 remains unchanged. Full package regression,
publication and P delivery are still pending; P15 remains the installed set.

## Startup boundary and pre-CPU release source review

The source-first pass distinguishes two failures. In wkman.c W32Thread,
exhausted original pfnInitTask retries call W32DestroyTask then
host_ExitThread(EXIT_FAILURE). Normal WK32KillTask also calls W32DestroyTask.
Original W32Exception's KillTask label sets RET_FORCETASKEXIT; unchanged guest
wow16cal.asm WOW16ForceTaskExit uses DOS4CFF rather than a new host scheduler.
The original exception-disposition/UI policy is retained. Actual InitTask
failure and exception-to-task-retirement acceptance remain A3, not inferred
from the ordinary sibling test or the native fixtures below.

A proposed multi-hook startup rollback concern was rejected after checking
all production registration sites: wow_user_session_attach is the sole
session_register_thread_hook consumer. With its single hook, rejection has
no earlier successful hook whose unbind could refuse. The generic API's
multi-hook possibility is not a demonstrated selected WOW defect. No new
fail-fast branch, retry mechanism, session redesign or production edit was
made for that hypothetical; the initial verbal suspicion is superseded here.

Extend checked-in thread_start_session_fixture.c and
verify-thread-start-boundary.ps1 as one terminal-boundary group. Four actual
thread-entry ABIs reject their single startup hook, return ERROR_INVALID_STATE,
and preserve only the parent binding (total1) without entering guest work.
Link the actual ccpu_thread_exit_compat.c against a clearly labelled terminal
witness: the CPU release call observes session already unbound, one terminal
cleanup, and no finishing intent left behind. The witness does not execute
original CPU stack/list freeing; nt_thred.c retains that owner and the build
redirect remains limited to its ccpu386exitthread call. Existing five normal
thread routes, refused-terminal fatal child and rejected/exceptional finish
intent reset cases are retained.

terminal-entry-r2 supervisor37200, child30108 start
2026-10-15T22:41:33.2803908Z, completes naturally exit0. All five compile steps,
link and fixture pass. fixture.out includes THREAD_START_ROLLBACK rejected4,
THREAD_PRE_CCPU unbind_before_cpu1/once1 and the prior terminal assertions.
Source hashes and per-step owned PIDs/deadlines are in checks/. This narrows
A3's host-boundary proof gap, not its remaining real guest failure cases.
No product/mirror/guest change or publication occurred in this group.

## Fixed F07 source disposition: synchronous versus separately entered

This pass reads the selected original call sites and provider map, rather than
inferring callback scope from RET numbers. Paths are relative to src/mvdm/wow32
unless specified. Original external comparison is pinned OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c; no source is imported or changed.
The following is a context disposition, not feature or runtime acceptance.

| Existing family/member | Registration, owner and lifetime | Result and remaining evidence |
| --- | --- | --- |
| Gray string | wuser.c WU32GrayString passes stack-local Gray to native GrayString/W32GrayStringProc synchronously. Guest text is resolved/released during sizing before entry; callback receives a guest numeric parameter, not a retained native guest pointer. Temporary DC alias is freed after return. | Signed SHORT callback result controls native drawing. Inherits initiating thunk; no independent scheduler/registry is needed. Actual gray-string feature acceptance is not claimed. |
| GDI object enumeration | wgdi.c WG32EnumObjects uses local EnumObjData; malloc16 can move memory, so original code releases and re-resolves frame/arguments before native EnumObjects. W32EnumObjFunc writes converted pen/brush data through guest-address helpers before CallBack16. | Signed SHORT return controls enumeration. Free original guest allocation after native return. Context is inherited; resource/conversion correctness is not proved by the font case. |
| Line DDA | wgdi.c WG32LineDDA passes local DDAData to synchronous LineDDA/W32LineDDAFunc; coordinates and guest callback/data are copied before native entry. | Callback has no output pointer. Original caller invalidates frame/arguments after enumeration. Inherited context, not asynchronous registration. |
| Font enumeration | wgfont.c W32EnumFontHandler supplies local FNTDATA; W32EnumFontFunc allocates guest callback data and performs original conversions before CallBack16. Compatibility-name repeat retains original logic. | Signed SHORT return controls continuation. Actual nested font callbacks and data reads already pass the timer probe; do not claim every font/compatibility mode. |
| Metafile enumeration | wgmeta.c WG32EnumMetaFile supplies local METADATA; callback bounds record size, writes through guest-address helpers, calls guest, then resolves/copies changed record and handle table back. | Signed SHORT result and native-record bound preserved. Inherited context; post-callback helpers, not cached guest pointers, own copyback. Full metafile features remain outside S2. |
| Print abort | wgdi.c W32AbortProc resolves CURRENTPTD/TDB abort pointer before guest entry. Original selected SetAbortProc/Escape registrations and original GDI synchronous output callback are the source context, not an arbitrary foreign-thread attach. | Low WORD BOOL; no abort pointer returns TRUE. Current-provider delivery still needs evidence if used to claim runtime closure; no printing implementation admitted. |
| Resource/module/memory/subclass services | wres16.c, wcall16.c, wcuricon.c, wdib.c, walias.c/wsubcls.c call guest services inside the initiating thunk/callback or original cleanup owner. | Preserve each original output conversion and no added early FALSE. Modeless unlock/free and generic resolver have actual evidence; these do not certify every resource API. Cleanup ingress remains the existing early task boundary. |
| EDIT word-break conversion | wmsgem.c EM_GETWORDBREAKPROC's RET_SUBCLASSPROC obtains a guest wrapper during message conversion; wcall32.c RET_SETWORDBREAKPROC is a separate original conversion. | Not evidence that full EDIT callback/storage functionality is connected. Common execution safety remains A2, concrete EDIT behavior S3. No new callback family is admitted. |
| Shell callback | wshell.c WS32ShellExecute passes W32ShellExecuteCallBack to WOWShellExecute. provider/wow32.dll.map resolves WOWShellExecute to SHELL32.dll, not a project implementation. Original shell/shell32/unicode/execnt.c installs lpfnWowShellExecCB around synchronous RealShellExecuteExA; shlexec.c consumes it on execution. | Callback allocates guest command text and returns low WORD from RET_WINEXEC. Historical synchronous source is NOT proof of modern private export behavior; retain modern-provider context/dispatch evidence as A2 and shell feature work with its owner. No replacement shell added. |
| Winsock custom blocking callback | wsext.c WWS32CallBackHandler uses per-thread WINSOCK_THREAD_DATA. Original Winsock BlockingCallback synchronously consumes that thread's registered hook; default path pumps existing messages. | Custom return masks low byte. This is inherited source context, not network feature acceptance. Prior Winsock concern stays with its existing proposal. |
| DDE and foreground idle | wdde.c free-handle callbacks are services inside existing message conversion; wddeml32.c is not selected. Idle is separately delivered by native Hook/original taskman and uses the admitted local registration/task guard. | DDE message association is S4, not inferred from handle cleanup. Real idle delivery passes; cross-worker isolation remains A2. |

Second-role conclusion: synchronous local callback records do not authorize
new task-acquisition wrappers merely because they call CallBack16 directly.
Conversely, original Shell/print/network source cannot stand in for actual
modern-provider evidence. The denominator remains seven families/F01--F12.
Pending distinct mechanisms stay explicit: frame alias/remap/failure,
foreign-worker idle, and unverified provider context where required. Existing
real normal/nested generic execution is retained, not rerun as a substitute.

Frame source review confirms DIV178 snapshots only wTDB/wLocalBP before the
second resolver, then the original algorithm releases callback aliases before
host_simulate and re-resolves the returned frame from VDMSTACK. Testing only
generic return values cannot establish stale-alias rejection or exception
output behavior. Any fixture of that lifetime must exercise the selected
CallBack16 body and explicitly distinguish controlled resolver/CPU witnesses
from real guest execution; the earlier retired frame-lease fixture cannot be
relabelled as such proof. No new frame policy or guest mutation is introduced.

## Selected CallBack16 body: invalidated mapping and failure contract

New checked-in verify-wow-original-callback-frame.ps1 compiles the complete
selected src/mvdm/wow32/wcall16.c using the current provider graph's x86 /MT
/O2 CCPU40 flags plus /Gy. It does not extract, copy or rewrite CallBack16.
wow_original_callback_frame_fixture.c supplies explicitly controlled resolver,
CPU register/simulation and observation boundaries. Full-source link-only
module-cache storage is labelled as unexercised. No production source changes.

The second resolver marks the original caller page PAGE_NOACCESS. Thus reading
the earlier pointer instead of DIV178's copied wTDB/wLocalBP is a real access
violation, not a counter assertion. The simulated callback then marks its
outbound page inaccessible and returns a distinct VP/mapping. The actual body
must resolve that returned frame, preserve caller task/local-BP, read AX:DX,
restore the original vpStack and IP, and never read either stale page.
Three separate cases raise a known exception at the first, second or returned
frame resolution. Each must propagate that exception, leave output deadbeef
untouched and execute the expected zero/one simulation count. This does not
invent a recoverable FALSE return or promise that a partially prepared callback
can resume after an exception; original outer exception policy remains owner.

Preserved fixture-only failures: original-frame-r1 supervisor11364 start
2026-10-15T22:48:13.3316137Z fails to compile the witness because it used CPU
implementation word/c_* spellings rather than the selected softpc.h exported
USHORT/__cdecl register ABI. r2 supervisor35760 starts15:49:13 local and fails
link on the complete source's module cache and legacy CRT printf declaration.
Neither reaches a runtime case; neither is a claimed product regression.
Correct only the witness declarations/link storage/CRT library.

original-frame-r3 supervisor31964 starts15:50:29 local and completes naturally
exit0. Phase0: resolves3, simulate1, output1234abcd, no exception. Phases1/2/3:
resolves1/2/3, simulate0/0/1, outputdeadbeef, exceptione4470002. The map records
the actual CallBack16 owner as wcall16.obj. Compiler commands, source/graph
hashes and child process bounds are retained in checks/.

Second-role evidence boundary: this closes the selected function's controlled
stale-alias/return-remap/exception-output assertions. It is not a real CCPU
stack relocation or guest fault-recovery test. frozen-generic-r5 separately
proves actual generic/nested PASCAL/CDECL guest execution and original
code-segment nonpresent resolution/discard/reload. Neither substitutes for
the other. A2 still retains foreign-worker idle isolation and outstanding
modern-provider context evidence; no physical guest-stack relocation is
claimed. A3 real startup-failure/exception retirement remains open. No
publication, mirror diff or S closure is asserted.

## Modern Shell provider witness: callback return is not accepted

Source contract is the selected original wowshlp.h: seven-argument WINAPI
WOWShellExecute, taking a DWORD WINAPI callback(LPSTR,WORD). wshell.c passes
its original callback through that interface; the linked map identifies the
system SHELL32 export. New checked-in wow_shell_callback_probe.c and
verify-wow-shell-callback.ps1 exercise that modern export in a standalone
x86 /MT /O2 native witness. The only executable target is the witness itself;
its --child path writes a build-owned fallback marker and exits immediately.
No user application, original guest or product routing is changed.

shell-provider-r1 supervisor34696 starts15:53:54 local. Build succeeds, probe
exits c0000005 before buffered output is preserved. Add unbuffered output and
an observation-only SEH recorder; do not convert the exception to a passing
return. r2 supervisor35852 starts15:54:41 local: modern provider calls the
callback once on the initiating thread, then faults after sentinel success42.
The recorder identifies Windows.Storage.dll RVA00193e39. r3 supervisor33564
starts15:55:51 local and retains call-site bytes as well as the same failure.
This is not enough to attribute a product fault:42 is not a real guest
instance, and the witness is not a registered WOW worker.

Separate that success-handle assumption: r4 supervisor26900 starts15:57:07
local, returns ordinary failure2 from the callback first. It still faults at
the same module/RVA after one same-thread callback. No success case passes.
The retained call-site ends FF D6 (indirect call), followed by result use;
these bytes alone do not prove an ABI change. A test-only CdeclControl,
supervisor3836 starts15:58:07 local, also fails at the same location. Therefore
changing the production callback calling convention is NOT a justified fix.
Per-run exact child UTC times, source/executable/system Shell hashes, compile
commands, observed command lines and exception details remain under each
checks/ directory. All these runs are terminal failures, not retries still
running or accepted provider tests.

Current conclusion is deliberately narrow: actual modern same-thread callback
entry is demonstrated; successful return/ownership is NOT. Next disposition
must distinguish missing actual-WOW initialization/context from a private
provider mismatch using the existing real generic-thunk test boundary and
source/call-site evidence. Do not silently call this provider unavailable,
import an entire Shell implementation, add a replacement launcher, or count
the test-only exception catcher as a repair. This stays within A2's already
frozen Shell-provider entry/return obligation. Concrete Shell feature work
remains with the existing Shell proposal; no new subsystem work is admitted.

## Shell callback ABI root cause and bounded adapter design

Static inspection of the exact SysWOW64 Windows.Storage.dll locates the
callback call at RVA005C3426: pushes at005C3412,005C341A and005C341D supply
three four-byte argument slots. The earlier sixteen-byte capture omitted
the first push. The fault at00193E39 reads [edi+130h] after the wrong stack
cleanup has corrupted restored caller state. Merely switching a two-argument
callback to cdecl does not satisfy this three-argument stdcall contract.
Read-only objdump ranges and process identities are in shell-static-r1;
original system images remain unchanged.

shell-provider-three-r1 supervisor24464 starts16:02:21 local. With three
stdcall arguments, the actual system export passes both ordinary failure2
and sentinel success42 unchanged; each invokes once on caller thread34992,
without exception. The third argument is the resolved working-directory
string. This supplies the missing cause, not a reason to change original
wowshlp.h or guest code. Original header still defines two arguments.

Admitted A2 correction: project-owned wow_shell_callback.c wraps only the
private modern export. Native callback consumes all three slots and invokes
the original two-argument callback from a thread-local synchronous scope;
finally restores any previous scope for nesting/exception unwind. wshell.c
alone binds WOWShellExecute to that adapter through its build flags. Original
WS32ShellExecute, W32ShellExecuteCallBack, argument conversion, WinExec and
directory synchronization remain in place; zero additional mirror diff.
No callback registry, scheduler, executable resolver or substitute shell.

Do not impose a new process-global working-directory operation: original
shlexec.c calls its callback with command/show only; wshell.c owns
UpdateDosCurrentDirectory. The OLDUNICODE-only execnt.c implementation has a
different temporary-directory path and is not grounds for importing that
behavior. The new directory ABI slot is accepted but not used to invent new
semantics. Broader explicit-directory Shell feature acceptance remains W12.
Missing modern export reports original Shell DLL-not-found failure plus
ERROR_PROC_NOT_FOUND, not a silent native launcher fallback.

Group acceptance is actual old two-argument callback failure/success through
the adapter, nested native provider calls, provider/worker incremental build,
then existing real generic/timer/resource guest regressions on a fresh sealed
package. Real WS32ShellExecute guest acceptance and all publication gates
remain required; a native ABI witness alone cannot close S2 or W12.

## Real original Shell guest path

Source-first trace selects unchanged shell.def ordinal20 -> WS32ShellExecute
in wshell.c -> W32ShellExecuteCallBack -> RET_WINEXEC (wow.h value55).
The original callback owns command allocation, UpdateDosCurrentDirectory,
CallBack16 and free16. No new launcher or guest execution implementation is
introduced. The independent win16_shell_probe.asm builds a parent and CHILD
variant. Both are disposable test programs, not patched/recompiled original
guest components. The parent invokes original ShellExecute for SHLCHILD.EXE
and a nonexistent NO447SHL.EXE, checks its stack and continues after both.
It yields to let its real child write its completion record. The existing
isolated probe runner records original SHELL.DLL and both test input hashes.

shell-guest-r1 supervisor8656 starts16:13:35 local and fails NASM parsing of
the test-only percent operator without surrounding whitespace. No runtime
case executes. Correct that test syntax, preserving the failed run.
shell-guest-r2 supervisor34984 starts16:14:05 local and completes exit0 on
the sealed frozen-shell-r1-guests package. Three12-byte parent records have
phases0/1/2, errors0, success5974 and missing-target failure2. The child writes
its separate12-byte SHL1/phase3 record. The parent's final record proves
continuation, not only process creation or a launcher return code.

callbacks.txt lines172/173/180 show before/simulate/after ret0037 on native
thread30116, task17BF, with returned AX1756 (=5974). Thus the actual original
guest WinExec callback returns to the initiating task. The nonexistent-file
failure is resolved before callback entry; do not claim a second RET_WINEXEC.
No source-based inference is substituted for this actual trace.

Second-role boundary: A2-S now has real success/failure/child/continuation
evidence, but no adapter-exception-unwind or exhaustive child-resource proof.
The parent/child report is not full terminal-refcount evidence. A3 retains
startup/exception retirement. No production bytes changed in this witness
group and no publication/commit/closure is claimed. The selected host source
review additionally confirms W32Thread exhausts original InitTask retry then
W32DestroyTask/host_ExitThread, whereas wow32.c W32Exception writes the current
frame's RET_FORCETASKEXIT after original response policy. These are the two
distinct pending A3 integration paths; neither is equivalent to merely
rejecting a host thread binding or calling the S5 hung-task API.

## Shell exception negative control remains a failure

shell-closure-r1 supervisor27488 starts16:16:56 local. It builds the actual
adapter through a test TU that includes its unchanged source and exposes a
read-only observation of private current_callback. No production export or
test branch is added. The real modern SHELL32 export remains the provider;
the fixture does not mock its dispatch. Ordinary failure2 and nested success42
pass on thread4076. A third case enters a nested callback which raises the
test-only exception e4470003. The intended assertion is inner TLS restoration
to the outer callback, propagation to the outer catch, then outer TLS clear.

Instead, the outer exception recorder reports c0000005 at Windows.Storage.dll
RVA0015677e, access0/address6170ba08. The adapter probe fails; its supervisor
fails and does not run the following guest stage. The earlier shell-guest-r2
pass remains its own evidence. This is not a new production defect attribution:
the exact first exception/handling/unwind cause is not yet established. Do not
convert the access violation into a passing expected exception, add a fallback
launcher, or relax the original return contract. Retain A2-S within its
existing exception-unwind scope. Additional thrown/caught/TLS observations
have been added to the failing witness for the next grouped verification;
they have not yet run and cannot support a restoration claim.

Source review also rejects a tempting unsupported-provider shortcut for A2-P:
wgprnset.c LoadLibraryAndGetProcAddresses returns TRUE after its loop even if
GetProcAddress failed (the per-export check is only WOW32ASSERTMSGF). Therefore
an absent modern WSOCK32 ordinal alone does not prove a safe unavailable
boundary. WWS32WSAStartup checks only that helper's BOOL before its calls.
No original-host repair is invented here and no network capability is claimed;
provider evidence must account for the actual selected failure behavior.

## Shell exception attribution: direct provider control

shell-exception-control-r1 supervisor10892 starts16:19:54 local. Both bounded
children finish; completed.txt means observations completed, NOT acceptance.
Direct three-argument stdcall control links no project Shell adapter. It passes
ordinary2/42, then raises e4470003 inside a nested modern Shell callback. The
outer callback catches that exact injected exception, returns43, and the
remaining native provider then faults c0000005 at Windows.Storage RVA0015677e.
The first-chance observer records e4470003 followed by c0000005. The failing
address is0000000a; no two-argument ABI mismatch exists in this control.

The actual adapter control has the same sequence/RVA (address e8500cf3),
thrown1/caught1/tls_clear1 on thread31956. Thus the original injected exception
did reach the outer catch, and the adapter's nested/outer finally scopes
restored/cleared their TLS. The subsequent access violation cannot be
attributed to stale project callback TLS or the original two-argument ABI:
the no-adapter modern control independently reproduces it. Both process exits
remain failures; do not relabel the whole sequence as accepted recovery.

Source disposition: original wow32.c W32Dispatch and W32TryCall wrap the
selected thunk in W32Exception; CallBack16 enters host_simulate and reads the
returned guest frame. An injected exception thrown directly from a native
witness bypasses this original inner thunk policy. This observation does not
prove that ordinary guest exceptions escape into the private Shell provider,
nor does it prove that they cannot. The remaining A2-S question is now that
specific reachability/failure disposition, coordinated with A3-X, not another
TLS rewrite or arbitrary Shell feature implementation. No exception swallowing,
deferred rethrow, native-operation replay or semantic waiver is introduced.
Original normal/missing-target guest evidence stays valid and limited.

## Original exception Close and surviving sibling

Source chain: original wow32.c W32Dispatch/W32TryCall -> W32Exception ->
WOWSysErrorBox, original nt_error.c WOWpSysErrorBox/NtRaiseHardError -> existing
wow_hard_error_dialog.c native response -> original current-frame
RET_FORCETASKEXIT -> immutable wow16cal.asm WOW16ForceTaskExit -> WK32KillTask
-> W32DestroyTask/RemoveTaskSharedList/host_ExitThread(EXIT_SUCCESS).
The guest DOS4CFF status is not the host thread's EXIT_SUCCESS. The existing
nt_thred.c build binding calls opennt_ccpu_exit_thread, which finishes session
cleanup before original ccpu386exitthread; terminal-entry-r2 separately proves
this adapter's ordering/refusal with the stated controlled CPU boundary.

Test-only additions reuse the existing generic NE/native-DLL witness. FAULT
records the actual GetCurrentTask identity; TEST_GUEST_EXCEPTION raises
e4470004 inside the real original generic thunk. No original guest media or
product executable is changed. The existing gameplay observer opens only its
unswitched isolated desktop and selects Close only when the dialog belongs to
the exact test worker, has class#32770, message40999 contains e4470004, and
button41001 has the existing Close label. It cannot dismiss unrelated dialogs.

frozen-exception-r1 supervisor23320 starts16:24:05 local. It reaches that real
dialog and task retirement, but the witness wrongly expects launcher255.
Source review of WK32KillTask proves EXIT_SUCCESS; correct only the witness
to require0 and retain the failed run. Preserve report data even on subsequent
observer failure instead of losing it with the isolated runtime cleanup.

frozen-exception-r2 supervisor23440 starts16:24:55 local; owned test16956 starts
23:24:56.4583614Z and completes exit0. Worker4976 hosts WINMINE and faulting
task1737. The faulting guest retains exactly one18-byte phase0 record, with no
post-fault continuation. The scoped error-dialog response is Close; launcher
37164 exits0. TaskRetirement has exactly one complete/status0 for1737, and
the retained stream has no before/simulate/after callback for that task after
retirement. The latter assertion was additionally replayed against the saved
trace and added to the checked-in verifier for subsequent grouped runs.

Surviving WINMINE returns a stable board, flag changes cell+counter, F2 restores
covered cells, left-click opens a cell, and WM_CLOSE completes normally.
Per-operation captures and source/guest/package hashes remain in game/.
This is actual original exception policy and sibling execution, not a
test-owned direct call to the cleanup helper or S5 targeted termination.

Second-role decision: A3-X has bounded local acceptance together with the
existing terminal refusal/pre-CPU source and fixture evidence. This does not
prove real InitTask failure (A3-S), all error responses, or arbitrary native
exceptions escaping across Shell. A2-S still owns that precise reachability
question; it must not grow into a whole hard-error/Shell subsystem rewrite.
No production changes/publication are made by this test group. Remaining
local units are A2-I, A2-S, A2-P and A3-S, followed by coherent delivery gates.

## Two idle workers: own execution observed, original Hook filter incomplete

New independent wow_idle_workers_probe.c runs through ordinary Run16/NTVWM
and its installed native Hook. It calls CreateProcessA twice with the existing
CREATE_SEPARATE_WOW_VDM flag, not a private task/service API. The independent
idle NE has a test-only TICK_LIMIT and PEER output variant: two timer ticks
versus twelve, with original INT2F registration/restoration unchanged. The
checked-in verifier uses one isolated package/service and one unswitched
desktop for both actual workers, preserving package and input hashes.

frozen-idle-workers-r1 supervisor33336 starts16:29:12 local. Native launcher
returns two distinct request PIDs18544/12212, first exits while peer is live,
and both eventually return0. The actual trace separately identifies workers
33552/task17B7 and26120/task17BF, each with original taskman idle dispatch;
the second continues after the first's last dispatch. These are not the
launcher PIDs. Both independent guest reports are preserved. The overall
verifier fails its task-retirement assertion: this separate-worker run lacks
the shared-worker TaskRetirement complete marker. Do not drop that assertion
without tracing the applicable original ExitKernel/process-exit path; ordinary
launcher completion is not a substitute for the claimed cleanup proof.

More importantly, pinned original windows/core/ntuser/kernel/hooks.c around
lines1055--1062 rejects a Hook whose registering thread has TIF_16BIT when the
current thread is non-WOW OR belongs to another process. Current
wow_idle_hook_binding.c checks only current local binding and local original
hIdleHook registration. Original WK32WowSetIdleHook still installs a global
WH_FOREGROUNDIDLE Hook with thread0. In another initialized WOW worker, the
local tests can all succeed: they do not encode the registering Hook's
process. Therefore own-worker callback success cannot prove the original
cross-process filtering contract, and additional count-only tests cannot
repair that source-level identity gap.

A2-I now has this concrete registration/filter seam to resolve. Preserve the
original taskman selection and INT2F callback. Do not invent a global callback
registry or assume replacing thread0 with only the registering thread covers
all original same-worker threads/native-modal entry. Audit registration,
delivery and removal together against the existing TD/session owners before
changing that seam. No production fix, new mirror diff, guest change or
publication is made by this witness. The four-unit frozen boundary remains.

## Source follow-up: process exit versus task rollback

The next source pass resolves two ambiguous acceptance targets without
claiming a new runtime pass. Original guest kernel31/i21task.asm around
709--742 tests fExitOnLastApp and num_tasks==2, then calls ExitKernelThunk
before the later SG_EXIT branch. Original wkman.c WK32ExitKernel calls
ExitVDM(WOWVDM, ALL_TASKS) and ExitProcess, not W32DestroyTask. Consequently
the separate-worker idle witness cannot require the shared-worker per-task
TaskRetirement complete marker on that path. Its replacement acceptance must
observe the actual worker process/registration exit and surviving peer, not
simply remove cleanup coverage. The verifier remains failing until that
replacement is implemented and accepted.

Original wkman.c W32Thread's pfnInitTask retry/failure branch is also distinct
from the application's KERNEL16 InitTask export. The former retries seven
times and then calls W32DestroyTask and host_ExitThread(EXIT_FAILURE); forcing
the latter's LocalInit failure would not exercise the frozen A3-S boundary.
Selected original queue.c xxxInitTask can reject application-name allocation
or copying, and can reject TDB allocation after it has allocated pstrAppName.

This audit finds an actual project carrier leak: wow_user_task_lifecycle.c
frees the carrier on rollback and retirement without releasing pstrAppName.
Original taskman.c DestroyTask frees the TDB; original queue.c
DeleteThreadInfo (pinned upstream lines1710--1712) separately frees the name.
The original kernel/thread finalizer is not the owner of our heap-backed
carrier and is not called here. Its ETHREAD, object-reference and queue-lock
closure is not composable wholesale. The existing project final-release
boundary now performs that same name release on both paths through its
existing cleanup_release allocator pairing. The failed-init return also
preserves the initiating GetLastError across rollback APIs and runtime leave,
rather than returning a cleanup side effect as the initialization error.
No new registry, guest change,
mirror edit or task-finalization algorithm is introduced.

Second-role review: this is a concrete A3-S rollback/normal-retirement fix,
not evidence that the original W32Thread failure path has passed. Include
name-allocation/TDB-allocation failure and normal retirement assertions in the
next coherent lifecycle group. Do not rerun a full package for this isolated
edit or identify the earlier sealed shell package as containing it. Candidate
publication remains pending; the four frozen units remain open.

### InitTask allocation/return verification group

The reproducible entry is verify-wow-task-lifecycle.ps1 -Case task-lifecycle
-InitRollback, using the existing provider and worker build roots. Two test
translation units include the complete selected queue.c and current lifecycle
source. Only their allocation/free boundary is observed or failed; no product
fault switch or guest modification is added. The verifier explicitly excludes
the corresponding ordinary objects from its archive. The retained link map
must select xxxInitTask from init-allocation.obj and lifecycle_init_style from
init-release.obj, not accidentally exercise an older duplicate implementation.

frozen-init-group-r1 supervisor8108 starts16:41:20 local and runs rollback,
ordinary task-lifecycle, then process-retirement serially with five-minute
child deadlines. Its inputs.json records the six affected source/script hashes.
Rollback reports errors0, failed_allocations2 and name_releases2: name allocation
failure leaves no allocation, TDB allocation failure releases its prior name,
and later successful task retirement releases its own name. Cleanup deliberately
sets ERROR_BUSY; both initiating failure codes must still survive. Ordinary
task-lifecycle also reports errors0/native_direct4. These are bounded native
fixtures with the existing controlled page-domain boundary, not real original
W32Thread's seven-retry/terminal failure acceptance. The process-retirement
case subsequently reports errors0 for startup rollback, atomic cleanup,
task/object/class/refusal/retry/event/native assertions. The whole group's
exit.txt is0. No runtime input changed while its children were running.
This accepts the bounded carrier release/error fix, not the remaining A3-S
real startup-failure boundary, A2-I native Hook isolation, or final publication.

## Separate-worker process retirement accepted; Hook isolation still open

frozen-idle-workers-r2 supervisor30104 starts16:47:13 local and finishes0.
The test-only native launcher now reads this run's idle trace while workers
are alive, opens read/synchronize process handles, checks their exact staged
ntvdm.exe path and creation time, and retains those handles through exit.
The changed verifier does not require shared-task SG_EXIT on original last-app
ExitKernel. It instead requires both real worker handles to signal with0,
both launch requests to complete, a live peer at first exit, peer guest idle
after the recorded trace byte offset, and no later idle from the dead worker.
All assertions precede supervisor cleanup; no forced exit counts as success.

Request PIDs5272/36940 are distinct from workers36932/task17B7 and
35996/task17BF. Worker36932 exits0 first while35996 remains live; the offset
is9866 bytes. Subsequent peer idle is observed; worker35996 then exits0.
Guest reports retain phases0/1/2, errors0, restored vectors and ticks2/12.
The hash-recorded sealed shell-r1-guests package is reused unchanged: this
test does not claim to contain the later name-release fix. It accepts the
applicable original process-exit boundary, not a broker-record census or
the unresolved native global-Hook registering-process filter. A2-I stays open.

## Selected Winsock provider is not absent

Read-only PE/source inspection rejects declaring A2-P Winsock unavailable
merely because its historical private entry might be missing. On this host,
SysWOW64/wsock32.dll SHA256
C8C95463A95AF66A1D098BA991BE26A86A860974C4CC25FAC68EAF50A31AEA29
exports ordinal109 -> ws2_32.WSASetBlockingHook,115 -> ws2_32.WSAStartup,
1000 -> ws2_32.WSApSetPostRoutine and1107 -> MSWSOCK.WSARecvEx. The selected
ws2_32.dll hash is
C454780CE7710AFED2C0E048BD4B061AA54EAF82A5C711E98C73DB584EBE2688.
Reproduce with objdump -p on the two SysWOW64 images, then -d at image base
10000000 plus RVAs31850--318BD,25B70--25B95 and31FE0--32023.

The actual modern blocking-hook setter retains the version<2 check and stores
the callback in its returned thread-state field+14; custom registration sets
the blocking dispatcher field+10 to RVA31FE0. That dispatcher calls the stored
callback inline and loops on its return, not by creating another thread.
The post-routine setter also has a real store, not an unavailable stub.
This matches the ownership direction of pinned winsock2/dll/winsock2/bhook.cpp
and original wsext.c's per-thread WWS32CallBackHandler. Original WOW requests
version1.1 and calls ordinal1000 after installing its hook. This is source/PE
evidence narrowing the next provider-context acceptance, not proof of actual
guest networking or a passing A2-P callback invocation. Retain the original
blocking-hook body; do not add an asynchronous callback dispatcher or pretend
the subsystem is unavailable to close S2.

## Selected Winsock blocking context: native acceptance

verify-wow-winsock-provider.ps1 builds an x86 /MT witness using the same
WSOCK32 ordinal109 registration and WSAStartup1.1 selected by original wsext.c.
Only an in-process IPv4 loopback pair is used. Two delayed peer sends exercise
blocking receive; a third receive is cancelled inside the registered Hook.
Every callback checks caller thread identity, its TLS marker and WSAIsBlocking;
every returned receive checks the blocking state is cleared. Null registration
must reject with WSAEFAULT without replacing the Hook. Cancellation must return
WSAEINTR, and restoring the prior Hook must return the installed function.

frozen-winsock-provider-r1 supervisor35472 starts16:51:16 local and fails only
SDK deprecation warnings under /WX. The test deliberately exercises Winsock1.1;
its source now defines the SDK's _WINSOCK_DEPRECATED_NO_WARNINGS macro, without
disabling other warnings or changing the API. r2 supervisor15612 starts
16:51:50 local, compiles and completes0: calls32, foreign0, outside_blocking0,
cancel0. Input/provider hashes, process deadline and outputs are retained.

This accepts the selected native provider's synchronous inherited-thread
mechanism, including cancellation and return, and agrees with the source/PE
trace above. It does not execute original WWS32CallBackHandler in a real guest,
prove all networking, or accept printing's separate abort callback. Keep those
coverage limits explicit in A2-P; no product callback rewrite is made.

## A3-S reached cleanup mismatch identified

Source follow-up reaches a concrete integration error after the allocation
fix. W32Thread exhausts its original seven InitTask attempts and calls
W32DestroyTask. W32DestroyTask calls W32FreeTask, then pfnWOWCleanup unless
TDF_TASKCLEANUPDONE, removes the original TD and exits the thread. After a
pre-commit InitTask failure the project lifecycle has already rolled back its
USER carrier, so registered_cleanup -> lifecycle_exit rejects !current_task
and registered_cleanup turns that into a fatal registered-task-cleanup error.
The legal incomplete-startup cleanup must not be confused with normal cleanup
on the wrong thread/task. An unconditional success for missing binding/task
would conceal the latter and is rejected.

Use original TD identity/initialization state where possible: W32Thread keeps
hThread NULL until after successful InitTask/notification; its htask16 and
current TD already identify the failed original task. Preserve notification
failure with a committed carrier as the existing full-cleanup case. Any fix
must cover both that case and never-committed startup, plus wrong identity,
retry and no post-terminal callbacks. No new failure bypass or extra task
registry has been implemented. Original WK32Yield already waits on both the
creator event and the new thread handle; a missing success event alone is not
proof of an original wait deadlock. The finite A3-S owner remains unchanged.

### Exact original-TD precommit cleanup binding

The candidate now handles never-committed startup at registered_cleanup's
existing boundary. wow_user_task_cleanup_uninitialized lives beside the
existing W32FreeTask adapter and uses original precomp.h/TD, not a copied TD
layout or a new task-state registry. It requires a bound runtime with no USER
thread/class carrier, a nonzero16-bit task ID, and the current original TD
still in gptdTaskHead under gcsWOW. Its htask16 and native dwThreadID must match,
and hThread must remain NULL (the original pre-success publication state).
It preserves last-error. A stale/foreign TEB value is compared with list-node
addresses before dereferencing; an unlinked, committed or wrong-task TD fails.

Only task cleanup with no selectors/count can use that result, after the
registration owner validates the runtime. Module unload, a live USER carrier,
and notification failure after successful InitTask retain lifecycle_exit and
its failure policy. No guest, mirror definition or runtime ABI field changes.
Normal cleanup and resource release remain in their original owners.

The grouped witnesses extend common-dialog's existing entry/early-cleanup
tests with exact-TD positives and foreign/committed/retired negatives. The
actual registration fixture now rejects seven style lookups before admission,
then calls its real pfnWOWCleanup with a controlled original TD. Its existing
seven failed notifications after admission now also call that cleanup before
normal unbind, checking that the committed carrier is not silently bypassed.
These native fixtures still do not execute the original W32Thread/guest path.
frozen-startup-cleanup-r1 supervisor14892 starts16:57:37 local; grouped identity,
registration and process-retirement verification is pending at this update.

The r1 build rejected duplicate original wowuserp declarations from combining
precomp.h and the full lifecycle header. Keep the original header unchanged;
the adapter now uses a narrow wow_task_cleanup_entry.h declaration boundary.
r2 then exposed the identity-only fixture's missing explicit controlled TEB
provider. Add that fixture-owned TEB, rather than linking a whole worker or
silently calling an unrelated native TEB. Both failed runs remain evidence.

r3 supervisor15220 starts17:00:31 local. The identity/resource group reports
errors0 for exact-TD acceptance, foreign/committed/retired rejection, three
fatal resource-failure edges and original early-free refusal. The actual
registration fixture reports WOW_INIT_PRECOMMIT_CLEANUP errors0/retries7 and
WOW_INIT_NOTIFICATION errors0 for registration/retry/exhaustion/exception/
retirement. The final process-retirement case also completes errors0 and the
group exit.txt is0; all three stages completed naturally without source changes
during execution. Input hashes are recorded in the group's inputs.json.
These results exercise the real registered cleanup and exact identity helper,
but the TD and initialization attempts remain controlled native witnesses;
do not relabel them as a real guest W32Thread startup-failure run.

### Coherent startup candidate and separate allocation diagnostic

frozen-startup-build-r1 supervisor33976 started2026-10-16T00:09:32Z;
its child26124 had a30-minute deadline and completed naturally with exit0.
The normal incremental worker build performs3 steps and the provider15 steps.
Worker SHA256 is630D9C3AB0EE48098D2AE2AA5571195E47C0FA0891403055170697B0A7D8FBE1;
WOW32 is23F9E2CA985DCC01B9B9C2ECCB941EAD765174F533094DE638E0DF2BF797C091.
This builds the current grouped changes; it is not application acceptance,
package publication or a new P delivery.

The diagnostic-only tests/observation/wow_startup_failure_provider.c includes
the complete selected queue.c body, replacing only its allocation boundary
in a separately linked object. For the independent GENPROBE module's exact
counted name, its second InitTask allocation fails; original retries, rollback
and W32DestroyTask remain selected. TLS fault state restores on unwind and
does not affect other module names. No guest is altered. The production
build graph never selects this diagnostic source.

build-wow-startup-failure-provider.ps1 substitutes that one object from the
actual production commands, directs DLL/map/import-library output to a fresh
build directory, records object/source hashes and verifies that the normal
provider was not overwritten. frozen-startup-diagnostic-r1 supervisor236
started00:11:35Z with child34336 and a5-minute deadline; both compile/link
complete0. The separate diagnostic DLL hash is
FEB0B94BA5D794C39FFB218DFF8CD271578A76F28D5445821ACAE013F10C96CA.
Its manifest explicitly states diagnostic-only and publishable=false.
It has not yet been exercised with the real guest and surviving task;
A3-S stays open. No release assets or installed system32 files were replaced.

### A3-S real startup failure and native-thread terminal evidence

frozen-startup-guest-r1 naturally completes0: worker33996/task1737 executes
seven original InitTask failures with error8, calls registered WOWCleanup
successfully and never enters GENPROBE's checkpoint. Same-worker WINMINE
flag/reset/open/close succeeds. This first witness did not observe the failed
task's native thread handle and was explicitly not treated as complete proof.

The test-only provider now records the exact PID/TID/task before each failed
attempt. No production tracing or original body is changed. The native
gameplay observer acquires SYNCHRONIZE/QUERY_LIMITED_INFORMATION access to that
thread while it is alive, validates its owning PID, then retains the handle
through termination. It requires exit1, independently of the run16 request's
exit0. Original W32Thread calls W32DestroyTask and host_ExitThread(EXIT_FAILURE)
on this path; W32DestroyTask calls cleanup then WK32DeleteTask. Existing
terminal-entry-r2 covers the selected pre-CPU/session refusal ordering;
frozen-init-group-r1 and frozen-startup-cleanup-r3 cover allocation ownership,
exact-TD rejection, registration rollback and committed-notification cleanup.

frozen-startup-proof-r1 supervisor30140 runs the diagnostic rebuild then the
guest test serially. frozen-startup-diagnostic-r2 completes0 and records DLL
F064E67E8DE87AC9B84CD3F68233EBE9D37D0700DD1F9BE1F25F83DCD5FD83F3;
normal provider23F9E2CA985DCC01B9B9C2ECCB941EAD765174F533094DE638E0DF2BF797C091
is unchanged. frozen-startup-guest-r2 child32472 starts00:16:15Z with a180s
deadline and completes0. Its actual worker22364/thread36300/task5943(1737)
is identical across all seven attempts. The held thread handle becomes
signalled with exit1; launcher18544 completes0. WINMINE's flag changes both
cell and counter, F2 restores the cell, left click opens it, and normal close
and launcher completion pass. No guest checkpoint file exists.

Second-role disposition: this discharges the specified A3-S startup-failure
unit, not every allocation site, guest-memory reclamation mechanism or S5
termination. Do not require TaskRetirement complete for a USER carrier that
never committed; that would confuse this path with ordinary task retirement.
All assertions must be retained for final candidate delivery. Three A2 units
remain; production publication is still P15 and guest remains unchanged.

### A2-S source-correct outer exception boundary contrast

Pinned wow32.c W32Dispatch encloses the complete selected thunk call in
try/except(W32Exception). W32Exception normally consumes a thunk exception
with EXECUTE_HANDLER (Close selects RET_FORCETASKEXIT); debugger continuation
and alignment restart retain their explicit distinct dispositions. This is
not the earlier native witness's catch-inside-an-outer-Shell-callback followed
by returning43 into the still-active outer provider. The latter failure is
retained, not silently turned into a product adapter defect or waived away.

verify-wow-shell-callback.ps1 -OuterBoundary now independently tests the
outside-the-entire-API boundary, both with -ThirdArgumentControl and with
-AdapterProviderBuildRoot. An injected e4470003 must propagate to that outer
handler, then the same thread invokes the provider again and gets failure2
normally. The actual adapter additionally asserts restored TLS and retains
ordinary nested success/failure coverage. The previous nested-exception case
remains available without -OuterBoundary and its failure is not weakened.

frozen-shell-outer-r1 supervisor25060 fails the required recovery marker:
the new test injection was mistakenly located in its exception-report filter,
so no callback exception occurred. Correct only that test location and retain
the failed run. frozen-shell-outer-r2 supervisor36852 completes0; direct
caller29136 and adapter caller37520 both catch exactly e4470003 at the outer
boundary and successfully invoke the provider afterward. Adapter TLS is NULL
after unwind; ordinary nesting still passes. No production source changed.

This narrows A2-S to the actual original guest/provider exception path and
debugger/terminal disposition relevant to it, rather than demanding a fix for
the artificial catch-and-resume sequence. Native boundary evidence alone does
not close A2-S. No exception swallowing, provider replay, changed guest policy
or new Shell implementation is admitted.

### A2-S actual guest exception witness: retirement fails

The diagnostic-only wow_shell_exception_provider.c includes the production
adapter unchanged, calls the real original W32ShellExecuteCallBack, then
raises e4470004 after its return. Thus actual guest SHLPROBE goes through
WS32ShellExecute, modern WOWShellExecute and original RET_WINEXEC before the
fault. Its independent SHLCHILD executes normally. The real W32Exception
dialog is answered Close by the existing PID-scoped observer. This is not a
guest modification or a production exception policy change.

build-wow-startup-failure-provider.ps1 -ShellException selects only the test
adapter object in a separate DLL and verifies the normal provider is unchanged.
frozen-shell-exception-r1 supervisor37468 builds diagnostic
EB8B3EC74FED0ACA5ABC25BA2A67623F4FBC1D8B60D157E0344873AF58811EAC.
Its actual worker37488/thread36204/task5943(1737) returns original Shell
instance5838, then records restored adapter TLS. The observer selects Close
on the matching error dialog. However launcher23308 is still waiting at the
10-second assertion boundary: wait258/exit259. The test fails17; its cleanup
is not normal retirement. No success or A2-S closure is claimed.

The lifetime trace proves child16F7 retires, RET0037 returns for1737, and no
retirement marker for1737 follows before test cleanup. Source narrows the next
check: original wcall16.c restores ptd->vpStack from the callback frame but
defers actual SS:SP restoration to W32Dispatch. The latter's SETVDMSTACK is
inside its successful try body, before the W32Exception except clause; Close
writes RET_FORCETASKEXIT using ptd->vpStack. An exception after a completed
callback may therefore leave CPU and logical frame positions different.
This is a source-supported hypothesis, not yet a proven root attribution:
record both frame positions at that reached boundary before changing runtime.
Pinned OpenNT has the same placement. The inspected NTVDMx64 wow32 patch
does not supply a matching correction; no new original-host fix is authorized
by this observation. Do not work around it with retries, swallowing exceptions,
guest edits or a forced worker kill presented as task cleanup.

For A2-P, a read-only Get-Printer census confirms Microsoft Print to PDF,
OneNote (Desktop) and Adobe PDF are installed. Printing cannot be classified
as provider-unavailable from an assumed missing printer. Any selected native
print witness must explicitly target its test-owned output file, not a
physical queue or a file-selection dialog. No print job has been submitted.

### A2-S logical/CPU stack divergence confirmed

frozen-shell-exception-r2 supervisor37784 adds only read-only stack-position
logging to the diagnostic TU. Normal provider remains unchanged; diagnostic
SHA256 is52A396318D4F2561CA38632D7004F730F8D7A5079C842932E8A1856C48207C75.
The actual worker34648/thread11088/task1737 records:

- before original callback: logical170F53BC, CPU170F53BC;
- after original callback: logical170F53BC, CPU170F5396;
- after adapter unwind: logical170F53BC, CPU170F5396.

The difference is26hex bytes. Original wcall16.c intentionally restores only
ptd->vpStack and defers actual SS:SP restoration to W32Dispatch's API return.
Original W32Dispatch performs SETVDMSTACK inside the successful try body;
exception unwinding bypasses it. W32Exception's Close branch instead writes
RET_FORCETASKEXIT through logical ptd->vpStack. Thus the measured mismatch is
retained after exception disposition, unlike successful thunk return.
The observer selects Close and launcher36616 still times out(wait258/exit259);
no failed-task retirement is claimed. Both guest and production code unchanged.

The selected W32Exception body and pinned OpenNT body have identical nonblank
lines after CRLF normalization; W32Dispatch's restore placement and original
CallBack16's deferred-pop contract also match. This points to the inherited
non-fast dispatcher exception edge, not failed Shell callback TLS restoration.
No matching correction was found in the inspected sibling SoftPC sources or
NTVDMx64 wow32 patches. Under the standing original-host policy, do not invent
a correction or relocate it to an adapter to bypass that policy. A2-S remains
a closure blocker, not transferred to a successor or accepted as fixed. A
minimal proposed correction would restore the current task's stack on the
handled-exception return as well, with early-frame/continue-search cases
explicitly protected; it requires matching-source correction or renewed owner
admission before implementation. Other bounded A2 work can continue.

### A2-P selected print provider callback context and cancellation

Pinned wgdi.c registers W32AbortProc through Escape(SETABORTPROC). Pinned
ntgdi/client/output.c maps that escape to SetAbortProc, stores the callback in
the DC's LDC and calls it inline from vSAPCallback. FALSE cancels the DC and
aborts its document. EndPage rejects a cancelled document; EndDoc explicitly
returns1 if DOC_STARTED is already clear. These are separate return contracts.

The independently authored x86 wow_print_provider_probe.c uses exactly that
Escape registration on Microsoft Print to PDF, with explicit normal/cancel
filenames below its fresh build evidence directory. It records thread/TLS/DC
identity at every callback and rejects calls outside the initiating operation.
There is no default-printer lookup, physical printing or file-selection dialog.

frozen-print-provider-r1 returns1: all10 callbacks have correct thread/TLS/DC,
but FALSE during StartDoc does not cancel that selected provider's output.
The produced cancel.pdf and failure are retained; this observation is not
silently declared compatible. r2 targets drawing-phase cancellation: EndPage
returns-1, EndDoc1 and no cancel.pdf exists. Its assertion incorrectly expected
EndDoc failure. Source review above identifies the original no-active-document
success contract, so the assertion is corrected, not the provider behavior.

frozen-print-provider-r3 supervisor28232 starts00:36:17Z and completes0.
Thread10428 receives8 normal and2 cancellation-case callbacks; foreign0,
outside0, cancellations1. Normal output has PDF signature and SHA256
5F8D92CEAC24CABCC08EC57BB9CD8E843DD5DF55EB68100E9A7BAA48096B424B;
the cancelled document produces no PDF. Read-only queue-after.json records
zero remaining jobs at00:37:02Z. Builds/probes run hidden with deadlines.

This establishes the selected native printing callback's inherited-owner
context and drawing-phase cancellation/return, complementing the native
Winsock blocking-hook witness. It is not a real W32AbortProc/guest invocation,
universal print compatibility, or acceptance of StartDoc FALSE behavior.
A2-P therefore remains open for original-caller/guest binding evidence and
explicit disposition of relevant boundary differences. No product, guest,
printer configuration or published asset changed. Shell stack correction
still awaits the additional original-host admission requested from the owner.

### A2-P actual Win16 print witness reaches callback, not acceptance

Added independently authored win16_print_probe.asm and the existing
verify-win16-hook-probe.ps1 -Case print. Original GDI ordinal381 registers
W32AbortProc; original ordinal377 marshals DOCINFO16 strings for StartDoc.
The test explicitly selects Microsoft Print to PDF and build-owned output
filenames, never the default printer. It asserts callback HDC/task identity,
normal/cancel return, two DC deletions and ProcInstance release. It changes
no original guest or production image and adds no replacement callback owner.

frozen-print-guest-r1 supervisor13828 fails independent NASM assembly because
the modulo operator lacked whitespace; no guest ran. r2 supervisor29720
assembles and reaches original RET_SETABORTPROC (0022), but times out.
r3 supervisor34060 removes the test's GetTickCount loop dependency, uses100
draws and keeps cancellation enabled through EndPage; it also fails. Both
failures remain evidence, not passes or justification for a product timer fix.

In r3 worker35668/task17BF/thread27612, original StartDoc/StartPage return;
the original abort callback enters and returns on task17BF. Only the initial
20-byte PRN1 record is persisted. The last dispatched thunk is RVA00021A80,
identified by the exact normal provider map as WU32SysErrorBox, not EndPage
(RVA0000C910). Observer timeout is30000ms; forced test cleanup is not normal
retirement. Thus the first timing hypothesis is not established: a reached
guest error path must be attributed before any production correction.

Next bounded source review is the independently authored probe's ABI and
original GDI/callback return path, including the error details. Do not count
the trace hit as successful print completion, add a print feature sweep, or
attribute the fault to unchanged guest code without evidence. A2-P remains
open. The tested package reuses the normal candidate worker/provider, not a
diagnostic DLL; exact inputs are in the run's package-hashes.json and
print/inputs.json. Published P15 and original guest remain unchanged.

### A2-P print ABI attribution and accepted bounded guest result

frozen-print-observer-r1 supervisor28952 builds the existing read-only observer
with private-desktop error-label reporting. frozen-print-guest-r4 supervisor7048
then captures PRNPROBE's General Protection Fault at KRNL386.EXE0001:8F87.
The selected binary's instruction bytes match original linterf.asm LocalHandle
at that offset (mov bx,[bx-2]); this is not a fault in W32AbortProc. Both the
new print probe and earlier Shell probe incorrectly imported KERNEL ordinal11
as Yield. Original kernel.def assigns11 to LOCALHANDLE and29 to YIELD. Fix
both independently authored probes; a sweep finds only these two Yield sites.
No production or original guest correction is warranted by this failure.

frozen-print-guest-r5 supervisor35548 now exits normally and frees both DCs
and its ProcInstance, but correctly fails cancellation:100 quick draws never
reach another250ms native abort check. Shell regression passes with ordinal29.
r6 uses a one-second drawing interval plus a65535-iteration bound. The guest
clock is not bypassed or patched; earlier timer attribution is rejected.

frozen-print-guest-r6 supervisor37624 starts2026-10-16T00:56:28Z and completes0
for both cases on the same normal candidate package. Print task17BF in
worker29400 reports errors0, normalCalls5, cancelCalls2, cancelled1, deleted2,
freed1. Original RET0022 entry/return count and process/task identity agree.
Native thread ownership has separate provider-r3 evidence; the callback trace
prefix itself is a process ID and must not be mislabeled a thread ID. Normal
PDF SHA256 is AB698056F9D4D95CC8C53D99E80486336FECC7B60529DFA2B6E9DC0907B367CB;
cancel.pdf is absent. Shell reports errors0, success5974, missing-target2,
child execution and continuation, preserving its separate exception limitation.

Queue audit finds three abandoned jobs8/9/10 from the earlier faulty probes;
their original StartDoc returns in r2/r3/r4 prove exact ownership. They are
selected only after matching the test document name. Remove-PrintJob for8 is
denied by Windows even through the approved elevated tool boundary; no removal
is claimed and no printer/service/security settings are changed. Jobs8/9/10
remain a known failed-test cleanup limitation. Test cleanup cannot
retroactively pass the failed runs. r6's
normal/cancel result remains bounded print callback acceptance, not universal
printing or StartDoc-FALSE compatibility. A2-P now retains only Winsock binding
disposition; A2-I and A2-S remain unchanged. No guest/mirror/product edits or
publication occurred in this group.

### A2-P Winsock immutable input and admission boundary

Read-only inspection after print acceptance finds no WINSOCK/WSOCK member
in assets/winnt.zip or the sealed frozen-print-guest-r6/package/system32.
This is a packaging/input fact, not proof that the native provider or original
callback is unavailable. The normal provider map selects original wsext.obj
WWS32WSAStartup and WWS32CallBackHandler (RVAs0002BFD0/0002BB20).
Original wsext.c loads WSOCK32 via wgprnset.c's existing loader, allocates
per-thread buffers, installs ordinal109 with WWS32CallBackHandler and ordinal1000
with WWS32DispatchPostMessage. The callback chooses the original default
message pump or RET_WINSOCKBLOCKHOOK and masks the guest result to8 bits.
Existing native-provider evidence checks109/1000 and actual blocking callbacks;
it still cannot substitute for this original guest path.

The carried src/mvdm/wow16/winsock/WINSOCK.DLL is2880 bytes, SHA256
629E86C59C83F66D3623388B9ED6AE9D5A664979E588A9EB45B8A54441126FD9.
The indexed archive's m0-t274-s1-non-guest-mvdm-union-ledger.tsv records
byte-identical OpenNT and OpenNT-4.5 provenance. The m0-t297 guest move
manifest retains that hash; m0-t301 supplement marks it already-tracked,
not a new supplement import. However m0-t348-s1-guest-artifact-matrix.tsv
explicitly classifies its runtime as deferred-wow16. No current package
selection or newly approved runtime admission was found. Do not interpret
mirror completeness as runtime permission, rebuild the guest, invent a
replacement DLL, or close A2-P merely because the package omits this input.

Minimal proposed admission is an exact-hash original DLL copy into the
isolated test package only, never assets/winnt.zip or published system32.
An independently authored NE client would exercise startup, custom blocking
callback normal/cancel returns, same-task identity and original cleanup with
a test-owned loopback peer. Retain the existing default callback separately;
do not repair the inherited unhook expression or add asynchronous networking
under this admission. Input and callback acceptance remain distinct from the
queued W13 complete network functionality. This admission is pending; no
runtime copy or guest execution of the deferred module occurred here.

### A2-I complete original idle-dispatch census

Pinned OpenNT windows/core/ntuser/kernel/hooks.c filters a WOW registration
before invoking it: GETPTI(phkCall)->TIF_16BIT requires a current WOW thread
and equal process owners. WH_FOREGROUNDIDLE has no HKF_INTRSND exception.
The predicate depends on the selected registration, not merely on whether
the current process happens to have its own registered Hook.

The original dispatch census is four callsites, not just taskman:

- taskman.c353--361: no runnable WOW tasks, temporarily select the current
  task, dispatch once, clear input-idle and rescan. The selected host taskman
  retains this through wow_user_task_lifecycle idle_hooked/idle_hook.
- queue.c3013--3015: foreground message/object wait before descheduling WOW;
  the hook call itself has no TIF_16BIT exclusion.
- queue.c3223--3226: ordinary input sleep's foreground dispatch is explicitly
  guarded by !TIF_16BIT; WOW uses taskman instead.
- queue.c3812--3814: xxxGetInputEvent foreground hook before input-event wait;
  there is no TIF_16BIT exclusion here either.

This rejects the tempting all-native-callback suppression plus taskman-only
solution: it cannot claim the other original wait paths are redundant.
It also rejects replacing the original global registration with one local
thread Hook: wkman.c explicitly requires global timing, and one thread does
not represent every task in a worker. A per-task registry/replication scheme
would need separate ownership/order/lifetime design, not a one-line fix.

The existing wrapper safely excludes unbound/non-WOW contexts and protects
initialization/retirement, but if two initialized workers share the same DLL,
its local registration query still cannot identify whose native global Hook
caused this callback. The public callback arguments supply no registration
handle or registrar PID. Original globals in each DLL instance are not a
cross-process registration identity. The existing two-worker survival run
does not assert this missing identity or native chain ordering.

No speculative runtime change is made. A2-I remains exactly the registrar
isolation/ordering gap; no new scheduler, registry, guest change or silent
loss of original wait-triggered idle delivery is admitted by this census.

### Owner-approved guest archive completion

Owner subsequently requests packaging all required guest files in the ZIP.
This supersedes the preceding pending WINSOCK test-only admission. Preserve
original inputs, not historical runtime-deferred packaging omissions; no guest
source compilation or binary modification is authorized or performed.

tools/audit/audit-guest-archive.py reads NE module names/imports from the35
existing members and original candidates. Original baseline imports already
resolve; dynamic support and ordinary application fixtures were missing.
tools/build/update-guest-archive.py selects15 original support modules and12
exact existing application/help fixtures, hash-checks against the prior audit,
backs up the ZIP and refuses any existing-member replacement or PE addition.
It validates CRC and the complete resulting static NE module closure before
updating assets/winnt.zip and assets/winnt-guest-manifest.json.

guest-archive-audit-r1/r2 supervisors26568/33512 record the inventories.
guest-archive-package-r1 supervisor36632 succeeds:62 files,27 added, zero
unresolved static NE module imports. Archive SHA256 is
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.
All35 pre-existing payloads are byte-identical; recoverable prior ZIP is
guest-archive-package-r1/package/winnt.before.zip. No installed system32 files
or thirteen-image host release are updated by this packaging operation.

New system32 members: WINSOCK, MMSYSTEM, TIMER, TOOLHELP, OLESVR(retail),
MCIOLE16, RASAPI16, CTL3DV2, LANMAN, LZEXPAND, NETAPI, PMSPL, VER, WINHELP,
WIN87EM. Root fixtures: WINMINE/SOL/WRITE/TASKMAN/WINVER, EDIT.COM/QBASIC.EXE,
and the five corresponding available application/editor help files. Exact
paths, formats, sizes, source selection and hashes are in the asset manifest.
Do not substitute source-tree WRITE for the established O:/winnt fixture;
their different hashes are explicit. Debug-only DDEML, debugger tools, DOS
extender stubs and unrelated local applications are not blindly bundled.

This establishes the bounded ordinary DOS/WOW package input closure, not
support for arbitrary third-party app-private DLLs, every dynamic feature or
full networking/multimedia acceptance. Legacy SYSTEM.INI WOA font names and
native mmdrv.dll are not NE imports; this change neither supplies a separate
DOS-hosted Windows desktop nor packages modern native Windows dependencies.
WINSOCK real callback acceptance can now use the unchanged approved module;
the outstanding A2 units and coherent product delivery gates remain.

### Winsock guest acceptance and observer last-error repair group

frozen-winsock-guest-r1 supervisor25080 completed with exit1, not a timeout.
The unchanged approved WINSOCK module reaches original RET_WINSOCKBLOCKHOOK:
task17BF receives35 callbacks, including one successful cancellation. The
independent guest then observes203 instead of WSAEINTR10004. Cleanup assertions
are not reached; runner teardown is not normal-retirement evidence.

Source attribution: original wsext.c WWS32WSAGetLastError reads the native
Winsock error directly. The selected CCPU40 MS_bop_1 calls project-owned
mvdm_softpc_report_wow_dispatch_return_state after WOWDispatchEntry and
mvdm_softpc_report_wow_bop_state/stage before its next invocation. These
observers query optional environment variables without restoring last error;
the disabled/missing-variable branch itself writes ERROR_ENVVAR_NOT_FOUND203.
The actual dispatch trace records call06B6 returning000000CB on thread7400.
This proves an observer transparency defect on the required route, not yet
that it is the only cause of this guest failure. Original Winsock stays intact.

Bounded F04/A2-P repair: preserve caller last error on every normal exit of
the existing softpc termination/reporting unit's diagnostic functions,
including disabled tracing, missing selector, output-open failure and successful
output. Already-protected observers retain their implementation. Do not wrap
the actual termination operation or change its error semantics. No new mirror
diff, guest change, Winsock policy, scheduler or service is required. Review
the complete unit together, test native observer error transparency and rerun
the actual normal/cancel/restore/free Winsock guest contract after the grouped
repair; WSAEINTR and normal cleanup remain mandatory assertions.

Implementation repairs35 previously unprotected diagnostic/init entries in
mvdm_softpc_termination.c with local saved error and one normal-return cleanup
label; the eight already-protected observers and actual session termination
remain unchanged. No new wrapper framework or mirror call-site edits.
verify-softpc-observer-error.ps1 links the real production unit with test-only
abort stubs for uncalled CPU/session dependencies. r1/r2 failed fixture build
setup (missing Windows forced include, then unused termination link symbols),
not runtime acceptance. frozen-observer-error-r3 supervisor36492 passes52
checks: all43 entries disabled plus three WOW observers with overlong path,
failed output open and actual successful output; caller error10004 survives.

frozen-observer-worker-r1 supervisor9128 rebuilds only the changed C unit,
its library and final x86 CCPU40 worker; exit0 and VdmTib owner check passes.
Worker SHA256373CF39CEA1264A38DDE72621412DE09072CA1F7205E2573DE7B470F9F2A2411.
Provider remains23F9E2CA985DCC01B9B9C2ECCB941EAD765174F533094DE638E0DF2BF797C091.
frozen-winsock-guest-r2 supervisor33092 exits0. Same original guest/provider
and independent probe now report task17BF,35 callbacks, one cancellation,
error10004, restored=closed=freed=cleaned=1 and errors0. Original callback
entry/return pairing and actual guest assertions pass; launcher exits normally.
This controlled worker-only repair establishes the observer defect as the
cause of the previously observed203. A2-P's fixed callback disposition now
passes locally; this is not all-Winsock feature acceptance or product delivery.
A2-I registrar isolation, A2-S inherited exception restoration admission and
the coherent regression/publication/commit gates remain open.

### A2-I debug-hook alternative and ordering review

The pinned original hooks.c1240--1250 invokes WH_DEBUG immediately before a
selected Hook, but explicitly sets DEBUGHOOKINFO.idThreadInstaller=0. Current
Microsoft DEBUGHOOKINFO documentation describes the installer of the debugging
filter, not a guaranteed identity for the selected destination registration:
https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-debughookinfo
The DebugProc contract provides hook kind/arguments and veto/chain behavior,
not an HHOOK for the selected destination. Therefore a proposed WH_DEBUG/TLS
registrar carrier has no demonstrated supported contract and is rejected;
no additional debug Hook is installed as a speculative product fix.

Original _PhkFirst/_PhkNext at hooks.c1730--1785 also proves that local hooks
precede the global chain. Replacing the WOW global hook with per-thread hooks
would change ordering relative to existing native global hooks. Even though
WK32ForegroundIdleHook ignores its arguments and ends the chain with0, a
foreign worker's duplicate callback cannot simply stand in for a local
registration: an interposed native global hook can otherwise be skipped.
The current process-wide registration boolean is not proof of equivalence.

This eliminates two concrete alternatives without adding a private Hook
registry, guest change or silent ordering waiver. A2-I remains the same finite
registration-identity/ordering blocker, not a new feature requirement. The
repaired observer worker is separately undergoing the existing generic,
timer/font, Hook, common-dialog and ordinary Shell regression cohort; those
results cannot discharge A2-I or the independent A2-S exception obligation.

frozen-observer-regression-r1 supervisor32564 (start2026-10-16T01:27Z)
completes with exit0 using the sealed frozen-winsock-guest-r2 package:
generic, timer, hook, common-dialog and shell each have result.exit0 and
their asserted result.txt. The verifier checks actual Pascal/Cdecl/nested and
failed/nonpresent resolver cases; ordinary/modal timers with nested font
buffers and cancellation; CBT query/nesting/veto/recovery/unhook; modeless
normal/exit cleanup before task retirement; and Shell child execution,
missing-target failure and parent continuation. No new product compilation
was needed for this cohort. This retains those five bounded capabilities on
worker373CF39C; it does not assert Shell exception safety, foreign idle-Hook
identity, complete application/DOS regression, or publication.

### A2-S dispatcher ownership cross-check and bounded admission request

Rechecking wow32.c978--1100, wcall16.c's deferred-pop tail and the original
i386/fastwow.asm return path confirms that the non-fast C dispatcher owns
actual CPU-stack restoration. It is not a missing responsibility of the
modern Shell ABI adapter. W32TryCall catches through the same W32Exception;
the fast assembly path reloads CURRENTPTD after that return and derives its
return stack from TD.vpStack. This is comparative original evidence only:
importing FASTWOW/kernel-V86 machinery is not an admitted CCPU40 solution,
nor is it a matching existing SoftPC correction under source policy.

Candidate minimum correction, pending explicit original-host admission:
in W32Dispatch, initialize a local stack-established flag to FALSE, set it
only after the existing ptd->vpStack assignment, and restore that task's
current vpStack in the existing handled-exception body only when established.
Keep successful restoration, W32Exception and CallBack16 unchanged. Expected
mirror budget: one declaration and three bounded executable statements in
wow32.c plus divergence registration; no new helper/framework, guest diff,
exception filter replacement, fabricated API return or Shell-specific branch.
Review the exact compiled statement count before selecting the final diff.

Required assertions are finite: ordinary and nested successful returns retain
the current frame; exception after completed RET_WINEXEC restores matching
logical/CPU stacks; original Close retires the task and preserves a sibling;
ordinary Ignore returns through the original selected frame; a failure before
stack establishment never dereferences an uninitialized TD; CONTINUE_SEARCH
and CONTINUE_EXECUTION do not run the new handler restoration. A blanket
finally is explicitly rejected because it would also modify escaping-exception
state. Real Shell exception evidence and original-dispatch boundary fixtures
are complementary, not substitutes. No correction has been implemented under
the automatic continuation; the standing original-host restriction still
requires renewed owner approval or an existing matching SoftPC fix.

### Owner admission and DIV-337 implementation

Owner now explicitly approves the proposed W32Dispatch correction and directs
registration-first autonomous handling of similar future original-host fixes.
The source policy records the superseding authorization; guest immutability,
minimum diff, exception provenance and positive/negative/lifecycle gates remain.
The inherited defect is now active approved S2 work, removed from unplanned
TODO rather than retained there as another permission blocker.

DIV-337 in the mirror register and local source comments records one BOOL,
one establishment assignment after original TD.vpStack publication and a
guarded SETVDMSTACK in the existing handled-exception body. No moved original
algorithm, duplicate callback implementation or unconditional finally is added.
The proof obligations in the preceding design remain; implementation alone
does not mark A2-S accepted, publish a candidate or close S2.

DIV-337 initial verification: hidden supervisor 14428, evidence roots
`build/M0-T447/S2/div337-provider-r1` and `frozen-shell-exception-r3`,
completed with exit 0. The formerly failing real original Shell callback
exception/Close now retires the selected task exactly once, restores callback
TLS, and permits surviving WINMINE gameplay and normal exit. The injected
diagnostic DLL remains test-only. Normal provider SHA-256 is
`E472B8919714B96340E577F23AEE42291362B0F27EC8DB9F08DAD35B0D041001`.
Hidden supervisor 36700 in `div337-regression-r1` copied this normal provider
into an isolated package and passed all five generic, timer, hook,
common-dialog and Shell guest groups (each exit 0, aggregate exit 0).
Governance `frozen-scope-docs-r45` and diff check passed before this evidence
append. Ignore continuation and early/disposition negative boundaries still
need their separate proof; Close success is not full A2-S acceptance. A2-I
and coherent delivery gates remain open. No guest change or new publication.

### A2-S completed local exception acceptance

This supersedes the preceding pending exception-boundary result, not A2-I or
the production delivery gates. Hidden `div337-boundaries-r1` supervisor37444
ran the independently authored Shell exception guest with the same original
provider closure and a one-shot diagnostic fault. `frozen-shell-exception-r4`
(Close) and `frozen-shell-exception-r5` (Ignore) both exit0. Entry:
`tests/observation/verify-wow-mine-gameplay.ps1 -ShellExceptionSibling
-ShellExceptionResponse Close|Ignore` with the hash-checked diagnostic manifest.
Close retains the first checkpoint only. Ignore records phases0/1/2, no guest
stack/operation errors, a completed child and an ordinary missing-file failure
on the next Shell call. Both prove adapter TLS unwind, original RET_WINEXEC,
exactly-once task retirement and surviving same-worker WINMINE gameplay/exit.
Ignore deliberately does not invent a successful result for the interrupted
API; original W32Exception does not define such a result.

The complementary entry `verify-wow-dispatch-exception.ps1` compiles complete
selected wow32.c, not an extracted/retyped dispatcher or exception filter.
`div337-fixture-r6`, supervisor33780, exit0 proves:

- normal dispatch: one stack restoration and original AX/DX return;
- exception in getSS before TD-stack establishment: zero restoration, no
  access through the unestablished local TD;
- exception after a simulated completed callback: one restoration;
- original STATUS_INVALID_HANDLE disposition: resume at the raising call
  without early restoration, then perform the ordinary successful restoration;
- an actual debugger-owned fixture child: original W32Exception sees its debug
  port, continues search to an outer handler, and performs zero restoration.

This fixture controls CPU/TD/error-box boundaries; it is not real guest proof.
The real Shell tests above supply that proof. Its closed link-only aliases
resolve unrelated DLL-init exports; the map check rejects retaining those
owners or their trap/data targets. No fake init service runs. Attempts r1--r4
failed linkage (initial duplicate resolver definitions, then unrelated
whole-TU dependencies); r5 passed four cases, r6 adds actual debugger search.
These fixture failures are not product regressions. Runtime source remains
the same DIV-337 candidate E472B891, with the prior five-group regression.
No guest bytes changed, no diagnostic artifact published. A2-S is now locally
accepted; A2-I registrar isolation/ordering is the sole remaining A2 unit.

### A2-I source-only convergence and exact decision boundary

The next goal continuation re-read current source before selecting another
implementation. Prior turn classification: progress, because A2-S implementation
and actual exception/negative evidence changed the accepted local state.
No live build/test was restarted and no speculative runtime patch was made.

Pinned original `windows/core/ntuser/kernel/hooks.c` xxxCallHook2 filters the
selected PHOOK's registering THREADINFO/process before callback delivery.
The non-inter-send WOW predicate is current TIF_16BIT plus equal `ppi`.
Current `wow_idle_hook_binding.c` knows current binding and any local TD's
registration only; neither identifies the selected native global registration.
Changing this Boolean to another local Boolean cannot supply that identity.

Additional reuse route checked: original wowhooks.c's32 static stubs and
W32GetHookStateData. Its opening contract explicitly says its data is NOT
shared between processes and relies on USER's prior process/thread selection.
HOOKSTATEDATA contains local index/guest task/HHOOK, not a cross-process owner.
Reusing that table as a foreign-registration discriminator would inherit the
same missing guarantee, and adding idle as another table entry would consume
an original finite slot without resolving it. Reject that proposed reuse;
do not introduce a second registry disguised as reuse of this alias table.

Related wuhook.c WU32SetWindowsHookInternal explicitly documents thread0 as
process-wide WOW (except journal hooks), while WH_MSGFILTER is task-targeted;
WU32SetWindowsHookEx likewise passes the selected THREADID32 to native USER.
This preserves the earlier S4 hook-chain ownership warning: existing local
CBT evidence is not whole global-Hook isolation acceptance. Do not expand
A2-I into all Hook features or declare those features passed by an idle fix.

The supported SetWindowsHookEx contract permits foreground-idle thread or
global hooks and states thread hooks precede global hooks. Thus replication
onto own WOW threads is a candidate isolation mechanism, but not proof of
original mixed native-global ordering. Previously rejected taskman-only,
WH_DEBUG identity, and TLS chain-dedup alternatives remain rejected; nested
native pumping can create a distinct event inside a still-active chain, so
TLS depth alone is not an event identity either.

No design selected here changes guest, original scheduler, native global Hook
order or callback cardinality. The owner is asked exactly one new boundary:
allow modern thread-scoped idle ordering while preserving all worker task
notifications/normal-wait/nested/lifetime obligations, or retain strict mixed
ordering and admit an explicit registration-identity mechanism. Standing
DIV-registration approval does not silently waive that ordering contract or
the frozen no-new-registry constraint. Any selected implementation still
needs a complete registration/create/rollback/dispatch/unregister/retirement
group and actual two-worker/interposed-native/nested failure acceptance.
This is a bounded architecture/semantics decision, not another request to
approve DIV-337 or permission to start ordinary authorized work. Pending the
answer, no changed idle runtime or new publication is claimed.

### A2-I owner-selected thread observers

Owner supersedes the pending choice: implement thread-scoped observers,
guarantee intra-worker coverage, and relinquish original cross-process Hook
priority. This does not waive native chain forwarding, task ownership,
registration lifetime, nested callbacks or original scheduling.

Source trace: i21task.asm Set_Vector installs the idle request when INT2F is
changed; wow16cal.asm returns through INT2F:1689. Original taskman can deliver
on a different thread from the registrar after finding no runnable task.
Thus registrar-thread-only replacement is rejected. Native experiment r4
under build/M0-T447/S2/idle-native-scope-r4 used real GetMessage waits on an
activated private desktop, restoring the input desktop afterward. Counts:
registrar-only other thread0; both threads installed11/14; other process
foreground0/0; removed thread0; assertion failures0. Earlier r1 had no
foreground and proves nothing; r3 wrongly included pre-activation transition
in the foreign-process sample. r4 records that transition separately. This
is native scope evidence, not WOW integration acceptance.

Implementation design: each existing lifecycle task carrier owns one native
WH_FOREGROUNDIDLE observer from initialization through rollback/retirement.
Original TD.hIdleHook remains the registration intent, borrowing that task's
real observer handle instead of owning another global registration. Original
task cleanup clears its intent; lifecycle alone unhooks the observer. No new
registry, shared identity table, generated DLL, scheduler, guest or protocol.
The taskman entry uses the existing scoped guest dispatch directly; the native
observer uses that same dispatch then forwards CallNextHookEx. Invalid or
unregistered native entries forward without guest access. Do not suppress
genuine nested events with a thread-local deduplication flag.

DIV-338 budget: declaration include plus replacement of the original global
registration call by a current-carrier observer lookup, and removal of the
original unhook ownership (retain NULL reset). All behavior bodies are owned
adapter code. Init failure, no requests, requests on a different task, new
tasks, nested native entry, teardown and peer survival require verification;
full package gates remain required before publication or S2 closure.

### Thread-observer first integrated acceptance

Hidden provider build idle-thread-provider-r1 PID14320 finished0; subsequent
idle-thread-group-r1 PID30736 rebuilt the lookup's already-held-lock handling.
The original callback scope fixture passes calls4, nested/exception, denied2,
foreign2, retired1. Production lifecycle fixture passes initialization
rollback (two allocation failures, two name releases) and verifies observer
lookup before initialization, after failed publication, while already holding
the runtime lock, after releasing it and after retirement. Real guest idle,
timer and CBT Hook cases pass. Guest images remain unchanged.

The group's common-dialog case fails: only the initial18-byte record exists,
despite target task retirement and launcher exit0. This is not acceptance.
The supervisor exited from the nested driver before writing its own final
marker; its guest-group exit1 is authoritative. No case was rerun while live.

Controlled follow-up idle-thread-controls-r1 PID28608 uses the prior sealed
div337-regression-r1 package and the newer frozen-print-observer-r1 observer.
Old baseline and current worker/provider both pass full54-byte common-dialog
phases. Comparing package contents found only ntvdm.exe and WOW32.DLL changed;
the initial failing driver used the older T445/S5 observer. This correlation
does not establish the failure cause; retain the initial failed witness.
The same current candidate passes the independent-worker idle test: workers
19684/task17BF and35944/task17B7 complete normally, with surviving peer idle
after actual first-worker exit. All three controlled cases exit0. Completion
marker is done.txt, not a product-release marker.

Still required: explicit registration-on-one-task/idle-on-another guest proof,
native-chain forwarding witness, observer-install failure/rollback coverage,
native-versus-taskman notification cardinality/ownership review, disposition
of the initial modeless-dialog failure, and the full delivery gates. No
publication, commit, push, A2-I closure or S2 closure is claimed by these runs.

### Native idle-chain forwarding acceptance

idle-native-chain-r1 failed to link because the fixture inherited WOW native
API redirection macros. No runtime result is claimed. The fixture now undefines
only the five redirected native window/pump APIs used by its owned desktop;
the production idle callback object remains unchanged and linked directly.
idle-native-chain-r2 supervisor PID22236 completes with fixture exit0:
WOW_IDLE_HOOK_GUARD errors=0 calls=4; WOW_IDLE_NATIVE_CHAIN errors=0 next=13;
WOW_CREATION_BOUNDARY_OK case=idle. Real USER dispatch reaches the next Hook
with and without WOW registration intent; the taskman direct entry does not
forward into that chain. The owned desktop restores the previous input desktop.
The scheduler/guest callback is mocked in this bounded chain witness, so this
does not replace the pending two-task same-worker guest acceptance. Other
remaining gates listed above are unchanged; nothing has been published.

### Thread-observer rollback and shared-task acceptance boundary

idle-acceptance-group-r1 supervisor20484 finishes. Its lifecycle fixture
links the actual lifecycle implementation with test-only native install/free
observers: CBT failure and FOREGROUNDIDLE failure both preserve ACCESS_DENIED,
withdraw task/context/list state, and leave no Hook. Existing original
InitTask allocation failures and ordinary retirement also release the real
native observers. Markers: WOW_IDLE_OBSERVER_LIFECYCLE errors=0
install_failures=2 live=0; WOW_INIT_ROLLBACK errors=0 failed_allocations=2
name_releases=2; WOW_USER_TASK_LIFECYCLE errors=0 native_direct=4.
No production test seam or guest-media edit was added.

The new SharedTasks mode uses ordinary Hook/CreateProcess flags0, a registrar
NE probe and an unregistered peer. The independently authored registrar
records GetCurrentTask inside its INT2F vector, to require actual execution
on the peer rather than merely two registrations. It has not passed:
r1/shared and r2/shared reach one worker with two distinct tasks but fail the
native host's bounded completion wait. r1/separate has a harness argument
splat error, corrected by explicitly constructing arrays; r2/separate then
passes the existing independent-worker retirement/surviving-peer assertions.
Neither initial failure is erased or counted as passing.

idle-shared-r3 (supervisor32564) adds guest progress records and retains the
existing dispatch/callback trace. Worker18988, tasks17BF/thread17440 and
177F/thread37196 both enter and return original RET_FOREGROUNDIDLE (0031).
WU32GetMessage returns1 and progress records contain WM_TIMER (0113).
The reports remain partial, with no completed idle/tick/restore sequence;
the native host returns1. Thus an entirely unwoken/deadlocked callback is
not established, nor is the required end-to-end shared-task proof achieved.
Source review confirms original GetCurrentTask only reads curTDB/headTDB,
and INT2F is not in Set_Vector's per-task intercepted-vector list. Next
bounded work is the callback-return/task-switch/probe-state boundary, not
an increased timeout or a semantic waiver. Keep A2-I blocking S2 delivery.
All these supervisors are terminal; no replacement run is left active.

Documentation/diff gate frozen-scope-docs-r50 passes after compressing
historical CURRENT wording below its control-plane size limit; this precedes
the shared-task fixture additions above and is not their delivery gate.

### Shared-task harness contract correction

Source review corrects the preceding failure interpretation: Run16 main.c
launch_vdm explicitly uses wait_wow_startup for shared BINARY_TYPE_WIN16
without --wait, whereas SEPWOW takes the completion route. The shared probe
waited Hook-created launcher process handles and required A to remain alive;
those handles were startup receipts, not task-exit proof. Its generic result1
did not distinguish timeout from successful startup completion. Therefore
r1--r7 do not prove a product deadlock, state-loss defect or callback failure.
The supervisor then cleaned up tasks that were still legitimately running.

The original guest loader's LMAlreadyLoaded also checks module names; peer
now has its own IDLPEER resident name. r4 still fails the invalid wait contract,
so that correction is not claimed as the failure's cause. A temporary bounded
read-only memory witness failed to produce accepted data (r5 compile failure,
r6/r7 no accepted snapshot); it is removed entirely, including its map input.

The shared harness now explicitly launches staged Run16 --wait for each NE
target, observes A's real initial guest checkpoint before launching B, and
logs each wait result and exit code. Its host is launched directly by the
private-desktop observer, not through another native Hook layer. Shared task
identity/callback/restoration checks are not weakened. r8's intermediate
hooked-host variant returned13 and is not acceptance. r9 was refused before
launch because a live non-test O:\WINNT broker existed. The final direct-host
variant remains unverified. The published binaries have not changed.

### Shared-task guest acceptance after wait-contract repair

idle-shared-r10 supervisor32716 finishes with exit0. Exact scoped closure
of the remaining O:\WINNT ntsrv4220 used the owner's standing authorization;
its executable/start identity is retained in idle-owned-project-close.json.
No other application was terminated. The isolated run uses worker1192:
registrar task17BF/thread20476 reports16 ticks,19 idle callbacks,4 callbacks
executed on peer task177F; unregistered peer177F/thread37872 reports4 ticks
and zero invocations of its own unused vector. Registrar restores its vector;
peer explicitly never installed one. Both explicit --wait launchers exit0,
with the registrar still running after peer completion.

Both task identities have exactly one retirement-complete marker. No task
has an idle-dispatch marker after its retirement, and registrar idle continues
after peer retirement. These additional assertions are replayed against the
sealed r10 trace and added to the shared-task verifier. The prior reports of
incomplete guest counters cannot establish a runtime state-loss bug: that
harness ended on startup receipts and cleaned up live tasks. No production
code change was needed to repair the acceptance contract.

A2-I's native-chain, install-failure/rollback, separate-worker and shared-task
acceptance edges now have local evidence. Remaining S2 delivery work includes
the original modeless-dialog failed-witness disposition, complete package
regressions, mirror/governance audit and coherent all13 publication/commit/push.
No new P or S2 closure is claimed here; published P15 remains unchanged.

### Delivery review: common-dialog witness interpretation

The initial failed common-dialog witness remains open. Comparing the old
S5 observer and frozen-print-observer-r1 shows identical .text bytes; an
observer-code regression is not established. Both saved launch command lines
explicitly request Run16 --wait, unlike the corrected shared-idle harness.

Source review corrects a second overinterpretation: absence of an IsWindow
W32Dispatch trace does not establish that the guest never executed IsWindow.
Pinned OpenNT base/mvdm/wow16/user/usercli.asm lists ISWINDOW among locally
thunked USER16CLIENTTHUNK entries. The failed report contains only phase0;
the probe has unreported fatal branches both after DestroyWindow and after
IsWindow. WU32DestroyWindow returns1 in the host trace, but that trace precedes
W32Dispatch's frame-result writeback. Consequently the retained evidence
cannot distinguish a return-boundary failure from stale guest-visible window
identity. Neither hypothesis is a proved root cause. No runtime workaround is
added on that basis.

idle-delivery-group-r1 supervisor6644 disappeared before producing script or
test output; an elevated process check confirms it absent. This is a launch
failure, not a failed or passing runtime test. Replacement r2 supervisor17068
stages original assets/winnt.zip plus hash-verified thirteen release images,
then replaces only the two identity-pinned candidate worker/provider files.
It runs generic/timer/Hook/idle/common-dialog/Shell serially, with per-case
180-second deadlines and recorded process identities. Results remain pending;
passing this group cannot erase the initial unexplained witness.

r2 subsequently completes with supervisor exit0 and all six case exits0.
The package input manifest and per-case inputs are retained in its build root.
After its common-dialog case completed, the independently authored probe adds
distinct failure records: errors0101 for guest-observed DestroyWindow failure,
errors0102 for IsWindow still true. Successful report semantics are unchanged.
This diagnostic probe revision is not covered by r2 and awaits its next grouped
run. It changes no original guest media or production image. The original
failed witness remains a finite S2 delivery blocker, not a passing result.

Source continuation: original windows/core/ntuser/client/wow.c IsWindow
uses ValidateHwndNoRip and, for WOW, rejects HANDLEF_DESTROY in the shared
handle entry. Current wow_user_private_access.h redirects DestroyWindow to
the existing WOW_NATIVE_CALL wrapper in wow_user_task_lifecycle.c. Both its
same-task and resumed-task return paths call publish_call_windows, which
refreshes the graph; project_graph drops native-dead observations, and owned
WM_NCDESTROY goes through wow_window_words_terminal. Therefore simply adding
another destroy refresh is not justified by this audit. The old trace alone
does not prove which guest failure branch fired.

The COMMDLG verifier now decodes complete partial records before checking
the unchanged mandatory three-record success shape, preserving the new
0101/0102 failure diagnosis instead of only reporting a length mismatch.
The r52 documentation/diff gate passed before this parser change.
idle-foundation-group-r1 supervisor13588 starts all twelve existing foundation
groups serially with 600-second per-case limits. Its first graph-snapshot
child29704 is live at the check, and dependency-selected Ninja steps report
no work. This is pending verification, not twelve passes or a new delivery.

Compiled-input audit confirms current wuwind.obj references
_wow_native_DestroyWindow@4, not merely a header declaration. Current linked
WOW32 and ntvdm hashes still equal the failed witness's recorded candidate
hashes (7EA20D15... and BC0E768D...). Thus an obvious stale worker/provider
mix or unselected DestroyWindow redirect is not supported by the inputs.
The graph-snapshot foundation case completes with exit0; the remaining
eleven cases continue under the same supervisor, without restarting it.

window-borrow also completes with exit0. idle-apps-group-r1 supervisor18712
starts the same sealed r2 package's SOL/WINMINE Off/Win16/All gameplay cases
serially, followed by acceptance of the amended COMMDLG diagnostic probe.
Each child has a recorded 600-second bound. This package group is separate
from the native-only foundation fixtures (which use existing broker seams,
not the live BaseSrv endpoint). WRITE's independent frontier and both DOS17
profiles are still required and are not implied by this gameplay group.

The mirror review presently spans four C files plus the divergence ledger:
wcommdlg.c, wkman.c, wow32.c and wutmr.c. In wkman.c, idle registration borrows
the lifecycle observer and original free-task withdraws intent without freeing
that borrowed Hook; callback-body and free-task symbol renames are registered
scope/cleanup entrances. No guest source or guest binary was changed by this
review. Complete diff/governance and publication review remain pending.

idle-apps-group-r1 now has six exit0 records: SOL and WINMINE each in Off,
Win16 and All modes. The amended common-dialog case is still running at this
checkpoint. Foundation window-reclaim also returns0 (three completed cases
total; no claim for the other nine). idle-frontiers-dos-r1 supervisor22972
waits for that exact application supervisor identity and its exit0 marker,
then serializes WRITE Off/Win16/All frontiers and Console/Window DOS17 against
the same sealed candidate package. Missing/failed predecessor evidence stops
the chain rather than restarting it. WRITE observations require separate
frontier review: a successful observation script is not functional acceptance.

The application supervisor subsequently finishes exit0. The amended common-
dialog probe records phases0/1/2, errors0, created2/closed1/hooks2/leaving1;
thus the new diagnostic source preserves the successful lifecycle contract.
It does not reproduce or explain the older short-report failure. The queued
WRITE/DOS supervisor has advanced to WRITE-Off after observing this terminal
success, with no overlapping real-package application group.

### WRITE/DOS input-contract correction

idle-frontiers-dos-r1 is terminal1: WRITE observation scripts exit0 but all
three modes show an illegal-instruction dialog (05D8:2219/221F, bytes
63 68 61 69 6E) before the previously accepted MSWRITE_MENU/DOC/PAGEINFO
frontier. DOS Console stops on its empty case with observer timeout53504354;
Window was not run. These are failed observations, not retained frontiers.
WRITE.EXE and SYSTEM.INI hashes match the previous payload-apps-r2 package.

Review finds the driver omitted the explicit environment allowlist used by
that baseline. message-boundary-audit's callback-package/callback-controlled
comparison and window-graph-audit's earlier P10 comparison already record
this inherited-environment illegal-instruction failure; the responsible
variable remains unidentified. Therefore the initial conversational claim
of a proved candidate version regression was premature. The new failure is
real but not a controlled binary comparison. It is neither a guest exemption
nor permission for product-side environment filtering.

The driver gains an explicit ControlledEnvironment switch restoring the
existing baseline allowlist in its own process only; it records allowed names,
not sensitive environment values. r1 supervisor22972 is confirmed absent
after its terminal1 record before r2 is started with this switch. r2 reuses
the unchanged sealed package. Its outcomes remain pending. The ordinary
inherited-environment limitation and initial COMMDLG witness remain unproved
and are not erased by any subsequent controlled result.

### Retained WRITE frontier assertions

Controlled r2 completes its three WRITE observations with visible
MSWRITE_MENU, MSWRITE_DOC and MSWRITE_PAGEINFO. Added reproducible checker
tests/observation/verify-wow-write-frontier.ps1 requires those visible classes
in all four samples with a stable live worker identity; it rejects an
illegal-instruction dialog and explicitly disclaims editing/saving/normal
exit. It does not convert the observer timeout into application completion.
idle-write-frontier-check-r1 supervisor2812 exits0: all three retained
payload-apps-r2 baseline snapshots and all three current r2 snapshots pass;
the inherited-environment r1 snapshot is rejected for missing MSWRITE_MENU.
This uses existing evidence, without another guest run. It establishes the
previous startup-window frontier under comparable controlled conditions,
not resolution of the inherited-environment failure.

Foundation group now has nine exit0 cases: graph-snapshot, window-borrow,
window-reclaim, process-retirement, dialog-lifecycle, menu-resource,
thunk-scope, menu-cleanup and native-send. Examined markers include actual
callback-cache failure refusal, retained backing/nested destruction,
retirement retry, four dialog-creation rollback cases and thunk exception
restoration. Native-nested/task-lifecycle/native-creation and DOS17 remain
running/pending; complete group success and publication are not yet claimed.

Controlled idle-frontiers-dos-r2 supervisor19588 is terminal0. Both saved
Console-17-summary.json and Window-17-summary.json contain exactly17 rows,
with zero Expected/Actual mismatches (34 total); the individual established
runner assertions still check console text/interaction. This does not change
the failed r1 environment result. Foundation native-nested is now exit0,
bringing the current completed count to10/12; its supervisor13588 remains
live for task-lifecycle/native-creation.

Full current mirror C diff review confirms wcommdlg.c's three callbacks and
wutmr.c's timer change only entry symbol names to retained original bodies.
wkman.c additionally transfers Hook lifetime to the task lifecycle owner;
wow32.c adds established-stack tracking and restoration only in its existing
handled-exception branch. These are registered DIV333--338, not guest changes
or replacement dialog/timer/scheduler algorithms. This review does not waive
the initial common-dialog failure disposition or final publication gate.

idle-foundation-group-r1 subsequently completes exit0 with all twelve case
exits0; supervisor13588 is confirmed absent after its terminal record.
Native-nested markers cover timeout with held0/1, exception with held0/1,
destroy-during-return116, dead-target1400 and receiver retirement with held0/1.
Task-lifecycle records errors0/native_direct4 and standard-class admission
projected9/calls10/retry1/errors0. This completes the current foundation group,
not the entire T. No supervisor from these runtime groups is left running.

The remaining local S2 evidence blocker is the retained initial common-dialog
short report: native success was logged before guest frame-result writeback,
and the original probe did not distinguish its later fatal branches. Its
successor now does. Current source/input audit and passing grouped evidence
exclude neither a transient return/identity defect nor an observation-input
effect; do not guess which. Final governance/diff/publication/commit/push gates
remain separate. S3/S4/S5 retain their previously defined obligations.

### Initial short-report source/trace narrowing

r54 documentation/diff gate exits0. The old common-dialog dispatch trace has
721 lines, below the2048 admission cap, and continues through task retirement.
The selected linked map identifies RVA19430 as WK32FileWrite, RVA25A00 as
WU32DestroyWindow and RVA28A30 as WK32SetCurrentDirectory. The initial report
write returns18 through WK32FileWrite; no further FileWrite is reached after
DestroyWindow returns1. Thus the short file is not evidence that the second
checkpoint attempted a write and lost it. The old probe's two immediate
post-destroy predicates remain the relevant unresolved branches.

First resource unlock/free callbacks in failed and successful traces use
caller stack179753D2 and callback stack179753AC, with matching before/after
stack transitions. Original wcall16.c restores ptd->vpStack from CBVDMFRAME,
then W32Dispatch re-resolves that frame and writes AX/DX before SETVDMSTACK.
Those observations do not expose the final guest AX or local IsWindow result;
they do not prove stack corruption. USER16 usercli.asm sign-extends HWND into
the local IsWindow call, while original client/wow.c validates the shared
entry and HANDLEF_DESTROY. No additional refresh, callback-cache patch or
guest change is justified from the retained trace alone.

### Guest-observed return boundary

The independent COMMDLG probe now emits CDL2/24-byte records including dialog
HWND16, DestroyWindow AX and IsWindow AX. It stores the actual register before
each predicate without another API call or changing flags. The verifier retains
the original creation/hook/resource/retirement assertions and additionally
requires nonzero destroyAX and zero isWindowAX after the first destruction.
Earlier CDL1 evidence is unchanged, not reparsed as the new format.

commdlg-return-evidence-r1 supervisor5264 completes; probe/result.exit=0 and
the process is absent. Task17BF phase1 records dialog0918, destroyAX1,
isWindowAX0, errors0. Phase2 retains created2/closed1/hooks2/leaving1; the
second HWND16 reuses0918, so no invalid assumption of permanently unique short
window numbers is added. The selected worker/provider are unchanged. This
proves the new diagnostic reaches both guest predicates on a successful
execution; it neither reproduces nor retroactively explains the old failure.

### Bounded same-task handle-reuse diagnostic

CommonDialogRounds (1--32, default1) selects a bounded number of ordinary
create/destroy cycles in the independent probe, followed by one live dialog
for original task-exit cleanup. Every round retains the original assertions
and records actual guest AX values; the verifier checks each round's exact
created/closed/hook counters and task identity, plus resource callbacks before
retirement. No original guest or runtime source changes are made.
commdlg-reuse-evidence-r1 supervisor26224 completes with probe/result.exit0:
16 normal destructions,17 creations/initial hooks, errors0, and exact-once
retirement. Each completed destruction has AX1 followed by IsWindow AX0.
The last three records reuse HWND16 0268. This is bounded coverage of the
source-identified reuse hypothesis, not proof of the old failure's cause.

Do not keep repeating that same diagnostic to manufacture closure. Owner is
asked whether to retain the old unexplained event explicitly as risk while
delivering the currently verified baseline, or to require attribution before
delivery. No waiver has been assumed; unrelated remaining diff/package
reviews can proceed. The recorded historical blocker remains until a decision
or causal evidence resolves its disposition.

### Delivery review: observer ownership and archive census

Reviewed the current idle binding, lifecycle diff, original wkman.c diff and
fixture assertions together. Registration intent remains in original TD
hIdleHook; the lifecycle owns the actual per-thread observer. Native entry
forwards CallNextHookEx, whereas taskman's direct dispatch does not enter the
native chain. The original callback body is retained behind the renamed
entrance. Initialization installs CBT then idle observers before guest InitTask;
rollback removes both, and retirement removes the idle observer before freeing
the carrier. Callback execution borrows task/object scopes and releases both
through nested finally blocks. This review does not extend the accepted
thread-before-global ordering to original cross-process priority equivalence.
Relevant assertions remain in wow_idle_hook_fixture.c and
wow_user_task_lifecycle_fixture.c; the preceding grouped results are the runtime
evidence, not this source review alone.

idle-archive-review-r1 supervisor14004 completes with exit0. The archive census
records62 packaged files and zero unresolved static module imports; its SHA256
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7
matches assets/winnt-guest-manifest.json. This is static import closure for the
selected archive, not proof that every dynamically requested guest component
or application feature is supported. No guest file was rebuilt or patched.
The unresolved historical common-dialog event still blocks unconditional S2
closure; this review neither waives it nor changes the published P15 package.
