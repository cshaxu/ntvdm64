# Project Status

**Active: M0 T447 S2 — WOW32 callback and object foundations.**

## Current Work

Owner accepts T446 and directs "验收通过 批准收口t任务 准入下一个t任务".
[T446 closure](../history/m0-t446-carrier-visibility-closure.md) records its
accepted delivery ca0abb7c4. The former queue head is now T447:
[WOW32 messages, callbacks and task execution](../proposals/proposal-wow32-message-task-execution-001.md).
Its W01 ownership and inherited evidence are retained. The
[working plan](../etc/operations/t447-wow-execution-plan.md) records S1--S6;
S1 source audit is delivered in f950ffd38 with documentation/link/diff checks.
Only S2 is active under the owner's continued WOW32 recovery instruction.
Owner additionally approves NT Monitor per-Win16-task Delete alongside
existing worker termination; the working plan records identity, escalation
and cleanup gates. Under the approved replan this belongs to S5, not a
concurrently admitted packet.
Owner approves design-only evaluation of public native callback/wait/reply
boundaries in place of unavailable private SMS association. Implementation
and any observable scheduling differences remain subject to design review;
the no-second-scheduler constraint remains in force. The later bounded SMS
evaluation approval below supersedes the earlier research transport limit.
The native-boundary-only draft has not satisfied early-reply ordering.
Owner now approves research of association-bearing WOW message envelopes
while retaining native USER delivery, with standing authority to resolve
similar necessary design-research questions without asking again. This is
not approval to silently change guest behavior, add a second scheduler or
declare an unproved design complete. Production edits and publication remain
frozen until the requested integrated design brief and mirror-diff budget.
Owner approves evaluating original SMS delivery for WOW-to-WOW calls within
one worker, resolving the prior design-admission blocker. See the
[boundary review](../etc/operations/t447-native-execution-boundary-design.md).
This permits dependency-complete design research beyond native-only delivery,
not wholesale USER-server import, a second scheduler, or implementation before
the integrated design brief and mirror budget. Native destinations, external
callers and implicit native sends require explicit interoperability treatment.
S2 is not complete; the production freeze remains while this design proceeds.
Owner now resolves the implementation-policy question: prefer original logic,
but a clean project implementation is permitted when original integration
would make the architecture excessively complex or error-prone, provided
semantics remain equivalent. This is not approval to relax reply/reentry or
conditional task-handoff behavior. The prior request for a semantic waiver
is superseded, not accepted. Integrated design and mirror-budget review remain
required before production changes; no further policy approval is needed to
evaluate such a bounded replacement under this equivalence requirement.
Owner requires original-source reuse with zero/minimal diff, not newly
authored substitutes for original connectable logic; source tracing identifies
gaps and tests accept newly connected code, not select successive patches.
Proven-composable original logic must replace its autonomous counterpart in
the same repair, deleting that counterpart rather than deferring migration.

## Active Packet

Owner approves adoption of the cross-T planning audit. The working plan's
[audit recommendation](../etc/operations/t447-wow-execution-plan.md#owner-requested-cross-t-planning-audit)
reconciles W01--W20 dependency transfers, residual scopes and adopted S
boundaries. S2 alone remains active: callback and object foundations; S3
owns basic controls/dialogs/menu dependencies, S4 full message semantics,
S5 task termination, and S6 final acceptance. Necessary earlier cleanup and
message dependencies cannot be deferred merely to match this sequence.
Owner also directs review and commit of all dirty files before continuing.
The [reviewed checkpoint](../etc/evidence/m0-t447-s2-checkpoint-review.md)
is an explicitly labelled development checkpoint, not a delivered P,
S2 closure, runtime acceptance or publication. Existing release stays intact.
The staged menu/standard-control helpers remain incomplete and must not be
mistaken for production-connected capabilities. Preserve their evidence and
finish their dependency-complete integration in S3 after S2 foundations.

Owner's continued execution instruction supersedes the blanket implementation
freeze above. The existing whole-execution inventory remains binding, but an
unresolved message-interoperability design does not block an independently
reviewed repair group. First admitted group: InitTask initialization commits
once; a failed launcher-start notification remains retryable by original
W32Thread without initializing the task again. Changes are confined to the
project-owned registration bridge, lifecycle carrier/header and its fixture;
expected additional mirror diff is zero. Verify transient/permanent failures,
identity mismatch, reentry and notification outside the USER lock. This is
partial S2 implementation, not completion or publication of WOW recovery.

Next reviewed C03/C10 group is the project dialog callback boundary:
W32InitDlg and the supplied WOW DLGPROC currently enter original guest-capable
code without the existing task callback scope; dialog-procedure exceptions
also bypass terminal property/borrow release. Use the existing task owner,
retain the original callbacks, and place terminal cleanup in finally even
when entry fails. No new scheduler, dialog policy or mirror diff. Verify
normal, nested-destruction, callback-exception and denied-entry cleanup.

The adjacent original DispatchClientMessage split is also reviewed: only its
tagged WOW branch invokes pfnWowWndProcEx; its native branch invokes WNDPROC
directly. The outer project gateway must not acquire WOW execution ownership
or publish guest callback state for that native branch. Retain window storage
across either call and leave dispatch selection to the original body. This
group changes only project code/tests, with zero additional mirror diff.

Hook entry admission: all 32 original stubs converge on WU32StdHookProc before
CURRENTPTD, message conversion and guest stack allocation. Rename only that
definition and provide its existing entry symbol in a project-owned wrapper
which pairs existing task and private-USER borrow scopes around the entire
original body. No hook selection/thunk algorithm is copied. Mirror budget is
one definition-line replacement plus its divergence comment (DIV-332); no
guest or wire ABI changes. Verify entry denial, normal/nested calls, exception
unwind, retained 32-stub routing and full provider link.

Class-menu dependency admission: original createw.c loads an absent top-level
menu from the selected class after CBT and before WM_NCCREATE. Native class
publication clears that menu name, but current creation omits the matching
load. Reconnect the existing original clmenu/W32 resource client at the
enrolled WM_NCCREATE boundary; preserve child IDs, explicit menus, failed
loads and menu destruction on failed creation. No extra mirror or guest diff.
Verify ordering, creation-message/GetMenu agreement and nested/failure cases;
then rerun real application initialization. WRITE's hidden main window is
consistent with this gap, not proof that this is its sole failure.

Proven dependency promotion: DATA-07/08/09/10/13 and MENU-01's client menu
graph are required before WRITE's original WM_CREATE can pass GetMenu. The
pinned USER.EXE performs GetMenu locally, reading WND+104 and then MENU.head.h;
current page-domain window publication leaves that field zero. Native menu
loading alone cannot repair this contract. Audit and implement the complete
bounded MENU/ITEM projection with original guest readers retained, shared
handle type/uniqueness, graph mutation and retirement. No guest redirection
to WOW32, native pointers in guest state, dummy menu headers, or second menu
policy engine. This promotes the proven W02/menu dependency into active S2;
it does not admit unrelated resource work or reduce message/reply obligations.

Class lookup dependency review: original GetClassPtr retains the hModClient
fallback, but lifecycle initialization leaves client_module zero. Its original
inner lookup interprets zero as any module, admitting unrelated private classes.
Bind this field to the loaded native USER32 module, matching original
InitializeClientPfnArrays, and test the initialized context through the retained
GetClassPtr. This independent owned-code correction adds no mirror/guest diff.
The broader missing standard-class metadata/creation/retirement closure remains
under source review; fixing this field does not claim to restore those controls.

Class-name prerequisite admission: the pinned USER16 GetClassNameA reader
rebases WND.pcls and CLS.lpszAnsiClassName at offset100. Current publication
leaves that pointer zero. Extend the existing worker-local publication call
with a counted, call-local ANSI name; allocate CLS and its copied NUL-terminated
name together and publish only guest server-form addresses. Keep original
reader, class policy and retirement intact. Verify the exact two-step original
rebase, source-buffer independence, invalid lengths and paired retirement;
rebuild both worker/provider for the changed private import contract. No
guest or additional original-source diff is needed.

Native-control procedure prerequisite: public USER may return an opaque
CallProc handle, whereas original DispatchClientMessage receives a resolved
procedure. Keep that handle in owned window storage, expose its original
identity to the existing subclass caller, and normalize only the dispatch/
cleanup field to a native forwarding procedure using CallWindowProcA. Never
put its FFFF value through the original WOW-tag test. Preserve original
DispatchClientMessage and cleanup bodies, with zero additional mirror diff.
Verify real A/W native procedure conversion, replacement/restoration,
no guest-entry acquisition, and terminal/exception borrow release. This is
one prerequisite of the reviewed standard-control closure, not its completion.

Standard-class descriptor admission: reuse original walias.c's classifier
and single stdClasses table, including its predefined atoms. Query native
class information by both original name and atom; never interpret
GetClassInfoEx's BOOL as an atom. Extend the selected original user.h with
13 exact FNID declarations (no original executable changes). This read-only
boundary registers no replacement controls and creates no second class table.
Verify identity agreement before connecting WC/CLS storage and its paired
window/retirement boundaries. Standard-control recovery remains open.

Class-procedure prerequisite: original wuclass.c already handles restoring
native CallProc identities, but owned class storage rejects them. Reuse the
window CallWindowProc forwarding entry: keep the native identity separately,
normalize the original cleanup field, and snapshot the logical class target
for each newly created window. Serialize the paired fields; leave original
class lookup, conversion and existing-window targets unchanged. Verify real
A/W identity restoration, new-window dispatch and no guest-tagged native
cleanup value. Additional mirror/guest diff budget is zero.

Creation rollback prerequisite: both CBT enrollment paths must withdraw their
own WW/object publication before returning a veto to native USER. The current
post-CreateWindow rollback can encounter an already destroyed HWND, which
the existing detach boundary correctly rejects. Handle own publication
failure, downstream-hook veto and hook exception while HWND is still live;
preserve existing borrows and the original failure. Verify owned-object and
actual allocation release, not merely a NULL creation result. No mirror or
guest changes, new creation policy or changes to another hook's decision.

Class-query prerequisite: reuse original ntcftxt.h GetClassInfo in the existing
ANSI class-client compilation. Its capture/failure/output-pointer behavior is
composable through the existing macros. At the unavailable NtUserGetClassInfo
boundary, select enrolled metadata with retained GetClassPtr, query native
class fields by that exact atom/module, and reveal its logical procedure and
client menu rather than the native gateway. Unenrolled native classes retain
native query behavior. Budget: approximately45 unchanged original body lines
plus subset comments; zero guest changes. Verify names/atoms, module-instance
lookup, subclass replacement, client menus, failed output and native passthrough.
This does not import private USER cursor/CPD/server machinery or establish the
still-missing standard-class publication and version-policy closure.

Class-registration projection ordering: the native registration callback
currently publishes CLS before retained _RegisterClassEx fills adwWOW and
hTaskWow. Move guest projection to the completed original call's return
boundary and remove the callback's duplicate task assignment. Preserve native
registration rollback and original client menu cleanup on projection failure.
Verify both WOW words, task identity, one publication, and failed-registration
cleanup. This fixes enrolled-class data, not the missing standard-class
factory; additional mirror/guest diff is zero.

Standard-control storage group: original clinit.c/init.c registrations are
already implemented by native USER, but lack the worker's shared WC carrier.
Populate process-public metadata from the retained walias.c table and native
descriptor without registering replacement classes. Reuse GetClassPtr and
the existing reference domain. Add paired metadata-only retirement, refusing
live-window associations and never unregistering borrowed host controls during
process teardown. Explicit UnregisterClass retains its original policy/native
result. Verify original lookup precedence, shared words, retained borrows and
native registration survival. This storage stage remains separate from
production startup wiring until creation, guest projection and retirement are
connected together; no new mirror or guest changes.

Native subclass-chain prerequisite: NTW16S installs SetWindowSubclass after
creation, putting a native carrier above the WOW gateway. Private GWL/GCL
procedure access must select an existing logical target from owned WW/WC,
not require the gateway to be the public top procedure. Preserve the actual
native chain; only original WOW logical target state changes. Unenrolled native
calls retain native behavior, but a tagged guest procedure must never fall
through into native USER when its logical owner is missing. Verify a real
comctl32 subclass above the gateway, replacements/restoration, and unchanged
native/CPD paths. Zero mirror and guest diff.

Standard-class cleanup prerequisite: retained _WOWCleanup restores standard
class procedures through its original atom-indexed table. Bind the nine
prepared public controls to that table before its first invocation. Preserve
each borrowed class's original native procedure separately from mutable
subclass state; normalize opaque CallProc identities at the existing class/
window boundary. Retain the original Edit-to-DefWindowProc exception and
module selector policy. Verify original cleanup after multiple procedure
changes, native identity restoration, class lifetime and new-window snapshots.
This remains staged with the standard-control group until creation/projection
and retirement are connected; no guest or additional mirror diff.

Shared class projection prerequisite: move the existing completed-class
projection into its class-storage owner so prepared native controls use the
same name/CLS allocation and retirement path. Keep original registration
ordering and rollback. Native procedures/CPD carriers must not enter guest
CLS; only encoded WOW procedure values are projected. Selected USER16's
GetClassLong/GetClassWord remain host thunks, while GetClassName reads the
copied name through WND.pcls. Verify both registration and all nine native
class projections in real CCPU memory, original rebase, shared identity,
duplicate-publication rejection and complete release. No mirror/guest change.

| Field | Contract |
| --- | --- |
| Identifier Mode | M0 T447 S2, Ordinary Mode; one implementer and subsequent separate review pass. |
| Admission And Approval | Owner's continued instruction to restore WOW32, original-source priority and minimal mirror diff; clean project implementation is allowed where original integration is excessively complex/error-prone, with equivalent semantics and explicit source/design justification. Sequential admission after S1 delivery f950ffd38. |
| Objective | Deliver original callback execution and dependency-complete W02 object/shared-view foundations: creation, first callbacks, updates, rollback and normal retirement. Retain the whole-T audit; S3 owns basic controls/menus, S4 full C01/C02 messaging, S5 task termination. No patch-by-patch discovery. |
| Non-goals | No guest patches, global re-audit, second scheduler, broad USER-server import or assumed heap diagnosis; no silent absorption of other W owners. Original same-worker WOW SMS delivery is admitted for design evaluation only. |
| Reference Baseline | f950ffd38 S1 source audit; accepted ca0abb7c4 thirteen-image release; T422 E91/E92/E93 and S3 checklist; T430 G03 handoff. |
| Files And ABI Surface | First repair: mvdm/wow32/wcall16.c DIV-178, wow32-dll callback-frame lease source/header and focused tests, registered provenance/evidence. Reuse original wow32.c resolver without rewriting it. Message binding changes require their exact source design before implementation. No wire/guest ABI change. |
| Applicable Rules | Execution, architecture, coding, documentation, source policy and WOW program gates; original x86 /MT CCPU40; background-only test/build runs. |
| Verification | Source design and independent diff review; after code connection test actual CallBack16 ordering/rollback, ordinary and nonpresent/remapped resolution, nested callback and lease failure; then affected production gates, DOS17, independent WINMINE/SOL/WRITE and coherent thirteen-image publication. No discovery test loop. |
| Expected Markers | Original resolver really owns the mapped result; no autonomous mode-dependent callback resolver remains; each acquired frame released on failure, no pre-entry state drift, no alias across CPU reentry; explicit outstanding message/lifetime results. |
| Asset Needs | Existing sealed package, immutable guest images, retained MSVC/Ninja and fixtures; new outputs only below build/M0-T447/S2 and approved isolated stages. |
| Reporting Requirements | Keep stable C/W/E/G IDs; report expected/actual results and confidence independently per app; inherited failures remain open. |
| Stop Conditions | Each implementation group requires source-based dependency and mirror-budget review; unresolved dependencies block that group, not unrelated reviewed groups. Existing callback edits are unverified candidates. No discarded resolver result, guest mutation, fake SMS identity or unapproved cross-owner scope. |
| Exit Criteria | Production-connected callback and object foundations, including paired startup/teardown and real guest boundary evidence; source/diff review, per-P non-regression gates, hash-verified publication to assets/release and configured system32, commit/push. Full C01/C02 remains mandatory at S4, with earlier prerequisites promoted when needed. The owner-directed development checkpoint does not meet this exit. |
| Original Owner Request | 单人双角色模式 复通wow32；原版可接入则立即替换自主实现；以代码追溯找缺口，测试只作接通后验收。 |
| Similar-Issue Sweep | Normal/early/duplicate replies; same/cross-task reentry; sender/receiver/task loss; callback frame/lease release; original init/yield/hung/wait paths. |

S1 baseline observation now reproduces WINMINE's main window and separate
SOL/WRITE initialization-error dialogs; see the
[execution baseline](../etc/evidence/m0-t447-s1-execution-baseline.md).
The already-started fixtures terminated: send/foreign/task-order passed;
early-reply failed with exit95 after native result114, still receiver-owned.
These are auxiliary
native fixtures, not guest acceptance. Source tracing identifies absent
receive-state integration and missing task-name allocation release; input
attachment and abnormal retirement remain under source review. No production
repair or new release is claimed. S1's second-role review, governance and
delivery completed in f950ffd38. S2 begins with the callback-frame prerequisite
without dropping message/reply or lifetime obligations.
S1 must trace missing providers rather than infer their cause from an OOM
dialog or repeatedly run tests to select patches. SOL/WRITE normal initialization is a mandatory
basic-functionality milestone across the existing owners, not satisfied by
matching their prior failure. Proven cross-owner dependencies require explicit
bounded promotion. Full production gates apply to subsequent runtime changes.

## Current Technical Baseline

The coherent13-image T446 S1 package is published to assets/release and
O:/winnt/system32. release-manifest.json records exact identities; T446 S1
publication verifies all tested/copy/deployed hashes and published-byte guard
behavior. Twelve unchanged images retain the T445 S11 baseline and its style
boundary evidence.
S9 P1 is266067264 and P2 is4f8c181487; S10 P1 is e046ee3d7;
owner-accepted S11 P1 is b39ceb7e6, acceptance record cbd0f7832.
The preceding T445 final closure changed documentation only; T446 S1 updates
NTVWM's private-carrier guard without changing the other components.
Guest media and ordinary runtime configuration remain unchanged.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll, ntw16s64.dll, W16Style.exe |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll, ntw16s32.dll |

--win16-style and --win16-style-all retain their propagated policy. WOW32
owns Win16 attachment; native Hooks use their matching-width NTW16S DLL.
S11 changes only NTW16S32/64 and W16Style among the13 images: generic dialog
background/link typography, flat separators and multiline EDIT attachment.
The other ten binaries retain the S10 identities. W16Style adds page28 for
dialog/link coverage; native branding and application-owned painting remain.
Title/menu/standard-control System bitmap type is20px. The demo layout
matches20px proportions. Generated caption prefixes are retired; application
titles remain unchanged. NTCON links RPC core without launch-style state.
See [S11 evidence](../etc/evidence/m0-t445-s11-dialog-coverage.md) for actual
verification and limits. SOL/WRITE retain their known initialization failures,
assigned to the existing basic-WOW queue rather than claimed repaired here.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T446 | Owner-accepted event-driven private Console hiding, completion ordering and coherent release. [Closure](../history/m0-t446-carrier-visibility-closure.md) |
| T445 | Modular Win3.x appearance, propagation and integration,20px demo/caption cleanup, default-button and dialog/control coverage. [Closure](../history/m0-t445-window-style-closure.md) |
| T444 | Owner-accepted indexed palette repair, audit-only capability disposition and measured host VT/API fidelity. [Closure](../history/m0-t444-indexed16-text-closure.md) |
| T441 | Repaired mixed NTVDM/NTVWM release that stalled COMMAND/ver; bounded variable I/O retained. [Closure](../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Verified graphics/transport/idle/build improvements retained; active root cause deferred. [Closure](../history/m0-t440-text-response-performance-closure.md) |
| T438 | Reversible Win3.1 path repair and CMD/PIF/GRP ownership. [Closure](../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited Win3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

Final owner-directed closure includes S10/S11 after the earlier S9 closure.
Earlier stages closed without manual testing remain explicitly distinguished;
S11 acceptance does not retroactively change their evidence. Native-menu
animation and bounded glyph/layout limitations stay in TODO, not represented
as fixed. The whole T retains registered S7 DIV-331; S10/S11 and final
documentation closure add no mirror/guest diff.
[Archived status](../history/m0-t445-pre-closure-status.md) retains the former
detailed chronology; no historical packet there is active.
