# T447 S4 message source audit

## Question, inputs and proof level

Continuing from clean fd079c68c, single-person implementer/reviewer. Determine
the complete execution/receive/reply repair group before editing runtime,
not by repeatedly running a failing application. The active M1--M6 scope is
in the [working plan](../operations/t447-wow-execution-plan.md#s4-message-closure-admission-and-fixed-source-inventory).
The [existing native-boundary research](../operations/t447-native-execution-boundary-design.md)
is essential retained source evidence, with historical decisions superseded
only by the current approved modern-USER contract.

Inputs: selected MVDM wumsg.c/wmsg16.c/wmdisp32.c/walias.c; selected
ntuser/kernel/taskman.c/userk.h; existing lifecycle, runtime, message bridge,
private-access redirects, callback registration and fixtures. Read-only
canonical sendmsg.c/focusact.c/queue.c at pinned OpenNT
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c supply the private-server reference.
No original guest or new runtime source is imported/edited by this audit.
Published DLL import evidence comes from S4 source-audit-r1, not a new runtime
test. All implementation conclusions below are source conclusions unless
explicitly identified as retained S3 execution evidence.

The expanded owned source-audit-r2 (PID35064, start
2026-10-18T02:23:08.6455032Z, exit0) pins the canonical version and records
the original/current input hashes and every selected task-selection write.
Canonical SHA-256: sendmsg.c
AF68BADCB022EFCB3947968FC6E3DBF148B697072EBEB2898F97FBCB774039F3;
focusact.c AAF521712C64F0E3E9EECBF383321519E6A1F8CE46A774940A88BFF3CEE3CB8C;
queue.c 0400C034BB5A782E81777E9FD805C07DBCD18D60EC9F1A876CFA67444577E0F8;
taskman.c E37F4D724650D5758EE05C54E181771AA584AEAD15F5A1376B322ECB7016E64E.
These are read-only reference inputs, not new source/build dependencies.

## Confirmed whole-chain findings

| Finding | Actual source / consumer | Consequence for the finite group |
| --- | --- | --- |
| Native receive/reply is already the real resource owner | Original WU32InSendMessage calls InSendMessage; WU32ReplyMessage calls ReplyMessage. Published WOW32 imports those actual USER32 functions. | Do not author another receive/reply engine or infer native sender identity from an adapter record. Native flags/results remain authoritative. This does not establish WOW continuation handoff. |
| Outer HWND does not enumerate implicit message receivers | Native-call begin resolves only its window argument. Canonical focusact.c xxxSendFocusMessages sends WM_KILLFOCUS to the old focus before WM_SETFOCUS to the new one; NULL or caller-owned new focus does not identify that old receiver. | The repair must cover implicit callbacks and actual execution readiness, not merely explicit SendMessage targets. Do not add filename/message-tuple heuristics or special-case SetFocus as a complete solution. |
| The current call carrier is not received SMS identity | Lifecycle begin writes sender psmsSent; end calls DirectedScheduleTask using the nominated receiver. No selected adapter produces psmsCurrent. Original taskman consumes that received record for cross-process lock accounting. | Retire target-based pseudo-reply scheduling as part of the replacement group; do not fix it by assigning psmsCurrent to an arbitrary pending send. Preserve original scheduling through truthful ready/suspended boundaries. |
| HEVENT_REMOVEME does not persist native-stack exclusion | Original xxxSleepTask skips self for that one scan, then InsertTask retains its real event count. A different task's later scan checks nEvents>0, not whether the first thread's native operation returned. | A native-suspended task with pending events can be selected although its stack cannot enter guest execution. Do not clear nEvents, poll, cap time or merely add another SleepTask call. Persistent eligibility must be paired with original selection and all other selection entries. |
| Public queue wake is not an executable guest continuation | Original queue SetWakeBit respects the current wake mask; native SendMessageTimeout owns SMTO_BLOCK delivery suppression. | No extra PeekMessage to force a blocked sender to receive, and no interpretation of arbitrary queued posts as native-return readiness. Preserve timeout/result independently of reacquiring the cooperative guest CPU. |
| Same-thread and nested callbacks do not prove a new received message | Original sendmsg same-thread branch does not replace psmsCurrent; true xxxReceiveMessage saves/replaces/restores it. With the approved native-only transport, native USER already performs that distinction. | Do not create an adapter current-message stack from HWND/message equality, hook notification or callback count. Execution scopes may nest, but cannot become another receive authority. |
| Original conversion has important movement and retry rules | WU32SendMessage copies scalar arguments before ThunkMsg16 and frees flat argument references before native dispatch; ReplyMessage explicitly warns of pointer invalidation. PostMessage retries eligible same-WOW private messages on ERROR_INVALID_PARAMETER. | Retain those original bodies. M4 pairs their existing converters and lease owners through return/failure/nesting; no universal marshaller or blanket posted-pointer refusal is selected. |

The eligibility counterexample is already established in the retained design;
it is not a new runtime discovery or permission to rerun old failed probes.
The original scheduler is usable for priority/event/task selection. Its
unavailable kernel message/wait eligibility input is the boundary to adapt,
not a reason to copy its algorithm into a project scheduling loop.

## Original recovery ladder and state authorities

1. Retain the already selected MVDM conversion, callback and original taskman
   algorithms. They are actual production owners, not reference-only candidates.
2. Bind the missing native-stack eligibility/execution transitions through the
   existing bound task and LIFO call/callback scopes. Exact selection seams
   must account for the entry set below before runtime selection. A minimal
   registered selection predicate is a source-shaped adapter candidate, not
   permission to rebuild the scheduler or reconstruct a native SMS.
3. Importing full canonical xxxInterSendMsgEx/xxxReceiveMessage/_ReplyMessage
   is not composable under the approved native delivery owner: these bodies
   own kernel SMS allocation/lists, wake/reply waits, desktop objects, hook
   delivery, receiver death and private USER transport. Their unavailable
   dependency closure is not solved by retaining their names or copying a
   few branches. The earlier local-SMS/notification candidates and negative
   mixed-origin proofs remain retained, not revived as a second transport.
4. Newly authored transport/priority/runnable queues are rejected because
   native USER and original taskman already provide those respective owners.
   No adopted third-party code intrusion is needed or selected in this audit.

Authority map: native USER owns receive state, delivery, reply, native timeout
and native queue contents; original taskman owns runnable ordering and guest
execution selection; current task/native-call scopes describe whether that
stack can actually enter guest code; original converters and existing lease
owners own their buffers; existing task/window owners own retirement. A
queued event is not another authority for native-stack readiness.

## Finite implementation review set

The replacement must be reviewed as one group, not a succession of send-only
patches. These are required cases, not an assertion that the design is already
implemented or that new compatibility concessions are authorized.

| Original selection / transition | Required rule and evidence |
| --- | --- |
| Priority/event scan | Retain original ordering/counts; exclude a native-suspended stack until actual callback or native return is ready. Preserve events rather than erase them. |
| Idle-hook temporary selection | Callback eligibility must hold before any guest-capable hook, not only before CCPU simulation. No new global hook policy. |
| WOWEXEC hardware branch | Retain original hardware/event handling and its real thread context; pending hardware work is not a generic fake native return. |
| InitTask forced selection | New committed task is a genuine ready continuation; preserve creator/startup rendezvous and failure rollback. |
| Task lock | Preserve CurrentTaskLock semantics. Do not clear nTaskLock or select another task merely to avoid an application-created lock dependency. |
| Callback nesting/inherited owner | Enter before guest-capable marshalling, preserve same-thread inherited execution, restore the outer native-suspended state after copyback; all retained callback families share the boundary. |
| Native return/failure/timeout | Actual syscall completion makes that caller ready, not any inferred receiver reply. Restore execution, return value, LastError and object publication before returning to guest. |
| Retirement | Invalidate call eligibility before thread storage release; no callback into a retired task or pointer into a returned sender stack. Normal cleanup remains independent of S5's requested task deletion. |

Existing adapter callout census (native/CPD CallWindowProc, creation/failure
destruction, modal loops, controls/menus and queue servicing) must be reconciled
against current S3-selected implementations, not restored from obsolete staged
versions. The current source already repaired many earlier lifetime/creation
items; do not reintroduce the old native-dialog approximation or duplicate gate.
In particular, an eligibility flag alone is insufficient: entry must not
manufacture a gratuitous task switch for a synchronous same-thread callback,
and queued input must not revive an outer blocked native frame during nested
return. No wait-state sampling or timing heuristic is an authority for either
decision. These obligations belong to the same eight-entry review, not new
features or permission for an undocumented scheduling concession.

## Acceptance-contract correction before implementation

The old native-identity fixture requires psmsCurrent to equal the pending
local psmsSent. That is an NT4-representation assertion incompatible with the
approved modern receive/reply owner, not a valid repair target. Replace it
in the full group with public native receive facts, duplicate/invalid reply
outcomes, once-only callback/result and mixed/nested isolation. Retain the
old witness as historical negative evidence; never manufacture a pointer
equality merely to make it green.

The old early-reply rendezvous waits without yielding inside the receiver
until the WOW sender executes. It tests the message-associated NT4 handoff
whose removal the owner subsequently approved for all native transport/reply
paths. Keep native transport return and cooperative WOW execution reacquisition
as separate assertions, with actual guest waits yielding through the original
task owner. Do not report the old test as passing or silently delete timeout,
duplicate-reply, error, single-execution-owner or lifecycle assertions.

Required integrated denominator remains all M1--M6: real two-task Win16 sends
and posts, same/cross-worker and both native directions, implicit native sends,
mixed nested traffic, early/duplicate/no-pending replies, queue filters/quit,
SMTO_BLOCK suppression, converted input/output and death/failure cleanup.
Native doubles supplement but do not replace those real boundaries. No new
test is run merely to rediscover a source-proven missing producer.

## Current disposition

This checkpoint identifies the persistent-eligibility integration and removal
set; it does not certify a finished runtime design. Complete the eight-entry
selection/transition review before freezing its exact mirror budget and wiring
the whole group. No new runtime/mirror/guest diff or product build/publication
is selected here. S4 remains active, the P22 package remains deployed, and
the full WOW32 objective is unachieved. There is no user-approval blocker:
continue within the existing native transport/original scheduler boundary.

## Registered eight-entry execution group staged

ADAPTER-WOW-077 and OPENNT-HOST-123 register the current complete execution
dependency group before its mirror edit. Five predicate operands enter the
existing taskman scan/idle/hardware/locked-wake/wait-completion conditions;
event counts, original priority/list policy and CurrentTaskLock remain intact.
Fresh InitTask storage is ready by construction; no inherited frame or guest
layout field is introduced. Terminal withdrawal happens only after refusing
cleanup edges have passed, immediately before original task removal under
the same lock. Retryable unbind refuses an active native scope unchanged.

The existing lifecycle task owns a borrowed LIFO frame pointer, never another
task registry. Native call frames are ineligible, real callbacks temporarily
publish ready frames, and actual native return reacquires entry-owned
execution through original DirectedYield. Synchronous same-task/inline calls
retain inherited execution; an actual different-task callback may park an
owner already in a native frame through the original nonblocking SleepTask
branch. That branch cannot wait or invoke a guest idle hook in this use.
No source or destination message identity is inferred by this transition.
Old receiver-id/pseudo-SMS/psmsSent/native-return pseudo-reply code is removed,
not retained as a fallback. Native USER InSendMessage/ReplyMessage stay direct.

The staged callout group includes inline Peek/receive, enumeration and next-
Hook dispatch, real MsgWait and native MessageBox/ShellAbout; original modal
dialog/control/converter bodies are unchanged. Existing object publication,
return/error and exception cleanup remain at the common owner. Original
resource-only pre-InitTask queue creation retains its separate data-lock pair.

Authored tests add persistent eligibility across different task scans without
discarding events, task-lock self wake, idle/hardware exclusion, actual nested
native frames and unbind refusal. Native fixtures now enter the actual common
callback scope before invoking WOW services; private-SMS equality is replaced
by native receive flags and zero fabricated associations. Early/duplicate
reply separates transport completion from an actual cooperative product wait,
retaining first-reply result and cleanup assertions. These are staged inputs,
not new passing tests or full real Win16 message acceptance.

The entire dependency group is now ready for its first grouped compiler and
native/original/guest acceptance. No partial candidate is published. Full S4
M1--M6, including real two-task mixed-message proof, is still required; this
implementation group does not shrink the goal or add a semantic exception.

### Renewed installed WINMINE startup report

The owner again reports very slow `run16 winmine`, now apparently hanging
compared with the earlier black-and-white implementation. Prioritize this
diagnosis before expanding S4; do not infer that the uninstalled S4 candidate
causes the installed symptom or discard the pending execution work.

`build/M0-T447/S4/installed-mine-regression-r1.ps1` runs hidden, owned,
serial installed-package observations with a360-second supervisor bound.
Supervisor PID28852 exits0. Production NTVDM and WOW32 still match the P22
hashes in Current and assets/release. No project process was present at the
initial identity inventory. No package/profile/guest or released file changed.

First, `verify-wow-logon-gameplay.ps1 -Installed` uses the full supported
CreateEnvironmentBlock for the current token, inherit=false, without filtering
or replacing PATH. It records45 entries/3349 UTF-16 code units including entry
terminators and final terminator. Off, Win16 and All pass real owned-window
flag/reset/open pixel changes, WM_CLOSE and run16 normal exit0. Probe-local
window-ready times are4266/4859/4953ms, followed by board-ready1781/1813/1860ms.
These are phase times, not exact user's shell-to-frame measurements or an
acceptable-performance declaration. The unstyled Off case remains native
frame=1; the two style cases report native frame=0.

Second, the same installed images and Console-owning observer run
`observe-wow-frontiers.ps1 -VisibleDesktop -WaitTarget -Guests WINMINE.EXE`
with inherited developer environment. Supervisor-start shape records78 entries
and5983 characters; this shape is not a byte-exact measurement of the eventual
guest environment, since the test/observer have their own process variables.
At4195ms, and still at8185ms, the worker has the error text
`CS:05e3 IP:20c2 OP:63 61 6c 5c 54` and no Minesweeper main window. The observer
eventually reports `result=timeout`, exit0x53504354. The frontier runner's exit0
means observation/cleanup completed, not successful guest startup. Error HWND
visibility is0 in this hidden observer run; do not claim the owner necessarily
saw that dialog. Cleanup is pinned to the test's installed image paths and
process identities, not a normal-exit assertion for the failing case.

This establishes an environment-dependent failed startup alongside passing
complete-logon controls, not a universal product freeze. It is consistent with
the already source-proven immutable COMMAND EndInit/EnvSiz discarded-INIT
limitation retained in TODO and the S3 writer audit. This new run does not
recapture that writer, inspect the owner's actual CMD environment or bisect
the first bad binary. Those limits prevent declaring the owner's particular
failure causally solved. No guest patch, environment truncation, workaround,
new production fix or publication is claimed.

### Renewed owner report: installed P2 startup comparison

The owner again reports that ordinary `run16 winmine` is extremely slow or
frozen. This takes diagnostic priority over the unbuilt079/343 extension
group; none of that group's source is installed. At observation start there
were no surviving installed project processes to inspect. Installed WOW32 and
assets/release both hash F3FD174A25B72EC17274979B6F24A1C5C5314A9E025BD233847356F9F45680F2;
installed NTVDM matches the formal r003 image and its link map at
12DA2E8FCFFB35B724791F7356C1A9A87A5208442928C29127415AE8DC812705.

`build/M0-T447/S4/installed-mine-regression-r2` runs the same committed
`verify-wow-logon-gameplay.ps1 -Installed` against O:/winnt, then
`observe-wow-frontiers.ps1 -VisibleDesktop -WaitTarget -Guests WINMINE.EXE`.
The hidden owned supervisor records PID/start/commands, deadlines,13 installed
hashes, environment shapes, reports and captured game surfaces. The complete
Userenv logon block has45 entries/3349 characters, without filtering. Off,
Win16 and All pass flag/counter change, reset, opening a cell, WM_CLOSE and
normal launcher exit0. Window-ready phase times are5922/6000/5704ms; the
subsequent board-ready phases are2593/2391/1843ms. These are probe phase times,
not end-to-end owner-shell measurements or acceptable-performance claims.

The inherited developer shape is78 entries/5983 characters. At4236ms its
worker has no Minesweeper main window and a hidden error dialog reporting
`CS:05e3 IP:20c2 OP:63 61 6c 5c 54`. The launch times out with0x53504354.
The observer's exit0 records completed observation/identity-pinned cleanup,
not guest success. Hidden diagnostic dialog visibility is not evidence that
the owner's ordinary CMD hides the same dialog. This independently repeats
the earlier environment-dependent failure on P2; it does not establish the
owner's exact environment, failed phase or first-bad release.

A supplemental `installed-mine-cpu-r1` compiles the existing read-only
`ccpu_worker_snapshot.c` using x86/MT/O2 in its own build directory, then uses
the verified matching NTVDM link map. Three non-atomic samples at about5s
remain in real mode (CR0=10h, CS0216/IP062E, `nt_init_called=0`); the window
snapshot has no application/error window and this separate18s observation
also times out. Do not interpret a timer-interrupt stack as an exception
frame or its heuristic EnvSiz candidate as a proved COMMAND variable. This
run establishes a pre-WOW stalled sample, not the writer cause. An earlier
attempt to use a retained probe with the wrong map filename returned64 and
produced no CPU evidence; it is not counted as a pass.

No product/guest/profile/ZIP change, publication, rollback or repair is
claimed. All diagnostics finish and clean their exact owned installed process
identities. The prior source-proven discarded-INIT limitation remains relevant
but is not automatically assigned to the owner's new instance. The remaining
discriminator is whether the reported owner launch never presents a main
window or presents one that stops responding; request that distinction rather
than treating the passing controlled launch as resolution.

### Clarified owner symptom: active-input latency

The owner clarifies that WINMINE's window appears normally; clicking a cell
responds slowly. This is not the inherited-environment failed-startup control.
The previous diagnostic turn was progress (two distinct installed-startup
controls and exact P2 identity), but it did not investigate this clarified
symptom. No startup failure is used to dispose of this performance report.

Source tracing identifies the complete existing hot path before measuring:
native queue/dispatch/input/default-control call returns through
`wow_user_native_call_end`, which calls `publish_call_windows`,
`wow_user_borrow_client_graph`, and `wow_menu_client_refresh_all`.
`project_graph` copies all committed window observations, samples them and
closes their relationships before a complete graph commit. Borrowed client
graphs and menu refresh then reread the same backing through
`copy_window_observation`. WND rectangles/relationships/class fields and each
class-name byte use separate CCPU SAS accesses. Original ccpu386/ccpusas4.c
implements c_sas_loads as a byte loop; phy_r32 splits into word accesses.
Each ultimately repeats the session/physical mapping path. This is not an
original Win16 application algorithm or an NTVWM/NTCON frame transport.
Blame attributes the copied observation reader to ead0aa64a0 and the complete
native-return graph/menu chain to cf2ac95206/984ee0441a/01cb95ad9c and later
borrow composition; those source attributions are not a first-bad runtime
bisect or a claim that P2 alone introduced the slowdown.

The new reproducible group is
`tests/observation/verify-wow-mine-latency.ps1`, using the existing actual
WINMINE identity/pixel helpers, plus `wow_mine_latency_probe.c` and the existing
read-only `wow_native_pc_sample.c` with optional bounded rounds/QPC stamps.
It posts twelve right-click down/up pairs, requires twelve actual cell-pixel
changes, restoration of the initial board, WM_CLOSE and normal run16 exit.
A test-owned event starts sampling only after the real board is ready.
The summary filters native PCs to the QPC interval of those actual inputs;
startup and later idle samples cannot dominate the active-input histogram.
This is posted-input-to-pixel latency, not physical mouse-device latency.

Installed P2 baseline `build/M0-T447/S4/mine-input-latency-r1` completes all
three serial cases using the complete Userenv logon block, without filtering
or product/guest/profile changes. Matching NTVDM and WOW32 maps and image
hashes are checked before use and copied into the sealed evidence. Results:

| Case | Inputs | Median | Mean | Min--max |
| --- | --- | --- | --- | --- |
| Ordinary desktop, no sampler | 12 | 480.635ms | 491.298ms | 296.918--626.532ms |
| Ordinary desktop, sampler | 12 | 424.013ms | 436.531ms | 239.169--615.700ms |
| Private desktop, no sampler | 12 | 31.594ms | 33.464ms | 30.497--50.970ms |

The ordinary/private median ratio is15.21, with identical images and the same
original application. There are296 PCs from the busiest task thread within
the5.256s active-input interval.165/296 (55.7%) fall in scalar SAS/session/
lease/physical translation helpers;74/296 (25%) are in win32u. Explicit
copy_window_observation and other page-domain/graph routines also appear.
These are unweighted statistical samples with nearest-MAP attribution,
not complete call stacks or exact exclusive CPU percentages. Sampling
perturbation/run variability is visible in the separate sampled case.
The result demonstrates a severe ordinary-desktop-dependent adaptation cost,
not a universal15x speedup promise for a proposed repair.

ADAPTER-WOW-080 is the finite first remediation group: preserve every current
native observation, structural validation, atomic commit and borrow lifetime;
reuse the already selected original CCPU c_GetLinAdd for bounded immediate
copies of the page-domain's known WND/CLS RAM. No cache, alternate state
owner, reduced refresh rate, external-freshness waiver, scheduler replacement,
guest edit or new mirror diff. The fixed copied-read closure comprises the
committed observation reader and outstanding-borrow WW-view reader. All
callers already require the active original domain/session/CR3 and committed
or pinned allocation. Original nt_mem rounds these allocations to pages;
the copied WND176/CLS108/inline-name256 spans fit the known RAM page. A
non-fitting span and a noncanonical class-name pointer keep the original SAS
path. Native aliases are consumed by immediate CopyMemory only and never
stored, returned, serialized or used across callbacks/teardown. This is the
first usable original service through the existing owner, not a new generic
pointer manager. Registration precedes implementation in ntvdm-exe/README.

The actual CCPU fixture adds immediate backing-edit visibility, exact class
descriptor/name, caller error preservation, cross-page noncanonical name
fallback, wrong-CR3 refusal with unchanged output, and recovery after restoring
the directory. Existing retained-generation, reuse, failed-free, class and
borrow tests remain required. Stage the whole read closure, then compile it
once and run its lifecycle group and the identical latency comparison before
deciding whether it sufficiently addresses the reported responsiveness.
Two runner-composition failures are retained, not performance passes.
Read-build-r1 links the optimized worker, but additionally selects an old
unqualified page-domain test target that omits the current fixture's borrow
profile; its undeclared test-only identifiers fail compilation. The formal
window-borrow/reclaim/graph rows select the complete current CCPU fixture.
Read-build-r2 uses those rows, but their intentional refresh of all provider
objects picks up the preserved staged343 definition rename without regenerating
the provider graph for its079 facade, so the fixture link correctly refuses
missing CallBack16. Do not undo that seam, weaken the test refresh, or count
old-object linkage as current-source verification.

Consequently the source-complete079/343 group joins080's build/acceptance
composition. This is the previously admitted extension group, not a new
capability expansion or a reason to lose the input-latency priority. Its final
kernel RET audit also traces GETCURDIR/SETCURDIR to the exact kernel
drive/directory routines in read-only wow16cal.asm; those explicit service IDs
may use the already restricted resource-live cleanup admission. Application,
Hook, WinExec, task-exit and foreground-idle IDs remain refused during dying
task cleanup. The supplementary fixture includes both directory IDs.
Native extension units, actual original MM body/public refusal/frame checks,
real extension guest witnesses and all retained floors remain required.
Installed P2 is unchanged. No performance repair, release or S4 closure is
claimed from the source or either failed runner.

Read-build-r3 exposes a declaration-closure defect: lifecycle.h after original
precomp repeats unguarded wowuserp.h. Unchanged native-call struct/prototypes
now have one small wow_user_native_call.h owner, included transitively by
lifecycle.h; the thunk facade needs only this closure. No ABI/mirror change.
Read-build-r4 links both production images; the original-MM supplementary
object link additionally needs three untested resolver references before dead
stripping. Its MM-only fixture now makes those exact references ExitProcess98
if reached, never a successful fake mapping. General memory/GDI resolution
and MMSYSTEM16-family behavior are not proved by this selected-MM-body test.

Read-build-r5 links worker/provider and passes all079 supplements: scoped
wrappers, actual original MM arguments/directory ordering/zero target/exception,
original public callback refusal and four original frame phases. The first
window-borrow run fails real native-slot reuse:663 attempts within the existing
15s deadline do not obtain the old slot; new copied-read assertions report0
errors. This is a failed acceptance run. `mine-view-lifetime-r1` repeats the
exact binary without source/timeout/assertion changes: both reuse modes obtain
the old slot on attempt1 and the complete CCPU fixture exits0. Fresh formal
window-reclaim and graph-snapshot also pass retained/reused generations,
failed frees, irreversible class failure and cleanup. The first allocation
witness failure is retained, not declared repaired by a later pass.

`mine-view-read-performance-r3` is explicitly diagnostic/non-publishable while
that gate is failed. Canonical ZIP staging uses all13 verified baseline images
and complete logon environments, never a copy of the installed tree. Its
worker-only sensitivity control replaces NTVDM with
41ECBAC3E612B255CBB6015196C7F9A796F8875DC52DF06B332CFFF29631F971,
keeping P2 F3 WOW32 and its sealed matching map. All three twelve-input cases
and normal exits pass: ordinary median220.459ms, mean210.676ms,
range91.832--250.662ms; sampled median231.076ms; private median30.457ms,
range29.765--31.980ms. Ordinary median improves2.18x against480.635ms while
the private floor hardly changes. This proves a contribution, not resolution:
220ms remains slow and active samples retain mapping/view/graph/native costs.

The composed-provider comparison does not pass. Initial readiness accepts a
transient patterned cell1e192115; the first timed1.698ms change is actually the
covered cell3cc8733b appearing. Later inputs follow flag/question/covered, so
the retained final-restoration assertion correctly fails at exit71. Exclude
that run from performance statistics; it does not establish broken game input.
The next probe must establish completed owned-window painting before arming,
use identical setup for compared versions and keep the final-cycle assertion.

All above supervisors are terminal. Installed/assets P2, guest and configuration
are unchanged. Actual079 guest extension evidence, corrected composed latency,
remaining responsiveness work, every-P floors and coherent publication/review/
commit/push remain outstanding. This dirty source is staged, not a delivered P
or a request for owner testing. S4's six groups and S5/S6 remain intact.

#### Continued latency group: paired measurement and owned-field publication

The owner reports improvement but still noticeable latency and requests
continued optimization. Installed P2 has not been replaced; that subjective
improvement cannot be attributed to the unpublished080 candidate.

Correction to the preceding terminal-supervisor statement: failed composed
performance-r3 left its owned stage broker34976/worker7816 alive. The verifier
disposed the failing probe before throwing, then its finally accessed the
disposed process and aborted remaining cleanup. Finish-Owned now retains the
process object on failure for its finally owner. Exact stage/path/start/report
identities were recorded in performance-r3/late-owned-cleanup.json and those
two owned processes closed; the original failed stage was retained. This was
a harness cleanup defect, not evidence of normal product retirement.

Performance-r4 correctly refuses that live old endpoint. R5's readiness fails
because it checks an occluded board before raising the owned window. R6 raises
and finishes painting the owned window before the unchanged patterned-board
and twelve-change/final-cycle checks. Both baseline and candidate use this
identical setup; force-paint completion is outside the measured input interval.
R6 exits0 in all six cases with normal launcher exit0:

| Image set | Ordinary median | Sampled median | Private median |
| --- | --- | --- | --- |
| Published P2 | 379.627ms | 293.202ms | 31.905ms |
| Composed079/080 | 200.988ms | 199.250ms | 29.611ms |

These are posted-input-to-pixel measurements, not physical mouse latency.
The ordinary first input is an outlier in both sets:2190ms versus598ms;
retain it in the mean/tail rather than dropping warmup. The paired ordinary
median improvement is1.89x, not the earlier worker-only2.18x under a different
run. A commentary estimate of approximately190ms preceded the completed
summary; the exact candidate median is200.988ms. Runtime images remain local.

The next finite081 group covers the same allocated view's field-write and
reference-read closure. Source audit traces original ccpusas4.c
bios_write_double and c_tlb.c xtrn2phy: original access request is PG_W,
PG_U when CPL!=3, plus accessed/dirty handling in VM. Reuse that exact service
and c_GetPhyAdd for immediate same-page WND field copies, retaining original
scalar stores on failed translation, non-fitting spans or active descriptor
diagnostics. Do not use read-only c_GetLinAdd to write or introduce a new page
mapper/cache. The allocated nt_mem RAM is not VGA/device memory; no alias
survives its synchronous copy or crosses a callback/native boundary.

Only the existing rectangle/style fields and four relation fields are written;
other original WND data remains untouched. The collector's existing allocated
WND references use the same bounded read helper without changing reachability,
sort/free/retry rules. No freshness reduction or external observation policy
change is selected. The original service is directly available behind the
existing adapter, so no mirror intrusion or new behavior engine is needed.
ADAPTER-WOW-081 is registered before source implementation; added mirror and
guest diff is0. Actual CCPU tests add both fast/trace-fallback refresh, exact
native rectangles/styles and sentinels in four untouched private fields;
existing wrong-directory, graph rollback, retained generation/reuse/free
failures remain strict. Group compilation/lifecycle and paired timing are
required before any publication claim.

081 field-build-r1 links both images and passes the full079 supplements.
Its window-borrow executable exits2 with empty output before run_fixture:
the limited sandbox denies CreateDesktop in main, not a removed-message
assertion failure. The high-level verifier's missing-marker error alone was
misleading. Field-lifetime-r1 repeats that exact binary with required desktop
permission, without source/assertion/budget changes: complete window-borrow
exits0. Formal fresh reclaim/graph groups also exit0; frozen sources and
linked outputs are captured. Later diagnostic launches use that permission.

Field-performance-r1 refuses the failed original prerequisite and starts no
runtime. Field-performance-r2 requires the passing complete lifetime group;
all six timing/normal-exit cases pass. Candidate NTVDM is
8171A66D17AC51420CB58299675DD654EED9A64947C3E3597C1B2FE1C47584BA,
WOW32 BB7FE03A705D2E23B69299F8DB67262D7C062EDBA25CF1B5C61DE297BD00BFDD.
Paired ordinary medians are294.484ms P2 and221.220ms candidate; sampled
296.948ms versus94.796ms; private30.011ms versus32.359ms. This is substantial
run/desktop variability, not proof that081 by itself improves on080's201ms
ordinary measurement. Keep correct bounded mechanics without claiming an
unsupported incremental speedup. All69 candidate active sampled PCs fall in
win32u; suspension/context sampling has syscall bias and is not exclusive CPU
profiling or proof of a specific native function's self time.

Actual extension-guest-r1 passes the unchanged default message group in all3
styles. Its extension-Off case fails a test ordering assumption: MSGEXT.BIN
actually exists and records errors0, both real callbacks447A0079, correct
receiver178F task and native thread, both original yield calls and invalid-size
rejection. Sender then tests its acknowledgement before the receiver's posted
475 has completed. Original WOWDirectedYield16 merely calls pfnDirectedYield;
it is not a posted-message completion barrier. The independent probe now
waits through original GetMessage for an explicit receiver completion478,
which follows the existing synchronous476 acknowledgement, retaining exact
callback/count/identity/retirement assertions. No runtime change or scheduling
waiver is made for that failure. Extension-guest-r2 actual Off passes with
errors0, both callbacks447A0079 and correct final retirement. R3 repeats the
default and extension groups across all3 policies with this explicit protocol.

082 follows the already approved route-E contract, not a new compatibility
choice. Source project_graph currently re-queries every represented foreign
window even though CURRENT/route E permit its committed consistent snapshot.
Reuse only complete, live, nonzero-foreign-PID rows in the one page-domain
view. Local PID rows, cold rows, explicit roots/typed payloads and new edge
targets still capture at the current boundary. IsWindow and full identities
still withdraw dead observations; structural normalization, replacement,
retained readers and atomic commit are unchanged. No second cache, dirty flag,
timer or private native state is added. Original shared-memory USER cannot be
used as a modern cross-process state source; this is the smallest existing
adapter application of the already accepted external-observation boundary.
Registry row ADAPTER-WOW-082 precedes implementation; mirror/guest delta0.

The actual cross-process hold fixture changes native help-ID via the public
SetWindowContextHelpId service, proves native authority changed while local
publication keeps the committed external row, then explicitly discovers that
external full identity and requires its new ID. Existing32 local updates,
dead-native copied commit, stale-edge normalization, generation reuse and
retirement remain. New marker WOW_EXTERNAL_REFRESH_BOUNDARY_CCPU must be0 in
the full graph verifier. Build waits for the earlier real-message cohort to
finish before changing mutable images; timing waits for all lifetime gates.
No082 acceptance result or publication is inferred from its source.

Extension-guest-r3 completes all6 actual default/extension style cases with
normal retirement. External-view-build-r1 passes its original-body/CCPU
borrow/reclaim/graph checks, including the initial external snapshot marker.
Its performance-r1 passes six timing/normal-exit cases: ordinary279.716ms P2
versus133.558ms candidate; sampled299.854ms versus132.133ms; private32.076ms
versus31.830ms. That is an intermediate082 input: source review identifies
explicit foreign native mutation as another required root, not optional
desktop observation. It cannot be delivered before this dependency composes.

The existing native-call carrier now retains the explicit operand's known full
HWND on entry, only when valid, then includes that identity with returned and
typed roots on return. Invalid input keeps its native failure/result/error;
dead full operands are not revived. The call-local value is not a new registry
or native lifetime pin. The cross-process fixture additionally binds the real
provider native-call boundary and requires SetWindowContextHelpId's foreign
target to become visible immediately without a separate discovery. It uses
the existing fixture's controlled resource task owner, not an actual Win16
task-termination or mixed-process guest acceptance claim. The strict marker
adds native_mutation=1.

External-view-build-r2 passes the entire composed worker/provider/079 units
and actual CCPU borrow, reclaim and graph groups, including this final marker.
Frozen worker8171A66D17AC51420CB58299675DD654EED9A64947C3E3597C1B2FE1C47584BA
and WOW32 5AA564051EE87A3815FC03A751F06A4FAD57A53B44C499733A9F0FF0ED63EB6D
compose082's complete known-operand boundary. Performance-r2 passes all six
cases with normal launcher exits and12 actual changes/restoration per case:

| Final paired inputs | Ordinary median | Sampled median | Private median |
| --- | --- | --- | --- |
| Published P2 | 299.871ms | 296.888ms | 31.691ms |
| Final079/080/081/082 | 131.725ms | 131.884ms | 32.257ms |

This is2.28x paired ordinary improvement, still about4x the private floor.
It is not a claim of fully eliminated stutter. Existing original-body/native/
actual guest/application/DOS/modal/logon gates are now queued serially in
mine-release-acceptance-r1 after these completed lifetime/performance gates.
No source changes may invalidate its frozen build. Publication/review/main
commit/push are still pending; installed/assets P2 remains unchanged.

Read-only follow-up, not another admitted code expansion during these gates:
CCPU c_GetPhyAdd enters the EMS-alias translator even when aliases is empty;
that translator runs session_valid, including a binding-count interlocked
read and lease-shape validation, before the empty traversal. The physical
page resolver itself is indexed by page, not a per-allocation linear search;
reject the initial quadratic-physical-map hypothesis. An empty-alias early
negative may be worthwhile only after proving the same output/null/invalid-
owner and alias lifecycle behavior. No change, measurement or speedup is
claimed for that lead, and it does not expand or delay this frozen delivery.

Second-role prepublication source review covers the actual staged079/080/081/
082 diff, not only the green examples.079's per-TU aliases reach the retained
original generic/MM bodies and native loader leaves; same-unit wkgthunk has
no additional calls to its renamed Yield exports. The CallBack16 definition
seam preserves its body/ASM/frame and propagates only denied admission at the
two public entries. The narrow native-call header removes duplicate unguarded
original declarations without changing the existing fields' meanings.
080/081 aliases are immediate, same-page allocated-view operations under
existing domain/session/CR3/lifetime prerequisites, never arbitrary guest RAM
or a read mapping used to write. Trace/mapping fallback and untouched fields
remain explicit.082 uses actual captured PID, not cleanup enrollment, to keep
all worker-local objects synchronous; explicit known target/returned/payload
identities refresh separately from optional foreign observations. IsWindow
does not pin a native lifetime or promise future global freshness. Full
structural preparation/commit/retention and native result/error restoration
remain unchanged. No alternate message transport, scheduler, resource-state
authority, permanent cache, helper HWND or guest-image edit is introduced.

The fresh selected-source audit is278 files:192 exact,86 registered content
differences,222 compile owners,0 gaps. This is the selected total, not a claim
that343 adds86 differences.343 adds one registered host-source exception at
three sites in an already divergent wcall16.c;080/081/082 add0 mirror files or
guest diff. Controlled-scope/MM link supplements are not actual guest proof;
the separate six real default/extension cases and retained floors are required.
Actual MM arguments/zero/directory/exception evidence does not close the whole
MMSYSTEM16 family. Remaining S4 M1--M6, S5 termination and WRITE editing/save/
normal-exit frontiers remain open; this performance/native-extension delivery
cannot be substituted for full WOW recovery.

#### P3 verified runtime delivery

Mine-release-acceptance-r1 exits0 after the frozen external-view-build-r2
inputs: focused task-order/native identity/reply/seen, selected-source audit,
fresh native20, both-width style boundary, original30, resource/menu guards,
all12 actual S3 guest groups, all6 real default/extension message groups,
WINMINE/SOL gameplay and normal close under Off/Win16/All, WRITE's same four
startup samples per policy, Console17 and Window17, native modal3 and whole-
logon gameplay3. WRITE editing/save/full exit and remaining S4 families are
still not proved. The standalone pure-native modal reference r4 is reused
only after its source/observer/executable hashes match; it has no WOW/worker
dependency. These checks do not certify physical mouse input or every pixel.

Mine-published-r1 backs up the prior coherent set and publishes the tested13
to assets/release and O:/winnt/system32. Only NTVDM and WOW32 image hashes
change; all13 and the manifest are verified. It requires the exact timed
candidate, actual extension input images/original guests, modal image sets
and frozen source/build identities, not an old passing cohort. Stopped-process
list is empty: no existing project/user session was closed. Original guest,
user configuration and assets/winnt.zip remain unchanged. Mine-release-
delivery-r1 then passes deployed both-width style and installed whole-logon
WINMINE gameplay/normal exit in all3 policies, rechecks both complete release
destinations, and exits0; rollback was not needed. The recoverable P2 set is
retained in the publication backups.

Current NTVDM is8171A66D17AC51420CB58299675DD654EED9A64947C3E3597C1B2FE1C47584BA;
WOW32 is5AA564051EE87A3815FC03A751F06A4FAD57A53B44C499733A9F0FF0ED63EB6D.
The paired131.725ms ordinary median remains a partial responsiveness result,
not a claim that all stutter or all WOW execution is fixed. No earlier failed
probe is relabelled as a pass. The unlocated older common-dialog risk,
unskinned-menu animation TODO and immutable large-environment COMMAND limit
remain explicit. This bounded P3 hands off source, reproducible tests,
registered mirror/adapter exceptions and matching release assets together;
S4, S5, S6 and the full owner goal stay open.

#### After P3: inactive EMS-alias service

Previous goal turn is progress:5133c97f2 commits/pushes the full verified P3
and leaves main/origin clean, with all13 installed/release hashes matched.
The full WOW goal remains open. Subsequent work starts from that baseline,
not an unfinished or mixed package.

Source chain ccpusas4.c:c_GetPhyAdd -> existing097 alias translator -> normal/
external physical lookup executes on every physical access. The translator
initializes its output to the input address, validates the whole session
(including an interlocked binding-count read and lease-shape checks), then
walks aliases even when that list is NULL. session_thread_current itself is
only TLS; no candidate cache or session-validation removal is selected.
The actual physical page resolver is indexed by page; do not incorrectly
attribute this cost to a per-allocation linear physical-page search.

The read-only diagnostic adds one optional native DWORD RVA to the existing
PC sampler, resolved from one unambiguous symbol in the exact matching worker
MAP. No remote writes or private USER decoding. Both map hashes and images
are checked/captured. The observation precedes statistical thread sampling;
its QPC must be after the actual patterned board is ready and before the last
of12 measured inputs, not at process startup/retirement. Presence-r1 exits0
for ordinary/sampled/private timing and normal close. P3 worker8171 remains
unchanged: native _aliases RVA002F3BE4, observed address00553BE4,
value00000000, QPC1134071841221; ready1134069295157,
last input1134081095790, frequency10000000. This is one read during that
actual input interval, not an atomic global-lifetime snapshot. The unchanged
P3 ordinary timing varies with desktop conditions; this diagnostic is not
evidence of another runtime speedup.

083 is the finite early-negative profile: after preserving the same default
output, an empty authoritative alias list returns the already inevitable0
without session_valid. Nonempty owner/session/range checks and map/remap/
unmap/free/teardown are unchanged. Other range APIs keep their existing
validation/status contracts; no broad mapping rewrite is selected. The
existing original host_map_page/host_unmap_page -> VdmMapDosMemory seam and
original CCPU fallback remain. Direct NT4 kernel VDM physical aliasing is not
a composable modern host service; the already registered source-shaped
adapter remains the first usable boundary. No additional original import,
mirror intrusion, generic pointer manager, enabled cache, guest change or
new behavior engine is needed. ADAPTER-WOW-083 is registered before editing.

Access-family review: scalar reads/writes, RMW, string moves, stack and fetch/
control all retain the identical original c_GetPhyAdd path when translation
returns0; the physical address and subsequent external/normal lookup are
unchanged. No opcode, access width, read/write permission, A/D handling,
interrupt, timing calibration or CPU profile setting changes. An active EMS
list still reaches every original check. Existing same-owner CPU/ICA mapping
serialization is unchanged; session_valid's interlocked counter read is not
an alias publication protocol. No claim of arbitrary concurrent unsynchronised
same-owner mapping visibility is introduced.

The complete supplementary fixture retains the earlier actual adapter/
session physical-observation matrix, then covers empty/unbound/invalid/null,
first/last/one-past addresses, nonempty unbound/different owner, two owners,
remap, teardown removing only its owner, final empty state and LastError/errno.
It links the selected production object/archive, never a successful mock.
Baseline-r1 passes all of these against the untouched P3 object; its2,000,000
empty calls take28225.100us (14.113ns/call), invalid results0. Exact object,
archive, source copy, compiler/graph and fixture inputs are retained below
build/. This microbenchmark is not CCPU/guest or overall performance proof.
Candidate verification must additionally run original external-memory/SAS/
EMS leases, real independent EMS COM, copied-view lifetimes and the retained
every-P WOW/DOS/application/modal/logon and paired input gates. No release or
candidate speedup is claimed yet; P3 remains installed.

The real EMS control initially fails on unchanged P3 with EMS NOT INSTALLED,
not a candidate regression. Original nt_pif.h defaults EMS limit/request to0;
config.c obtains the DOS allocation from PIFEMSMemSize, while WOW deliberately
uses0. Reuse tests/observation/ems_pif_builder.c to request1MiB in a private
E30.PIF/E30.NT/E30AUTO.NT profile. The existing memory-transition runner uses
SUBST and foreground tools, so the new observer runner reuses its producer,
not that obsolete orchestration. No installed profile or original guest is
changed. Control-r2 records sandbox stage denial; r3 records a compiler-driver
invocation error before any guest execution. Neither is a runtime verdict.
Control-r4 passes the actual original INT67 map/switch/alias/unmap/move/
exchange/window/free COM marker and normal launcher exit0. Fresh build-r2
refreezes the corrected runner and passes the production-object physical
matrix, original external-memory/SAS/EMS lease group, window borrowing,
reclaim and graph snapshots; performance-r2 and complete acceptance remain
required before publication. Preserve the exact P3 baseline maps from
build-r1/before rather than substituting mutable candidate maps.

Performance-r2 completes both versions' real EMS controls and all six actual
WINMINE cases with12 pixel changes/final board restore/normal launcher exit.
P3 ->083 medians: ordinary81.7935 ->74.1595ms; sampled65.3175 ->65.6155ms;
private31.478 ->31.989ms. Candidate service fixture is5.032ns/call versus
baseline14.113ns, invalid results0. The ordinary difference alone is not proof
of a stable end-to-end gain: the other comparisons are essentially flat and
desktop conditions vary.083 is a correct minimal negative-path improvement,
not the principal stutter fix. Full acceptance-r1 starts only after this
successful group and is still pending; installed/assets remain P3. Quality-r1
passes scripts/diff/document governance before this evidence append; final
documentation requires a fresh quality gate before delivery.

The next source lead remains repeated owned-view projection work, not guest
CPU pacing: collect_window_view counts and copies across the full65536-entry
identity table, copy_window_observation copies each retained WND/CLS, and
graph validation allocates its full slot index per transaction. Existing P3
active-input samples hit these functions; nearest-PC attribution is not an
exclusive-CPU or causal proof. Audit their entire publication/borrow/reentry/
retirement closure before selecting another finite optimization. No cache,
stale local view, identity truncation or validation removal is approved here.

##### Continuing source review while083 acceptance runs

Previous goal turn is progress (real EMS profile/control, complete composed
build and paired input evidence), not a blocked wait. This turn reobserves
the same live owned acceptance session25825; no mutable rebuild is started.

copy_window_observation calls window_borrow_link(backing), a linear list search,
for every copied published row. Thus a complete view repeats backing searches
quadratically, in addition to its fixed slot scans. The source-shaped original
shared-desktop reader has no counterpart for this project-owned metadata
search; no additional original import solves a duplicate adapter traversal.
page_domain_prepare_window creates the existing lifetime record before every
ordinary/replacement publication; page_domain_commit_window exposes that
backing only afterward. Retirement first clears the slot, then marks the same
record retired; final free unlinks it. Desktop backing is separate and can
lack a record. Pending/replaced/retired records therefore must never be
enumerated as live merely because they remain in window_borrows.

A finite next design is to use the existing list only as an iterator, with
the current full-handle/backing table still deciding publication. Preserve
the current table-derived required count/capacity gate; copy desktop explicitly,
then only exact currently published backing generations, passing each
already-known metadata record to the row copier. Sort resulting rows by slot
to preserve the existing ascending order and reject any count disagreement.
This removes repeated list lookup without another registry/cache, lifetime
owner, relaxed identity or native freshness change. Required proof includes
sparse/high slots, desktop without/with metadata, unpublished preparation,
retired pinned old generation beside a replacement, row order/count/capacity,
rollback and current metadata. No production edit or speed claim for this
design yet; finish083 delivery and audit all callers before admission.

Separate duplicate native captures are also source-visible: project_graph
captures every local retained row, then refresh_graph's roots resample managed
locals, and typed payloads can sample them again. Deduplication is not silently
selected: a required root's failure/liveness and cross-thread native mutation
must remain correct, not just its apparent field equality. Keep that question
separate from the purely owned-metadata traversal above.

#####083 acceptance refusal and inherited input-membership defect

empty-alias-acceptance-r1 terminates1: its native20 cohort reports19/20,
native-nested exits0xC0000374 after receiver-exit held0 and before held1.
No original30, actual guest, application or publication result may be inferred
from this aborted coordinator.083 has not been released or committed; installed
P3 is unchanged. Do not retry the whole cohort to replace this failure.

Reuse wow_exception_observer.c for a sealed-fixture control. Initial debug-r1
renames the fixture to run16.exe and causes a separate CopyFileA self-image
sharing violation at fixture line1132; retain this invalid probe outcome.
The new explicit --exe mode preserves fixture.exe and the fixture's separate
run16.exe self-copy child assertion. Exception handling/disposition stays
unchanged; heap-corruption first chance is now logged as well as second chance.
The tracked verify-native-fixture-exceptions.ps1 successor requires actual root
process exit plus all lifecycle/held0/held1 markers, not debugger exit0.

Debug-r2 runs8 sealed P3 fixture trials and8 candidate trials. P3 fails heap
corruption on rounds2/8 (2 of8); candidate on3/7/8 (3 of8). Other trials finish
the unchanged lifecycle marker errors0. These are debugger-perturbed native
fixture frequencies, not population probabilities or actual guest proof.
P3's image/MAP is from mine-release-foundations-r1-guest-floors/native-core/
native-nested; candidate from empty-alias-foundations-r1-guest-floors. Both
copies are hash-recorded, not recompiled against current source. This proves
the defect predates083; it does not waive the failed gate.

Both captured P3 failures place the last fixture frame at RVA0xD8CE, inside
destroy_queue (start RVA0xD8B0) calling HeapFree. Source/blame links the unsafe
embedded queue/ctype sharing and destroy callback to79051a8ada (2026-09-21),
not the one-OR083 change. init_style assigns both pcti and pq to task->queue;
RecalcThreadAttachment/Recalc2 shares a pqAttach among16-bit threads; retirement
frees the containing task without releasing or republishing queue membership.
A surviving attachment can then reference a freed embedded field, and the
destroy callback may pass that interior pointer to HeapFree. The callback
also omits original DestroyQueue's reference decrement. Do not merely ignore
an invalid free: that retains stale cross-task membership.

Pinned original queue.c distinguishes embedded THREADINFO.cti from separately
allocated Q: lines991,1286--1300 establish those owners; DestroyQueue1511ff
decrements cThreads and defers physical FreeQueue until cLockCount permits it;
thread destruction/deletion releases queue references before final THREADINFO
free. Original input.c RecalcThreadAttachment is only preparation: ReattachThreads
5701ff consumes/clears pqAttach, AttachToQueue5493ff transfers membership and
releases old Q references. Current reattach calls only preparation and reports
success. The working plan already names this exact integrated task-init/input
membership gap; it is now a demonstrated S4 foundation blocker, not a newly
invented performance feature or an S5 deferral.

Next bounded source group must reconcile CTI versus Q, original preparation/
commit, shared-reference and queue/task retirement plus init rollback together.
Trace the real native input/resource boundary before selecting any original
subset/service adapter; full NT4 RIT/desktop machinery is not imported just to
satisfy an auxiliary symbol. Required evidence includes two/three live tasks,
every retirement order, failure before/after grouping preparation, surviving
task reuse, original refcounts and zero dangling/double/interior free, plus
the same native-nested and actual guest/application floors. The owned-view
optimization design above waits; no production quick-fix or waiver is selected.

#####084 finite implementation selection

The previous turn is progress: it reproduces the identical failure on sealed
P3, captures the actual failing release frame, and ties it to the prior input
membership inventory. All its background coordinators are terminal; no live
run is restarted or mutable build changed merely because of an observation
timeout. The current group restores native input membership, not performance
alone. Source inputs also include the retained S2 boundary audit's Input
attachment repair and Native input boundary sections; do not rediscover its
middle-exit and duplicate-reference counterexamples through ad-hoc tests.

Source selection: keep unchanged AddAttachment/Recalc2 for the actual explicit
edge and same-process TIF_16BIT fixed point. Modern USER owns actual input
queues. There is no public native Q pointer/cThreads snapshot; retaining fake
native Q storage or restoring full RIT/PWND/message redistribution would
violate the ownership boundary. RecalcThreadAttachment's private old-Q reuse
strategy is not a necessary observable grouping algorithm; its production
facade is replaced, not falsely certified as real native attachment. The
complete component calculation instead uses preallocated call-local labels
and original AddAttachment/Recalc2, then erases every pqAttach pointer before
native calls. No retained source-shaped Q is another system-resource owner.
Original allocation-path unit tests remain original tests, not a claim that
their private allocator owns real modern input queues.

wow_input_group_lifetime.c owns only native attachment increments acquired by
this provider. Desired component pairs are fully connected: surviving members
retain explicit edges regardless of which member departs. Existing desired
edges are not attached again; only this owner's obsolete/departing edges are
detached. Application-owned duplicate attachments remain untouched. Plans and
all potential missing edge records are allocated before original InitTask;
its reattach callback uses the original function's actual newly set16-bit
flags. Thus ordinary memory/identity preflight failure returns through the
existing pre-init retry path, not an ignored reattach FALSE after TDB creation.
Original scheduler insertion and committed startup-notification retry stay
unchanged. No allocation/native mutation occurs during the pure fixed point.

The real thread ID and query/synchronization handle belong to the existing
task carrier. Plan validation rejects stale/dead/duplicate members, nonmember
explicit edges and orphaned owned links before entering original InitTask.
No private ETHREAD interpretation or new task table is introduced. Retire
owned links before freeing the carrier/closing its thread handle. A signaled
old native thread capability means USER has already removed its links; never
target a recycled numeric TID in that case. Native attach/detach failure uses
the established noncontinuable adaptation-failure owner, not partial success,
input replay or restoration of a guessed keyboard/focus snapshot. This is
not a claim that public USER offers an atomic rollback of multiple operations.

Native service contract is public AttachThreadInput (Microsoft Learn,
https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-attachthreadinput,
read2026-10-18). Original no-callback membership computation is retained;
modern native input/focus/capture behavior is supplied by USER. Historical
private-desktop focus/capture/caret and callback-count cases remain required
service gates, not universal host-version equivalence proof. SendMessageTimeout
documentation describes a same-message-queue fast path; sharing input state
does not by itself prove that separate native message threads take that
path. Existing nested timeout assertions are not preemptively weakened.

Deletion set: embedded shared queue, zero-count HeapFree substitute and
recalc-only success; retained per-thread queue masks are now explicitly client
observations. Reuse the existing ConnectIfNecessary native leaf through a
narrow header; it already handles IsGUIThread(TRUE)'s nonzero error8 result.
No duplicate GUI-thread conversion implementation is added. Actual source
changes are WOW32's lifecycle/native-resource group, its native identity view,
and generator selection. No additional original mirror/guest diff or import.
083 remains pending in the same undelivered worktree, not discarded or
published in isolation. No084 build/result is claimed before grouped tests.

Supplementary verification source links original input.c and includes the
current production input owner with only actual-call counters and fault
injection. Private native threads prove real keyboard sharing, origin/middle/
last departure, newcomer, repeated idempotent membership, independent client
masks, no escaped label pointer, three allocation cutpoints and external
duplicate reference preservation. A separate injected third native attach
refuses after an earlier successful new attach; its test failure callback is
explicitly not the product fail-fast implementation. The existing full native
lifecycle/nested callback and actual guest/message/application/publication
gates remain mandatory. Source/tool identities and process deadlines are
recorded by verify-wow-input-lifetime.ps1; no computer use or guest changes.

#####084 first composition and native-control correction

input-lifetime-build-r1 links the real worker/provider and passes a freshly
rebuilt native-extension/original-frame supplement. It then refuses publication
at the new input test: all five positive keyboard-sharing checks read0, while
native attach/detach returnTRUE and final allocated storage is0. Diagnostic-r2
logs exact original components (all expected members in group1) and successful
real native pair operations. Diagnostic-r3 adds the retained probe's explicit
PeekMessage queue initialization and a pre-group native control: SetKeyboardState
then GetKeyboardState already reports own_Z=0 before any new group function.
This separates the nonactive-private-desktop observation prerequisite from
the production membership algorithm. All three failed roots remain non-passes.

The keyboard witness now follows the retained wow_native_input_group_fixture
scope: only newly created fixture threads' USER tables on their current
desktop, no visible windows, SendInput, external thread attachment or global
keyboard configuration. --private-desktop retains the failed-control path;
it is not silently passed or removed. Diagnostic-r4 obtains own_Z=128 before
grouping and passes every original-component/real-native membership, rejoin,
origin/middle/last departure, external-reference, allocation and lifetime
assertion, live allocations0. The injected third attach reaches exactly
INPUT_TERMINAL/input-native-attach/error5/attaches3 after an earlier successful
new native connection; its test callback exits91 as documented.

Second-role review preserves LastError explicitly on successful plan prepare,
commit, disposal and retirement; supplementary assertions now independently
set/check each sentinel. Existing ConnectIfNecessary conversion is reused,
including its explicit check after IsGUIThread(TRUE), rather than duplicating
the incorrect nonzero-success assumption. input-lifetime-build-r2 refreezes
this complete source/test group; worker/provider link and extension supplements
pass, and its input-lifetime group passes with the new LastError assertions.
Borrow/reclaim/graph, actual nested callback and notification groups remain
running; no final build/guest/publication verdict follows from the earlier
service pass.

The tracked verify-native-input-services.ps1 reuses the three existing source
fixtures instead of another custom input/focus model. input-native-services-r1
passes original explicit/implicit fixed-point membership and allocation failure,
native duplicate-reference and middle-exit/rejoin counterexamples, plus the
invisible private-desktop focus/capture/caret witness. The latter reports
callbacks0 and equal merged state for both argument orders, with complete
thread/desktop cleanup. This does not prove every NT4 foreground winner or
universal callback absence on untested host paths. Service fixture source/
tool hashes and real owned process deadlines are retained under build/.

input-lifetime-performance-r1 and input-lifetime-acceptance-r1 are owned
dependency-waiting supervisors, not passing tests: they start actual package
execution only after the exact live composed build and then paired EMS/input
groups finish0. The every-P native20/original30, actual guests, three-app/DOS/
modal/logon and coherent13 publication obligations remain unchanged. No
additional mirror or guest changes, publication, commit or push yet.

The refreshed composed build-r2 is now terminal0. Actual native-nested emits
both held0/held1 receiver-exit retirement markers, all callback-local/native
exception, destroyed-target and timeout ownership markers, and final lifecycle
errors0. start-notification, CCPU borrow/reclaim/graph, original external-memory,
physical alias and fresh extension/original-frame groups also pass. The old
timeout assertions were retained unchanged; no hypothesis about attached
input queues was used to lower their bar.

Paired performance-r1 is terminal0: P3 ->083/084 median ordinary61.961 ->
61.073ms; sampled61.836 ->61.460ms; private30.3645 ->30.347ms. Both real EMS
controls and all six12-change/final-restore/normal-exit input groups pass.
This is essentially flat, not a meaningful stutter solution. Peak/tail values
from12 inputs do not establish a stable tail improvement, and background
diagnostics/system load are not a controlled CPU benchmark. Retain correctness
without attributing latency changes to queue repair. Full acceptance is now
running its retained foundations; the sealed-fixture repeat diagnostic is
separately owned. No acceptance or publication result is inferred yet.

The strict sealed-fixture repeat diagnostic completes all16 trials: P3 has
5 complete/3 heap-corruption exits;084 candidate has8 complete/0 heap failures,
including held0/held1 normal retirement each time. Overall diagnostic exit1
is retained because its baseline really fails; do not relabel that root as
a passing acceptance group. Candidate rows/identities and unchanged mandatory
markers independently show8 successful repetitions. This supports the source
lifetime correction, not a statistical universal guarantee or full product
closure. The main every-P cohort still requires its own full native20 result.

### Execution-frame ownership review during grouped acceptance

Execution-build-r1 and its r2 no-work refresh exit0. Acceptance-r2 passes
original task-order/persistent eligibility and native identity/reply/seen
before starting the retained full native cohort. That cohort exposes a
source-level ownership mismatch in the staged077 implementation: resource
fixtures legitimately bind an original THREADINFO projection without creating
another lifecycle-private task. Native begin now requires current_task from
that private list and terminates before the removed-message assertion. The
retained rows remain failures, not omitted or weakened assertions.

The existing runtime_bind contract accepts a bound original thread independently
of private lifecycle allocation. Execution readiness describes that bound
thread's stack, not its allocator. Therefore the finite correction is to keep
the single borrowed LIFO top on the existing wow_task_order_thread carrier.
Both real original task selection and native/callback scopes consume that same
field directly. Remove the added host readiness vtable leaf, lifecycle-private
duplicate pointer and current_task lookups introduced for this purpose. No
second task record, test-only production branch or fabricated InitTask is
needed. Real task retirement still withdraws the borrowed top only after all
refusing cleanup edges, under the existing owner lock. Native resource-only
bindings without an original pwpi own no cooperative execution to release;
original TIF_16BIT task bindings retain the actual scheduler prerequisites.

This is the same077/123 registered execution boundary and unchanged five
original selection operands, not another transport, callback policy or semantic
waiver. The current frozen cohort completes before these source/test-input
edits; then the entire affected group is rebuilt and accepted together. No
failed candidate is published and actual Win16 M1--M6 proof remains required.

Acceptance-r2 finishes the retained20-case native cohort with10 failures and
10 passes; it stops the outer acceptance before original/guest floors, exit1.
All old evidence remains. Source review additionally identifies native-nested's
WM_APP+71 test as a terminal ExitThread from inside its callback, but its direct
runtime binding never sets production session_finish's terminal intent.
The fixture now supplies terminal_exit explicitly before unbinding; ordinary
active-frame unbind refusal remains required and unchanged. The model still
performs real thread exit during a native SendMessageTimeout and retains
receiver death, sender return/error/owner and retained-borrow assertions.

The whole correction is implemented after that terminal run: readiness is
directly on the bound original-thread carrier, the extra host vtable and
private-task pointer/search are removed, and the scheduler fixture drives the
actual shared readiness predicate rather than a mock host readiness leaf.
The original five selection conditions, counts, priority/task-lock algorithms
and modern message owner are unchanged. Full replacement-group verification
is pending; no passing result or new publication follows from these edits.

Execution-build-r3 passes with candidate WOW32
D53DE4E497F508C7A268CF8C6AD5330D8394F95B0177AEDC2665575F13C90C4B;
worker stays P22-identical. Acceptance-r3 passes19 of the20 retained native
rows, including native nested/receiver-terminal exit and original modal/style
prerequisite. Graph-negative alone rejects held=1 property/enumeration callback
locking. Source attribution is finite: that fixture intentionally has no
original TDB, so native_call_begin's old pre-InitTask early return skips the
lock-release that the replaced enumeration-local pair used to perform.
The newly shared execution call boundary must distinguish data locking from
the optional original task, not skip both because no TDB exists.

The correction now pairs caller-held data locking for every bound runtime;
only a committed original task gets a stack-readiness frame and scheduler
transition. Actual native return/unwind restores the entry lock state. Existing
graph-negative held=0/1, atom/string/signed-HWND/stop/exception/removal/invalid
assertions are retained. Queue creation uses this same common branch, deleting
its redundant local pre-InitTask lock-release/restore pair. There is no fabricated
task, skipped callback assertion or new original-mirror diff. Full rebuild and
whole-group acceptance are required again. The message-guest-group-r1
supervisor correctly refuses to start its runtime because acceptance-r3 failed.

Independent actual USER16 message inputs are now authored separately:
win16_message_probe.asm, wow_message_peer.c and verify-win16-message-probe.ps1.
They cover two real same-worker Win16 tasks, nested actual/native sends,
early/duplicate reply results, post versus received state, queue filtering,
no-remove/removal/thread post/quit, bidirectional text conversion and original
task retirement. The native peer records real PID and distinct thread identities;
it supplies no task/scheduler/converter double. These are unverified authored
inputs, not accepted capabilities. Cross-worker/external-native-process/bitness,
implicit focus, full converted structures, SMTO_BLOCK and pending receiver
death remain in the unchanged M1--M6 denominator. No guest or publication changes.

### Actual-message witness source/owner review

The new witness is one complete same-worker/native dependency group, not a
replacement queue test. MSGSEND uses original KERNEL WinExec to admit MSGRECV;
USER16 GetWindowTask records distinct16-bit identities. The loaded independent
MSGHOST peer checks both guest HWNDs belong to its actual worker PID, records
their distinct actual native threads and its third native thread, and uses
only native USER delivery. It never binds a fake WOW thread, supplies callback
conversion, reads private structures or calls the adapter scheduler itself.

| Fixed group | Actual selected chain exercised by these inputs | Still not established by these inputs |
| --- | --- | --- |
| M1 | USER16 SendMessage/PostMessage/PostAppMessage/GetMessage/PeekMessage/DispatchMessage -> original wumsg and message converters -> native USER. Repeat PM_NOREMOVE, one removal, exact filters, task post and filtered WM_QUIT. | Cross-worker and broader queue/order/external-native boundaries. |
| M2 | Original InSendMessage/ReplyMessage in actual16-bit window procedures, nested same-thread receive-state inheritance, first versus duplicate reply result and actual native peer sending back into the pending Win16 sender. | Implicit focus receiver, external native process and all mixed nesting forms. |
| M3 | Real two-task WinExec admission, original Yield inside reply processing, product native/callback scope and original taskman; pure native peer never becomes a WOW task. | Every lock/wait/timeout/death permutation in actual guest code. |
| M4 | WM_SETTEXT/WM_GETTEXT input/output through both Win16 task conversion and native-to-Win16 conversion; exact strings/results asserted. | Full structures, movement, timeouts and exception buffer-retirement closure. |
| M5 | Normal receiver window/task retirement, dead HWND failure, sender cleanup, exactly one original retirement record per actual16-bit identity, native peer thread/window exit. | Pending receiver death/cancellation and every failed converted call. |
| M6 | All three style policies against the coherent candidate on a private unswitched desktop, complete supported logon block, independently authored immutable inputs and normal launcher completion. | This bounded group does not replace retained S3/application/DOS/ordinary-desktop floors or the rest of S4. |

Input/lifecycle review: NE procedure thunks and classes retire after their
windows; the peer DLL is freed only after its native thread has exited. Test
reports contain result counters, task/native identities and environment shape,
not environment values. Compiler outputs stay under build; runtime probe copies
stay in the owned short-root stage's tests directory. Existing package cleanup
pins actual paths and identities; forced cleanup cannot establish a normal
exit. The run's retained limitations explicitly distinguish the known original
large-environment failure from complete-logon-block acceptance.

Execution-build-r4 exits0. Candidate WOW32 is
B56BFBD8D9705BF626104F0FDC142A4C4D2ACBD20125E7ABC8B82E969BD8779E;
NTVDM remains12DA2E8FCFFB35B724791F7356C1A9A87A5208442928C29127415AE8DC812705.
Acceptance-r4 is running in its owned hidden supervisor PID32500, with full
original/native/guest floors selected. Message-guest-group-r2 PID25984 waits
for that exact supervisor identity and success before starting its own serial
runtime group; build and probe source hashes are checked before/after.
Neither an active process nor these source-review conclusions is a test pass.

Acceptance-r4 subsequently passes the full20-case native/real-CCPU cohort and
both-width style boundary. The original30 boundary-fixture build then fails
LNK1120 only in wow_dialog_modal_entry_fixture: its whole wow_title_policy
object retains the unused ShellAbout/MessageBox native-call functions. This
does not indicate a production DLL link/runtime failure or a passed fixture.
New-T447S3CallBoundaryNinja now selects function-level COMDATs for that object
and OPT:REF for this isolated modal/context link. Used original modal/context
and all existing assertions remain; no fake successful native-call service,
test removal or production behavior change is introduced. Validate this whole
30-case fixture group and the remaining real-guest floors afresh. Reuse only
already-passing native/style rows whose exact source/artifact identities remain
unchanged; do not rerun them simply because a different fixture's link changed.

The source-led outgoing-call review also identifies a further finite M3/M4
dependency: original wkgthunk WK32ICallProc32 and wmmedia WMM32CallProc32 enter
caller-owned native code, while original wcall16 WOWCallback16/Ex and exported
WOWYield16/WOWDirectedYield16 form the return/yield directions. The original
wkgthunk comment explicitly relies on no16-bit memory movement during the
native thunk unless it calls back. Therefore blindly wrapping every extension
call as a descheduling native wait would change that original contract. Audit
the complete native-extension/callback/explicit-yield and pointer-fault return
closure together before choosing the smallest binding. This is already part
of fixed S4 execution/conversion scope, not new feature admission or authority
to alter original generic argument order, callpr32 stack cleanup or memory
movement. No extension-boundary replacement has been implemented or verified.

The function-level COMDAT/OPT:REF fixture-link attempt still reports the same
three unresolved native symbols (acceptance-r5, exit1), so that attempt is not
retained or described as successful dependency selection. The corrected unit
composition instead links wow_modal_native_unreached.c solely into the isolated
modal/context executable. Every entry immediately reports and exits97; it
cannot fake a native success or satisfy the unit if a native-dialog path is
actually invoked. This follows the unit's existing explicit original-manager/
execution doubles and claims no native acceptance. The actual native/CCPU and
guest gates still require the production owner, never these sentinels. The
new file is not selected by any of the old native20 compile/link commands or
the production manifest; all previous source/artifact hashes remain required
before reuse. No production source/image change is needed for this test-only
link repair. Complete original30 and fresh real-guest rows remain mandatory.

### Verified message baseline and finite identity/retirement correction

Acceptance-r6 completes original30 and all12 actual S3 guest groups, with
explicit hash-checked reuse of acceptance-r4's passing native20/style and
focused message/source rows. No failed row is reused. The production image
remains build-r4 B56B..., with no new publication. Message-guest-group-r4 fails
NASM before any runtime (% adjacent to512 parsed as a macro token); r5 corrects
that lexical error and runs the authored message group. The probe's DOS header
also has an extra zero word, placing40h in e_ovno rather than e_lfarlc. It still
loads as NE, but that authored input defect must be corrected, not retained.

Actual r5/Off sender phases0--3 have zero assertion errors: same-worker nested
Win16 messages, native peer both directions, first/duplicate reply results,
post/thread-post/no-remove/filter and text copyback execute. Receiver reaches
phase6 with zero errors, but its module cleanup rejects status6 and aborts the
worker (launcher0x42b). This is failed lifecycle acceptance, not a pass for the
whole group. GetWindowTask also reports927 (WOWEXEC fallback) rather than the
receiver's self-reported6031; sender identity is6079. The unchanged final
cross-record identity and retirement assertions remain mandatory.

Source trace identifies two already-present, but unconnected boundaries:

1. Original WU32UnregisterClass calls public UnregisterClassA, bypassing the
   already selected wow_class_client_unregister -> original ntcftxt.h ->
   original _UnregisterClass -> class storage owner. Native removal succeeds
   while its owned class-list/backing record remains; later module cleanup
   encounters that stale registration. Reconnect the existing original client,
   including its original captured-name/menu-free ordering; do not implement
   another removal algorithm or suppress cleanup failure.
2. Original WU32GetWindowTask calls GetWindowThreadProcessId on HWND32, which
   walias.h deliberately sign-extends from16 bits. Canonical winmgrc.c uses
   private ValidateHwnd/PTI/shared data unavailable on this host. The existing
   original handles.c GetFullUserHandle already supplies the missing alias
   input. Normalize there, then use native USER as the sole actual PID/TID
   authority; preserve ThreadProcID32toPTD and WOWEXEC fallback for real
   non-WOW windows. Do not substitute the lifecycle's task list.

Native-handle-contract-r1 is an independent owned x86 native-API diagnostic,
not a WOW capability pass: full HWND035311F6 returns TID16320/PID12624; unsigned
and signed short11F6 both return TID0/error1400 and leave the PID sentinel
cccccccc unchanged, although IsWindow accepts each. This directly verifies
the missing host input and argues against blanket rewriting of all HWND APIs.
Canonical winmgrc.c SHA256 is
26F0174E6A0DB53D942506CBDC12EFBFEBC34A943D780170E7BF9D95775514A4;
ntcftxt.h is E7F3D42CE8939BBE0B14EA34BC693107334C19F8B2D7862EC15B75A501D11DAB
at the pinned OpenNT revision. Its invalid ValidateHwnd branch returns before
writing PID too, so do not invent a failure-output overwrite.

Register ADAPTER-WOW-078 before implementation. First usable reuse is the
already selected complete class client and original full-handle algorithm
plus a finite public native query. Reintroducing NT4 PTI/shared native heap
for its trivial fast path would duplicate modern USER's real identity owner;
no imported algorithm is replaced by another task/identity registry. No
additional original/guest diff is needed. Pair original public unregister,
normal/module retirement and post-native failure with real USER16 identity
and native unsigned/signed/full/dead query assertions as one bounded group.

The078 source review also pairs the irreversible-release branch with an
owned negative child: register a native class through production class storage,
publish its real WND/class association, destroy the native WND while deliberately
retaining that test publication, then enter production native unregister.
The original page-domain owner must reject the stale class projection after
native removal; the existing worker-failure owner must terminate, never return
an ordinary retryable failure. The parent requires the actual child exit,
pre-entry event and production traces for both native-unregistered=1 and
release-class-after-native-unregister. No guest modification, production fault
hook or cleanup bypass is involved. Full/signed/unsigned live/dead thread-query
rows preserve native PID output, last error and inherited lock state. These
are source/assertion dispositions, not runtime passes until the fresh group
finishes. The old r4 package remains immutable and unpublishable as the current
source078 candidate.

### Native-extension boundary: resolved source composition

This is the next finite M3/M4 dependency group, not additional T scope and not
yet a runtime implementation. Pinned original hashes: wcall16.c
4875844ABFB8D22025480C14174481A194779D83759CE3A696E978B68C55A9F0;
wkgthunk.c787DFD9D5135B71CDFF6CA2BCC8145BD3EDF5E2EA26B7E9C87B14176F82EFEEC;
wmmedia.cFBC4695791937FEA487C91A0BA964E19625EBF4793C0123238100628FA95FD08.

| Complete entry family | Retained original owner | Required bounded seam and rejected alternative |
| --- | --- | --- |
| CallProc32W/ExW, multimedia native thunk | WK32ICallProc32 / WMM32CallProc32, original callpr32.asm argument copy, order, convention and stack recovery | Inline native execution scope retains inherited execution; only actual callbacks/yields can move guest memory. Per-unit entry renaming can preserve these bodies and the byte-exact assembly. A descheduling wait around these arbitrary callees violates the explicit original memory contract. |
| LoadLibraryEx32, FreeLibrary32, GetProcAddress32 | Existing wkgthunk bodies, including x87 fsave/frstor and pointer resolution/free order | Per-unit underlying native API aliases let the scope enter at the actual native call, inside the original FPU protection. Do not move scope work before the original fsave or replace the loader/argument conversion. |
| All original CallBack16 consumers, including same-unit memory helpers and public WOWCallback16/Ex | Original CallBack16 performs guest frame preparation, original CCPU reentry and return restoration; original public Ex argument validation remains | Acquire READY execution before original GETFRAMEPTR/CBFRAMEPTR, pair on exceptions and return. A definition-only rename plus one original-shaped adapter entry is smaller than wrapping20 same-unit public/internal callers. Whole-unit CallBack16 renaming alone is rejected: it also renames those20 calls and bypasses the adapter. Gating host_simulate is too late, after guest frame writes. |
| Explicit native WOWYield16 / WOWDirectedYield16 | Original WK32Yield / BlockWOWIdle / DirectedYield pair | READY callback scope encloses the original yield and restores the outer inline-native state. No second scheduler or invented runnable event. |
| Fault/resource reentry and teardown | Existing original force-segment callback and wow16cal.apfnWOW16Func; existing early resource-cleanup owner | Review the actual RET target, not a numeric range or a claim that every callback is a resource helper. Terminal application callbacks remain rejected; trusted memory/resource operations require the already-live early cleanup scope. Never make public WOWCallback16 an arbitrary-code exception during teardown. |

The guest-side wow16cal.asm table is read-only source evidence: it explicitly
distinguishes application procedure dispatch, foreground-idle/winsock hooks,
task exit, kernel memory/resource operations and ForceSegmentFault (IsBadReadPtr).
No guest build/edit is required. Existing callback guards outside CallBack16
remain necessary: window/dialog/message conversion can touch guest state before
reaching that central frame builder. This composition cannot remove those
earlier boundaries merely because a central guard is introduced.

Review must pair real native-extension callbacks/yields, argument/result and
error preservation, invalid public Ex arguments before guest entry, exceptional
return, cleanup refusal and real application floors. Existing MSGHOST's native
thread/message witnesses do not establish exported WOWCallback16 or explicit
native Yield coverage. These remain unverified, not inferred from successful
generic loading or a native-message pass. Exception registration and complete
group source/tests precede any next runtime build.

### Fresh078 build and native acceptance

Execution-build-r5 completes with WOW32
F3FD174A25B72EC17274979B6F24A1C5C5314A9E025BD233847356F9F45680F2;
NTVDM remains the published12DA... identity. Execution-acceptance-r7 runs the
entire fresh group without reusing r4 native results. Its native-core summary
records20 cases, zero failures. Graph-negative logs contain all four
WOW_WINDOW_THREAD_QUERY_CCPU held0/1 and dead0/1 rows, including full/unsigned16/
signed16/native-output assertions. The owned class-failure child records
ClassRetirement native-unregistered=1, then the exact
release-class-after-native-unregister status5023; the parent verifies
STATUS_ASSERTION_FAILURE c0000420 and the expected production traces.
This proves the deliberately stale-publication terminal boundary, not that
ordinary unregistration should fail.

The two-width style boundary also completes; original30, remaining real S3
guest floors and actual-message group-r6 are still pending at this checkpoint.
The latter pins supervisor43396/start2026-10-18T03:50:14.8492640Z and requires
success before starting. Product-gate-r1 pins message supervisor29848/
start2026-10-18T03:50:33.9078295Z, requires its success, then selects the retained
WINMINE/SOL/WRITE and Console17/Window17 floors. These are serial prerequisites,
not passing runtime claims. Execution-review-r2 passes documentation governance,
diff whitespace and selected PowerShell parser checks. No publication or new
commit is claimed; guest/profile/ZIP and the installed coherent13 remain intact.

Acceptance-r7 subsequently exits0: original30, resource guard, MENU and all12
actual S3 guest groups pass afresh, not via reused native evidence. Actual
message-guest-group-r6 also exits0 for Off/Win16/All. Each mode records sender
6079, peer lookup6031 and receiver self6031; all guest/native assertion counters
are zero, sender nested2, native received4, destroyed1 and observer exit0.
The runner separately requires exactly one original complete/status0 retirement
per actual task in the verified native worker. This corrects r5's927 fallback
and failed module retirement through the selected original owners; no assertion
or required phase was removed. WINMINE and SOL gameplay/normal close pass all3
policies in product-r1; WRITE startup, DOS and ordinary-logon groups are still
pending at this checkpoint. Full S4 external-process/cross-worker/implicit
message/converted-lifetime and native-extension obligations remain open.

### Final review: direct native-modal coverage and restored baseline

Product-r1 completes all6 WINMINE/SOL gameplay cases, WRITE's four startup
samples per policy and Console17/Window17 with zero Expected/Actual mismatches.
Ordinary complete-logon gameplay passes3 policies (window-ready5156/5875/5750ms,
then board-ready1032/1000/1234ms; phase timings, not a shell-to-first-frame or
performance-improvement claim). Delivery-r1 backs up and publishes coherent13,
hash-verifies both destinations and passes deployed-style plus all3 installed
complete-logon gameplay cases. Neither publication nor recovery closes an
existing owner session: both stopped-process inventories are empty.

The second-role review then identifies a missing direct assertion: changed
wow_title_message_box_a / wow_style_shell_about_a pair native execution scopes,
but original30's isolated modal unit explicitly cannot enter those native
paths. Add independently authored win16_native_modal_probe.asm and the native
dialog peer/runner. Actual USER16 MessageBox ordinal1 and SHELL16 ShellAbout22
must return IDOK/success1 into guest checkpoints0/1/2, then retire exactly once.
Sources are original wudlg.c::WU32MessageBox, wshell.c::WS32ShellAbout and
wow16/user/user.def, shell/library/shell.def; no original guest changes.

Native-modal-group-r1/Off fails: peer exits8 after one posted OK and a5-second
close deadline; guest report remains one14-byte phase0 record and observer
reports timeout0x53504354. The observer process exit0 is not guest success.
R2 adds visible/enabled/responsive prerequisites and fails peer6 (no ready
dialog in20sec). A pinned read-only snapshot before watchdog cleanup shows
the real MessageBox/OK and WOWEXEC windows; it does not prove their earlier
readiness or modal-loop progress. R3 aligns the new peer with established
gameplay's DESKTOP_WRITEOBJECTS/SetThreadDesktop protocol, logs actual ID1 and
enabled states, but still lacks a completed return. These are failed witness
runs, not accepted runtime rows or proof of a specific product root cause.

Restored-execution-r1 validates all27 recovery targets/backup hashes and
restores the exact previous coherent13/manifest at both destinations. Published
WOW32 is again3D2CEB931C6E43E097FA72185049B626332F346407A94A007ADDB7259411D3A9;
F3FD... remains only in the sealed candidate. No commit/push or guest/profile/
ZIP change follows the rejected supplemental gate. The independent
native_modal_control.c now exercises the same two public native APIs, captions,
buttons and desktop/peer protocol without WOW/style/adapter code; its owned
run must distinguish native-host/probe prerequisites from the actual adaptation
before any runtime correction is selected. Do not weaken the guest return or
retirement assertions, or infer a cause from merely seeing the dialog.

### P2 native-modal proof, final delivery and second-role review

Native-control-r1/2 are failed probe-protocol controls, not proof that native
USER is broken: this host's MessageBox does not expose a direct IDOK HWND and
does not honor the assumed internal WM_COMMAND path. Native-control-r3 uses
the public WM_CLOSE route and records MessageBox=1/ShellAbout=1 plus normal
exit; native-modal-group-r4 uses exactly that verified protocol on the sealed
WOW candidate. All3 policies pass: actual task6079 records phases0/1/2,
message=1/about=1/errors=0, both real dialogs close in the same actual native
thread, launcher returns0 and original task retirement completes exactly once.
No product patch was needed for these probe failures. This proves native modal
entry/guest continuation/return and retirement, not physical OK-button input,
all native control layouts or remaining mixed-message families.

The reproducible pure-native reference is now
tests/observation/verify-native-modal-control.ps1 with mandatory Observer,
CompilerDriver and fresh build-owned BuildRoot. Native-control-r4 passes that
committed-form runner, explicitly x86/MT/O2; native_modal_control.c and the
shared wow_native_modal_probe.c never load WOW/style. The actual guest runner
is tests/observation/verify-win16-native-modal-probe.ps1 with PackageRoot,
Observer, CompilerDriver, the actual-message group's LogonHelper assembly,
fresh BuildRoot and WindowStyle Off/Win16/All. Each root records exact inputs,
image hashes, real owned processes/deadlines, guest returns and retirement.
Both runners keep failed native or observer outcomes distinct from success.

Delivery-r2 requires the added direct-modal and native-control gates, repeats
coherent13 backup/publication/hash checks, then passes deployed style and all3
installed complete-logon gameplay cases. Both destinations again contain
WOW32 F3FD174A25B72EC17274979B6F24A1C5C5314A9E025BD233847356F9F45680F2;
the other12 images retain their previous hashes. No existing project session
is closed. This supersedes the temporary recovery, not its retained evidence.
Original guest/profile/ZIP remain unchanged; the owner's particular inherited-
shell WINMINE failure is not claimed solved by complete-logon controls.

Second-role scope review: original taskman retains counts/priorities/list/lock
and wait policy; its only new mirror mechanics are five registered persistent-
readiness predicate operands/local comments (OPENNT-HOST-123).077 removes the
independent nominated-target/psmsSent pseudo-identity, rather than retaining it
beside native USER.078 reuses the already imported original unregister client
and full-handle algorithm; no additional mirror/guest diff, registry or native
private object decoding is introduced. Selected-source audit reports278 files,
192 byte-exact/86 registered content differences and zero gaps. This is not an
added-file/diff count or proof of untouched branches. Native extension/callback/
explicit-yield composition remains the next finite, source-defined S4 group;
cross-worker/external-native/implicit/converted-lifetime coverage and S5/S6 are
not closed or transferred to manufacture P2/T completion.
### Input-membership P4 complete candidate acceptance

`input-lifetime-acceptance-r1` completes exit0 against the frozen
`input-lifetime-build-r2` sources and outputs: fresh native20/20, original30,
12 real S3 guest groups, default/extension message groups in all3 policies,
WINMINE/SOL gameplay and normal exits, WRITE's existing startup frontier,
Console17/Window17, direct native modal3 and complete-logon gameplay3.
The WRITE floor does not prove editing/save/normal exit. Source registration
remains278 selected files,192 byte-exact/86 registered differences, issues0;
083/084 add no original mirror or guest change. The retained old-baseline
diagnostic's exit1 is not rewritten: its old3/8 heap failures are negative
controls, while all8 candidate trials and their mandatory lifecycle markers
pass. The production native-failure terminal path is source-reviewed; the
focused injected native-call failure proves its callback seam, not an actual
production fail-fast process exit.

The P4 delivery supervisor requires these complete gates, successful native
input services, sealed candidate repeat inputs and exact package identities.
It does not use a passing debugger supervisor as fixture success. Publication
and installed validation are separately recorded: input-lifetime-delivery-r1
exit0, coherent13 in both destinations, both-width deployed style and installed
logon/gameplay3 pass, final13 hashes match the release manifest. Its exact
pure-native modal control reuse is recorded in reused-native-control.json;
original guest, installed configuration and assets/winnt.zip are unchanged.
Paired posted-input performance remains essentially flat (ordinary61.961 to
61.073ms), not a stutter fix, physical-input measurement or stable tail claim.

### Owned-view traversal085 source-complete batch

Following clean main/origin7dd8fa32b, the owner requests continued improvement.
The exact P4 sampled-input run has25 active observations, including3 nearest
copy_window_observation symbols,1 copy_window_view and1 graph validation.
These are statistical addresses, not stacks or weighted time; they do not
prove this is the principal remaining hotspot. Source proves the avoidable
quadratic metadata search: each published slot invokes window_borrow_link,
which walks the same lifetime list. The caller also already holds that borrow
when reading a single retained view. No test-driven feature expansion follows.

Four-rung disposition: pinned OpenNT ntuser/rtl/getset.c GetWindowData reads
the original shared WND directly; selected original CCPU still owns guest RAM
reads. There is no original standalone copied-view enumeration to import.
Composing the entire NT4 private USER shared heap would violate the approved
modern resource boundary. Keep the already admitted copied projection and
optimize its traversal; no original mirror intrusion or new behavior engine
is needed. Source paths prepare_window/commit_window prove every ordinary
published backing has an existing borrow record. withdraw_window clears the
slot and marks retirement under the same exclusive context; final reclamation
unlinks the record. The cold desktop is the explicit record-free exception.

085 passes the already known borrow to row copying. A whole view retains its
table-derived required count/capacity refusal, copies the desktop separately,
then walks existing records and accepts only the currently published backing.
Retired records are skipped before any guest storage read (failed reclamation
must not make that read safe by assumption). Exact copy count is checked, and
qsort restores the original ascending-low-word row order. No new allocation,
persistent index/cache, native query suppression, timestamp, scheduler change
or guest/mirror edit. The original publication/lifetime table remains the
authority; the list is only an enumeration path. Existing failed-capacity
untouched-output and LastError checks remain. Added actual CCPU checks cover
the record-free cold desktop, ascending output and a pinned old generation
beside its current replacement, including output-end sentinel preservation.
Existing graph/retirement/rollback cases remain mandatory, not replaced by
these new assertions. Full worker/provider composition, paired ordinary/
sampled/private timing and every-P release gates are required before delivery.

window-copy-build-r1 links worker/provider and passes extension boundaries,
but its unchanged input supplement fails the native prerequisite own_Z=128
with own_Z=0 before grouping on the restricted desktop. It is not a passing
build or evidence that the input algorithm failed. The first performance
supervisor cannot observe the prior process from its restricted job; r2 can
observe it but correctly refuses runtime because build-r1 failed. Their
failures remain recorded. Complete build-r2 runs under the already authorized
normal user context with unchanged source/keyboard assertions and sealed P4
baseline images/maps from r1. It deliberately verifies the source manifest
before reusing the stable production build roots; no clean rebuild or test
condition weakening is required. Later results are not yet implied.

The full normal-context window-copy-build-r2 now completes exit0. It composes
worker/provider, fresh extension/original-frame/input supplements, physical
alias service and original external memory, actual CCPU window borrow/reclaim/
graph snapshots, native-nested and start-notification. The same old/full slot
reuse witness passes with attempts1 and distinct backings, including the added
no-old-row/current-row/output-sentinel/order checks. Local observation commit
and32 external-process/local-update iterations also pass; native held0/held1
receiver retirement restores ownership, final lifecycle errors0. The new cold
desktop copy runs before any descriptor publication. No completed whole
runtime acceptance/publication or speedup is inferred from this build group.
The legacy paired pixel observer polls20ms between unchanged cells, so its
small differences are quantized and cannot establish a few-ms latency gain.

window-copy-performance-r3 completes exit0 with both real EMS controls and all
six actual input/final-board/normal-exit cases. Baseline P4 versus candidate:
ordinary median60.949/62.069ms and p9563.838/93.846ms, sampled median61.349/
61.425ms, private30.2675/30.243ms. No material overall improvement is proved;
the ordinary tail is worse in this small observation set, not suppressed from
the report. Source-level removal of repeated searches is not promoted to a
principal-stutter diagnosis or a stable physical-input/tail claim. Statistical
addresses still include class reachability collection and SAS/native services;
their complete ownership paths require source review before another batch.
The exact P4/P5 nested debugger group completes exit0: each8/8 passes required
held0/held1 receiver retirement and normal fixture exit, with zero observed
heap corruption. Unlike P4's earlier failing P3 control, this diagnostic's
overall result is actually passing. Full retained runtime acceptance remains
required before publishing085; P4 stays installed until then.
The paired timing workload is12 right-button flag-state changes of one cell,
not a measurement of left-button multi-cell reveal or physical mouse delivery.
The separate application floor does exercise left-button open and reset, but
its success alone is not a latency distribution for those operations.

#### Next source lead, not another mutation of the frozen085 candidate

The complete class reachability path is borrow_leave, class unregister/release,
task teardown, observed-window creation/graph commit and final WND collection.
collect_observed_classes first retries retired WNDs, then allocates capacity
65536 plus the record count on every applicable call, scans the full slot table
and scans all borrow records again, sorts class pointers and performs final
strict retirement. class_is_referenced repeats the slot scan before the same
record scan; release_class must distinguish published WNDs from retired pins.
This is a source-proved duplicate traversal/allocation, not yet a measured
whole-UI bottleneck. The exact original class.c at the pinned OpenNT revision
uses cWndReferenceCount and DestroyClass's zero-association precondition; native
NT4 shared-heap reference ownership is not a usable extra authority for these
owned copied generations. Do not add a duplicate independently updated count.

The finite next design is to derive reachability from the existing lifetime
list plus the record-free desktop. All ordinary published/prepared and pinned
retired backings already belong to that list. A temporary reference array
therefore needs at most one plus the record count, not65536 plus that count.
Keep every retired/prepared record for final CLS reachability; unlike live-view
copying, skipping them would be wrong. Keep the existing initial WND reclamation,
qsort/search, final strict retirement, allocation/failure retention and errors.
Only release_class's existing published-association refusal filters current
slot ownership; retired pins still retain a released descriptor physically.
Review class_is_referenced, collect_observed_classes and release_class as one
closure. Existing managed-class, retained old-generation, release/free failure,
rollback and teardown assertions plus real CCPU and paired-input floors must
all remain. This is neither implemented nor admitted by this read-only lead;
finish085's current verification/delivery safe point before another source edit.

#### 085 P5 delivery verdict

window-copy-acceptance-r2 exit0 completes fresh native20/20, original30,12
actual S3 guest groups,6 default/extension message policies, WINMINE/SOL
gameplay/normal exit3, WRITE's retained startup3, Console17/Window17, native
modal3 and complete-logon gameplay3. Source registration remains278 selected,
192 byte-exact/86 registered differences/issues0. window-copy-delivery-r1
exit0 publishes coherent13 to both assets/release and O:/winnt/system32,
passes deployed both-width style and installed-logon/gameplay3 and verifies
all final identities. The input service reference and pure native modal
reference are reused only with exact-input checks; fresh input lifetime and
new exact P4/P5 nested candidate repeats also pass. No original mirror, guest,
installed configuration or ZIP changes. The reviewed source/floors and flat
paired timing are separate conclusions; neither full S4/T closure nor a
stutter/physical-left-click improvement is claimed. NTVDM SHA-256
DA32FCE789F3964DF5F8AA7938A8ED823A5323C56674700BD3D625B4588AA628;
WOW32 SHA-2562259617984A63A651DACDA8366F1CB34D1386BB018C5AD6A3B7264BDFAEC0237.

### Class physical-reachability086 source-complete group

After clean reviewed main/origin7c78d3019,086 implements the complete finite
lead above, not another change to P5's frozen candidate. Re-read original
class.c DestroyClass/DereferenceClass and production class_words unregister/
standard teardown: the existing logical cWndReferenceCount and native
UnregisterClass stay authoritative and unchanged. These do not substitute
for physical retention of observed/prepared/old-generation copied CLS views.
The original source's shared-heap clone/reference owner cannot directly own
these extra copied backings; reuse its zero-association requirement through
the existing projection lifetime, not a second independently maintained count.
No new original import or mirror intrusion is necessary.

class_is_referenced now checks desktop and every lifetime record, including
prepared/retired pins. collect_observed_classes uses the same exact reference
closure and reserves one desktop slot plus the record count, retaining its
old overflow/allocation failure, initial WND retry, sort/search, strict final
retirement and error path. release_class still refuses current published WNDs
but permits physical retention by old pins after successful native unregister.
Its published predicate is factored from the exact P5 live-copy predicate;
retired records are rejected before reading their potentially failed-free
storage. The slot table remains authority, not list position or a new flag.
No callback/native query, lower freshness, extra persistent state or guest
change. This shrinks temporary capacity and deletes duplicate slot scans; it
does not yet prove a user-visible speedup.

The actual CCPU32-class matrix is strengthened from2 to16 repeated collections
while16 live and16 retired/pinned generations coexist; all original lifetime,
class refusal, no-resurrection and exact final free-space assertions remain.
Existing failed-decommit/retry, managed/window generation, graph and real
native/application floors remain mandatory. Compile this whole source group
before any further selection, then paired timing and full retained delivery.

The full class-reachability-build-r1 now completes exit0, composing worker/
provider plus fresh native extension/original callback frame/input membership,
original memory/alias services and real CCPU borrow/reclaim/graph/nested/
notification. Actual32-class16-live/16-retired and16 collections preserve all
descriptors until final pin release and reclaim the same virtual space;
live-class refusal/no-resurrection, real slot reuse, graph rollback and failed
decommit/retry groups pass. Native held0/held1 receiver exits restore owner,
final lifecycle errors0. Quality-r1 also passes. No measured speedup, complete
runtime acceptance or successor publication is inferred from these build
results; the chained performance/runtime/delivery supervisors remain pending.

class-reachability-performance-r1 now completes exit0: both real EMS controls,
all6 actual flag toggles/final-board restoration/normal launcher exits pass.
P5 versus086 ordinary median61.650/61.347ms, sampled60.915/61.245ms, private
30.0405/30.2845ms. Ordinary p9594.329/62.441ms and sampled62.275/66.390ms
move differently; with12 observations and20ms pixel polling, no material
overall gain, principal-stutter repair or stable tail result is proved.
The source still justifies deletion of duplicate traversal/oversized temporary
storage without a new authority, but this must not be sold as the user's
requested responsiveness fix. Runtime foundations/application/full release
and exact P5/P6 nested repeats remain separate outstanding gates.

####086 acceptance refusal and slot-observer repair boundary

class-reachability-acceptance-r1 fails its native20 floor19/20. The sole first
failure is standard-class fixture line930: required same-slot/new-generation
observation absent after970 managed creations within the unchanged15s bound,
old07ff0dee versus last064a1326. No creation or matching-generation semantic
assertion fails before that missing precondition; independent errors counters
later remain nonzero because that failure is retained. Its class-release/
32-class matrix and other19 native cases pass. This is not a complete pass or
proof the runtime is defect-free. Delivery-r1 rejects publication; P5 remains.

reuse-native-control-r1 compiles an independently authored native-only control
with kernel32/user32, no project products, on an owned unswitched desktop.
It observes0 matching uses of the fixed old slot in2048 native BUTTON creates,
then128/128 actual same-slot/different-full-handle pairs when arming a fresh
native STATIC old window before each new BUTTON. Owned hidden supervisor,
source/input hashes, real deadlines and exact results are retained; exit0
means this positive native observation, not fixture/product acceptance.
Do not infer a uniqueness-bit limit or exact Windows private allocator rule.
The proved test-design problem is depending on reuse of one selected old slot,
which native USER does not promise. The actual reason that particular slot
was unavailable is not needed to preserve the product contract.

The observer correction re-arms the entire native-observed/pinned/sentinel old
generation after a replacement uses another slot, then retries within the
same original deadline/attempt bound. Both managed and raw native loops retain
the actual same-slot/different-generation requirement and all old/new words,
class, promotion, retained-storage and final-release assertions. No successful
skip, manufactured handle, product hook, guest change or assertion weakening.
Failed evidence stays sealed. Rebuild fresh fixture groups against unchanged
086 production inputs and repeat complete acceptance; publication remains
blocked until that complete group actually passes.

class-reachability-build-r2 completes exit0 against the observer correction,
including fresh standard-class alongside every retained group. It observes
the real managed same-slot/new-generation pair at attempts9 and null-class
at attempts1, then passes all retained-generation/source-word/refusal/final
reclamation assertions, standard9-class/storage and32-class matrices.
Production source closure is unchanged; the build records the sole fixture
source before/after SHA and fresh image/MAP hashes. A direct identity check
does not find all four generated artifacts byte-identical to build-r1; do not
claim that unchanged production source implies identical rebuilt artifacts.
No old failed result is reused as a pass. Full acceptance-r2 and publication
remain separate pending gates; why a particular native slot was unavailable
is not asserted or emulated.

Identity-check correction: NTVDM image/MAP remain exactly8DB13636.../
A46ED99C..., but the provider generator regenerates three ABI/entry-bridge
objects/import libraries and relinks WOW32. Do not describe all binaries as
unchanged. Compare sealed performance-r1/media/WINNT/system32/WOW32.DLL
E70C1DB5... with fresh8276F1E8...: equal length, only four differing bytes
at file offsets120/121 and91CB4/91CB5. PE parsing identifies the two affected
fields as COFF TimeDateStamp (offset120) and IMAGE_DEBUG_DIRECTORY.TimeDateStamp
(offset91CB4, type13/coffgrp; export timestamp is instead94204). Native
dumpbin headers independently report Debug Directory RVA932B0 and its same
timestamp6AD48C9C. Entire .text SHA-256 is identical
2E22CEC858B932F25ED7BA456A9FBF573EB38933D1E6E403A6EF9AB948CCD128;
.data/.rsrc/.reloc are also identical. Thus the observed difference is these
two verified relink timestamps, not a presumed runtime code repair. Full fresh8276
artifact testing still uses exact recorded identities, not source equivalence
as a substitute for acceptance.

#### Exact borrowed-record lookup lead after086 timing

The29 active086 input samples contain four identical release_window_backing
offset105 addresses (runtime00C38459). record-hotspot-audit-r1 pins the actual
8DB136361964E56F8A428955F145B44761002AA27FE7FD41411164595C4E8C6F
image, its exact MAP, source and sample CSV. Hidden owned dumpbin diagnostic
exit0 resolves the sampled instruction to preferred00568459, a conditional
branch inside the inlined window_borrow_link loop (cmp record.backing at
00568456; next-link advance0056845B--61). This is evidence for this particular
list traversal, not merely the nearest release function or a weighted CPU
percentage. It still does not prove the sole/principal visible-stutter cause.

Source review covers every list mutation and all four pin entrypoints:
borrow_window, retain_host_window, borrow_callback_window and
borrow_window_relation. Preparation inserts records, physical retirement
unlinks, and current identity remains the slot-table/backing pair. Read and
release operations repeatedly find records by a linear walk. A large desktop
can therefore place actively reused local records behind unrelated records.
Do not add a second index/cache or read an unvalidated backing header to
guess its record: invalid/reclaimed input must retain its current safe refusal.

The bounded next candidate is ordinary move-to-front locality on the existing
list, only after successful validated pin acquisition, covering all four
entrypoints together. The lookup primitive itself must remain non-mutating:
collectors and commits keep pointer-to-link cursors. Existing published copy
order is sorted independently, references/identities/metadata stay unchanged,
and failed acquisition must not mutate the list. Audit disposal is complete:
the API explicitly permits only nonthrowing host deallocation with no domain/
USER/guest reentry; production release_generation_storage zeroes backing
fields, then words_release/class_words_release perform host-only final frees.
No collector/commit list walk can call these pin entries through that closure.
This avoids a scan-mutating generic lookup, duplicate registry or unsafe
native/guest-pointer shortcut. Preserve cold creation, retained callbacks,
relations, invalid/overflow/storage mismatch, physical failure/retry, sorted
view order and class lifetime tests. This candidate is not implemented or
admitted here: finish the frozen086 whole runtime/delivery safe point first.

#### Bulk ownership trace supersedes the locality-only candidate

Do not implement move-to-front merely from the sampled loop. Following the
complete caller shows project_window_graph pins every observation through
borrow_window, looks every record up again in metadata commit, then releases
every row by backing in the same forward order. A locality-only change can
still thrash this all-rows workload; it would not close the whole known-record
access group. The instruction evidence does not itself identify the caller.
No locality change has been implemented.

The complete finite next design reuses call-local ownership instead:

- Reuse the existing zeroed present array as a row+1 index, filled by the
  unchanged full-identity/cycle validator. Pass that array into the private
  validator instead of allocating its identical second65536-entry index.
  Keep exact duplicate/invalid/cycle error behavior and private path state.
- Walk existing live record metadata once, matching full identity and current
  backing through that row index, to associate each update with its record.
  This is temporary controlled projection, not another registry or authority.
  Each update records backing only after it actually owns a pin. Unprocessed
  entries therefore never decrement an unowned reference on failure.
- Existing records can be validated and pinned directly; the cold desktop
  remains its existing record-free exception. Private prepare_window returns
  its newly allocated record alongside backing to this private caller (other
  callers need no new ownership). All prepared/old-replacement pin/rollback
  rules and descriptor ownership remain.
- Metadata commit consumes that exact pinned record, not a new lookup. Factor
  private release-by-known-record from release_window_backing: public callers
  still validate through the safe nonmutating lookup, but the already-pinned
  batch releases its known records directly. Lookup is only needed when final
  physical retirement actually unlinks storage. Do not read arbitrary guest
  memory to accelerate an invalid public backing input.
- Retain complete graph validation, exact slot generations, initial/final
  class collection, overflow/failure/LastError and native freshness policy.
  No list reorder, persistent cache/index, new wire ABI or guest/mirror diff.

This covers preparation, pin ownership, commit and both success/failure cleanup
as one group, rather than patching the release loop alone. Existing public
pin/read/retained-callback/relationship and host-storage cases remain mandatory;
the known-record helper is private and cannot outlive its actual owned pin.
Disposal's audited host-only/no-reentry closure remains required when unlinking.
This is still a read-only fully specified successor design:086 remains frozen
until its complete runtime/publication/commit safe point is achieved.

Historical ownership cross-check for that successor: pinned OpenNT
ntuser/kernel/userk.h HMUnlockObject decrements the already known PHEAD and
only enters HMUnlockObjectInternal on the last reference. handtabl.c
ThreadUnlock1 pops TL.pobj, then unlocks that exact object; it does not search
all desktop objects by address again. HMUnlockObjectInternal handles the
marked-destroy/in-destroy boundary before HMDestroyUnlockedObject. Direct
composition would require real NT4 PHEAD/HANDLEENTRY/private server destruction
and ETHREAD stack/TL ownership, which are unavailable and must not be faked
with native HWND/USER pointers. The existing copied-generation reference seam
is the admitted bounded facade; preserve its ordering/refusal/physical-retention
rules while carrying its already pinned record through the private transaction.
This is source-shaped generic lifetime mechanics, not a new window behavior
engine or recovery of the entire original USER object manager. No additional
original package/mirror import is required.

###086 second acceptance refusal: common-dialog identity

Acceptance-r2 passes all20 native cases and original30 but stops at actual
common-dialog-Win16. guest-report has0/1/1, error258(0102), created1/closed0/
hooks1, task17BF, dialog0986, DestroyWindow AX1 and IsWindow AX1. Dispatch
records original WU32DestroyWindow return1; no WU32IsWindow thunk follows.
Original usercli.asm USER16CLIENTTHUNK IsWindow sign-extends HWND16 and calls
the local wowk.asm helper. That helper uses ValidateHwndNoRip and checks the
shared HANDLEF_DESTROY bit; it can execute without a host IsWindow thunk.
Original ssend.c LOCKPWND/UNLOCKPWND saves/restores CallbackWnd around a real
ThreadLock, and handtabl.c marks destruction while references defer physical
free. Current copied-slot retirement, retained callbacks and full-handle
selection must be reconciled with those exact facts. Do not presume a cache
fault or legitimate16-bit alias merely from these observations.

The complete target resource callbacks run at task retirement after failure,
but their existence does not prove the earlier native destruction or current
slot generation. Source plus retained trace cannot yet distinguish an
incorrect native target, stale copied publication, a reused16-bit slot or
guest-local cached validation. Do not label this the old S2 risk's same cause.
Delivery-r2 correctly refuses publication; installed P5 is unchanged.

Next diagnostic is a separately configured unpublished WOW32 using copied
project-owned wow_user_object_bindings.c and wow_user_task_lifecycle.c only.
Trace full-handle resolution and native-return target/native liveness/current
copied full handle/metadata under the existing owner, preserving return and
LastError. Existing environment trace path remains the only enablement. No
guest, original mirror, runtime patch, hidden window, private USER layout or
public request/transport change. Pin original/copy/patch/artifact identities;
run the same immutable authored common-dialog probe against P5 and086 workers
with this same diagnostic DLL. These are diagnostic runs, not releasable
acceptance. Product086 source remains frozen until evidence selects a fix.

The broad diagnostic build0 records same-DLL P5/P6 cases2+8 pairs (20 total),
all0 with unchanged16-round original probe assertions. Successful destruction
rows show old native identity dead and copied slot FREE; this does not prove
the cause of the earlier failed boundary. Preserve it as unexplained, without
new waiver. A second isolated diagnostic copies only project lifecycle source
and traces only completed DestroyWindow, after the same begin/native/end path.
No full-handle or other native-edge logging remains. It records raw argument,
begin-resolved full target, native result/liveness and copied current/metadata;
preserves LastError and result, adds no state or mutation. Fixed4 paired rounds
are diagnostic-only and cannot replace complete uninstrumented acceptance.

Narrow build-r2 finishes0, DLL SHA-256
AAB76F2ED1746DA8F493156DFD8EE8529E9E5A5C4D89A93F5F6552B53350C78E.
common-return-diag-r3 completes all8 cases0 (4 P5/P6 pairs). Actual successful
rows record raw short argument, full target, result1, native_live0, current0,
current_native_live0 and metadata00010000/valid1. Across broad and narrow
diagnostics28 cases pass unchanged assertions, but no trace has captured the
failed boundary. This is evidence of non-reproduction, not repair, same-cause
classification, a semantic waiver or authorization to publish. No more
identical repetition is useful as a proof; the missing failed-boundary state
remains the finite M5 prerequisite, with086 production source still frozen.

### Failure-only report boundary diagnostic

The original authored common-dialog probe calls local IsWindow, saves AX,
then reaches its existing _lwrite report without another USER call. Source
wkfileio.c/WK32FileWrite receives FILEIOWRITE16 (18 bytes); CDL2 is24 bytes,
with errors at6 and dialog at18. Source wow_user_invoke_thunk is before its
normal borrow scope. A copied project thunk binding therefore records only
the last completed DestroyWindow's raw/full input and result in TLS (nested
calls use the local invocation snapshot), and logs only a nonzero CDL2 error
at that report-write entry. No per-destruction I/O or new native USER query
precedes the guest IsWindow decision. This is a diagnostic, not retained
production state, a handle registry or a result reinterpretation.

At failure it reads existing copied metadata/full identity, old/current native
liveness, and the callback HWND/PWND pair through a read-only guest lease.
The TEB is resolved exactly as usercli.asm's FLATFS (KGDT_R3_TEB38h|RPL3),
using the existing protected-selector resolver, not a new guest_teb export
or a guessed modern TEB. Return/LastError/file-write and all original guest
code remain unchanged. Diagnostic r3's duplicate unguarded historical includes
fail compilation; r4 compiles but links unsuccessfully because guest_teb is
not a parent DLL export. Both are retained and never run. Fresh r5 removes
the redundant includes, uses &frame->bArgs per original GETARGPTR, resolves
the existing guest selector and builds0. DLL SHA-256:
2BA0D804D49F1FEE8F20B8021F54FCCD91A9505EE6814DAE2EB148EE3D96A617.

common-return-diag-r4 first runs an independently authored synthetic D001
negative-control copy, never original guest or a product-success case.
The unchanged rejection checks return1 as required; failure-only tracing
records destroy_full039A119A/result1, old native dead, current0/FREE,
cached HWND/PWND0 and cache_valid1. The original P5/P6 cases then both pass.
This validates observation placement and the guest-selector read, not a
repair or a native/cache state for the historical failed row.

common-return-context-r1 admits one context replay, not another retry loop:
the existing S3-floor owner runs its original native20/style/original-control/
resource/menu prelude, then unchanged controls/dialog/common-dialog policies
with frozen P6 worker and this failure-only diagnostic provider staged from
canonical ZIP plus coherent release inputs. Native prelude remains on the
exact frozen formal provider; only subsequent authored-guest cases use the
isolated diagnostic DLL. Exact sources/artifacts, process identities, deadlines
and failure reports stay under build; no publishing controller consumes it.
Its completion is diagnostic-only and cannot overturn refused acceptance.

The context replay finishes0: native20/20, both-width style guard, original30,
resource/menu guards and controls/dialog/common-dialog/hook in all3 policies
(12 guest cases) pass. Its prerequisite D001 negative control and one P5/P6
pair pass their explicitly separate diagnostic criteria. Quality-r3 diff and
documentation governance is0 before this final evidence update. Original
uninstrumented acceptance-r2 stays failed; no causal repair is established.
Do not add further identical reproducer repetitions as a substitute for source
work or alter its guest AX/lifetime assertions.

### Source-led thread-client projection follow-up (M3/M5)

Pinned original client/wow.c ValidateHwnd[NoRip] reads GetClientInfo's current
thread callback pair; GetSystemMetrics reads that thread's dwExpWinVer and
selects observably different3.1/4.0 metrics. usercli.asm uses FLATFS's TEB;
queue.c xxxInitTask initializes pti->pClientInfo->dwExpWinVer. These are
thread-client facts, not worker-wide desktop facts. Original userk.h's
EnterWowCritSect/ExitWowCritSect is the centralized CS execution-owner change.

Current page_domain_create maps one guest TEB for the entire worker.
set_expected_windows_version writes its one scalar; all production callers
are lifecycle task initialization and retirement. callback_window/restore
save/restore one pair for scoped nesting; wow_user_runtime enter/leave binds
the resource lock, not a per-task guest-client view. Existing pClientInfo is
already per task, and CPU execution ownership remains original taskman.
These exact facts require a complete execution-handoff audit; resource-lock
acquisition must not be mistaken for guest-CPU ownership. No captured failure
proves this lead caused0102, and no speculative cache-clear or thread-local
projection patch is selected from that hypothesis.

The finite remaining source work covers task initialization before a TDB,
original yield/sleep/acquire/release, inline and descheduled native returns,
cross-task callback entry/exit, resource cleanup and terminal retirement.
Determine which projected fields are original per-task state versus shared
desktop state, retain existing borrow ownership for suspended callback pairs,
and avoid a second state authority or changes to immutable guest code. This
belongs to existing M3/M5 foundation restoration, not S5 or a new feature.
Only a source-complete same-shaped adapter and whole positive/negative/nested/
retirement group may follow; the performance successor remains unimplemented.

###087 source-complete execution-view restoration

The handoff closure is source-complete: queue.c xxxInitTask assigns the real
CLIENTINFO version before its one EnterWowCritSect; taskman.c acquires in the
WOWEXEC hardware and ordinary scheduled-return paths and releases in
xxxSleepTask and DestroyTask. Inline native return retains its owner; actual
cross-task callbacks park/reacquire through those same original paths.
Resource-lock enter/leave alone proves no CPU execution change and must not
transfer the view. Original USER's native per-thread TEB cannot be mapped
directly into a separate CCPU address space; importing GetClientInfo without
the private NT thread mechanism does not solve that representation boundary.

Selected same-shaped adapter: preserve those original bodies and put two
guarded host-operation calls in their shared userk.h helpers (124: two comments
and two guarded call statements, one already-different mirror file). No new
original package or guest change. Three original acquire sites and two release
sites thereby share one boundary rather than scattering writes at thunks.
Version remains original pClientInfo authority. Each inactive client keeps
only its callback HWND/backing pair; the active TEB is its execution view,
and existing scoped/retained ownership, not this pair, retains storage.
There is no new HWND registry, cache policy, scheduler or persistent view-owner
pointer. Shared TEB self/desktop/delta stay shared.

The parent preflights both incoming and captured callback pairs against an
existing retained generation, including retired pins; failure leaves the view
and output unchanged. The small exchange allocates nothing, calls no native
USER/guest code and preserves LastError. Aliased input/output is snapshotted.
A same-thread acquire preserves the live callback pair and refreshes only its
authoritative version. Original InitTask's forced owner switch saves the old
owner before replacement. Remove preparation/failure/retirement's unconditional
version writes, which could otherwise overwrite another executing thread.
Only terminal unbind of the currently bound dying task discards its inactive
snapshot without reading a possibly withdrawn CPU view; no guest resumes that
thread, and the next original execution acquire overwrites before guest use.
Native-only models without host state have no guest view; production carriers
always install the operation. An active live failed restore is worker-terminal,
never an ordinary callback result or a fake successful no-op.

The complete worker/provider and all selected input/header/fixture dependencies
must rebuild before tests. Actual CCPU cases cover distinct versions, callback
suspension, retired generation retention, aliasing, failed restore/no mutation,
wrong address space and unchanged shared fields. Original task-order tests
check hook placement. A controlled second ThreadInfo view exercises the actual
production operation inside the registered modal owner against CCPU memory;
it is explicitly not a second registered task or scheduler witness. Real
native and guest lifecycle/nested/rollback/terminal floors remain required.
P6 predecessor image/MAP/import/source identities are sealed before stable-root
rebuilds. This group is independent source restoration, not attribution or
waiver of the historical0102 witness, and does not implement the performance
successor. The admission above precedes the composed results recorded below.

###087 composed and actual-task proof

client-context-build-r1 links worker/provider and passes extensions, then fails
the input fixture's own native keyboard control (own_Z0 before grouping).
Non-sandbox r2 reads128 and completes the whole composed group0, including
actual CCPU view exchange/retired pin/alias/atomic rejection, production
operation, task order, task/process retirement, rollback and nested cases.
NTVDM5217AEE26B1AA36D047E9982DFD7F627237676EB244ED69828ABF18D25656668;
WOW32A8E5C8F84960720F505E05D6EC1F4F238D44C6BD7CBAFF6F7CE11906F90C7672.
Actual profile P5/new ordinary61.281/61.383ms, sampled61.452/61.5455ms,
private30.249/30.435ms remains flat (12 flag operations,20ms pixel polling,
not physical left-click latency). Strict nested controls both pass8/8.

Full acceptance-r2 fails native-reply cleanup: both normal/early reply
transports return114 correctly; first failures are raw helper join/exit
checks863/864, followed by class/session errors from a live helper. Source
posts WM_QUIT, offers DirectedYield, then kernel-waits while sender execution
may be eligible again; helper destruction enters its WOW callback. A scheduling
opportunity is not a join barrier. Correct that test-owned join with existing
native-call begin/end/finally, retaining3s, normal exit and all final assertions.
No Sleep, pump, longer wait, task-state mutation or production patch. Fresh
compiled grouped proof is still required; the failed run stays failed and
does not explain the older0102 row.

The optional ThreadClientView authored message witness uses sender NE030A and
receiver0300. Guest instructions save EAX/FS, read original FLATFS003B:0050
before any host query, restore registers and record mismatch/check count in
MSG2 at callback entry/exit, send/post/peek returns and phase reports. Original
MSG1 assertions remain. All four ordinary sender/receiver/default/extension
images are assembled before/after and proved byte-identical to HEAD.
client-context-thread-versions-r1 isolates a real P5 defect: both actual USER16
tasks and normal message/exit phases complete, with only error1000 and no
unrelated bits. Sender6079 expects030A, reads0300 after receiver6031 runs,
then0000 after receiver exit (61 sender/35 receiver checks). Exact r2 candidate
images pass Off/Win16/All with each task's version retained across normal,
nested, early/native sends and peer exit. This is not a forced product failure,
pixel inference or claim of cache/0102 causality. The initial setter appears
in806598b9b (T422 S2); that attributes this missing handoff only.

Grouped r3 checks exact unchanged production source/images/MAP/ABI and Ninja
no-op builds, then freshly compiles corrected fixtures. New message witness
sources are frozen too. The r2 profile is reusable only with exact runtime and
benchmark-source identities; no timestamp-only regeneration, failed-row reuse
or weakened lifetime assertion is allowed. Full current native/guest/coherent
delivery gates and remaining M1--M6 obligations are still open.

Grouped r3 completes0: production Ninja reports no work in both stable roots,
the recorded image/MAP/import identities are unchanged, and all fresh composed
fixtures pass, including native-reply's scoped join. Whole acceptance-r3 then
passes native20, original30 and both-width style/resource guards. Guest,
application and deployed gates are still in progress, not implied successful
by those foundation results. Neither the fixture correction nor this partial
acceptance establishes the cause of the retained0102 common-dialog witness.

### P6 second-role source review (delivery gates pending)

Reviewed the complete production, fixture, build/export and registry diff,
not just the successful run log. Scope remains086 physical class reachability
plus087 original execution-view restoration. The two host-operation tables
are production lifecycle and the explicit native scheduler model; neither
leaves a callable projection slot uninitialized. No wire/guest structure ABI
changes. One parent export is linked through the private x86 provider import
set, and the native-only test substitute refuses accidental view exchanges.

Class reachability includes every prepared/published/retired lifetime record
and the record-free desktop. Unregister refusal uses current publication,
whereas physical reclamation includes pinned retired readers. These are
different predicates by design, not competing class authorities. The new
exchange preflights full generation and a real existing lifetime owner before
writing; it does not acquire a new reference or make snapshots owning. Active
CLIENTINFO is the execution view, inactive callback fields are projections,
and original pClientInfo remains the sole version authority. The original CS
owner is assigned/cleared only by original code after the adapter operation.

The slot observer still requires real same-slot/different-generation native
reuse within its bound; re-arming repairs an unpromised allocator precondition,
not product identity rules. The reply fixture's scoped native join releases
execution eligibility through the existing runtime boundary; it does not
weaken timeout, reply, cleanup or normal-exit checks. Authored distinct-version
NE probes are diagnostic programs, not rebuilt or patched original guests.
Actual P5 failure and corrected candidate versions prove that one defect;
they do not prove the old0102 cause, callback-cache failure causality or speedup.

No new observable semantic concession is requested or inferred. An eventual
reviewed partial baseline must retain every historical failed run and the
owner-approved unlocated common-dialog risk, keep current strict assertions,
and explicitly leave S4 M1--M6 and S5/S6 open. Passing fresh gates is necessary
for publication, not proof that every historical anomaly has been repaired.
No publication is performed by this source-review entry.

### Observer environment ownership correction

Full client-context-acceptance-r3 completes foundations-r3 exit0: focused
original/native, native20, original30, style/resource/menu and all12 actual
guest groups including16-round common dialogs per policy. Default messages
also pass all3 policies. Extension-Off then fails before observer launch:
CreateEnvironmentBlock rejects the raw token with error6. No guest report or
observer process exists for that row. The whole acceptance remains failed.

Five observer entry points directly passed GetCurrent().Token from a temporary
WindowsIdentity. The raw IntPtr does not retain its owning SafeHandle. Keep
the identity explicitly alive through Read and Dispose it in finally in
message, EMS, native-modal, logon-gameplay and mine-latency runners. No product,
guest, environment allowlist or profile contract changes. The authored
verify-logon-token-lifetime runner passes16 held reads with forced collection
and rejects the deliberately closed-token negative with error6. It proves
the ownership requirement, not the historical process's exact GC schedule.
The same-input input-native-services group is rebuilt too because087 changed
its shared host-operation header; it completes0.

Grouped r4 freezes all corrected observers, regression and exact existing
production/fixture inputs; it verifies only those observer scripts changed
since composed-r3, reuses explicitly identified successful fixture/foundation
evidence and the unchanged13-image staged package, and reruns all affected
timing/message/native-extension/distinct-version/product/modal/logon paths.
It never overwrites prior manifests or converts failed aggregate rows to
success. Original12 guest probes do not consume the changed observer files.
Native20/original30 sources/artifacts and the complete staged package must
still match before reuse. Publication remains pending actual final gates.

R4 timing completes0: ordinary P5/current61.2445/61.761ms, sampled61.172/
60.8055ms, private30.2555/30.262ms. Again no material improvement. All9 actual
message policies pass, including native extension and distinct-version tasks.
Product then fails SOL-Off's deal pixel assertion: full initial tableau,
enabled command1000, unchanged patterned hashbd8543cd for10s. The observer's
ffffffff is from post-failure cleanup; no captured product crash/failfast is
proved. SOL's NE expected version is030A. The immutable original DOS GET_TIME
uses BIOS INT1A ticks; the existing probe's1100ms host wait does not itself
prove the actual guest seed or command dispatch. Neither a timing coincidence
nor087 causality is yet established.

Admit one paired P5/current diagnostic with identical test source and exact
runtime identities. Its original successful path/assertions stay unchanged;
only after the original deal assertion fails does it capture the unchanged
board, query responsiveness/menu state, try a synchronous command and post
stock input, recording each separate result. It still returns failure6 even
if later diagnostic input works. These are public native observations on the
owned private desktop, no guest writes, broad trace or production edit. No
repeated-green strategy, weakened deadline or publication from diagnostic0.

sol-deal-boundary-r1 completes the one P5/current pair with both ordinary
successful paths (no failure-only branch reached). It neither reproduces nor
attributes the failed product row. Source follow-up confirms immutable SOL
8:00FC calls DOS date2A/time2C, reads DH seconds and builds CRT time, then
1:09C7 masks AX to15 bits and reseeds. Canonical kernel31's WOW2C entry passes
through DOS; wdos.c's native GetCurrentTime bypass is explicitly disabled.
DOS GET_TIME consumes INT1A's BIOS counter. Do not assume a modern timer.

sol-clock-boundary-r1 adds only paired-MAP read-only observations of CCPU's
original Start_of_M_area and its BIOS46C counter, before/after the unchanged
1100ms wait. Source ccpusas4 owns that backing; no private native USER object
is interpreted and no remote write/suspension occurs. Paired image/MAP hashes
are checked. P5 advances309165->309185, current309462->309482; both deals and
stock inputs pass. These non-atomic observations prove those sampled clocks
progressed, not why the previous failure occurred or that it is now fixed.
No more identical Off replay is admitted. The untouched remaining SOL policies,
WRITE and DOS matrix execute once in product-tail-r1 while preserving the
original sol-Off failure and an explicitly failed aggregate. This is not a
publication candidate or a semantic waiver. Historical common-dialog and
new SOL evidence remain separate unlocated witnesses.

### Deterministic Deal oracle, not a runtime workaround

The isolated copied-thunk discriminator reuses the exact formal current object
closure, replacing only wow_user_thunk_scope.obj. First configure-r1 rejects
an incorrect resource-manifest path before compilation; r2 corrects that path
and compiles/links0. Production source/images remain unchanged. Diagnostic
WOW32 SHA57D661D07820464AEA70891B81A422CAB897BDE74D1FB504269CD6C4BFAC9392.
Existing guest pointer leases read only the actual DispatchMessage argument,
current task/instance, and hash-pinned SOL data seed058A/game0028/busy002C.
No alias survives the real thunk; LastError is preserved. Two scalar snapshots
are logged after the completed command, not every guest instruction or native
edge. This is an isolated diagnostic layout, never a product file heuristic.

The first zero-wait control crosses a second (3435->3436) and passes; it is not
a negative witness. The one source-refined warm control finishes an ordinary
synchronous deal first, captures that board, then posts command1000 immediately.
sol-command-control-r2 records actual task17B7/instance1746, seed34A5/34A5,
game2A2C/2A2C, busyFFFF/FFFF, result0. The tableau48b42423 stays unchanged and
the old pixel inequality fails. Public WM_NULL and synchronous Deal both
complete; the latter changes pixels388054f8 and stock input then changes waste.
Thus a real completed Deal may legitimately repeat the board. This proves an
invalid universal observer assertion, not that the earlier1100ms failed row
had the same seed or cause. Its failure is retained, not promoted by replay.

Strengthen the formal application contract without randomness or a host Sleep:
observe the initially empty waste, draw actual stock and require nonempty
changed pixels, post Deal and require that waste to clear back to its exact
initial hash, require a stable patterned tableau, draw again and require
changed waste, then normal close/launcher completion. A lost/ignored Deal
cannot satisfy the clear, even when repeated seeds would hide behind a stable
tableau. All original10s bounds and owned-window identity remain. This is
different observable behavior with an actual before/after precondition, not
skipping gameplay, increasing timeouts or replacing asynchronous delivery.

The diagnostic positive completes this full sequence. An authored input-only
negative deliberately omits Deal (no guest writes or product fault injection)
and fails exactly deal-waste-cleared with nonempty c6fff2f0 versus expected
empty65174fed, fixture6/runner1. Formal P5/current3-policy checks now use the
actual production DLLs, not the diagnostic image. Only those affected tests
need new compilation; unchanged fixture/source/image/MAP and other passed
application/message/DOS evidence must be explicitly revalidated before reuse.

### P6 current full-contract reconciliation and review

sol-deterministic-acceptance-r1 passes the omitted-input negative and all6
formal P5/current policy cases with the unchanged production images. The new
observer does not depend on diagnostic hooks or private state. Its positive
and negative behavior prove actual Deal state transition, not randomness or
the cause of the historical1100ms row. Current source/runtime package is
unchanged from087 throughout these observer-only corrections.

client-context-reconcile-r5 completes0 with explicit per-case provenance.
Build-r5 reuses source/image/MAP/ABI-identical composed-r4 and adds the actual
SOL observer source coverage; no native production recompilation is required.
Product-r5 combines mine3 from product-r4, newly compiled deterministic SOL3,
WRITE startup3 and DOS17/17 from the tail. Acceptance-r5 combines only exact
passed default/extension/distinct-version/modal/logon cases; foundations-r3
retains native20/original30, style/resource/menu and12 actual guest groups.
Old acceptance-r2/r3, verify-r4 and product-r4 failures remain unchanged and
are not reused as successful aggregate rows. A successful explicit reassembly
is not described as rerunning those unchanged cases.

The second-role source/requirements review retains the old common-dialog0102
witness without causal attribution to087 or to S2's older accepted risk.
Its original guest assertion remains strict and passes the fresh current
16-round/3-policy floor; no runtime semantic boundary is relaxed. The current
source-proved thread-view correction and complete passed release contract
justify a bounded partial correctness baseline, not a claim of historical
anomaly repair, full M5/S4 completion or a blanket future-failure waiver.
This changes the earlier diagnostic-only hold after actual normal-contract
evidence; it does not publish a diagnostic binary or silently transfer a
foundation obligation to S3/S5. Quality, coherent deployment/installed checks,
reviewed commit/push and the complete remaining M1--M6 scope still apply.

### P6 coherent publication

client-context-delivery-r2 completes0: quality-final-r2, coherent13 publication
to assets/release and O:/winnt/system32, recoverable27-file backup, deployed
both-width style boundary and installed normal-desktop logon3. The installed
complete-token environment has45 entries/3349 characters and is unfiltered,
not the existing developer shell's snapshot. All13 hashes match the tested
manifest in both destinations. Only NTVDM and WOW32 images change; no diagnostic
DLL, original guest, assets/winnt.zip or user configuration changes.
NTVDM5217AEE26B1AA36D047E9982DFD7F627237676EB244ED69828ABF18D25656668;
WOW32A8E5C8F84960720F505E05D6EC1F4F238D44C6BD7CBAFF6F7CE11906F90C7672.
Original mirror delta is124's two guarded calls/two comments in the already
different userk.h; selected totals stay192 exact/86 registered differences,
zero registration gaps. Commit/push and clean-handoff verification remain.

The private-known-record/scratch group described above is the next bounded
performance lead after that clean handoff, not a claim that unweighted sparse
PC samples establish the sole cause. Retain all source-defined pin, generation,
failure/rollback and collection rules; no permanent record index or weakened
freshness. Current timing is flat and full WOW/M1--M6/S5/S6 remains unfinished.

###088 complete private transaction implementation

Clean P6 main/origin d347d0594 is verified before admission. The seal retains
exact source, image/MAP/private import and all13 release bytes. Implement the
entire previously specified group: caller-owned zeroed row+1 index supplied
to the unchanged cycle/full-identity validator; one live-record association
walk; private backing borrow that keeps unknown/cold safe lookup/allocation;
constructor optional record return; exact pinned record for metadata commit
and success/failure release. Only final physical unlink searches the list.

Association does not set cleanup backing or acquire a reference. Invalid input
may leave unprocessed raw associations, which cleanup must never decrement.
Retired records are skipped before guest-memory reads; any cold/absent record
still uses the old safe path, including ordinary absence rather than adding
a desktop-only rejection. Full generation/current backing matches precede
known pin use. Constructor output is NULL until success; failed preparation
keeps its old physical-retention path. No native/guest callout occurs in the
private commit or audited deallocation closure. LastError, overflow/refusal,
class reachability, prepared/retired pins and safe public arbitrary input
validation remain. Public relation/callback pins are not replaced by raw data.

The direct CCPU rollback extension rejects32 transactions at first/last row
descriptor positions, not just the managed wrapper's early preflight. Two
independent held readers must remain readable with unchanged generation/style
after every rejection; repetition exposes accidental consumption of unowned
associations. Existing cold preparation, cycles, real native slot reuse,
class matrices, failed physical release/retry and normal rollback remain.
This is source-complete but not compiled or runtime-verified yet. No new
public export, mirror or original guest change. Build stable Ninja roots
without regeneration because no generator/header/export contract changed;
record exact compiler/config identities rather than forcing all211 provider
objects to rebuild. One complete composed group precedes timing and release.

088 build-r1 compiles only page-domain C plus its two libraries/two links
(five worker targets); provider reports no work. Alias/external memory,
extension/input, original task/lifecycle/reply/process and CCPU borrow/reclaim
pass. Graph's32 new direct rollback negatives, initial preparation, actual
slot reuse and local/external snapshot cases pass, but its window-cache
terminal child returns1 instead ofC0000420. Parent did not retain child stderr
or report whether injection was reached. Other terminal negatives pass.
Whole-r1 is failed and verify-r1 refuses timing/product/publication.

One direct diagnostic invokes the exact same child binary/option with a fresh
named event and redirected stderr. It reaches injection and fails fast with
C0000420. This proves that captured run, not the cause or repair of the old
failed group. Do not repeat identical children until green. The finite test
observer correction adds an explicit --terminal-log private fixture protocol:
child redirects its own CRT stderr before initialization and normalizes back
to the existing option/event arguments. Parent still uses non-inheriting
CreateProcess(FALSE), the same injection/event/deadline/exit assertion, and
reports reached/exit/path. No unrelated handles are inherited, no product
parameter/hidden HWND/runtime behavior is added, and failed startup now retains
its original stage. Fresh composed-r2 uses unchanged088 production source and
new fixtures; no performance or release claim is inferred from the diagnostic.

088 composed-r2 completes0. Worker and provider Ninja both report no work;
production source is unchanged, while all fixtures are freshly compiled with
the explicit child-log protocol. All32 direct rollback cases, class/native
reuse/retirement and7 terminal fault children pass; each terminal child reaches
injection and exitsC0000420. No old failure is relabeled. Group covers original
task/reply/process, extension/input/alias/external memory, actual CCPU borrowed/
retained/failed-free/graph/class cases, initialization rollback and modal owner.
NTVDM032616347AB793F234E9F2DB338A73285E32563139956DB0884321BD403DD87A;
MAPAFA5019F64180E94643CED4B9513E83207175705E84978C2E26793657994E151.
WOW32 remains exact P6 A8E5C8F84960720F505E05D6EC1F4F238D44C6BD7CBAFF6F7CE11906F90C7672.
Known-record-verify-r2 now runs paired timing, strict repeated native lifetimes,
full native/original/guest/application/DOS/modal/logon and distinct-version
extension policies. No current performance or publication result is claimed.

Source-only residual lead, not another edit to frozen088: every original thunk
invocation leaves a scoped alias owner, whose leave calls collect_observed_classes
even for GDI operations. That collector builds/sorts one class pointer per WND,
including many duplicate classes, then asks membership for the much smaller
allocation list. Actual sparse qsort PC samples do not distinguish this sorter
from the window-view row sorter or establish its CPU weight. A future bounded
candidate can derive candidate-class references in a small call-local class
vector instead of sorting all WND references, preserving the independent final
strict retire check and physical-failure ownership. No permanent mark/counter,
managed-class reference reuse, skipped cleanup or new freshness rule is implied;
original managed associations are not these copied physical readers. Review
the complete collector closure after088's safe baseline, not a partial change
or a claim that another local asymptotic improvement solves the UI symptom.

088 performance-r2 paired ready-board measurements complete0: ordinary
P6/candidate medians61.7765/61.574ms, sampled61.7185/60.780ms,
private30.117/30.090ms. Ordinary p95 changes94.084/62.617ms and sampled
93.089/65.094ms, but twelve operations do not establish a tail improvement.
The observer includes20ms pixel polling and posted input, not physical mouse
latency. Sparse active PCs include qsort but neither its invocation count nor
CPU weight is established. Do not call088 the stutter fix. Both sealed/current
native nested controls complete8/8. Complete foundations, original30 and12
actual guest groups pass; remaining product/installed gates are still pending.

Further source review corrects the proposed class-vector shortcut before any
implementation: publish_named_class always allocates a new sampled CLS for
each fresh observation; project_window_graph marks it observed. Therefore
candidate CLS count need not be much smaller than WND count. Sorting a larger
candidate record instead of ULONG references is not a proved improvement.
Do not admit that edit on the duplicate-class assumption. Trace every WND.pcls
writer and its physical lifetime before choosing a simpler source-grounded
collection algorithm; no new counter or alternative authority is implemented.

088 second-role source/requirement review checks the actual seven-file diff:
association is distinct from acquired ownership on both first/last-row
failure; fresh records retain their constructor pin; pinned existing records
cannot disappear during commit/cleanup; the full slot generation and current
backing must match before reusing an association. Shared scratch is zeroed
before the same cycle validator. Public arbitrary backing resolution stays
non-mutating/safe. Private deallocation has no application/guest/native USER
callout; final unlink still searches the authoritative lifetime list.
Failure preserves the original error and no unowned association is released.
No original guest, selected mirror, ABI header, export, request transport,
policy/freshness or runtime environment changes.088 is registered in the
worker README. Existing192 exact/86 registered mirror differences remain278
selected inputs with no new mirror delta. The fixture-log change affects only
independent native tests, preserves non-inheritance and all terminal assertions.
Publication and final source/manifest review still depend on complete gates;
neither source review nor the paired timings closes S4 or its M1--M6 inventory.

088 final runtime/publication evidence: known-record-verify-r2 exit0 includes
paired timing, both8/8 native controls, full fresh foundations/native20/
original30/resource/style and12 actual guest groups, default/extension and
distinct-version thread-extension in all3 policies, WINMINE/SOL gameplay,
WRITE startup, DOS17/17 in both frontends, modal3 and normal-logon3. Quality
passes. Known-record-delivery-r1/known-record-published-r1 exit0 backs up27
files, publishes coherent13 to assets/release and O:/winnt/system32, checks
both-width deployed style and installed normal-logon3, and verifies all13
hashes in both destinations. NTVDM is032616347AB793F234E9F2DB338A73285E32563139956DB0884321BD403DD87A;
WOW32 remainsA8E5C8F84960720F505E05D6EC1F4F238D44C6BD7CBAFF6F7CE11906F90C7672.
Only NTVDM and its manifest entry change. Original guest/ZIP, user environment,
selected mirrors and other12 binaries are unchanged. Ordinary median remains
flat; no material stutter improvement or historical-failure repair is claimed.
P7 Git handoff awaits final governance and exact nine-file review/commit/push.

### P7 handoff and source-led residual cost attribution

Known-record-git-delivery-r1 proves clean main/live origin cf0f33ddf and
all13 hashes in both targets. Next source audit confirms original class.c
ReferenceClass/DereferenceClass are creation/destruction ownership operations;
their desktop-clone/kernel allocation closure is not a modern HWND facade.
Current copied physical WND lifetime is distinct from original managed class
association count. Do not substitute that count or a guessed small CLS vector.

Admit only isolated diagnostic attribution before a new production candidate:
mechanically copy the current project-owned page-domain C, preserve bodies,
wrap class collection, retired-WND collection, view copy and graph projection
with inclusive QPC counters, and wrap its two qsort sites separately. Link
the copied object ahead of unchanged sealed production libraries into a
build-owned diagnostic worker; do not overwrite the stable Ninja outputs.
The native latency probe reads diagnostic counters via ReadProcessMemory,
using the exact diagnostic MAP RVA, before/after its existing twelve posted
inputs. No target memory write, private modern USER layout, original guest
change, new runtime policy or production profiling overhead is selected.
Counter snapshots require a stable even sequence; inclusive/nested durations
must not be summed as disjoint costs. Observed instrumentation overhead and
20ms pixel polling preclude claiming physical input latency. Retain actual
call counts, rows/sort sizes and elapsed intervals; no hotspot conclusion from
sparse nearest-symbol PCs alone. This diagnostic is not a new runtime release
or a replacement for M1--M6. Original source remains read-only comparison.

#### Reproduction and actual diagnostic results

Run verify-wow-page-cost.ps1 -BuildRoot build/M0-T447/S4/page-cost-profile-r1,
then summarize-wow-page-cost.ps1 with that root. Independent controls use
verify-wow-page-cost-probe.ps1 -BuildRoot build/M0-T447/S4/page-cost-probe-control-r1.
Both owned background groups and the summary exit0. Diagnostic image
8B111B2626E42E6E4873CFA8B3B6069A2B4AB8FD23C2CA24F0E27693EC36227D;
MAP C0299E3518FF70A8267139A3B1B2212D2E0411910F61C7EAD7250B2A5D651F82.
The generated bodies reverse exactly to current adapter source, the old archive
page-domain object is absent from the link, VdmTib ownership audit passes,
EMS passes, normal uninstrumented controls and diagnostic ordinary/sampled/
private twelve-input/normal-close cases pass. End-of-run hashes prove every
original library/output/source/guest ZIP unchanged. This copied image is never
deployed. Native controls prove nested inclusive totals, sorted results,
LastError preservation, a real read-only even-sequence snapshot and refusal
of wrong magic/version, busy sequence, out-of-image RVA and wrong PID.

| Counter over12 inputs | Ordinary320 rows | Sampled320 rows | Private15--16 rows |
| --- | --- | --- | --- |
| View copy | 484 calls /313.536ms | 496 /328.489ms | 650 /57.442ms |
| Graph projection | 143 /235.722ms | 143 /237.280ms | 179 /78.184ms |
| Class collection | 327 /44.377ms | 313 /43.559ms | 370 /2.795ms |
| Class sort (inside collection) | 327 /11.404ms | 313 /10.746ms | 370 /0.574ms |
| View sort (inside copy) | 242 /73.256ms | 248 /78.305ms | 325 /2.747ms |

Read exact totals in cost-summary.json/cost-operations.csv; displayed values
are rounded. Ordinary counters bracket876ms while the posted-input stopwatch
totals526ms. Waiting for a quiescent snapshot occurs outside that stopwatch
but inside the counter bracket. No counter/stopwatch ratio is a CPU percentage.
Baseline input total546ms, instrumented526ms; sampled589/453ms and private
360/370ms show environmental/observer variation, not a new product speedup.
Nested kinds cannot be added; lifetime maximum fields also include startup.
The experiment changes priority: repeated full view copy/projection, not
class-sort micro-optimization, is the source-grounded next cost closure.

#### Source-defined next candidate, not implemented in P8

Only project_graph and wow_menu_client_refresh_all call copy_window_view.
Successful graph projection143 times explains286 count/copy calls; the other
198 calls are99 menu refreshes. Each menu refresh allocates/copies/sorts320
complete observations (relations, rectangles, class identity/style/extra/name)
but reads only full HWND, style and process identity. Original menu attachment
logic needs those fields, not a full window/class descriptor snapshot.
Keep the existing refresh points, all original/native menu tree behavior,
retirement/failure policy and ordered full generation identities. A separate
typed three-field copied projection can remove this unnecessary dependency,
without replacing menu behavior, weakening local synchronization or inventing
another cache/registry. Native USER and the existing WND lifetime records stay
authoritative. Review the complete cold desktop/current versus pinned-retired,
capacity/order/LastError, native and independent guest lifecycle cases and the
private export/build seam as one group before compiling a runtime candidate.

Full graph copying also calls page_domain_class_allocation for every row,
performing a class-list search each time. That is an actual source O(W*C)
cost but its exclusive weight is not separately measured. Do not repeat the
rejected assumption that fewer classes exist or treat a permanent native
reference counter as free of lifetime obligations. Reassess only after the
typed transfer group's composed evidence, without shrinking full S4 scope.
P8 changes test/observation/governance only, no product source, ABI, mirror,
guest, release bytes or accepted semantics. P7 remains the installed runtime.

P8 second-role review checks all12 changed documentation/test files against
the owner contract: original-source reasoning leads the experiment; the
isolated copy preserves original adapter bodies; no original algorithm is
replaced or modern USER token interpreted. Opt-in compilation leaves ordinary
probe behavior unchanged and its actual three-case control passes. Stable
sequence and image-range rejection are independently exercised, and recorded
nested/observer limits prevent misrepresenting times as exclusive CPU shares.
All compiler/linker/probe objects are build-owned hidden background outputs;
no product profiling switch, helper HWND or recurring runtime file is added.
Final governance, reviewed test-only commit/push and unchanged coherent13
hash verification complete this diagnostic handoff, not S4/T447 closure.

###089 typed menu projection: complete source/entry design and admission

Clean P8 main/live origin3aa4ff054 follows P7 installed runtime. The finite
089 group changes only the data transferred to wow_menu_client_refresh_all,
not its original/native menu tree policy, callbacks or lifecycle. The existing
menu refresh discards full view relations/rectangles/class descriptors and
uses exactly full HWND, style (including original child+popup interpretation)
and generation PID. Native GetMenu remains the attachment authority; existing
local/external publication and error rules remain byte-identical after the
new row field spelling. There is no equivalent original cross-process copied
window table transfer to import: NT4's menu getter reads WND.spmenu directly;
its server object/desktop heap is not a modern HWND or CCPU backing record.

Add a private three-ULONG window-menu row and copy_window_menu_view export.
Factor existing current-publication enumeration/sizing into a private typed
helper used by old full view and this row, not a generic behavior engine.
Both retain table-derived count, cold desktop, full current WND backing and
PID from its lifetime record, ascending slot order, insufficient-buffer count
and untouched output, invalid arguments/context errors and saved LastError.
No native/guest callout or lifetime transition may occur between sizing/copy.
Pinned retired/prepared WND records are not published rows. The menu projection
does not parse CLS/name/relationship fields its consumer does not use; errors
from those unrelated full-view descriptors remain in their actual full-view
consumers, not new menu dependencies. No persistent index or added allocation
is needed inside the parent copy. Actual row storage remains caller-owned.

Extend only the existing private header/export generator and menu consumer,
then independent actual-CCPU fixtures for cold desktop, exact full-view subset
equivalence, sentinel/capacity/LastError and invalid-pointer arguments, old pin
exclusion after real native slot reuse. Retain all full-view/rollback/cycle/
class/failure tests. Register089 in worker README; no mirror/guest delta.
One complete stable-root build (including dependency-required header consumers)
precedes grouped acceptance and paired P7/current ready-board timing. Publish
only the coherent13 tested group to both destinations with installed checks;
review/commit/push as P9 only after actual gates. No full M1--M6/S5/S6 scope
is removed and visible stutter improvement remains an evidence question.

089 source implementation is now complete: old full copy and the new typed
menu copy share one private enumeration helper; their field copiers/comparators
remain explicit. The menu path reads only exact backing handle/style and the
same lifetime-record PID. All old full descriptor/name/relationship handling
is unchanged. Native client/wow.c GetMenu confirms direct validated WND style/
spmenu access, not a class descriptor dependency. Menu consumer diff is only
row type, copier name and full HWND field spelling; native tree/refresh/error
code is unchanged. New actual-CCPU checks cover all three subset fields in
three live/reused/retired-pin phases, order, sentinel/capacity and argument
refusals, plus cold desktop and unbound context in every fixture.

Build source graph has no new object or flag: the only generator change is
one private export name. Generate canonical exports in a fresh configure-only
root and verify its definition differs by exactly that line before copying
the generated .def into the stable worker. Do not overwrite stable Ninja or
regenerate unrelated provider generated C files just to add the export.
Existing header dependencies and parent import-library dependency drive the
complete required rebuild/link. Seal source/image/MAP/import/Ninja first and
record actual work, not a forced clean211-object provider rebuild.

089 second-role source review: full and menu views share sizing/context/
current-generation/order mechanics, while each typed copier spells out its
required fields. The full route still calls copy_window_observation with the
same known record; no descriptor semantics change. The menu route has no
class pointer/name/relations dependency and cannot introduce an owner or pin.
Nullable sizing, invalid arguments, insufficient-capacity untouched output,
saved success error and generation exclusion remain common. No extra parent
allocation or whole-table index is added. Comparator uses the same low-slot
ordering only after exact publication selection. Caller remains serialized
against guest/publication, with no native callout during copy; this projection
does not assert a frozen native desktop. Menu refresh consumer changes five
type/API/member lines only; native calls, child+popup test, local PID policy,
tree failures/attachment updates and final retirement are unchanged.

Cold/unbound and full-view equality fixtures exercise the actual CCPU body,
not a fake numeric table. Real native HWND reuse proves the old retained
backing can coexist without becoming another copied row. New helper failure
returns remain test failures; the existing30-second reuse bound and all
terminal/rollback assertions are unchanged. Private export is additive with
paired parent/provider; no public command, broker wire, original mirror or
guest ABI changed. Coherent13 production publication is still mandatory.

089 menu-projection-build-r1 completes0: generated .def differs by exactly
one export, stable worker/provider Ninja hashes are unchanged; worker11 and
provider13 dependency-selected targets build/link. The whole original task/
reply/process, extension/input/alias/external memory, actual CCPU borrowed/
retained/reclaim/graph/class/menu-resource/native-nested/start and init/modal
rollback group passes. Graph marker reports errors0 phases3 fields3 capacity1
retired1; old/new actual HWND06be0fca/06bf0fca share a slot with distinct
backings00226000/0022a000. All32 partial rollbacks and7 reachedC0000420
terminal negatives remain passing. No historical failed aggregate is relabeled.
NTVDM35A7CBBBE10E82C928D9A228A1E81C9534D623114263D3050C12E8419E622307,
MAP5B987527F14CF7A21F437FBE671AF0973EDE0EBA480E1EAB01C8E44CD50339AA;
WOW325AD48DBA7F44C6E4F10ABF1A81759DA80C4908580047FB1739B468C7B207CAE9,
MAP3DE1A5753E5348144B6BBE191C57C79B6B1984F549239D284A53538827381FCD.
Menu-projection-verify-r1 now runs paired ready-board timing, eight-repeat
native controls and complete fresh acceptance; no publication/speedup claim yet.

089 first paired timing completes0 but contradicts a speedup claim: ordinary
P7/candidate median30.3815/61.7125ms, sampled32.520/61.143ms,
private30.253/30.1045ms. Both ordinary p95 remain approximately62.6ms.
Sampled thread CPU406.250/453.125ms over active wall530.327/624.596ms.
These twelve-operation runs are not physical click latency or causal proof,
but the regression cannot be silently accepted. Identity-checked delivery
controller11236 is explicitly stopped before any publication child starts;
independent full verification continues. P7 remains installed, no rollback or
runtime change is made. Retain this failed performance expectation as evidence.
Source/instrumentation review and a bounded same-input cost/control comparison
must distinguish candidate cost from environment/observer quantization before
any performance disposition or publication. No repeat-until-green gate.

The first stop check correctly refuses because the controller owns a conhost,
not a publication child. Follow-up reads exact PID/start, confirms only its
conhost and no delivery.process.json, then stops only controller11236. Recorded
held-for-performance.json proves no publication; independent verification and
its runtime children are not stopped. One predeclared four-run sequence
candidate-1/baseline-1/baseline-2/candidate-2 waits that actual verification
identity and exit0 before it starts. It uses sealed exact P7 and candidate
image/MAP pairs, identical public input probe/all12 operations and all3 desktop
environments, no test/runtime source edits or changed assertion. Every run is
retained whatever its timing; no automatic publication or acceptance-by-retry.

Full acceptance subsequently fails hook-Win16, not timing: guest-report556
bytes instead of600 decodes complete phase0 and4 only. Final errors1,
freed1, zero created/queried/nested/veto/destroy/unhook. Original authored NE
source shows exactly this path when SetWindowsHookEx returnsNULL; report and
class-history writes complete and launcher exits0. Exact paired MAP dispatch
shows WU32SetWindowsHookEx rva21A50 returns0 for task17BF/native thread43336.
This is not a window callback crash or a menu-capacity assertion. Native20,
original30, control/dialog/common-dialog3-policy and hook-Off passed before
the failure. Full verification exit1 remains failed; fixed-order controller
correctly refuses to start any control because that prerequisite failed.

Original wuhook.c WU32SetWindowsHookEx has two failure owners: original hook
stub allocation (W32GetThunkHookProc), or native SetWindowsHookEx with module
and THREADID32 target. The existing trace retains neither result stage nor
native error/target identity. Next finite diagnosis is one copied original
host thunk object with transparent imported-call wrappers around those two
owners, logging actual return/LastError/target/current identities to the
existing dispatch trace's sibling file. Original function bodies, arguments,
native call/return and error are preserved; copied diagnostic DLL only,
original mirror/guest and production outputs remain unchanged. One complete
Off/Win16/All group, exact image/MAP/source identities, no assertion relaxation,
no repeated-until-green or attribution of old common-dialog risk is admitted.

Hook diagnostic-r1 captures all3 failures: original stub succeeds with
module6a180000, callback6a1a69b0; native WH_CBT target equals current thread
44024/50136/21064 respectively, returnsNULL/error126. Task17BF is consistent.
This rules out full original stub slots or a wrong numeric target in those
runs; it does not establish why the named module registration fails. Copied
DLL FAAF0679173A83293E0E0496BD1CD1EA411CD4182C7F1580570FDF1EC963C02C,
MAPBE62D9021242BD1E3276C183F7D2883DA4656F37825A374ABB2E17951145E83B.
All current production/source/object hashes remain unchanged.

Independent menu-hook-native-control-r1 succeeds with current-thread WH_CBT
both NULL module and named own module (actual file path79chars); its own
global/local atom add/delete also succeeds. Thus broad host Hook/atom resource
failure is not established. Original client/clhook.c queries a nonNULL hmod's
filename; kernel/hooks.c registers that module via GetHmodTableIndex and maps
failure to ERROR_MOD_NOT_FOUND. This is a source lead, not a claim that modern
USER internals have identical implementation or that a hook should drop its
module argument. No replacement is selected.

Follow-up module-identity observer keeps native call/return unchanged and adds
GetModuleFileNameW, GetModuleHandleExW(FROM_ADDRESS|UNCHANGED_REFCOUNT) and file
attributes after it. Diagnostic-r2 controller fails before compilation due to
empty PSCommandPath under a dynamic ScriptBlock; retained as runner failure,
not product evidence. R3 invokes the saved mechanical generated script instead
and runs a fresh three-policy capture. Original mirror/guest and production
outputs stay frozen; no module load/global hook or helper window is introduced.

R3 captures3/3 native errors126 again. From-address lookup succeeds and returns
the same6A180000 module as original HkData; GetModuleFileNameW succeeds with
the actual30-character staged WOW32.DLL path, and file attributes20 prove
the file exists at that instant. Target=current27200/2732/48220 and original
stub allocation succeeds. Diagnostic DLLFEBCE4F4E7AB39CFC8DB48A9FA98D979026D71B1BE0A30DB5C38C50045D13CEE,
MAP463BFFF45E50B3877549C7ECFCF25FF0D5CCAD1B095B6AA1FDD3D6C394A1179A.
Thus neither missing file, wrong callback-module identity nor wrong target
explains the error. Current native control success is not proof that all
module/loader paths are healthy. The failure stage is established, its deeper
cause and whether089 introduced it are not. Do not label a passing P7 history
or diagnostic perturbation as a bisect. Next comparison must keep immutable
P7/candidate image identity and separately test named DLL registration without
the WOW dependency closure; no production hMod=NULL workaround is selected.

At this safe pending state, menu-projection-verify-r1 is terminal failed,
fixed order control terminal refused (no measurements), publication controller
explicitly stopped before publishing, and both scoped diagnostic groups are
terminal captures, not acceptance passes. Product source remains the complete
unpublished089 candidate; installed/release13 remain exact P7. No original
guest/mirror modification, new semantic concession or goal closure is claimed.

###090 native Hook module boundary: source and controlled diagnosis

Menu-hook-identity-controls-r1 runs the predeclared alternating six image
cases; exact P7 NTVDM03261634/WOW32A8E5C8F8 and089 NTVDM35A7CBBB/
WOW325AD48DBA all fail the same556-byte phase0/4 contract in Off/Win16/All.
This proves the current failure is not exclusive to089, not its introduction
date or the cause of an older isolated failure. Menu-hook-dll-control-r1 then
loads the same independently authored native DLL as probe.dll and WOW32.DLL:
both native WH_CBT installations fail126 without any VDM dependency. Canonical
backslash path control also fails; exact same EXE/DLL bytes copied to an owned
C-volume temporary directory fail too. Only those three copies and the empty
directory are removed; original build evidence is retained. O: is a physical
fixed NTFS volume, not a SUBST/network mapping. No matching project module
block appears in scoped CI/Defender logs. Loader census examines409 processes,
finds no project WOW32/test DLL host; this is not a kernel table inspection.

Original ntuser/client/clhook.c obtains a nonNULL module's full filename;
kernel/hooks.c always calls GetHmodTableIndex for it, even for a local thread.
kernel/libmgmt.c uses the CLIBS32 session module table and outstanding process/
Hook dependencies. An archived Microsoft-authored diagnosis identifies error126
when32 distinct Hook DLL paths occupy that resource, with retirement requiring
all dependencies and loads released. Original URL is unavailable; author/date
and mirror provenance are explicit, no executable source is imported:
[Alejandro Campos Magencio, 2010-04-09](https://www.sysadmins.lv/retired-msft-blogs/alejacma/setwindowshookex-fails-with-error-errormodnotfound.aspx).
This supports a historical mechanism, NOT proof that current Windows has
exactly32 occupied entries or the same private layout. No kernel decoder,
resource reset, unrelated process kill or relogin is selected.

The modern public [SetWindowsHookEx contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa)
specifies NULL module for callbacks associated with the current process's
threads. Original WOW always supplies its fixed stub DLL, even for those local
requests. Select a clean input-boundary adaptation, not retry-on126: for a
verified current-process target and callback in the binding's own permanently
loaded provider module, passNULL to native USER. Keep every other parameter,
original Hook state/stub/conversion/order/retirement and native result/error.
Global target0, foreign/unknown/dead threads, mismatched callback/module and
NULL original module forward unchanged. No filename/program special case.

The binding validates module identity through public GetModuleHandleEx
FROM_ADDRESS|UNCHANGED_REFCOUNT, comparing actual module handles for callback,
supplied module and binding. Current thread needs no lookup; another target
uses an owned query-only native thread handle/GetProcessIdOfThread through the
one native call, then closes it without changing that call's LastError.
Failed metadata queries do not manufacture errors: forward original inputs.
Original nt_bop.c retains hWOWDll after successful initialization and frees
only failed initialization paths; hooks belong to tasks retired before worker
exit. Thus this specific provider code lifetime is already owned, not a new
module cache or untracked unloaded callback. Other modules never lose native
module validation/retention. No new cross-process priority/semantics waiver.

Admit090 as the finite existing S4 Hook/native-resource boundary gap paired
with unpublished089. Expected group: one narrow native binding/header, existing
private redirect header and provider source manifest, source registration,
independent argument/lifetime/error controls plus actual Win16 hook3-policy
and retained native/guest/runtime floors. No guest/mirror/wire change. Seal089
source/images/MAP/imports before rebuilding; pending P9 remains unpublished.
This supersedes the earlier no-selected-hMod-change investigation status only
for the source-defined local provider boundary, not global/foreign requests.

090 complete source group is implemented before compiling: narrow owned C/
header, existing private API redirect, provider source manifest and independent
24-call boundary fixture. The original wuhook.c/wowhooks.c bodies are unchanged.
Same-module native identities, current/live sibling thread ownership and
query+synchronize handle lifetime are verified before normalization. A dead
held thread forwards unchanged; native USER remains final liveness authority.
No atomic native-desktop snapshot is claimed. Temporary query handles close
after the single native registration with its returned error restored.
Original hMod remains in HOOKSTATEDATA; only that one native API argument is
adapted, so original conversion/CallNext/unhook/task cleanup still see their
real original module/stub identity. Process-owned provider lifetime remains
the original successful nt_bop load. No repeated failure fallback is added.

Pre-hook089 source/images/MAP/imports are sealed. Local-hook-build-r1 starts
one owned complete group; native-boundary passes errors0/calls24, current+
sibling17, global/invalid/null/mismatch/null-callback/dead/foreign one each,
saved/native error propagation and handles172/172. This unit uses a recording
native-call seam, not a USER implementation; actual Hook3-policy and complete
native/guest lifecycle remain required before a runtime claim or delivery.

Local-hook-build-r1 worker/provider link succeeds (provider135 actual required
targets after the private-header/source-list change). Actual unchanged CBT
guest passes Off/Win16/All with original phase0/1/2/4, errors0, created/queried4,
nested1, vetoed1, destroyed3, unhooked1 and freed1. This proves local provider
Hook execution/conversion and retirement under the actual three-policy entry,
not global module-table capacity, all cross-task Hook forms, historical error
causality or complete S4. Full composed checks remain live; performance and
remaining release gates must run after this new source group completes.

Local-hook-build-r1 then completes0, including original task/reply/process,
extension/input/alias/external-memory, borrowed/reclaim/graph/class/native-
nested/start/menu-resource and init/modal rollback. NTVDM remains exact089
35A7CBBBE10E82C928D9A228A1E81C9534D623114263D3050C12E8419E622307,
MAP5B987527F14CF7A21F437FBE671AF0973EDE0EBA480E1EAB01C8E44CD50339AA.
WOW32 is510E9DAC0CA8E01FB908D85F82A77B90E9EED7BC13F3EE4D764E3A7A8FD7B79C,
MAP113092864703158E5F8E6D178DB89C399FAC0D167DD4AD308C78DC7D80E976AD.
No diagnostic logging object is in that production DLL. Actual group is0;
prior089 acceptance/performance expectations remain failed and are not waived.

Local-hook-verify-r1 is the fresh owned complete controller. After exact build
identity/source/image checks it copies a sealed candidate image/MAP pair,
then uses verify-wow-paired-latency's predeclared baseline/composed/composed-
repeat/baseline-repeat ABBA order:12 actual posted flags in ordinary, sampled
and private desktop per run, unchanged pixel polling/retirement assertions,
EMS control per run and all source/image identities. Every result is retained;
completion does not decide speedup or authorize publication. It then requires
both8/8 nested controls, fresh full native20/original30/actual-guest/application/
DOS/modal/logon and distinct-version thread-extension3-policy plus quality.
No automatic publisher is running. Original fixed-order089 controller remains
refused, not restarted against changed sources. P9 requires explicit actual
performance review and complete delivery/installed/commit/push gates afterward.

090 second-role review confirms same native resource/Hook chain owner and
original callback state remain. The public ownership check is call-local,
not another task registry. Callback and binding must inhabit the same actual
provider; arbitrary DLLs keep original native module retention. Sibling native
thread queries are read-only plus zero-time synchronization, with one native
registration and exact error restoration. Global/foreign/unknown/dead inputs
are unchanged, not replaced by process-wide listener emulation. The source
contract and observed local failure justify the adapter independently of an
unproved modern32-slot-table diagnosis. No mirror/guest diff or new observable
semantic waiver is selected. Full mixed/cross-task Hook and message evidence
is still part of the unchanged S4 inventory, not inferred from the CBT3 pass.

### Source-only M4 reconciliation while090 verifies (no runtime admission)

Retain the full original conversion group rather than substitute a universal
marshaller: wmdisp32.c W32Win16WndProcEx selects aw32Msg, calls the original
THUNKMSG body, executes CallBack16, then calls UNTHUNKMSG even when that callback
returnsFALSE, before its existing fallback. The selected window entry validates
its immutable callback/target before reaching that body. A source branch with
zero vpWndProc16 is not itself proof of an integration leak; trace the actual
caller contract before changing it. Original WM32GetMinMaxInfo and
WM32WindowPosChanging own stackalloc16/copyback/stackfree16. WM32CopyData instead
owns GlobalAllocLock16 and original CopyDataAddNode/Find/Delete pairing; its
reverse wmsg16 path owns separate free_w storage and COPYDATA_16 provenance.

The existing wow_callback_frame_lease only bounds a guest-memory alias and
commit, not these converter allocations. Native-call/borrow finally scopes
restore execution and object references, not automatically every original
message allocation. Do not use their passing fixture as proof of M4's entire
buffer-retirement contract or add a competing allocation registry. Original
WU32ReplyMessage explicitly warns that flat16-bit aliases can be invalid after
reply and releases its parg alias accordingly. That warning alone does not
prove any specific native lParam owner is lost: message-specific input/output,
early reply, source direction and converter provenance must be reconciled.

Thus preserve the imported converter bodies; M4's complete structure/movement/
reply/timeout/failure group remains an explicit successor source-and-real-guest
obligation within S4 after this safe baseline. Current text tests prove only
their named strings/results. No converter fix, original-host defect attribution,
new mirror diff or M4 completion is inferred by this source-only review.

### Fixed ABBA timing disposition for089/090 (not a stutter-closure claim)

Local-hook-performance-r1 completes its predeclared four runs, all twelve
posted flag toggles per environment, EMS and normal window/task retirement.
Normal medians in baseline/candidate/candidate/baseline order are30.858,
30.901,30.6135 and61.3985ms; sampled medians30.8635,34.006,34.4335 and46.121ms;
private medians30.3285,30.394,30.380 and30.5485ms. Normal per-run maxima
(the12-sample p95 convention) are62.778,63.808,63.628 and94.153ms.
The two candidate runs do not reproduce089's earlier ordinary62ms result,
but the two baseline runs differ substantially. These observations establish
neither a causal speedup nor elimination of the owner's physical-input stutter.
Retain every earlier result, including the failed089 expectation.

Second-role performance disposition permits continuing the remaining release
gates:089 removes demonstrably unused copied fields while preserving the
original refresh points/generation/order/errors;090 repairs a separately
reproduced native registration boundary. The fixed comparison shows no
consistent candidate regression; it is not a speedup acceptance criterion.
The owner's authorization to retain correct optimizations that are not the
principal bottleneck applies. Publication still requires the fresh complete
regression, installed verification and reviewed commit/push, not this table.

Source review also bounds the witness itself: wow_mine_latency_probe calls
the existing wait_cell, which reads pixels through BitBlt/GdiFlush and sleeps
20ms between failed observations. It posts paired messages rather than
physical input. Its roughly31ms floor cannot be treated as measured guest
execution time or evidence that all remaining physical delay is in WOW.
Keep this unchanged acceptance witness for comparison; any finer attribution
must independently account for capture/polling and native controls, not alter
runtime waits or re-label the current samples. No additional runtime edit,
new timing result or full M6 completion follows from this source observation.

After that explicit performance review, owned local-hook-release-r1 waits
the pinned local-hook-verify-r1 PID/start and requires its whole-group exit0,
including the later thread-extension and quality gates, before invoking the
existing coherent13 publisher and installed checks. It cannot publish from
the earlier089 failed aggregate or just the four timing passes. Review is
recorded separately in local-hook-release-r1/performance-review.json; original
sealed performance evidence is unchanged. Delivery remains pending while
those prerequisites run; current installed images are still P7.

### P9 complete grouped verification, publication and second-role review

Local-hook-verify-r1 completes0 after local-hook-build-r1's native24 and
composed checks. Fresh foundations pass native20, original30, both-width
style/MENU/resource boundaries and all12 real Win16 control/dialog/common-
dialog/Hook groups. Common-dialog retains16 rounds in every policy. Default,
native-extension and task-version/thread-extension messages pass all3 styles
(nine groups), with normal retirement. WINMINE and SOL gameplay/normal exit
pass3 policies each; WRITE's four-sample main/document/page-info startup
frontier passes3 policies, not editing/save/normal exit. Console17/Window17,
direct modal3 and complete-token logon/gameplay3 also pass. Nested baseline
and candidate8/8 controls pass; source/MAP/image identities remain frozen.
The fresh documentation/parser/diff quality gate passes. These are bounded
runtime floors, not proof of all M1--M6 or S4 completion.

The separately recorded fixed ABBA disposition authorizes retention without
claiming a speedup. Local-hook-release-r1 waits for the whole verification
exit0, then local-hook-delivery-r1 completes0: publication, both-width deployed
NTW16S boundary and installed ordinary-logon/gameplay Off/Win16/All pass.
Publication preserves27 recoverable prior files; stopped-processes.json is
empty, so no existing owner session needed closure. Exact13-image hashes
agree at assets/release and O:/winnt/system32. Only ntvdm.exe and WOW32.DLL
change; the other11 retain P7 identities. NTVDM is
35A7CBBBE10E82C928D9A228A1E81C9534D623114263D3050C12E8419E622307;
WOW32 is510E9DAC0CA8E01FB908D85F82A77B90E9EED7BC13F3EE4D764E3A7A8FD7B79C.
Original guest, assets/winnt.zip and user configuration are not replaced.

Single-person second-role review covers the actual source/test/build diff:
089 shares the old traversal and full-view implementation; only the existing
menu consumer selects12-byte rows. No fresh-state suppression, parallel
index or menu algorithm is added.090 performs one unchanged native registration,
after verifying actual module and live local-thread ownership; its temporary
query handle closes with native return/error preserved. Native USER remains
Hook/resource authority; original WOW stubs, state and conversion remain.
The seam fixture is explicitly not a replacement for real USER16 acceptance.
No new mirror file/content difference or guest mutation: selected192 exact/
86 registered content differences (278 total) are unchanged. Old0102, prior
child-exit and GDI16/19 risks remain unlocated, not erased by these passes.

Reviewed source/tests, coherent release assets and manifest constitute bounded
S4 P9. Final documentation quality and main/origin synchronization are recorded
by local-hook-final-quality-r1 and local-hook-git-delivery-r1, including live
remote equality, clean worktree and both deployed13 identities. This does not
close S4/T447 or the persistent goal. Next source-led attribution must separate
observer/capture time from actual input/conversion/rendering; full remaining
implicit/cross-process messages, converted-data/reply lifetime, failure/death
and M1--M6 reconciliation remain in this S, with S5/S6 unchanged.

### P10 observer-cost source audit and fixed native/guest attribution

Question: does the P9 witness's roughly31ms floor measure WOW work? Source
shows wait_cell captures through BitBlt/GdiFlush then sleeps20ms after a
nonmatching hash. Add test-only T447_OBSERVER_COST leaf brackets, reset per
operation and snapshot immediately at the original stopwatch end, before the
extra verification capture. Original helper bodies, pixel coordinates,
twelve posted down/up pairs,20ms request and normal-exit assertions remain.
Default compilation has no new instrumentation. Native API arguments,
success/failure results and incoming/native-return LastError are preserved;
five mock leaf calls assert exact QPC accounting and reset. This is observer
unit evidence, not a native USER or original converter replacement.

verify-wow-observer-cost seals exact P9 thirteen images and paired MAPs,
canonical guest ZIP, observer/compiler and all diagnostic sources. Actual
project ordinary/sampled/private observations use the same installed P9 bytes
in isolated packages and complete logon environment, never a diagnostic
runtime DLL. The independently authored native control is only a repaint/
posted-pair reference for the same sampled rectangles, not an implementation
of WINMINE or an equivalent CPU workload. It requires all12 paired inputs,
twelve visible changes, restored initial pixels, at least13 paints and normal
close. Per-operation original and diagnostic QPC totals must agree, native
leaf intervals are nonnegative/nonoverlapping, capture/flush counts agree and
sleep count equals capture count minus one. No timer resolution or system
animation setting changes; all objects/build outputs stay in owned test roots.

Observer-cost-r1 passes native-leaf controls and all36 actual project samples,
but fails before native control readiness; its aggregate remains1. Do not
reuse it as a completed comparison. Source review identifies inherited
STARTUPINFO hide policy as a possible test-window visibility issue. The
reference now explicitly shows only its own window with no activation through
SetWindowPos(SWP_SHOWWINDOW), records before/after visibility/startup facts,
and reports both owned process exits on readiness failure. Fresh r2 proves
the cause: both native controls report visible-before0, visible-after1,
startup-flags257 and startup-show0. Both finish down12/up12/paints14/flag0.
No product/window behavior is changed by this test-fixture correction.

Observer-cost-r2 completes0: unit controls,36 actual P9 and24 native
observations, all original twelve-toggle/pattern/normal-exit checks, exact
source and both deployed13 identities. Median values are microseconds:

| Reference/environment | Total | Sleep call wall time | BitBlt wall time | GdiFlush wall time |
| --- | ---: | ---: | ---: | ---: |
| Native ordinary | 30232.30 | 28247.40 | 1339.75 | 16.55 |
| Native private | 30261.35 | 28423.20 | 1102.60 | 16.50 |
| P9 ordinary | 30090.55 | 28506.75 | 943.65 | 15.10 |
| P9 ordinary sampled | 34810.25 | 31819.80 | 1596.85 | 22.10 |
| P9 private | 30651.15 | 28503.05 | 1061.05 | 15.50 |

These are separate medians, not additive CPU shares. A capture leaf can wait
for painter/compositor; guest work can run while the observer sleeps. Thus
do not subtract28ms to claim a measured2ms guest response, or infer physical
mouse smoothness. Strong conclusion: this witness's approximately30ms floor
also exists without WOW and is dominated by observation sleep. It cannot
resolve the remaining cost below that window. Preserve P9's actual timing
values and every failed aggregate; no past speedup conclusion is rewritten.

Second-role review selects a bounded next measurement, not a runtime Sleep
change or guessed optimization: timestamp the existing native-to-WOW callback
entry/return in a reversible, test-only project-provider copy, correlating
the exact input HWND/message/QPC with the external posted-input stopwatch.
Keep production/original bodies untouched, validate nested/failed/error
instrumentation and read-only record identity, then compare exact P9 against
the diagnostic copy. This can separate queue-to-entry, callback and later
observation intervals without a faster busy-poll loop; it does not replace
M4's full original data-lifetime audit or other M1--M6/S5/S6 obligations.
No callback instrumentation runtime is admitted or claimed by this P10
diagnostic delivery. First complete the default-observer noninstrumented
protocol and final quality/clean main-origin delivery; runtime stays P9.

Observer-cost-final-r1 completes0: the default (no instrumentation switch)
observer recompiles and passes normal/sampled/private twelve-toggle and
normal-retirement protocol against the sealed P9 package, without diagnostic
output leaking into the default witness. Parser/diff/documentation quality
passes. Final record updates require observer-cost-final-quality-r2; the
reviewed nine source/test/document paths are the diagnostic-only S4 P10,
not product changes. Observer-cost-git-delivery-r1 records committed/live-
remote equality, clean status and unchanged13 images at both release targets.
No deployment is needed because no runtime image changed. The normal native
reference is slower than the project sample within observation noise; that
does not prove the emulator is faster than native code. The supported
conclusion remains measurement-floor attribution, not stutter resolution.

### P11 callback source boundary, immutable recording and phase attribution

Question: where does actual mouse handling spend time, independently of the
observer's20ms wait? Source chain is native immutable entry ->
wow_window_dispatch_immutable -> dispatch_bound -> original
DispatchClientMessage/W32Win16WndProcEx/CallBack16, with existing task/borrow/
projection scopes. Original WU32DispatchMessage routes the guest message
through native DispatchMessage. Trace this actual production entry, not a
test-supplied WNDPROC or an inferred receiver identity.

verify-wow-callback-cost mechanically copies only the owned words-binding
translation unit, renames its one public entrance to a private body and
adds a transparent wrapper. Version2 additionally brackets only the existing
guest DispatchClientMessage statement; marker removal and name restoration
must exactly recover normalized original text. No original statement, mirror,
guest, production source, stable object or published image changes. The exact
Ninja compile/link commands and all selected inputs/dependencies are sealed;
only the copied object replaces its original in an isolated diagnostic DLL.

Native data is append-only64 slots, uniquely reserved atomically with no
reuse. Each writer publishes immutable native HWND/message/thread/QPC/result/
error/normal-return fields with a final interlocked completion. Frequency is
atomically initialized. The diagnostic TLS active slot is masked/restored
through every nested wrapper, including untracked messages; phases have one
writer and must occur once in order before accepted actual rows are used.
The finally path preserves the exact BOOL result, original output, native
LastError and propagated exception. It never dereferences rejected/exceptional
output pointers. This is diagnostic state, not a production object owner,
message transport or scheduling input.

The reader opens only the pinned worker read-only, locates the exact WOW32
module/RVA inside its image range and checks ABI/version/capacity/frequency,
stable reservation count, complete records and phase ordering. Readiness
waits occur after all timed intervals. Twelve right-button pairs plus three
left-button reveals (F2/reset outside each interval) require30 callbacks,
actual observed full HWND/native thread and fifteen matched stopwatches per
environment. Unexpected identities/counts/order are failures, not filtered
into a pass. The existing default flag witness remains unchanged when the
new switches are absent. Native Windows owns all message delivery and drawing.

QPC cross-thread/process comparison follows the supported host guidance with
one-tick ordering uncertainty, not CPU-affinity changes or RDTSC. See
[Microsoft's QPC guidance](https://learn.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps).
Same-thread phase intervals partition the wrapper; they are inclusive wall
times, not exclusive guest CPU. Observation-after-return may be negative:
pixels can change before mouse-up/callback completion. Preserve those values.

Callback-cost-r1 completes0 with90 actual rows/45 pairs and recorder/reader
controls plus parser15 positive/seven negative cases. The normal flag median
queue-to-entry is17.885ms, callback span26.911ms, and nine of twelve pixel
observations precede mouse-up completion. This is a new execution-level fact,
not a causal speedup. Before version2, callback-cost-source-seal-r1 preserves
every changed test input with original/sealed/hash mapping. R1 is not rerun
against newer headers or relabelled as phase evidence.

Fresh callback-cost-r2 completes0 with reversible phase marks, native unit
nested/rejected/exception/untracked/result/error/overflow controls, nine
rejected record states, three rejected module/process/range identities and
equal133/133 handle counts. Parser controls prove fifteen matched pairs,
signed negative lag, nested intervals and ten rejected identity/state/phase
cases. Exact P9 and diagnostic copies run the same45 input pairs across
ordinary/sampled/private environments; all90 diagnostic callbacks match
their real HWND/thread, one complete phase and input stopwatch. Normal task/
window/launcher completion and board restoration pass. Separate per-component
medians below are milliseconds, not additive CPU shares:

| Environment/input | Before callback entry | Down pre / dispatch / post | Up pre / dispatch / post |
| --- | ---: | --- | --- |
| Ordinary flag (12) | 18.126 | 3.683 / 3.151 / 0.308 | 3.530 / 15.220 / 0.312 |
| Ordinary reveal (3) | 23.422 | 3.432 / 7.728 / 0.263 | 3.234 / 17.190 / 0.284 |
| Sampled flag (12) | 21.059 | 3.076 / 3.434 / 0.315 | 3.274 / 12.655 / 0.276 |
| Sampled reveal (3) | 25.680 | 3.525 / 9.033 / 0.374 | 3.983 / 15.017 / 0.279 |
| Private flag (12) | 3.627 | 1.006 / 1.923 / 0.181 | 1.023 / 5.010 / 0.182 |
| Private reveal (3) | 3.306 | 1.221 / 3.046 / 0.182 | 0.894 / 13.605 / 0.182 |

Thus final callback cleanup is sub-millisecond here, not the dominant tail.
The dispatch phase includes original WOW/guest work AND native-service
adaptation/publication within those calls; this is not evidence blaming the
unchanged Microsoft algorithm. Random reveal depth, small sample size,
desktop differences and tracing overhead prohibit causal performance claims.
Pure pixel timing is especially insufficient because it can precede completion.
Source follow-up targets the repeated copied-window transaction inside native
returns, retaining all six message groups and M4 original converter obligations.

### Source-proved prepared-graph redundant lookup (successor, not runtime repair)

Current page_domain_replace_window_links builds its call-local updated index
from the complete captured graph and owns prepared/backing pins before edge
commit. In every five-identity row, it nevertheless calls the old published
page_domain_window_lookup first, then overwrites that result whenever the
same full identity has a prepared update. The lookup's only ordinary work is
slot validation plus c_sas_dw_at of the published handle; it owns no lifetime
or mutation. The staged identity/backing already won by design, including
fresh replacement generations. The discarded read repeats for self and every
known relation at every publication; its individual cost is not yet measured.

The smallest successor design computes the existing replacement predicate
first and selects prepared backing directly; only a genuinely unstaged/legacy
operand calls the unchanged original lookup. Keep all prior full-generation,
duplicate/withdrawn/graph validation, acquired pins, rollback and atomic commit.
Do not add a cache/index/counter or alter freshness/scheduling/guest. Original
USER's retained-object validation likewise does not rediscover a known locked
object; importing more USER code cannot remove this project-owned redundant
lookup. This is source-proved unnecessary work, not yet a proved stutter cause
or permission to change unrelated SAS/class stores. Seal and deliver the P11
diagnostic group first, then admit and verify this complete graph-resolution
group against the existing real CCPU generation/partial rollback/retirement
and fallback contracts, paired timing and every-P gates. Runtime stays P9 now.

Callback-cost-final-r1 completes0 after the complete r2 group: the current
default observer, without any new switches, recompiles and passes all three
environments, twelve flags, restoration and normal exit with no callback/open/
observer instrumentation output. Frozen current inputs and sealed r1 sources
remain exact. Parser/diff/documentation quality passes. Single-person second-
role review retains this P11 as diagnostic-only: one bounded native recorder,
transparent scopes, strict read-only identity/phase validation, preserved
signed/nested timing and unchanged default protocol; no runtime capability
or stutter closure is inferred. Final record checks and main/origin delivery
are retained in callback-cost-final-quality-r1 and callback-cost-git-delivery-r1,
including live remote equality, clean status and unchanged13 images at both
destinations. No test-only DLL is published. Full S4/M1--M6 and S5/S6 remain.

### Prepared graph resolution, ADAPTER-WOW-091 / prospective P12

Admission follows clean diagnostic P11 ad7da140e. This bounded owned adapter
change selects an existing prepared full-generation backing before, rather
than after, the legacy published-slot lookup. It removes only the discarded
read; unstaged/legacy lookup, acquired pins, full-identity equality, duplicate
and withdrawn checks, graph validation, atomic publication and rollback are
unchanged. There is no new cache, scheduling policy or ABI. Original-source
and facade rungs cannot remove redundant work inside this project-owned
transaction; no original mirror or guest is edited. README exception091
records the generic-mechanics disposition. Mirrors remain192 exact/86
registered (278 total), with no added mirror divergence.

prepared-graph-sealed-r1 preserves P9's coherent13, parent EXE/MAP/import
library, complete provider object/link inputs and relevant sources before the
build. prepared-graph-build-r1 completes0: five incremental worker targets,
provider Ninja no-op, unchanged export definition. Fresh graph-snapshot,
native-nested, input-lifetime and external-memory groups pass. Actual CCPU
rows additionally assert all five graph links against the submitted complete
view, marker WOW_PREPARED_EDGE_RESOLUTION_CCPU errors=0 rows=4 fields=5
staged=1 full_generation=1, alongside prior generation/atomic/rebase/retirement/
reuse and 32 partial-rollback checks. Omitted focused groups are not declared
fresh passes; the whole native/original/guest/product gates are separate.
Candidate NTVDM SHA256 is
6BB27895E269A3244468B13AA6FEF425F4C2A9185C6189820C00E8144BACBC96,
MAP946D29FC765922855631DFA6B5B49A254AA777108C55982603CFA33291C09C6E;
WOW32/MAP remain P9's510E9DAC/11309286 identities.

Input selection adds only opt-in coherent build/release ImageRoot to the
callback diagnostic. prepared-graph-image-inputs-r1 rejects all four negatives
before compilation/runtime: outside scope, duplicate manifest, corrupt image,
and mismatched selected image/MAP producer. prepared-graph-performance-r1
completes fixed ABBA baseline/candidate/candidate-repeat/baseline-repeat,
three environments each, 45 measured pairs/90 callbacks per run,360 callbacks
total, plus each original uninstrumented workload. No retry or best-run
selection. Ordinary flag median entry/up-dispatch/callback-span ms:
baseline30.415/18.107/42.381; candidate24.354/14.944/34.294;
candidate-repeat31.329/20.682/48.029; baseline-repeat19.949/14.724/36.626.
Private flag up-dispatch/span respectively6.184/13.938,6.225/14.645,
6.362/16.530,5.396/13.889. Reveals have only three random-depth samples per
run. Results are mixed, not stable speedup, statistical noninferiority or
root-cause proof. Owner permits retaining correct non-primary optimizations;
this retention rationale is source-proved redundant-work removal, not a
claim that these timings establish a benefit. Functional delivery gates
remain mandatory; no known regression is waived by this interpretation.

prepared-graph-input-review-r1 separately audits all actual diagnostic link
commands: the absolute shared export-definition path has exactly the sealed
producer's58D29066 content, actual parent import paths select the intended
old/new worker, all recorded inputs and226 sealed provider objects are
unchanged, and instrumentation remains reversible. This accounts for actual
inputs rather than pretending Ninja's absolute /def path was rewritten.
prepared-graph-nested-r1 passes both old/new eight-round controls. Full
prepared-graph-verify-r1 remains running at this record; no new publication,
S closure or full-goal completion is claimed.

####091 full-gate refusal, preserved failure and bounded attribution

prepared-graph-verify-r1 now exits1, not a passing group. Native20/20,
both-width style, original30, resource/menu guards, controls3/dialog3 and
common-dialog Off/Win16 pass. common-dialog-All then fails its first normal
destruction round: records0/1/1, errors0102, created1/closed0/hooks1,
task17BF/dialog11EA, guest DestroyWindow AX1 and local IsWindow AX1. The
original WU32DestroyWindow at RVA25910 returns1 in dispatch.txt. Resource
unlock/free and exact task retirement occur afterwards; they do not prove
the earlier native/copy/cache state. Later Hook3/message/product/DOS/modal/
logon/extension/quality gates were not reached, not implicitly passed.
prepared-graph-delivery-after-verify-r1 exits1 on the complete-group guard
before invoking publication. Both destinations remain coherent P9; no
backup/replacement, runtime commit or P12 delivery has occurred.

prepared-graph-commdlg-control-r1 completes one fixed oldP9/candidate/
candidate-repeat/oldP9-repeat All-style diagnostic,16 rounds each. All four
produce19 records,17 creations/16 normal destructions/17 resource Hooks and
one exit cleanup; assertions remain unchanged. This is non-reproduction,
not a successful rerun of the failed whole gate or evidence of repair.
Do not repeat identical controls until green or classify equal0102 report
values as equal causes. Its four diagnostics do not authorize publication.
All owned controller sessions have finished; failed evidence is preserved.

Source revalidation uses canonical original windows/core/ntuser/client/wow.c
IsWindow and the supplied wow16/user/k/daytona/wowk.asm/usercli.asm:
ValidateHwndNoRip may use the thread's callback pair; WOW IsWindow additionally
checks HANDLEENTRY.HANDLEF_DESTROY. kernel/handtabl.c HMMarkObjectDestroy
marks logical death while locks defer physical free; HMFreeObject later
clears the handle and advances uniqueness. Existing copied withdrawal writes
FREE metadata00010000 and preserves a retained callback WND. That alone is
not proof the failed report had an active cache or a freed rather than
replacement identity. The current native fixture deliberately verifies
retained callback access after withdrawal/republication; it does not exercise
the original local IsWindow decision at the failed guest boundary. Preserve
that distinction rather than speculatively clearing a live callback cache or
setting a guessed destroy flag.

The prior failure-only diagnostic and D001 control above already capture
native target/liveness, copied full identity/metadata and guest-selector cache
at the exact report boundary. They must be reused/reconciled with current
source if further failed-boundary evidence is needed; another broad tracer
or identical replay framework would duplicate established work.091 remains
unpublished pending this finite lifetime gate, not a new performance claim,
unbounded S4 expansion or request to waive an assertion. M1--M6/S5/S6 and the
full persistent goal remain active. No original mirror/guest/ZIP changed.

### M4 authored structure group: original ownership, not a replacement converter

The previous goal turn made progress by exposing a whole-gate refusal and
preserving P9; it was not a successful delivery or a verified wait after its
controllers exited. Revalidate the worktree and continue the full S4 objective.
Do not allocate a narrower performance-only goal or repeat identical0102
probes. Source tracing now distinguishes USER16's usercli.asm pushINT_LONG
argument from a native full HWND: IsWindow passes a sign-extended SHORT,
ValidateHwndNoRip compares the entire argument against the callback cache,
and its fallback accepts uniqueness0/FFFF while checking actual entry type.
Thus a full cached HWND with other uniqueness cannot be blamed for this
specific short-argument fast path. The failed trace lacks that full identity;
this does not universally rule out a cache problem or justify a cache clear.

One correction to the earlier allocation ledger is important: original
wcall16.h defines stackfree16(vp) as an EMPTY macro. walloc16.c stackalloc16
reserves from the current task's vpStack/vpCBStack and clears
TDF_INITCALLBACKSTACK when starting the reservation. Original CallBack16 then
reserves an aligned CBVDMFRAME, stores the prior vpStack in that frame,
reenters the guest, and restores vpStack from the completed frame; the API
thunk's actual return restores guest SS:SP. This is the existing temporary
stack owner, not a separate heap allocation/free pair. Do not add a competing
allocator or infer a leak just because stackfree16 does nothing.

| Family/direction | Actual original owners | Verified behavior and retained limits |
| --- | --- | --- |
| MINMAXINFO16,16->32 | wmsg16.c ThunkWMGetMinMaxInfo16/UnThunkWMGetMinMaxInfo16, selected by ThunkWMMsg16/UnThunkWMMsg16 | All ten signed coordinates arrive at native input; first/last output fields and native returned result copy back. |
| MINMAXINFO,32->16 | wmdisp32.c WM32GetMinMaxInfo, original CallBack16 stack frame | Guest forwards the received far pointer through a nested converted sibling send, reloads it after reentry and validates copied-back values; native checks the entire final structure. |
| WINDOWPOS both ways | wmsg16.c WINDOWPOSCHANGING cases, wmdisp32.c WM32WindowPosChanging, wstruc.c getwindowpos16/putwindowpos16 | HWND short-carrier input, insert-after TOP, signed x/y, extents/flags, modified x/y/cx/flags and return values. No claim that an unchanged special HWND proves every output identity case. |
| COPYDATA16->32 | wmsg16.c WM_COPYDATA, original COPYDATA_16 provenance/free_w and CopyData node pairing | Exact dwData/cbData/eight payload bytes reach native receiver; no original global is replaced. |
| COPYDATA32->16 | wmdisp32.c WM32CopyData, GlobalAllocLock16 and original CopyDataAdd/Find/Delete | Exact scalar fields and copied far payload reach the real16-bit receiver; native source bytes remain unchanged on synchronous return. Normal success is not proof of allocation-failure/exception/task-death cleanup. |

The opt-in StructuredPayloads test switch extends only the independently
authored win16_message_probe.asm/wow_message_peer.c/runner. It adds no product
CLI, provider export, native helper in production, mirror diff or guest edit.
MSG3/MSG4 and the52-byte native report are explicit test schemas; old MSG1/
MSG2 and48-byte native reports remain unchanged without the switch. Native
endpoint windows belong only to the test peer, not a hidden product workaround.
Native calls preserve the original generic thunk and native USER send path;
no callback, converter, scheduler or allocation double supplies the result.

message-structures-r1 completes0: old P9 and candidate091 each run Off/Win16/
All with StructuredPayloads+NativeExtension+ThreadClientView. Six real guest
groups cover three structure families in each of three directions (Win16/
Win16 and both native directions). Each group asserts four new converted
callbacks in each actual Win16 task, six native direction checks, distinct
3.10/3.00 task client versions, existing explicit/nested/reply/post/filter/
quit/text/native-extension behaviors and exactly-once normal retirement.
The two additional default cases (P9 Off/candidate All) also pass, eight total.
All compilation and execution use pinned owned hidden processes, exact
source/13-image hashes and real deadlines; no failure is retried to green.

message-structures-review-r1 completes0. Both default sender and receiver NE
images in both default cases are SHA256-identical to the sealed prior
local-hook-acceptance-r1/default-Off artifacts. A mechanically derived authored
native negative changes only MINMAXINFO.ptMaxTrackSize.y from-11 to-10;
the unchanged guest checks reject it at phase2 with error8000 and runner1.
That is an expected synthetic failure, not a product regression or passing
runtime gate. Exact positive source, derived negative source and images are
pinned in the two runs' inputs/probe/runtime CSVs and process records.

Single-person second-role review: normal input/output, signed conversion,
nested temporary-stack restoration and normal retirement now have actual
structure evidence rather than two strings alone. This does NOT close M4:
physical segment movement, pointer use after early reply, timeout/exception/
allocation-failure retirement, remaining message/control-specific structures
and interrupted receiver death remain finite existing S4 obligations.091's
failed full gate stays failed, installed P9 is unchanged, and no runtime
delivery or full-goal completion follows from these test-only passes.

message-structures-final-r1 also completes0: the only changed inputs from the
earlier091 frozen build inventory are these exact three authored test files;
their old bytes remain sealed under prepared-graph-sealed-r1/source/tests/
observation with hashes equal to the earlier inventory. No historical input
CSV is rewritten to fit new sources. Production EXE/MAP/import artifacts
remain exact, both published13 sets remain P9, and current PowerShell parsing,
diff checks and documentation governance pass. The091 whole gate remains
failed, not reclassified by input reconciliation. Current changes are retained
uncommitted while that runtime delivery remains incomplete.

#### Next M4 finite owner question: input lifetime after early reply

Modern [ReplyMessage](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-replymessage)
allows both sender and receiver to continue; the documented
[WM_COPYDATA](https://learn.microsoft.com/en-us/windows/win32/dataxchg/wm-copydata)
input is read-only and must not be retained for later use after SendMessage
returns. Those contracts alone do not prove whether a particular same-process
USER delivery copied the native header or how long USER retains its capture.
Do not claim a use-after-free from documentation alone.

Original WM32CopyData copies native data to guest GlobalAllocLock16 storage,
but its normal UNTHUNK also dereferences the native COPYDATASTRUCT to obtain
the data node's Mem32 deletion key. That read occurs after CallBack16 returns,
not at WU32ReplyMessage. Existing passing scalar early replies and this normal
structure group do not exercise that interval. First establish the exact
allowed native behavior using an independently authored native receiver that
consumes input, replies, and does not touch it again; sender-owned test-page
protection plus an explicit acknowledgement makes the lifetime interval
deterministic. A native control failure is not a WOW defect.

Apply the identical owned input/barrier protocol to the actual Win16 receiver,
which also consumes everything before ReplyMessage and never reads the payload
afterwards. Keep normal COPYDATA, nested original provenance reuse and the
unchanged default cases together. CopyDataAdd/Find/Delete remains the single
original conversion-provenance owner. Record sender reply value, receiver
continuation/completion, exact protected input addresses, errors and normal
cleanup; native controls and synthetic failures are explicitly separate.
No arbitrary Sleep, private USER object decode, guest patch, fabricated
timeout success, alternate message queue or product helper HWND is admitted.

This is the next finite source-defined M4 lifetime verification group, not a
selected runtime repair or permission to change an inherited original body.
If it exposes an integration defect, finish its source owner/caller/provenance
closure before designing the smallest adaptation; do not wrap one caller and
leave equivalent entries unsafe. Original defects still follow source policy.
No automatic failure waiver, M4 completion or091 publication follows.

### M4 early reply and nested COPYDATA: composed original paths verified

The preceding goal turn was progress: three-family actual conversion evidence
plus an expected bad-copyback rejection, not merely a plan or waiting state.
Revalidate all worktree/runtime inputs before the next group. Source remains
single-person implementer/reviewer; there is no additional task or waiver.

Before edits, message-structures-seal-r1 refuses its slash-spelling comparison
even though all three SHA256 values are unchanged. r2 normalizes full paths
and seals the exact tested runner/NE/native peer and prior evidence. The r1
failure is retained; no original run/input CSV is rewritten. This is an
observer bookkeeping repair, not a product behavior repair.

Original kernel/sendmsg.c xxxInterSendMsgEx sizes/captures COPYDATASTRUCT and
its pointed bytes into SMS.pvCapture (around1249--1463). _ReplyMessage marks a
reply while the receiver can still use that message (around227--275).
The sender marks SMF_RECEIVERFREE when the receiver is still busy (1833--1856);
the receiver clears busy and unlinks only after window-procedure completion
(2199--2216). UnlinkSendListSms finally frees pvCapture (2764--2767).
This is the original owner/lifetime explanation, not a modern private USER
layout claim or permission to port the whole NT4 USER server. Existing
modern User32 remains the real message-resource owner. No original converter,
CallBack16, queue, scheduler, guest or mirror body is replaced.

The new independently authored shared test header and pure native control
exercise one deterministic protocol: receiver consumes immutable header/
payload, replies51, sender returns51, sender protects only its own VirtualAlloc
page PAGE_NOACCESS, verifies ReadProcessMemory refusal and signals its event,
receiver observes ISMEX_REPLIED and continues without rereading input. The
outer full-return boundary restores/frees only that test allocation. No Sleep,
private native object, injected runtime patch or product helper HWND exists.
Native control endpoints belong only to the independently authored test.

Native wrong-reply52 is an intentional negative: sender must NOT protect the
input, the report has nonzero errors, native exit1, and the normal report
assertion must reject it. The84-byte RPLY schema records exact sender/receiver
threads and input addresses, reply value/flags, protection/read refusal,
continuation and cleanup boundary. Public native COPYDATA input addresses are
observed before reply; actual WOW receiver-private addresses are explicitly
UNKNOWN, not inferred or fabricated. The current native control observes
different original/receiver header and payload addresses (receiverAlias0).
Protecting the original page does NOT prove the lifetime of every internal
Windows capture; the test's limits retain that distinction.

verify-win16-message-probe.ps1's opt-in ReplyPayloadLifetime requires the
entire StructuredPayloads group. It keeps original normal/nested/reply/text/
post/filter/native-extension tests, real distinct-version task client views
and normal retirement. The original generic thunk enters the native peer,
whose separate real thread sends the special COPYDATA to the actual sender
Win16 task. Guest consumes it before ReplyMessage, uses the original generic
thunk to await the native acknowledgement and never reads that input again.
Native early51 must win over the receiver's later53. Original unthunk and the
whole nested window procedure return before the peer frees its owned page.
The result binds RPLY's threads to the independently verified native report;
both the original allocation mechanism and message conversions remain
original, not a test replacement.

message-reply-payload-r1 completes0: native positive/wrong-reply controls,
P9/candidate091 in Off/Win16/All with structures/reply/extensions/client views,
one normal structures-only case and two default cases (nine actual cases).
reply-payload-seal-r1 preserves all seven exact authored inputs before adding
the originally planned nested-buffer witness. These are real successful
paths, not full M4 or a fix for0102.

WU32CallWindowProc's real16-bit-procedure branch uses CallBack16 directly;
it must preserve the received guest far input while that original outer
conversion is alive. The added guest nest compares the exact segment:offset,
validates data in the inner procedure, reacquires/validates it after returning,
then replies if this is the early-reply variant. No flat native alias is kept.
MSG5/MSG6 extend only opt-in structure reports with copyNested; default MSG1/
MSG2 reports and byte identity remain unchanged.

message-reply-payload-r2 first fails ordinary COPYDATA on P9 Off: the outer
callback returns447A0000, sender reports814A, while the inner callback sees
the same175F0000 pointer and returns low004A. The new probe incorrectly keeps
its normal/early selector in BX across CallWindowProc; trace shows callback
BX6, and that nonzero marker selects the early branch when its native page
has not been created. This is not proof of a WOW calling-convention defect.
reply-payload-failed-seal-r1 preserves all seven exact failed inputs. Correct
the authored probe by eliminating the register-carried flag and classifying
from the read-only header again BEFORE reply. Do not change original runtime
or weaken any pointer/value/count assertion.

message-reply-payload-r3 completes the full corrected group0: native controls,
six old/new three-policy combined cases, one structures-only case and both
default controls, nine actual cases. Each combined case additionally proves
two nested COPYDATA far-identity returns in the sender and one in the sibling
alongside four converted-structure callbacks in each task, six native
structure checks, earlier message/extension/client-view assertions and exact
normal task retirement. All six RPLY reports have errors0, flags9, reply51,
senderReturned/protected/unreadable/continued/fullReturn1 and the expected
two distinct actual threads. Both default NE pairs equal the prior sealed P9
bytes. No product image/MAP/import artifact or original guest is rebuilt.

Second-role review retains only this named coverage. CallWindowProc's native
procedure-thunk branch and every CopyDataFindData16 provenance-reuse branch
are NOT proved by the real16-bit branch. Physical segment movement, timeout,
allocation/exception failure, interrupted/dead receiver and all outstanding
conversion families remain S4 M4/M5 work. A successful callback/empty symptom
is not proof of zero global CopyData nodes or every guest heap release. No
extra conversion registry is added to make that claim. Original algorithm
retention wins here: the exercised normal/early/nested paths need no new
runtime patch.091's full gate stays failed and P9 remains installed; no new
runtime publication/commit/push or S4/full-goal closure is claimed.

message-reply-payload-final-r1 completes0 after second-role review: it requires
nine distinct r3 cases and frozen actual inputs, verifies the failed r2 source
seal, and derives one current-source bad native MINMAXINFO copyback. The
actual guest rejects that synthetic case at phase2/errors8000 with runner1;
it is not a product failure. Current production source/build dependencies,
candidate image/MAP/import artifacts and both deployed coherent13 P9 sets
remain exact. PowerShell parsing/diff/documentation quality-r1 passes at that
checkpoint. This evidence update receives a separate final authority recheck;
it does not relabel091's failed gate or the still-uncommitted work as delivery.

### M4 timeout completion and M5 death-query negative evidence

message-timeout-payload-r1 completes0: native normal/intentional-wrong-reply/
timeout controls and ten actual guest cases. Six P9/candidate091 three-policy
combined cases require the real native SendMessageTimeout failure (status0,
immediate error1460), an entered live Win16 receiver, three nested COPYDATA
returns in the sender, no ReplyMessage for the timeout branch, sender-owned
page protection/read refusal and receiver continuation/full return. Recorded
InSendMessageEx changes1 to9; this is observation, not an assumed required
post-timeout flag. Normal RPLY84 stays unchanged; explicit TMOP104 records
timeout fields. Early-only, structures-only and both byte-identical default
NE controls pass too. No converter, mirror, guest or production image changes.

message-timeout-final-r1 completes0. The actual timeout report is accepted;
nine synthetic field negatives, wrong consumer mode and two malformed-byte
controls reject. Failed-API output and after-flags remain unconstrained.
These parser controls do not simulate actual receiver death. The same owned
review compiles/runs wow_window_death_query_control.c: eleven public-query
observations cover direct/self destruction and both sides of NCDESTROY.
Every logically dead observation also fails the complete production capture
predicate. No dead-but-queryable row occurs. Thus this native source-predicate
lead supplies no evidence for adding an IsWindow guard or altering terminal
callback retention. It does not explain0102 or prove every concurrent window
lifetime safe. Preserve the negative evidence rather than patching a guess.

The next owner-performance diagnostic targets the full provider graph path:
capture_window, normalize_observation_chains, project_graph, refresh_graph
and its project_observed commit. Use reversible source copies of unchanged
P9 provider bodies, bounded inclusive native counters and existing posted-input
workload in ordinary/sampled/private environments, with uninstrumented P9
controls. No cross-kind sums, private USER decoding, timing-as-correctness
claim or changed refresh policy is admitted. Existing091 remains held behind
the failed whole gate; these named passes do not authorize retry-until-green,
publication or full M1--M6/S5/S6 closure.

### P9 provider graph cost: measured partial result, not another tiny fix

verify-wow-graph-cost.ps1 creates a reversible diagnostic copy of the exact
P9 wow_user_object_bindings.c body. Only static definitions are renamed and
wrapped, plus one reversible project_observed call interval. Actual provider
compile/link commands replace only that object and pin the sealed P9 parent
import library. Both image/MAP pairings and coherent13 release identities are
checked. No production object, image, guest or original mirror is overwritten.
Native fixture proves nested inclusive counters, LastError preservation,
exception-finally publication and refusal of overlapping writer accounting.
The existing read-only stable-sequence observer targets the diagnostic WOW32
symbol, not private USER memory. Counter kind labels are explicitly changed;
this is not the old worker page-cost interpretation.

graph-provider-cost-r1 compiles/links and passes its native fixture, all three
uninstrumented P9 controls, and both ordinary diagnostic environments. Its
private diagnostic refuses the first counter read with game74. Whole run1
is retained as failed. No post-failure counter bytes were captured, so neither
an overlap flag nor an odd sequence can be claimed as the proven refusal cause.
The read-detail-r1 control preserves the prior verifier and exact diagnostic
image, adds failure-only raw counter reporting and runs only private. That
run passes twelve inputs/normal close and emits no refusal. This does not
repair or relabel the first failed whole group; do not rerun until green.

summarize-wow-graph-cost.ps1 validates exact twelve-operation/six-kind pairs,
monotonic counters, even snapshots and actual QPC brackets. It omits the failed
private case explicitly. graph-provider-review-r1 completes0 as a review of
PARTIAL evidence, not a successful whole diagnostic or runtime delivery.

| Inclusive counters over12 inputs | Ordinary | Ordinary with PC sampling |
| --- | --- | --- |
| Native window capture | 1269 calls /7.800ms | 1395 /14.679ms |
| Chain normalization | 134 /2.423ms | 148 /3.126ms |
| Full graph project | 134 /332.769ms | 148 /408.394ms |
| Root refresh, including graph | 134 /360.011ms | 148 /439.927ms |
| Commit, inside graph | 134 /172.720ms | 148 /207.373ms |
| Committed observation rows | 42880 (320 per commit) | 47360 (320 per commit) |

Ordinary counter brackets total598.223ms while the posted-input stopwatch
totals380.680ms. Sampled brackets710.720ms/input435.649ms. Waits for a stable
counter snapshot occur outside the input stopwatch but inside these brackets.
Nested rows cannot be added, divided into CPU percentages or equated with
physical click latency. The measured source direction is nonetheless useful:
duplicate root capture is real (existing local rows are sampled, then explicit
roots sampled again), but capture alone is small here. Do not prioritize a
few removed native queries as a visible-stutter solution. The repeated full
observation copy/validation/commit is the larger measured provider path.

Source follow-through finds copy_window_observation calling the linear
page_domain_class_allocation for each full observation. That is O(W*C)
membership work in an already complete, execution-excluded view copy. Its
exclusive time is NOT isolated by these counters. Any candidate must reuse
existing ownership or call-local derived input, preserve foreign/noncanonical
CLS/name fallback, class lifetime and exact errors, and avoid a second
persistent class/window registry. Do not weaken full generation, retirement,
local freshness, copied external consistency or rollback to optimize it.
091 remains uncommitted and held; its failed common-dialog0102 gate has not
been solved by this diagnostic. P9 remains the installed coherent baseline.

### ADAPTER-WOW-092 full-copy ownership membership group

The previous turn was progress: actual graph counters changed the priority
from duplicate native sampling to whole-view copy/commit. class-membership-
seal-r1 preserves exact pre-edit091 source, all working provider/worker
objects, images, maps and imports.092 derives sorted owned-CLS addresses
from class_allocations once per complete full-view copy, replacing the per-WND
linear membership walk with binary search. It stores no descriptors or state
between calls. The original class list and current guest-backed fields remain
the authorities; released-but-linked classes retain identical membership.
Single/retained reads use the existing lookup. Empty/overflowed membership
or unavailable scratch allocation retains that same path, not a new failure.
Menu-only copies allocate no membership scratch. Capacity/count/refusal order,
full generation, class/name scalar fallback, sorting, error preservation and
class mutation exclusion remain. Every acquired scratch block is freed on
success or existing failure. No private export, wire or mirror change.

class-membership-build-r1 completes0 in the stable Release Ninja roots. Actual
CCPU graph-snapshot, native-nested, input-lifetime and external-memory groups
pass. The new required marker has errors0/phases4: owned canonical, owned
cross-page name, non-owned fixture CLS and invalid non-owned name. Whole-copy
rows equal independently pinned scalar reads field-for-field; both invalid
paths preserve ERROR_INSUFFICIENT_BUFFER. Existing menu-view checks additionally
compare full rows with scalar reads through real generation retirement/reuse.
Cold empty-class desktop, malformed graph, partial rollback and terminal
restoration assertions remain. Exhaustion of the native scratch HeapAlloc is
not actually injected: its unchanged-lookup fallback is source-reviewed and
that lookup is the independently exercised scalar oracle, not a claimed OOM
injection pass.

Candidate NTVDM SHA25668E6B0499DC90EFE047877D0C6D6B52E16C4B82F48CE01BDC7D3282F2D1AF74B,
MAP AA8F68BD600A63482E2B836F058790AF07E43D9A390D7C55ACEE3909C7DD3065.
Provider remains exact P9. Source-review-r1 completes0 and verifies no new
src/mvdm/src/opennt-host working diff. class-membership-verify-r1 owns the
fixed091/new/new/091 callback timing, nested controls, complete original/native/
guest/application/DOS/modal/logon floors and combined structured/early/timeout
messages. Neither a passing focused fixture nor a new full pass proves the
historical0102 cause repaired. Publication stays held; performance interpretation
and full gate results are pending, not speedup claims or full S4 completion.

At one live checkpoint a restricted-context Get-Process could not see the
owned elevated timing process. Checking in its actual execution context
confirmed matching PID/start identity and liveness; the first candidate exited0
and the fixed sequence advanced normally. No run was restarted or declared
failed from that observation failure.

### Remaining M4 source boundary: COPYDATA failure and procedure-thunk paths

While the frozen runtime group runs, source-only review compares complete
WM32CopyData and CopyDataAdd/Find/Delete regions with the pinned read-only
OpenNT files: both regions are identical (2736/2721 normalized characters).
malloc_w in original wow32.c uses HeapAlloc(flags0) and can return NULL;
malloc_w_or_die is separately restricted to initialization. Thus allocation
failure is not made impossible by the original allocator. WM32CopyData and
reverse wmsg16 COPYDATA paths ignore failed CopyDataAddNode; DeleteNode begins
by dereferencing the head without a NULL check. A missing allocation/node
can therefore break the intended rollback/callback-input contract. This is
an inherited source-level failure hazard, not evidence that092 or modern
delivery introduced it, nor actual injected-OOM/crash reproduction.

The finite existing M4 continuation must trace both complete converter callers:
DispatchClientMessage already honors a FALSE converter result before guest
entry; ThunkMsg16 likewise suppresses the native send on FALSE. Preserve those
original control owners and the single existing provenance list. Pair all
header/data/node acquisition failures with rollback before exposing a partial
input; do not add a competing allocation registry. Separately exercise the
original CallWindowProc native-procedure-thunk branch using a native procedure
returned through original GetWindowLong/GetThunkWindowProc, with the actual
received COPYDATA far input. The existing real16-procedure nest is not that
branch. Full canonical source/consumer closure and any minimal registered
mirror exception precede implementation; the currently running candidate
remains frozen. This is existing M4 work, not another S, new semantic waiver
or permission to edit/recompile original guest media.

###092 fixed performance result and retained native-return dependency

class-membership-performance-r1 completes0 in the fixed091/new/new/091 order;
strict nested exception controls also pass8/8 on both sides. These are real
completed controls, not a performance acceptance decision. Ordinary flag-up
callback medians are14.131/14.313/13.611/13.694ms, and full pair spans are
28.575/27.326/28.229/27.157ms. Sampled flag-up medians are15.751/20.562/
19.727/16.052ms; private4.807/6.558/7.578/6.755ms. The ordinary result is
essentially flat; sampled candidate values are higher. Do not describe the
algorithmic reduction as demonstrated responsiveness improvement or automatic
noninferiority. Retain the exact source/candidate and complete whole gates;
publication/disposition stays open. No extra timing retry is selected.

Tracing the actual return path explains why an API whitelist is not a clean
shortcut: wow_user_native_call_end always composes publish_call_windows,
borrow_client_graph and menu_client_refresh_all after execution restoration.
The same wrapper family covers InvalidateRect/SetCursor as well as real window
mutations and synchronous sends. native_call_begin can suspend the caller for
a non-inline native operation; another task may then change local state.
Even same-task inline calls release the data lock around native services.
Simply omitting refresh because an API usually leaves geometry unchanged
would not prove local state fresh after reentry/concurrency. Keep native
execution, callback, full identity, sampling and local-state refresh owners.
The remaining optimization direction is to prove a publication actually
unchanged within its existing transaction, not maintain a persistent dirty
cache, guess from API/message names or weaken return-time freshness. Any
such group must retain local WW/name refresh, class/retired collection and
all failure/rollback/terminal checks; it is not implemented by092.

###092 whole-gate refusal: retained combo SetFocus stop

class-membership-verify-r1 is terminal exit1, not still running. Native20,
both-width style, original30 and resource/menu guards pass; actual controls-Off
then times out after30s. The guest records phases0--4 with errors0, button1,
single/multiline EDIT1, ListBox1 and scrolling1. No completed combo/ordinary
exit follows. Observer reports result=timeout/0x53504354, not a guest assertion
or native exception. Later styles/dialogs/common-dialog0102/message/application/
DOS/modal/logon rows have not run and are not passes. No publication occurred.

class-membership-control-stop-r1 preserves exact raw inputs and disassembles
the unchanged P9 provider return site. Dispatch pairs independently derive
WOW32 base6A180000; the guest HWND/task thread36108 has native stack return
6A1BC520, preferred1003C520 in original ComboBoxWndProcWorker. The actual call
at1003C51B targets10061E40, wow_native_SetFocus. The guest listing places the
outstanding WU32SendMessage at file0CA9, return0CAE minus code base0C0 =0BEE,
matching trace CS:IP179F:0BEE. It is the first droppable combo's WM_LBUTTONDOWN,
not CB_GETDROPPEDSTATE and not an inferred modal mouse-tracking loop. Original
combo.c calls NtUserSetFocus there before opening/capturing the dropdown.
WOWEXEC thread11476 is captured in wait_for_task_or_message/xxxSleepTask/
wow_user_task_lifecycle_wait. Those stacks identify the blocked boundary, not
the full wait graph, focus recipient, lock ownership or092 causality.

Source identifies a concrete M1 seam to examine. Original taskman uses
private queue events (including QS_EVENT) and reaches xxxReceiveMessages
through its selected task path. The modern wait binding waits with public
MsgWaitForMultipleObjectsEx, marks an event/count on native queue wake and
does not dispatch there. Existing callback_enter can park a native-suspended
owner only AFTER a native callback actually arrives. A task selection gate
before native notification dispatch can therefore form an ordering dependency;
do not claim it is the actual failed cycle without deterministic owner/receive
proof. Preserve task locking and the single original scheduler.

Public [SetFocus](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setfocus)
can notify the previous/new focus windows and activate the hierarchy; its
explicit target alone is not its complete receiver set. Public
[MsgWaitForMultipleObjectsEx](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-msgwaitformultipleobjectsex)
also wakes for internal system events requiring a Get/PeekMessage call, even
without matching queued input. Public
[PeekMessage](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-peekmessagea)
supports sent-only processing without removing posted messages, but ordinary
peek/hook/change-bit and execution-return effects still require whole-boundary
review. Blind MWMO_INPUTAVAILABLE or an extra unfiltered peek can alter old
posted-input/WaitMessage behavior; neither is selected as a quick patch.

The required continuation is one finite native wait/notification group using
existing lifecycle, bridge and cross-task fixtures, not another polling loop,
hidden window or inferred SMS identity. Source/caller proof must cover entry
without guest ownership, real callback admission/parking, native return back
to the scheduler rather than prematurely to guest, and full publication before
guest execution resumes. The unchanged generic native_call_end presently
publishes the graph on every initialized return, so it cannot be blindly reused
from an unselected wait context. Keep092's failed gate and performance limits,
historical0102 and all M1--M6/S5/S6 obligations open. This records the next
concrete source dependency, not a completed repair or a semantic concession.

native-wait-pre-edit-seal-r1 completes0 after the failed whole run is terminal:
it preserves exact tested092 source, objects, images/maps/imports before any
new runtime edit. Second-role resource review then catches an added obligation
in092 itself: goto cleanup covers BOOL failure but not SEH from a lower-level
read. The staged source now wraps the entire allocated-scratch lifetime in
__try/__finally, preserves LastError and lets the same exception propagate.
No native fault catch, translation or new fallback is introduced. This latest
cleanup is source-reviewed only; the68E6 image and all prior results refer to
the sealed pre-cleanup candidate, NOT to the new unbuilt source. Do not run
another piecemeal whole build until the native wait/notification group is
closed and the exceptional-exit control joins it.092 performance/disposition,
both whole-gate refusals and complete M1--M6/S5/S6 remain open; P9 remains
published. All owned runtime controllers from this group have terminated.

### Native wait/receive and publication group (ADAPTER-WOW-093)

The admitted group retains original taskman/queue selection, event counts,
task locks and callback ownership. The modern native queue wake now receives
only synchronous sends before returning to original selection; InitTask's
original no-receive boundary is retained. No hidden HWND, inferred receive
identity, polling delay, guest change or mirror change is introduced.
Ordinary receive no longer performs a second unfiltered peek in metadata sync.
This avoids both an extra posted-message Hook observation and a redundant
full native-return publication. A wait entered without guest execution returns
to the scheduler without pretending to publish a guest return. Actual scheduled
DirectedYield/WaitForMsgAndEvent/WaitMessage returns publish the current graph
before resuming guest execution; GetMessage retains its next normal native
peek/publication. Actual callbacks still use the existing entry/exit gate.

native-wait-build-r1 fails before compilation because the controller selected
legacy powershell.exe without Get-FileHash. r2/r3 retain strict /W4 /WX fixture
compile failures (signed constant; conservatively uninitialized output).
Neither failure changed a runtime verdict or package. r4 uses the established
pwsh runtime and fixes the fixture, not compiler warning policy.

The r4 frozen negative links the same new authored witness to immutable actual
old worker/provider objects from native-wait-pre-edit-seal-r1; no Ninja rebuild
of the negative owners is allowed, and their hashes are rechecked. It proves
two distinct failures: WOW_SENT_ONLY_RECEIVE errors=2/posted_hook_once=2 with
the posted message and WM_QUIT still retained, and WOW_NATIVE_INDIRECT_WAIT
errors=1/accepted=0/error1460/result0 with the sender owner restored. The new
built owner passes both: posted_hook_once=1, accepted=1/result114/error0,
owner1, lifecycle errors0. This is a deterministic real native cross-task send
counterexample, not proof of the exact focus recipient/wait cycle in the
earlier real-guest controls timeout.

The exact production full-copy source-body unit separately distinguishes the
sealed092 exceptional cleanup defect (old errors1/exit1) from current finally
cleanup (new errors0/exit0). It covers normal, BOOL failure, propagated native
exception, allocation fallback, menu-only and cold paths. It is not a substitute
for real CCPU class-membership equivalence or guest lifecycle acceptance.
native-wait-build-r4 exits0: complete worker/provider links, six focused cases
(indirect/seen/nested/order/real-CCPU graph/InitRollback) and external memory
pass. Worker5001C298681700FBEAE98A517A23A10AFB375BD94B9B5B5EA961F3B1A5D21BB9,
provider8F81F573DA49F82AE79C8D1A08A6D0411FA4C2F9A312F607BEC498FC419F9BD7
are staged as coherent13 from the immutable ZIP/P9 cohort, not published.
native-wait-acceptance-r1 separately passes real controls Off/Win16/All through
phase8/errors0, including three combos/two dropdowns and teardown. The full
native-core group then passes21/failed0, including actual original modal CCPU
and the new native-indirect witness. native-wait-quality-r1 passes diff/parser/
documentation checks. Remaining original/real guest foundations, fixed-order
timing, message/application/DOS/modal/logon and installation gates are pending.
No publication, speedup claim or historical0102 repair is implied. The queued
performance/product controllers run serially only after their pinned prior
controller has terminated successfully; they do not waive a failed row.

native-wait-acceptance-r1 now exits0. In addition to its first three controls,
native-wait-foundations-r1 completes the four S4 focused checks and source
registers, native21/style, original30/resource/menu checks, then all12 real
guest controls/dialog/common-dialog/hook policy combinations. Public dialogs
run16 rounds per policy; All's final record is errors0/closed16/hooks17/
DestroyAX1/IsWindowAX0. This closes this candidate's named regression gate,
not attribution or repair of the historical intermittent0102 witness.
The fixed091+092 versus current091+092+093 ABBA measurement has begun only
after correctness passes; its old/new/new/old runs and subsequent product
checks are not yet passing evidence. Source and image hashes remain frozen.

native-wait-performance-r1 now completes its fixed four runs, exit0 meaning
measurement completed, NOT non-regression. Flag callback wall-time medians,
milliseconds, in old/new/new/old order:

| Environment | Guest up dispatch | Down/up callback span |
| --- | --- | --- |
| normal | 20.476 / 18.652 / 22.680 / 18.057 | 48.497 / 48.459 / 55.767 / 44.878 |
| sampled | 18.213 / 21.813 / 22.140 / 17.551 | 45.795 / 53.471 / 55.818 / 42.967 |
| private | 5.192 / 6.276 / 6.995 / 6.398 | 12.667 / 15.223 / 18.122 / 16.442 |

No stable speedup is demonstrated; sampled and repeated normal cases worsen.
Do not publish this as a performance improvement or use measurement exit0 as
its disposition. The exact fixed results remain retained, without replays.
The already-owned product/lifetime group continues to its terminal result;
its frozen source must not change during that run. Publication remains held.

Source review of taskman.c244/537/590 and lifecycle's return helpers identifies
a finite excess in093: DirectedYield/WaitForMsgAndEvent/WaitMessage now publish
unconditionally whenever the task owns execution, even when no native wait
receive occurred or another ordinary native return already published. Original
UserYield already receives after SleepTask. Tighten the seam using only an
explicit outstanding native-wait-publication obligation, not guessed HWND
dirty state, an alternate scheduler or a window snapshot cache. Set that
obligation when the new native wait frame returns without execution and full
publication; discharge it after a successful existing full publication. Only
an owned scheduler-to-guest return with a remaining obligation needs another
publication. Failure/unwind must retain the obligation; initialization,
GetMessage's subsequent peek, nested native frames and retirement keep their
existing owners. This is a registered093 refinement within the same finite
M1 group, to implement only after the current frozen run terminates and is
sealed. Source correctness plus positive/negative and fixed timing evidence
remain required; no actual new implementation or speedup is claimed yet.

native-wait-product-r1 terminates1 at modal-Off, after all15 message cases,
native payload controls, nine application frontiers and both DOS17 matrices
pass. Its modal native peer returns0 after closing actual MessageBox and
ShellAbout with IDOK in one worker thread40672. The observer instead records
run16 exit0x42B (1067); lifetime's final line is worker29112/WOWEXEC thread2224/
task00000000/stage=publish-scheduler-return/status1400. This is a new093 return
site, not a guest modal assertion, timeout, or evidence of normal retirement.
Other modal styles and logon were not reached and are not passes.

The initial refinement-seal controller refuses because it required product0;
no seal was created and no source was edited. Retain that refusal and revise
the seal to accept a TERMINAL failed cohort while recording its actual exit1,
never as a passing prerequisite. Seal source/objects/13 images before runtime
edits. The finite remaining group includes both redundant synchronization and
the local/native window-retirement dependency: identify which owned/background
root makes publication fail, distinguish explicit required operands from
background observations, and retain full identity, callback borrows and
normal destruction. An obligation bit alone is NOT proven to fix this1400.
Do not skip failed refresh, suppress the error, infer HWND generations or retry
until green. The successful narrower foundations remain valid evidence but
do not make this candidate publishable. P9 remains the installed baseline.

native-wait-refinement-seal-r1 completes after terminal product failure,
with productExit1 explicitly recorded; controller r2 only repairs the seal's
incorrect product0 precondition. The complete source/object/13-image seal is
verified before any runtime edit. A planned publication-helper control-flow
unit exists only under build, uncompiled; production remains exact r4.

Failure-only reversible diagnostic copies add result logging to the three
publication components, required roots, local-name preparation and page commit.
The first diagnostic fails linking its logger's secure-stdio import; the second
links all three overlays but its inventory enumerates the CSV being written.
Neither starts runtime. r3 resumes the exact second diagnostic DLL/maps without
recompiling and preserves both setup failures. Fixed Off/Win16/All returns1/0/0.
Off31504/WOWEXEC12972 reports local-project1400 then scheduler-graph1400, with
neither required-root, local-name nor page-commit failure. Its native peer again
closes both real dialogs; guest has only28 bytes/two records, not normal return.

Exact source narrows the remaining1400 branch in project_observed: HeapAlloc
failure is8, property-publication inconsistency sets5023, name/page errors
would have their own diagnostic lines. The remaining direct failure is
wow_window_words_acquire returningNULL with a non-ERROR_NOT_FOUND status.
That function first calls own_window/GetWindowLongA(GWL_WNDPROC); only after
that native check does it take publication_lock and query the property. A
captured local native HWND can therefore fail a later live-window check during
modal destruction. This does not yet identify the exact full HWND or prove
whether it has original owned state; no blanket stale-local permission or
GetWindowLong security bypass is selected. One deeper, failure-only Off run
will log this exact branch, full handle and native liveness/identity results,
not rerun until green. Source and all published images remain unchanged.

The detailed one-case r4 diagnostic completes Off0 without reaching a failure
logger. It is nonreproduction, not repair and not a replay-to-green disposition.
The exact1400 HWND remains uncaptured; the prior diagnostic/source branch
exclusion remains the limited evidence. Its sources only add reversible
failure logging; all runtime controllers are now terminal. After sealing r4,
the093 refinement is implemented in lifecycle only: an omitted native wait
publication creates a pending obligation; successful full publication clears
it; an owned scheduler return only publishes while it remains outstanding.
Partial failure and SEH do not clear it. Existing input_plan lifetime replaces
the duplicate initializing flag, leaving one additional task-private boolean,
not an extra window authority, dirty cache, HWND registry or scheduler. The
new exact-helper control-flow unit joins the retained real native/CCPU/guest
checks. These latest source changes are not built or verified yet. Hold
publication and test the actual failed modal route before full timing/release.

native-wait-refined-build-r1 stops at /W4 /WX control-flow fixture name-shadow
warnings before building production. Rename fixture globals, retain the strict
warnings, and preserve that failure. Refined-build-r2 exits0. Exact old helpers
fail the no-obligation/discharge checks; new helpers return the zero-error
NATIVE_WAIT_PUBLICATION_BODY marker covering no debt, native continuation,
one discharge, ordinary return, BOOL failure, SEH retention and LastError.
All six real production-object/CCPU focused cases and external-memory pass.
Worker remains5001C298681700FBEAE98A517A23A10AFB375BD94B9B5B5EA961F3B1A5D21BB9;
provider becomesED6BF90234AFF000A3A46420BB3E6BD382D8B3F549B11802F079A876C2D39024,
map0A0BB0C9B2903C4A76D00F65504F1D573C531F35068395815484B5912D6036A0.
The unchanged worker closure reuses Ninja's valid objects; only lifecycle and
the provider final link rebuild. No public export/mirror/guest change.

Refined-smoke-r1 passes real controls Off/Win16/All and the previously failing
modal-Off, now including guest continuation and exactly one normal task
retirement. Remaining modal modes are pending. This is a fresh changed-input
test, not a replay of the failed r4 image or proof of every native lifetime
race. The queued fixed timing group compares published P9 versus refined
candidate, not only the known slower intermediate r4. Publication still waits
for both performance disposition and the remaining full release gates.

Refined smoke completes controls3/modal3 with normal modal retirement. The
fixed P9/new/new/P9 timing completes0 but does not prove stable speedup: ordinary
flag-up medians15.385/11.697/13.023/11.590ms and callback spans39.142/29.085/
30.456/29.244ms overlap; sampled21.794/12.802/10.322/12.255ms, private5.821/
5.624/5.407/5.317ms. The last P9 itself approaches candidate timing. The reviewed
result allows full CORRECTNESS gates without a speedup claim, not a waived
performance regression. Exact summary SHA CFFA8A2B2FEDADCD756EBE1997987112951350B75496B9AEA14956A9DB79E0D2.

Permanent verify-wow-projection-return-unwind.ps1 now runs both current exact
bodies and separately accepts an explicitly selected sealed source while
propagating its true exit. Refined-product-r1 passes its current bodies and
requires both old source runtime assertions to fail1 with named nonzero-error
markers; a compile/setup failure cannot satisfy those negatives. Foundations
then pass native21, original30, all12 real guest groups, and all15 subsequent
message modes/policies pass. Nine-application/DOS gate instead terminates1 on
the FIRST WRITE mode, Off, after six MINE/SOL gameplay modes pass. The observer
script exits0 because it only observes; the separate frontier validator fails.
The first commentary misidentified Win16 mode; raw filenames show only Off,
and the correction was immediately reported. Win16/All WRITE, DOS, pure modal
control and logon were not reached and are NOT passes for this candidate.

The Off snapshot shows real Write error text "Not enough memory for Write to
complete this operation", invisible MSWRITE_MENU and no visible document/
pageinfo across four samples; it is not merely an early screenshot. Canonical
OpenNT wow16/write/write.rc maps that text to IDPMTCantRunM (201b). initwin.c
FInitWinInfo's InzFailed reports it for a broad startup failure, not only a
failed allocator; commdlg.c InitCommDlg(0) unconditionally returnsFALSE, so
the error text alone does not prove printer/common-dialog failure. Guest code
is research-only and will not be rebuilt/changed. No original-guest exemption
or runtime cause is inferred from this string.

Keep all source/binaries fixed and perform one declared P9/refined/r4/P9
WRITE-Off comparison using existing balanced dispatch/window/file traces.
trace_thunk/trace_return are already bracketed by preserved LastError in
wow_user_invoke_thunk. This compares historical binaries, not another source
patch or replay-to-green. Preserve every frontier and failed return; use
original startup source to interpret the first changed API/result. Publication
remains blocked, P9 remains installed, and the full WOW objective stays active.

The declared WRITE-Off P9/refined/r4/P9 comparison completes four startup
frontiers with no error dialog. Those runs enabled balanced existing dispatch,
window and file traces. This is nonreproduction, not a repair, an exemption or
evidence that the failed refined application gate passed. Canonical OpenNT
WRITE.EXE SHA08EE1659788880FB593815FF9609BFE76743C1182D05E6702764BF3DFF68B30B
differs from the immutable product guest
EB44B5D5F5A405B6A8CD018B9F790BCA2E7E895DAD4F7E1F5462EC14CCDAA87A;
the historical startup source therefore explains the error family only, not
an exact guest instruction or a proven original-guest defect.

Use a diagnostic-only TLS ring of128 copied scalar thunk records, dumping
only immediately before WU32MessageBox. No per-call file I/O, guest payload
inspection, new object authority or production edit is introduced. Exact
source reversal and generation-checked nested/wrap/pending/LastError tests
pass. Ring-r1 stops after its P9 frontier passes because the harness replaces
PATH before a later bare Ninja recipe read. Ring-r2 pins Ninja, its P9 frontier
also passes without a ring dump, then the refined diagnostic link fails1104:
TEMP/TMP still point into the already removed first physical stage. These are
diagnostic harness setup failures, not refined runtime results. Ring-r3 pins
Ninja and restores the harness's initial TEMP/TMP/PATH after each fixed cohort;
it retains the two predetermined P9/refined cases and all earlier failures.
Neither harness defect is attributed to the separate application gate, whose
WRITE-Off stage was still live when its actual error dialog appeared.

Ring-r3 completes0: P9 and refined both pass WRITE-Off, neither reaches the
MessageBox dump. Nested/wrap/pending/error preservation controls pass; provider
and guest production inputs remain unchanged. This still cannot explain the
failed release gate. A final declared predecessor control uses each fixed
diagnostic package in P9/refined order, first MINE and SOL gameplay in all
three styles, then WRITE-Off, matching the immediate six predecessors of the
failed applications gate. It changes a concrete setup variable (preceding
application lifecycle/resource activity), not the production source or an
acceptance assertion. It is not a full release rerun or replay until green.
Record both outcomes even if the first fails. The earlier broad foundation
and fifteen message groups are not reproduced by this limited control.

Performance interpretation remains bounded: the12--13ms refined figures are
the flag-up guest-dispatch phase only, not complete input-to-display latency.
The native entry delay and down/up callback span remain separate columns;
posted input is not physical pointer hover, hit-testing or hardware latency.
Sparse saved PC samples do not prove a dominant symbol or a wait duration.
Do not optimize session_thread_current merely for its sample count: its source
is a direct TLS variable return, not a wait/lock implementation. Further
production performance changes require a source-defined owner/call group and
measured phase justification; neither a new sleep nor an unchecked cache is
selected by these observations.

Predecessor-control-r1 completes0: P9 and refined each pass all six preceding
MINE/SOL gameplay cases and WRITE-Off, neither dumps the failure ring. Thus
the immediate application sequence alone does not reproduce the earlier
failure in these two observations. This does not exclude broader prior
message/foundation activity or an intermittent lifetime/resource issue; no
root cause is proved and no failed acceptance result is converted to pass.
All diagnostic children/stages terminate through their owned cleanup. Final
production NTVDM/WOW32 remain the unshipped5001/ED6 pair; P9 remains installed.
Quality-r2 passes changed-script parsing, diff whitespace and documentation
governance. No commit, publication, guest change or S4 closure is claimed.

### Required-root capture deduplication (ADAPTER-WOW-094)

The next finite source-defined group follows refresh_graph -> project_graph
in wow_user_object_bindings.c. refresh_graph's required roots are the desktop,
callback receiver and every enrolled worker window. project_graph first
refreshes every committed local observation, then unconditionally recaptures
every required root and replaces the first observation before validation or
publication. capture_window is public USER queries only; it does not install
state or call a guest procedure. That first successful sample is overwritten,
not a publication, pin or prerequisite for the final capture.

Retain the complete final capture, in original required-root order, as the
authoritative observation. Temporarily mark full root identities in the
already allocated transaction index and retain the old row until its mandatory
replacement. Do not skip a matching low word with a different generation,
non-root local observation, explicit payload capture or closure edge. After
compacting observations, clear/rebuild the same index as ordinary row offsets
before put_observation can consume it. No extra allocation, persistent index,
native-resource authority or freshness interval is introduced. A dead root,
failed final sample or conflicting live generation must still reject without
publication; successful LastError remains the caller's saved value.

Recovery ladder: selected original USER clients/USER16 shared-view readers and
their original algorithms remain unchanged. NT4's live server desktop heap
cannot be used on modern USER without the prohibited private layout/server
closure; the approved route-E same-shaped copied-view facade already supplies
this data. Reuse that existing facade and its transaction scratch rather than
add another window mechanism. No imported-source intrusion or newly authored
behavior engine is needed. This changes only redundant adapter observations,
with zero new original-mirror/guest/ABI changes. It does not assert equivalent
sampling of every concurrent transient; the retained final required sample,
local publication, and already approved external observation contract remain
the observable boundary, not success of an earlier discarded observation.

root-capture-pre-edit-seal-r1 completes0 before editing, preserving every dirty
source plus the full5001/ED6 worker/provider object/map/library closure with
hash-verified inputs.csv. Keep all earlier failing release aggregates and the
unlocated WRITE-Off error. Supplement the exact transaction body with lower-
service controls for final values/capture count, absent roots, full-generation
reuse/conflict, failed final capture, payload capture, external snapshots and
transaction rollback; then build the complete affected production closure and
run actual graph/lifetime/CCPU and guest floors as a group. No timing or
publication claim follows from call-count reduction. All release gates,
including strict independent WRITE3 startup, remain required for the next P.

required-root-build-r1 completes0. The exact project_graph/put/normalization
body's new controls report REQUIRED_ROOT_CAPTURE errors0; the reconstructed
old source matches the preceding frozen source hash exactly and fails two
runtime assertions (duplicate required capture and duplicate payload-root
capture), not compiler/setup checks. Final values, non-root local freshness,
retained external values, explicit foreign roots, mandatory dead/failed roots,
live-generation conflict, absent/reused roots and zero-commit failure remain
checked. These lower-service stand-ins are not native concurrency or guest
proof. The production patch is13 added/1 removed lines in the existing adapter;
no new allocation or public interface. Two roles review the index phase split:
full-identity marks are not read as offsets, and normal offsets are rebuilt
before put/closure/normalization/publication.

Ninja worker reports no work. Provider rebuilds only the changed object and
final link. NTVDM remains5001C298681700FBEAE98A517A23A10AFB375BD94B9B5B5EA961F3B1A5D21BB9;
WOW32 becomesE10B0B8DEB15A619DD90DADD80AB4BC3A616C52E5C7DC45219DB5AB931D0DA36,
map28E9FE076A460D34409F57990E91ADC354AA23E42E72ABA9CF2642BA3556E399.
Build graph and private provider DEF match the sealed predecessor. Actual
production-object native-indirect/native-seen/native-nested/task-order/
graph-snapshot/task-lifecycle InitRollback and original external-memory pass.
The CCPU group includes fresh native observations, discovery/property
exceptions/removal, full/short/dead thread queries, retained/reused backing
and both resource-held states; this is stronger than the helper controls but
does not replace real guest applications.

required-root-first-runtime-r1 waits on the exact original build PID/start,
then checks the new frozen inputs. Its fixed order is strict WRITE3 startup
first, predecessor/new/new/predecessor callback measurement, then the complete
fresh native/original/S3 guest foundation floor. The predecessor is the sealed
5001/ED6 candidate, isolating094 rather than mixing its effect with093. Never
call completed timing a speedup automatically. Remaining actual messages,
application/DOS/modal/logon and publication gates are still separate and
pending; P9 remains installed and every unlocated earlier failure is retained.

The new required-root cohort's early WRITE-Off/Win16/All observers and their
separate four-sample main/document/pageinfo validators all pass. No diagnostic
ring is linked or active in these images. This validates startup only; it does
not attribute or claim repair of the preceding093 error. required-root-quality-r1
passes diff whitespace, every changed/new PowerShell parser check and document
governance after the source/brief/evidence updates. The fixed callback pairing
is now live on its first baseline, followed sequentially by complete fresh
foundation tests; neither is recorded passed before terminal success.

### M4 COPYDATA caller/ownership sweep during frozen094 verification

Source-only continuation does not alter the running094 inputs. The complete
original conversion owners are wmdisp32.c::WM32CopyData (native-to-guest),
wmsg16.c::ThunkWMMsg16/UnThunkWMMsg16 (guest-to-native), and wdde.c's existing
CopyDataAdd/Find/Delete list. The latter stores paired full native/far numeric
addresses plus To/From16 and direction Flags; it has no reference count.
DispatchClientMessage sets fFreeTRUE before the forward converter, rejects
FALSE before CallBack16, and unthunks after the callback's returned result.
ThunkMsg16 likewise suppresses its caller's native operation on FALSE.
WU32CallWindowProc's Proc32 branch uses ThunkMsg16 and the selected original
unthunker; its Proc16 branch directly calls CallBack16. The passing existing
16-bit-procedure nesting therefore does not exercise Proc32 provenance reuse.

The sweep distinguishes concrete source hazards from unproved native claims:

- Both directions ignore failed list-node allocation. Forward header/data
  allocation and reverse native header/data allocation can therefore leave
  partial conversion state while returning success. The existing FALSE caller
  paths are already the correct admission owner; no alternate dispatcher is
  needed to reject incomplete conversion and roll back owned resources.
- CopyDataDeleteNode dereferences an empty head before a search. This is a
  definite missing-node/rollback hazard in the original body, not a proved
  cause of WINMINE latency or the unlocated WRITE failure.
- Forward reuse returns an existing far header while retaining its caller's
  default fFreeTRUE. If that header's existing node has Flags0, the inner
  unthunker can free storage still borrowed by the outer converter. This is
  a source-defined same-key nested-ownership counterexample; a real production
  or exact-body negative must prove it before calling it a reproduced crash.
  Existing WM32MSGPARAMEX.fFree is a call-local ownership carrier; do not add
  another provenance table or infer ownership solely from a matching address.
- WU32PostMessage resets fWhoCalledFALSE before its selected unthunker, whereas
  COPYDATA release is guarded by fWhoCalled==WOWDDE_POSTMESSAGE. If native
  posting fails after a newly owned COPYDATA conversion, this path skips that
  release. The statement is conditional source flow, not an assertion that
  a particular current Windows failure/status has been tested. Do not change
  all DDE flag semantics to fix this COPYDATA-only ownership boundary.
- fWhoCalled is process-global, but ordinary WU32SendMessage resets it before
  the native blocking call and reestablishes it for unthunking. Its declaration
  alone is not proof of a cross-thread race. Preserve DDE behavior and audit
  actual guest-allocation/reentry edges rather than make a speculative TLS
  replacement. MSGPARAMEX.MsgBuffer already supplies per-message scratch;
  WM32MSGPARAMEX has fFree/dwTmp. Any necessary ownership projection belongs
  in those existing call records, not a generic allocation/frame manager.
- Forward cleanup rereads native COPYDATA header fields after callback return.
  Passing early-reply/timeout controls protect the sender's page, not private
  receiver capture allocation. The native control deliberately disclaims
  that undocumented lifetime. Do not claim an observed USER use-after-free
  or waive its boundary from those passes. Captured scalar release identities
  in existing call scratch can remove an unnecessary reread without changing
  native delivery/reply ownership, but require exact lifetime/negative proof.

The next M4 implementation remains one complete bidirectional acquisition,
reuse, callback/native-operation, rejection, unthunk and release group, with
all fault stages and exactly-once cleanup/nesting/Proc32/ordinary send controls.
All original algorithms and the single original list remain selected. No
original-source or guest edit, new registered exception or runtime result is
introduced by this read-only sweep. The current094 timing/foundation process
continues unchanged, and full M1--M6 plus S5/S6 remain required.

###094 fixed timing result and complete-gate continuation

required-root-performance-r1 completes0 in predecessor/new/new/predecessor
order, summary SHA CD395046F72C0BD1EBC094F618009E25E8E78E34105803FEFC47A7291FDC1112.
Ordinary flag callback spans30.228/30.787/34.339/30.643ms do not show improvement;
the candidate repeat is slower. Sampled35.071/27.756/28.118/33.848ms improves
in this setup; private14.820/15.608/14.948/15.134ms overlaps. Guest flag-up
dispatch medians ordinary12.507/11.908/14.707/12.081ms also do not establish
a stable win. The four-run result is not equivalence/noninferiority, physical
pointer latency or a principal-stutter explanation. Separate hash-bound
review.json permits continuing correctness verification only, never release
or a performance claim. The deterministic source-body call-count improvement
remains distinct from measured UI benefit; no retry for a preferred timing
result is selected.

The same first-runtime supervisor advances to complete fresh foundation tests:
task-order/native-identity/native-reply/native-seen/source-registers pass before
the whole S3 floor starts. required-root-all-gates-r1 waits on that exact live
supervisor PID/start; only terminal success permits its fixed15 message groups,
independent MINE/SOL/WRITE policies, both17-route DOS modes, native modal3 and
logon3. It also runs the existing exact-body cleanup/publication and native
payload controls, checks frozen source/image identities and unchanged13-image
installed/release hashes, then quality. Queuing or starting is not passing.
Neither supervisor publishes, commits, closes S4 or waives an earlier failure.

The complete forward-converter caller sweep further rejects a two-converter-
only repair. aw32Msg[].lpfnM32 is also invoked by wudlg.c::W32DialogFunc and
wowhooks.c::ThunkCallWndProcHook/ThunkMsgFilterHook. The dialog and call-window
Hook stack records do not initialize fFree before invoking a converter that
reads it; MsgFilter explicitly sets FALSE. DispatchClientMessage explicitly
sets TRUE. These are reached original paths, not an allocator invariant or
a reason to replace the original Hook/dialog algorithm. putmsg16 also sets
FALSE, but its actual selector permits only COMMAND, DDE and DROPFILES, not
COPYDATA; do not expand this group into unrelated DDE based on a generic
function-pointer match.

Consequently merely setting fFreeFALSE whenever a COPYDATA header is found
is insufficient: it must distinguish an actively owned nested borrow from
a previously converted, intentionally unfreed observation awaiting its actual
consumer. The existing CPDATA Flags/MsgParam call fields are candidate owner
carriers, not permission to invent a second map. Audit those handoff semantics
and the original Hook's continued native delivery before selecting a flag or
count change. The caller list, rejection, initialization, allocation rollback,
normal/unwind release and native-procedure forwarding must be one M4 group.
The process-global fWhoCalled must not be changed wholesale, and the current
frozen094 package remains unchanged while this source dependency is resolved.

094's complete native-slot group now terminates0 (all21 listed cases), as do
the original30 control/dialog fixtures, common-dialog resource guard, menu
resource and both-width style boundary. The same foundation supervisor has
advanced into its twelve real guest groups; controls-Off and controls-Win16
pass first. This is partial foundation progress only. Original failure0102,
the preceding WRITE-Off failure, full M1--M6, pending message/application/DOS/
modal/logon gates and every publication/commit obligation remain open.

### Comparative evidence resolves the synchronous caller initialization

Read-only OpenNT-4.5 nt/private/mvdm/wow32 retains the same missing fFree
assignment in W32DialogFunc and ThunkCallWndProcHook. The source hashes are
wudlg.c8A0DFCB7CBFEA232C491FD9249C10048946F9F0D0E0B4120227DCCD6AB37109E,
wowhooks.c345220990242CDE0890884F478BEF8622C2FC537C9E6200FA9B289145BE0CC67.
It supplies no matching correction, not evidence that the field is initialized
by an unseen allocator. Selected declarations are automatic C stack records.

Comparative-only Win2000SRC/private/mvdm/wow32 supplies useful later evidence:
wowhooks.c::ThunkCallWndProcHook explicitly initializes wm32mpex.fFreeTRUE;
wmdisp32.c::W32Win16DlgProcEx, which succeeds the older dialog entry, also sets
TRUE before its converter call. Wudlg's older function has moved, so no later
unit or wrapper is imported. File hashes are wowhooks.c
DCA0216965E763F4D99BAA362BD4B802DEB065376EE2936B5C4A97F96224A6BC,
wmdisp32.cD7ADDDE8646A4F1A94EDEDC1F3D30DCE077FBFD5FD557D12E9CF402335811D00,
and wudlg.c0D4EF71A3DA661DE64D1C37C4F9681DE8BF26DEED9EF9DF748E8D8F04F9BF26D.
Microsoft copyright notices remain in those read-only originals. This is
comparison/provenance only: zero bytes, source selection, build/link inputs,
guest or runtime/acceptance dependencies are imported from the later tree.
The selected original DispatchClientMessage already supplies the same TRUE
producer. A minimal original-host initialization correction can therefore
follow the selected owner contract with corroborating evidence, rather than
choose arbitrary caller flags or replace its algorithm. Register and verify
both assignments as part of the entire future M4 group, not a tiny separate P.

The later COPYDATA and list-delete bodies still have the same unchecked
allocation/empty-head hazards. Thus the later evidence resolves these two
caller initializations only; it is not a complete ownership/fault fix. Native
contracts also delimit the lifetime: [WM_COPYDATA](https://learn.microsoft.com/en-us/windows/win32/dataxchg/wm-copydata)
specifies input read-only during processing, and the
[CALLWNDPROC Hook](https://learn.microsoft.com/en-us/windows/win32/winmsg/callwndproc)
observes before native window processing, without modifying its message.
[CallMsgFilter](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-callmsgfiltera)
can modify/consume its supplied MSG. Those different roles must be preserved;
do not infer a single unqualified free rule or a private USER capture lifetime
from the docs. Constructor-owned versus nested-borrowed COPYDATA storage and
all failure/unwind paths remain the next finite source implementation work.

### P12 complete091--094 correctness delivery and second-role review

required-root-first-runtime-r1 and required-root-all-gates-r1 both terminate0.
All21 native cases, original30, resource/menu/both-width style, all12 real S3
guest groups,15 real message policy/mode groups, native payload reference and
negative, independent MINE3/SOL3 gameplay/normal exit, WRITE3 startup, Console17/
Window17, actual MessageBox/ShellAbout3 continuation/normal retirement and
whole-logon gameplay3 pass on the frozen5001/E10 cohort. Earlier failed cohorts
are not reused as passed results. Source/body/memory controls and exact13-image
hashes are checked. The fixed timing review still proves no stable overall
speedup and does not identify the residual stutter root.

Single-person reviewer inspects the actual production diff:091 uses already
prepared full-generation backing instead of a discarded lookup;092 derives
class membership only in full-copy scratch, with scalar fallback and finally
cleanup;093 preserves original task selection/locks/LIFO and synchronizes sent
notifications without duplicate unfiltered posted observation, deferring only
an actually omitted native-wait publication until owned guest return;094 skips
only the first overwritten exact-root sample and rebuilds scratch offsets
before closure. Existing original/host/page-domain authority, required local
freshness, full identities, failure propagation, LastError and teardown remain.
No guest, mirror, public ABI, scheduler, helper HWND or semantic waiver is added.
Unlocated0102, the preceding093 WRITE-Off and other previously retained risks
are not declared repaired or relabeled original-guest defects. Current changed
candidate strict gates pass; this is a bounded correctness baseline, not full
M4/M1--M6 or S4/T447 closure. New COPYDATA source defects stay in active S4.

required-root-publication-r1 derives the established publisher with only the
actual native denominator20->21 and associated labels changed; every other
source/actual-guest/application/DOS/modal/logon/hash gate remains. It backs up
the previous coherent13+manifest and publishes only the two changed images to
assets/release and O:/winnt/system32. stopped-processes.json is empty: no owner
project session was closed. required-root-delivery-r1 then passes the deployed
both-width style boundary, actual installed full-logon WINMINE gameplay in all3
policies and final13 hashes at both destinations (exit0). Its outer catch would
restore the coherent backup on any post-publication failure; none was needed.
Only NTVDM5001C298681700FBEAE98A517A23A10AFB375BD94B9B5B5EA961F3B1A5D21BB9 and
WOW32E10B0B8DEB15A619DD90DADD80AB4BC3A616C52E5C7DC45219DB5AB931D0DA36 change;
the11 other images, original guest/configuration and assets/winnt.zip remain.
The live installed and asset manifests are identical to tested candidate13.

All staged M4 probes are independently authored supplemental opt-in evidence;
original converter bodies are unchanged. Exact-body mocks are labeled as
models, not guest/scheduler proof. Frozen-P9 graph-cost instrumentation is a
historical diagnostic with fail-closed image/MAP guards, not a new generic
runtime or an acceptance substitute. No additional timing rerun is selected
to manufacture improvement. Final whitespace/parser/document governance and
reviewed main/origin commit/push follow this runtime handoff; retain the full
S4 objective, source audit and next complete M4 ownership/caller closure.

###095 bounded implementation admission after the clean P12 handoff

Baseline is reviewed main/origin3ce9b7d0bf2d50cca9aa38a0c0a0bd25eba5077a,
clean worktree and published5001/E10 coherent13. Permanently authorized
original-host correction is selected only after the complete caller sweep.
Original WM32CopyData/ThunkWMMsg16/UnThunkWMMsg16 and CPDATA Add/Find/Delete stay
the conversion/provenance owners. Private message fields, not a second map,
carry constructor ownership and exact scalar release keys. A newly constructed
COPYDATA view belongs to that synchronous conversion scope; a reused active
view is borrowed. Filter and CWP have input valid through their callback,
not a cross-callback promise of identical allocated far addresses. Preserve
same-active-message nested reuse and native input/output identity; the later
CWP TRUE producer corroborates this per-callback lifetime. Do not change DDE's
FALSE filter producer or its algorithms to manage COPYDATA storage.

Return FALSE before exposing any partial allocation, using the existing
original unthunker for rollback. Helpers in the adapter supply only finalizer
mechanics/LastError preservation. Capture release keys before native/guest
reentry rather than reread caller buffers later. The empty original list must
return FALSE, not dereference NULL. A native-procedure thunk may reuse an
existing received mapping or construct the original native copy from valid
guest input when no mapping exists; never dereference a missing node.

There is an additional concrete caller distinction: ThunkMsg16 returns HWND,
so a valid thread message returns NULL even when conversion succeeds. Append
one host-private fThunkSucceeded bit to MSGPARAMEX, written by ThunkMsg16 and
getmsg16, and check it at currently unchecked callers. Source search finds no
assembly/worker/wire use, variable-size allocation or hard-coded offset of
this native C record. Preserve every old field and PARM16 offset. getmsg16's
existing selective conversion adds COPYDATA explicitly; do not replace it
with generic all-message/class routing. A COPYDATA receiver requires a real
window, unlike ordinary opaque thread messages. Preserve legitimate NULL-HWND
thread success and reject failed conversion before native execution.

Scope the original operation-private fWhoCalled/fThunkDDEmsg and Hook context
to their native threads; keep the shared CPDATA provenance list process-wide
under original execution exclusion. Save/restore original private modes at
thunk/Hook boundaries. For MsgFilter, retain original same-message VP reuse,
but save/restore recursion scratch across a different nested message or
exception and release only this frame's allocated MSG16. No persistent manager,
native queue replacement, helper HWND, original guest change or new scheduling
contract is introduced. Negative-code Hook policy and broader DDE remain
separate open M2/family boundaries, not claimed solved by095.

Implement the complete selected constructors/list/callers/finalizers and
independent exact-body plus actual guest/fault/nested/Proc32 witnesses before
one composed background build. Keep prior source/binaries sealed and perform
the full every-P gates only after this group is connected. MVDM-HOST-DIV-344
registers the minimal original-host intrusion; new adapter helpers cannot take
over the conversion algorithms. Actual line/diff/classification accounting and
all unproved boundaries are required before delivery; this admission is not
a runtime result, performance claim, or S4/goal closure.

### Residual stutter priority and096 allocated-view read closure

The owner's renewed report says improved, not smooth. COPYDATA095 remains
admitted but not implemented and is not presented as a performance cause.
residual-graph-profile-r1 uses the exact P12 worker5001/providerE10 and a
reversible graph-only diagnostic DLL. All three baseline and three instrumented
actual twelve-click/normal-close child groups pass. The outer run fails in
the newly added summary invocation: it tries reading the not-yet-written
outer exit.txt. This is an observer ordering defect, not a runtime failure;
retain outer exit1, raw successful child results and partial summary, not a
whole-run pass. Remove this premature call and retain the existing standalone
post-terminal summary workflow. No product/guest/installation changes occur.

The ordinary diagnostic brackets contain135 refreshes and45,900 committed
window rows for12 clicks. Inclusive refresh328.9925ms/project302.227ms/
commit152.7581ms overlap; they are not disjoint CPU percentages. Native field
capture totals9.9746ms, so removing a duplicate GetWindowInfo alone cannot
explain most remaining projection cost. The native observation domain has340
rows versus about16 on the private desktop. Brackets include read-side
quiescence and differ from the371.317ms actual input stopwatches; do not divide
these overlapping values into a claimed input-time percentage.

096's finite source scope reuses080's page_domain_read_owned_word helper for
still-scalar reads whose addresses come from the domain's allocated handle
table, published WND slots, or retained WND records. Cover slot lookup/menu
identity, current-generation tests, public handle metadata, the already-pinned
callback/relation source, and all graph prepare/retirement identity/class
reads. Do not redirect arbitrary relationship targets, guest-provided backing,
non-owned CLS/name, LDT/descriptor or general SAS calls. Current full reads,
required local freshness, capture ordering, failures, generation checks and
retention remain. No new cache/table/allocation, public ABI, mirror/guest
delta or semantic concession. The existing reader remains page-bounded and
uses original c_GetLinAdd synchronously; no pointer survives the call.

Seal before editing: copydata-pre-edit-seal-r1, terminal0, P12 source, complete
provider/worker objects/maps/libraries and coherent13 images verified. Exact
body controls must distinguish unchanged results/identity from changed SAS
operation counts, exercise invalid/dead/reused/retired/overflow cases and the
page-crossing fallback. Whole CCPU graph/lifetime floors and fixed ABBA
performance follow a grouped build; no success or publication is claimed yet.

096 first implementation uses13 changed read expressions plus one private
forward declaration, no new function, retained state or export. Only the
already admitted080 reader is reused. The single-person reviewer checks the
actual diff by address provenance: slot reads use domain.guest_windows;
metadata uses the domain's allocated handle table; the callback/relation
reads first find an existing retained record; graph preparation/retirement
reads only a current slot or an existing borrow record. No native/guest
callout can free those allocations while execution is excluded. Arbitrary
relationship target reads, cold unregistered backing and non-owned CLS/name
reads remain scalar. The post-load class pointer is still merely a value,
not authority to map an arbitrary target. No original host/mirror diff is
added and no original guest bytes are changed.

owned-view-read-build-r1 passes the new exact-body fixture, then fails before
starting its P12 control because the COPYDATA-oriented seal did not include
the then-unmodified SoftPC source. Retain that setup failure, not a failed
old implementation assertion or a pass. r2 reconstructs that file from
Git3ce9b7d0b, matching the tested P12 source SHA256
941044309B157296AD72D4CB3A2C8BE723C1839FEEDAEB62655F30E56A9B73E2 exactly.
The old and new exact-body controls both pass;128 iterations preserve full
identity/metadata while the selected640 scalar reads become640 existing
bounded mapped reads. Invalid/withdrawn/replaced/retired/inactive cases and
LastError/page-crossing fallback assertions pass. These lower-service
stand-ins prove the local mechanism only, not real scheduling or latency.

r2's complete production incremental build succeeds: worker5 actions,
provider no-work. Actual CCPU graph-snapshot/window-borrow/window-reclaim pass;
five selected-production-object fixtures (task-lifecycle with InitRollback,
native-indirect/native-seen/native-nested and task-order), plus original
external-memory, also pass. The native fixtures are not real guest scheduling
proof. The CCPU tests include genuine
generation reuse/retained storage/rollback and the existing exact execution
scope assertions; it is not full S4 acceptance. Candidate NTVDM is
0E924EF84528D05F0A6E2BF605973CEB4CC7ACCC9876057F4C1EA6035CE43CA3;
provider remains P12 E10B0B8DEB15A619DD90DADD80AB4BC3A616C52E5C7DC45219DB5AB931D0DA36.
Fresh WRITE startup3 also passes before the fixed performance sequence.
Full foundations and every-P gates remain queued serially. Neither this
mechanism result nor a zero test exit establishes a visible-stutter fix.

owned-view-read-performance-r1 completes the fixed P12/candidate/candidate/
P12 sequence, all30 callback records and actual15 input pairs per environment
and run. The exact summary SHA256 is
E8E73E38793CA6A0B25CBD7EF70927ED078B61D1CB532E5F2758E2ED49599FD6.
Ordinary flag callback-span medians are47.57865/45.05605/26.30040/25.26835ms;
posted-input/pixel medians60.81170/60.59290/29.68385/30.82905ms.
The sampled and private observations remain in that exact JSON; their
instrumentation/desktop differences are not pooled with ordinary inputs.
The first ordinary callback comparison improves, the last does not; large
temporal variation and mixed private results preclude a stable speedup,
noninferiority or principal-stutter claim. Both versions retain the same
twenty-millisecond polling observer and normal window-close assertion.
The second-role review permits completing correctness gates for this small
bounded reuse, not publishing it as a proven perceptual fix or declaring S4
complete. Continue the larger observation copy/commit source investigation
after the safe verified baseline; do not turn unrelated COPYDATA work into
an explanation for measured input delay.

### Native queue precondition failure: fixed old/new source attribution

The first full foundation run stops at native-identity: the real cross-thread
identity/send checks pass, but the later hardware-only wait and two unfiltered
quit assertions fail (lines1315,1327--1328,1335--1336 of the frozen fixture).
All-gates refuses that prerequisite; nothing is published. The local native
fixture keeps the real page domain inactive; new096 owned-view reads do not
explain a native posted-message ordering difference. Do not waive the gate
or mark the failed full group green.

native-queue-detail-r1 fails before compilation on mixed slash spelling; r2
normalizes the two evidence roots. It makes a reversible copy of the original
fixture with wrappers logging only existing wait/message return values and
MSG fields, preserving LastError. No added PeekMessage/GetQueueStatus, queue
removal, state repair or runtime edit. Exact failed fixture support objects
are retained; only its page-native object changes between tested P12 and096.
Fixed P12/new/new/P12 all fail identically with five assertions. Every run
returns a pre-existing NULL-HWND message034A/wParam0/lParam0 ahead of the
expected WM_QUIT on both ANSI and Unicode calls. Its producer/name is not
proved and is not guessed. This is not evidence to filter that ID in runtime.

The [native GetMessage contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getmessagew)
retrieves unfiltered queued messages, returning nonzero for ordinary input
and zero only when WM_QUIT is actually retrieved. The fixture incorrectly
assumed its queue was empty after unrelated window/callout setup, and that
the first retrieval after posting quit must be quit. Repair only that test
precondition: a bounded64-message ordinary native pump establishes the
hardware-only test's empty queue while explicitly failing on a leftover own
command/quit. Retain the original hardware-only sleep assertion. Both quit
tests remain unfiltered, add deterministic ordinary sentinels, process/log
unrelated prefix messages and require the ordinary sentinel exactly once
before the exact quit/exit value within64 calls. No numbered-message special
case, new production discard policy or suppressed assertion is selected.
Strict runner markers enforce the precondition plus both stronger order tests.
An independently authored direct USER32 A/W reference additionally disproves
the old first-call assumption with controlled ordinary posts before quit.

Freeze this complete harness correction before the next grouped verification;
do not repeat the already-completed ABBA sequence to seek improvement.096's
production source/images and the performance probe are unchanged. Reuse its
successful performance/WRITE child evidence only after exact recorded source,
artifact and guest hashes revalidate. All failed outer gates remain failed
records and cannot be reused as passes. Fresh native/whole runtime gates
still have to pass before any publication.

The first harness batch-r3 passes the native reference, both exact-body
controls, three actual CCPU cases and task-lifecycle/InitRollback, then
native-indirect exposes the same earlier first-queued assumption in
verify_sent_only_receive's retention check. Its unfiltered PeekMessage reads
an unrelated prefix instead of quit, leaving the owned quit pending; the new
empty-queue guard correctly refuses to hide it. Retain that failed batch.
The complete adjacent sweep checks every PostQuitMessage/PostThreadMessage
quit producer and exact-quit assertion in the selected native/CCPU/observation
fixtures. The existing wow_user_message_bridge_fixture already selects the
quit filter for this retention-only purpose. Reuse that established test
pattern in verify_sent_only_receive; keep the later A/W unfiltered
sentinel/order tests. Other native peer/application message loops already
consume ordinary messages until quit, and actual CCPU cases do not have this
first-queued assumption. No production code is changed to pass these tests.
The next grouped build covers all seven native send/identity/foreign/reply/
indirect/seen/nested modes together, alongside three CCPU cases, InitRollback
and task ordering, not just the first failing mode.

r4 confirms the initial suggested quit-only filter is not sufficient for
the PostQuitMessage retention producer: filtered Peek returns FALSE while
a later unfiltered drain still sees exit41. Do not interpret that timing as
a lost runtime quit, make up the opaque034A producer, or hide the leftover
own quit in cleanup. Replace the retention check with a bounded ordinary
native message pump until the exact quit, checking the earlier posted81
never reappears. It preserves the retention assertion, leaves native Hook
observation rules intact and does not assume immediate quit availability.
The direct native reference covers both PostThreadMessage and PostQuitMessage
in both A/W retrieval modes, with a deterministic ordinary sentinel first.
The standalone tracked producer is verify-wow-native-queue-order-reference.ps1;
its source/build inputs and result are independent of WOW/guest acceptance.
This supersedes only the earlier proposed retention-test filter, not the
unfiltered order/sentinel requirements or unchanged runtime queue contract.

r5 freezes the complete correction: both quit producers/A-W direct reference,
ordinary-post/quit order, empty-queue precondition and sent-only retention.
All seven native message modes now pass together, as do the actual CCPU
graph/borrow/reclaim cases and InitRollback; the full grouped result is12
cases, with task-order/external-memory completing separately. No runtime
message filter or producer-ID guess was introduced. The tracked standalone
native reference also passes all4 modes. The performance source, worker0E92
and providerE10 identities remain unchanged from the completed fixed ABBA.

### Next source lead while full096 gates run: unchanged publication work

#### ADAPTER-WOW-097: native COMMDLG execution boundary admission

The fresh096 full gate passes native21/original30 and actual control/dialog
groups but times out at first original COMMDLG.FindText. No gate waiver is
selected. owned-view-commdlg-control-r1 fixes P12/096/096/P12: all four time
out with CDL2 phases0/1, created0, before FindText returns. Experiment exit0
means the comparison completed, not a regression pass or causal proof.
The exact source/map show original wcommdlg.c directly imports SDK COMMDLG
without the existing native execution scope. A synchronous native activation
can call another WOW task while the caller is still recorded runnable.
The retained last trace is WOWEXEC WM_NCACTIVATE; this supports a missing
execution boundary, not a proven explanation of WINMINE click latency.

Source owner is canonical OpenNT base/mvdm/wow32/wcommdlg.c at the pinned
revision recorded above; selected original algorithms and source stay intact.
097 budget: one typed private header, one same-family native-leaf adapter,
one owner-specific build binding, tests and registers. All seven GUI entry
points receive the existing begin/end/finally protocol, including exact native
returns, output structures, LastError and modeless returned-window publication.
Non-GUI GetFileTitle/CommDlgExtendedError remain direct. No new private USER
state, transport, scheduler, helper window, mirror diff or guest mutation.
Positive/negative/nested/unwind witnesses and the actual immutable guest
FindText lifetime gate must validate before coherent delivery. Merely adding
the scope is not a passed root-cause proof; retain any further failures.

097's exact production wrapper bodies pass42 typed scenarios across seven
leaves: success/cancel/rejection/native exception/nesting/terminal end failure,
one native call and finalizer, exact owner/return/output/LastError and modeless
return publication. common-dialog-boundary-build-r1 fails at compilation on
uninitialized-result warnings; initialize the typed failure value, retaining
exception propagation. r2 builds/links and proves all seven original wcommdlg
references bind the wrappers, but its supplementary native UI test fails.
r3 stops before guest execution because sandbox denies O:/tmp staging; r4
uses the authorized normal context, without a new timeout or guest change.
Its first FindText now returns and two destroy/IsWindow rounds pass, then
the third reports the old0102 shape (destroyAX1/isWindowAX1). Whole gate exit1,
not a pass, not attribution of the historical risk and no publication.

The native supplement's pass-through begin/API diagnostic (build-owned
common-dialog-boundary-diagnostic-r2, not production) shows successful begin,
followed by native FindText/ReplaceText NULL with extended error0 and no app
hook. Independent direct native reference passes both standard-template APIs.
The model fixture substitutes page-storage leaves: manually attaching its
owner WW incorrectly attempts real implicit-dialog enrollment through that
incomplete storage closure. native-commdlg-host-owner-r1 instead uses a valid
native host owner without projected WW. Both real APIs now create/destroy and
their WM_INITDIALOG hooks synchronously enter the other registered WOW task:
two callbacks/result114, restored caller ownership, errors0. This qualifies
the native execution boundary, NOT implicit WOW dialog projection; the real
immutable-guest gate remains responsible for that separate path. All existing
native-indirect/sent-only/quit/order assertions remain. Retain the failed
model/enrollment rows rather than interpreting them as production repair.

commdlg-destroy-observation-r1 is an invalid build-only generator attempt;
r2 derives the current lifecycle body and adds preserved-error read-only
native/window-slot observations around DestroyWindow. It changes no selected
production object and is not a release image. All16 rounds plus final live
dialog/task retirement pass in that diagnostic: nativeLive0, mapped0 and free
metadata after native destruction and after normal return publication.
This does NOT reproduce or explain0102, and is not a replacement for the
unmodified strict acceptance. No retry-until-green or hidden waiver follows.
Freeze the corrected supplementary test group, then make one fresh strict
cohort; preserve any failure. Installed/release remain P12 throughout.

common-dialog-boundary-build-r5 freezes that corrected complete group (exit0):
42 exact wrapper scenarios plus the independent direct native reference,
all seven original-object aliases, unmodified sixteen-round actual guest
COMMDLG in Off/Win16/All, native-indirect/seen/nested and original task order.
Both receiver-exit held states pass; native indirect includes the two genuine
COMMDLG cross-task calls. Worker remains0E924EF84528D05F0A6E2BF605973CEB4CC7ACCC9876057F4C1EA6035CE43CA3;
WOW32 isB3FD3BC037414C5FF88B41D9E35AFA88C9B11F7DD7C35539D2376C4AEEE191FB.
Reusing the verified generator/graph avoids rewriting identical generated
inputs; worker/provider Ninja report no work. No diagnostics enter these images.
This locally verified group is not complete package acceptance or delivery.

common-dialog-boundary-first-runtime-r1 and its verification supervisor both
exit1 at WRITE-All startup: Off and Win16 pass all four samples. All's first
sample at4181ms contains WOWEXEC but not WRITE; at8149ms and the later samples
MSWRITE_MENU/MSWRITE_DOC/MSWRITE_PAGEINFO are visible and the same worker is
alive. This is startup lateness, not proof of a hang, introduced regression,
or cause of WINMINE's cell latency. Keep the strict failed four-sample gate;
do not extend it, waive it or rerun it until green. Full gates/publication
were not reached. No assets, installed files, ZIP, commit or push changes.

The separate common-dialog-boundary-diagnostic-verification-r1 explicitly
retains that release blocker. Its fixed P12/new/new/P12 callback/gameplay
comparison independently requires every real click/result/normal exit and
all exact frozen inputs. Foundation diagnostics follow serially. Their results
cannot override the failed WRITE gate or authorize publication.096's earlier
E10-provider ABBA is not reused for this B3FD-provider candidate; all its
no-stable-speedup limits remain. Do not alter source during these frozen runs.

That B3FD-provider ABBA now completes all four fixed legs (exit0,360
instrumented callbacks). Full summary SHA-256
630E0203AA0280BA54D1D3AC76CFF6B2A032D3677E2D4C7C15E8BF228FE928ED.
Ordinary flag posted-input/pixel medians ms are64.04/64.95/64.78/64.98;
inclusive callback spans47.65/57.29/54.19/52.54. Private pixel medians
31.60/32.06/31.53/32.10. These are rounded summaries of independently
validated inputs and normal exits, not physical mouse latency. No stable
overall speedup or noninferiority is proved; normal callback spans are higher
for these candidate samples. Do not claim the correct096 allocated-read reuse
or097 COMMDLG execution repair solves cell stutter. review.json explicitly
denies release authorization and retains WRITE-All and0102 limitations.

The source-defined window-publication lead needs a controlled workload:
ordinary native desktop population can vary across fixed legs. The queued
private-population-control experiment uses independently authored native
windows (0 versus256, hidden on its owned private desktop), unchanged actual
WinMine twelve-input/normal-exit assertions and fixed old/new/new/old order.
The helper must prove exact native ownership, survival during measurement
and normal destruction; original guest, timeouts and product images stay
unchanged. This isolates dependence on native window population without
closing or modifying unrelated applications. It is not another product cache
or a proposed refresh reduction. It waits for the preceding runtime owner.
Its r1 queued supervisor was cancelled before any compiler/test because
pre-execution review found a nonunique derivation anchor; r2 corrects the
anchor and retains that cancellation, not a failed product test. Probe source
and derivation currently live only under build; no capability/publication claim
or assertion weakening follows from this diagnostic setup.

Keep this as a source lead, not another untested runtime change or speedup
claim. project_window_graph prepares existing WND.pcls directly from its
current backing; when that value is nonzero, replace_window_links writes
that same value again for nonfresh rows, with guest execution/callback
mutation excluded between them. An existing cold desktop can have pcls0:
preparation then creates a class and must still write it. A fresh-only guard
would therefore be wrong; compare the actual owned field or explicitly retain
that missing-class transition. This lead does not justify such a guard.
The complete captured view also rewrites rectangles/styles/class numerics
when already identical. The same page-domain's publish_menus already derives
a call-local changed decision from authoritative copied storage and retains
unchanged content without a second cache. Audit that existing pattern before
inventing a new mechanism. Any later window use must retain all validation,
pin acquisition, full generations, rollback, retirement and local capture.
Shared-CLS row ordering must still be last-writer correct: a preparation-time
comparison alone is insufficient if an earlier commit changes the same CLS.
Non-owned/noncanonical pointers keep their original service boundary. Do not
trade correctness or freshness for a tick cap, background publisher, private
USER interpretation or an unregistered object cache. Implement nothing from
this lead until the current verified baseline reaches its safe handoff.

The next source review finds a smaller alternative before adding no-op
publication: replace_window_links still uses scalar SAS for WND.pcls and the
two public CLS numerics on every row. Reuse081's existing bounded writable
mapping for these known-owned fields first; it already preserves original
write access/CPL/VM, trace and failed-map scalar fallback. Do not add a dirty
cache, throttle, new publisher or preparation-time equality suppression.
WND backing is held by the transaction. CLS needs actual allocation membership,
not an address-range guess: retain scalar semantics for non-owned classes.
Its style offset64 and window-extra offset76 are NOT contiguous; offsets68/72
are private and must survive. Keep original row commit order/shared-CLS last
writer, cold desktop/new class, validation, rollback and collection ordering.
The class catalog is stable through commit; collection follows temporary-pin
release in project_window_graph's done path. Avoid adding an O(rows*classes)
lookup to each steady refresh: reuse092's existing call-local sorted membership
construction, factoring it only if the second real consumer is implemented.
New classes created after that snapshot have direct allocation provenance;
an unknown earlier pointer remains on the scalar route even if later storage
is allocated at that address. No new persistent class authority is needed.
This is a candidate, NOT admitted implementation or measured speedup. Extend
actual CCPU owned/non-owned/cross-page/trace/private-sentinel/rollback and
shared-class order assertions as one group before any production build.

### Residual click attribution: controlled population and source exclusions

The owned private-population-control-r2 diagnostic completes its fixed
P12/new/new/P12 order, each with0 and256 independently authored invisible
native windows. All eight actual twelve-input/pixel/normal-exit cases pass;
the native helper proves ownership, survival and normal destruction. Median
posted-input/pixel times are31.0855/31.774,30.5875/32.0845,
31.4115/32.152 and31.0015/31.1535ms (0/256 respectively). This does not
reproduce the ordinary desktop's approximately65ms median or prove speedup.
Source and runner remain build-owned diagnostic artifacts, not product helpers.

population-page-cost-r1 and its cohort exit0. Its reversible private source
instrumentation leaves production caches/images unchanged and also passes
original EMS. Paired counters prove that the added windows actually enter
the copied view: average copy rows15.795 becomes272.978. Inclusive copy cost
20.0031 becomes101.8141ms; projection57.1046 becomes124.5228ms; window-row
sorting1.0709 becomes34.9222ms across the respective whole measurements.
Call counts differ (copy342/274, projection171/136, view sort303/240).
These nested, quiescence-inclusive spans are neither additive nor CPU shares.
The view has genuine population-dependent work, but that is not proof that
it alone causes the ordinary/private latency difference.

analyze-click-pc-r1 exits0 without running or suspending another application.
It reconciles the saved exact MAPs with actual callback task-thread identity
and the union of twelve flag input / twenty-four callback intervals; idle gaps
are excluded. Exact local native-module export identities and sampled offsets
are recorded. Samples repeatedly occur near qsort, physical mapping, session
TLS, copied-view routines, memset and native allocation/free service entries.
Only35/40/etc samples survive per leg: this is not a call stack, CPU percentage,
or unique bottleneck proof. NtFreeVirtualMemory samples include offset12,
both before callback dispatch and within original dispatch. The currently
delivered/candidate images, raw observations and timing limitations stay frozen.

Source exclusions: session_thread_current already returns direct thread-local
thread_instance; physical_mapping_resolve finds this session's record and
indexes its page array. It is NOT a per-window range search. A second TLS cache
would add lifetime hazards, not fix a demonstrated search. Original phy_r32
uses two phyR reads for the retained video/memory contract, not a wrong-build
endian workaround; do not bypass arbitrary/device reads or edit that mirror.
copy_current_window_view copies in lifetime-record order, then qsort moves
complete observation records to preserve ascending slot order. Small-key sort
is a possible bounded experiment, not an admitted fix: preserve read/failure
order, full identities, sizing, allocation-failure behavior and both menu/full
views. Neither persistent indexes nor weaker publication are justified.

write-startup-attribution-r1 is a fixed four-leg P12/candidate/candidate/P12
All-style diagnostic against the unchanged4/8/12/16s strict WRITE gate. It
retains the first failed snapshot and every comparison outcome, without changing
timeouts, the observer/parser, guest media or installed assets. Passing later
legs alone cannot erase that failure, prove its cause or authorize release.
All four fixed comparison legs now pass the unchanged strict gate; first
samples are4228/4202/4156/4175ms. The one earlier4181ms missing-WRITE outcome
remains unexplained, not a repaired or waived regression. No release follows.

analyze-click-phases-r1 reuses the same frozen callback records (no application
rerun):288 exact down/up callbacks across four legs and three environments,
each accepted/normal with exactly one ordered DispatchClientMessage phase.
For right-button-up, ordinary pre/dispatch/post medians are4.69/19.51/0.45ms
in P12 and5.90/23.82/0.49ms in the candidate; private equivalents are
1.14/6.44/0.19 and1.30/6.60/0.19ms. Repeat legs retain the same qualitative
shape. Separate medians are not additive. Most of this sampled ordinary/private
callback difference is within DispatchClientMessage, not final cleanup alone.
That phase includes guest execution, nested host calls and their adaptation;
it does not prove the original guest is slow or permit skipping publication.
Retain this narrower boundary for the next source-led attribution rather than
claiming that faster outer copies alone will resolve the reported stutter.

### Owner resumes fixed S4 COPYDATA functional closure

Owner accepts performance as stable, stops that lane and explicitly resumes
S4 functionality. copydata-resume-seal-r1 exits0 before source edits: all
then-uncommitted files, full original WOW source/provider/worker products and
P12 delivered images are recoverably sealed.096/097 remain uncommitted,
not secretly published; historical WRITE-All and0102 are not declared fixed.
No new timing/sort/cache work is admitted by this functional continuation.

095/344 source group is now staged, not accepted. Original converters retain
their allocation, payload copying and sole CPDATA Add/Find/Delete algorithm.
Their existing message scratch carries acquisition bits and scalar release
keys; failed acquisitions/SEH unwind through the original unthunker. Reused
active mappings have no owned bits. Unlink nodes before reentrant guest frees;
release never dereferences a returned native/guest input header. Delete on an
empty list returns FALSE. Reverse data nodes use the actual far input identity,
not a transient mapped native address. No second map or guest change follows.

All19 stack-record owners (11 USER message,2 dialog,5 Hook,1 window callback)
are initialized and finally discharge outstanding COPYDATA only. Original
normal unthunk remains; scratch is consumed once, so finally is idempotent.
Unchecked ThunkMsg16/getmsg16 callers consult the appended private status,
not HWND0. Existing required-window callers retain their original checks.
Original dialog/CWP callers explicitly initialize the synchronous fFreeTRUE;
filter FALSE remains for DDE. Original operation/Hook context becomes TLS and
is restored at nested thunk/Hook boundaries. Filter same-message VP reuse
remains, with prior different-message recursion scratch restored and only
this frame's allocated MSG16 released on unwind.

A source-defined unsafe filter output needs an explicit disposition: its
old reverse path dereferences NULL when no native pair exists. Constructing
that reverse fallback now permits ordinary native-procedure forwarding, but
a native MSG returned by a filter has no consumer-owned release boundary for
a new COPYDATA buffer. Such a modified-output conversion is rejected; leave
the original MSG unchanged and roll back this frame's new storage. Existing
active native-pair forwarding remains valid. This is not removal of a working
old ownership transfer (the old path faults), a native-pointer lifetime claim
or support for asynchronous pointer messages. Do not broaden DDE or negative
Hook policy to implement this guard. Ownership lies with the synchronous
converter, native USER with message transport and original taskman with WOW
execution. All other M4/external/M1--M6 obligations remain in S4.

Supplementary verify-wow-copydata-lifetime.ps1 extracts the complete selected
CPDATA functions, WM32CopyData, ThunkMsg16, exact forward/reverse COPYDATA case
fragments and actual adapter finalizers. Fake allocators independently fail
each of four stages per direction, test nested ownership and mutable/noaccess
release headers, zero data, exception rollback and NULL-HWND status. It is
not original full-structure-layout, real guest or native transport proof.
Existing original-object thunk/Hook fixtures additionally assert nested
mode/context restoration. Actual verify-win16-message-probe gains opt-in
CopyDataProc32 (normal structured mode only): real USER16 GetWindowLong135
obtains a native-procedure thunk, CallWindowProc122 executes fresh and active
received COPYDATA paths on the sender task thread, checks payload/results and
retains the original nested far buffer plus both normal task retirements.
Native report separately requires fresh1/borrowed1; ordinary six structure
checks stay unchanged. Default independently authored NE bytes must remain
unchanged when the new mode is off. Full real boundary/negative/lifecycle
and every-P application/DOS/publication checks remain required.

The first095 group build stops in the supplementary fixture, before provider
compilation: its independent declarations omitted the profile's effective
FASTCALL convention. R2 additionally exposes missing model lReturn and a
function-pointer declaration; r3's W4/WX analysis requires explicit filter
hwnd32 initialization. These failed compile cohorts remain evidence. R4's
21-case exact source/Hook model, seven COMMDLG leaves, actual worker/provider
build and Hook guard pass. Thunk-scope linking then correctly rejects the
fixture's duplicate fWhoCalled/fThunkDDEmsg definitions: the full archive
already supplies the original TLS owners. Remove those two fixture doubles,
retain its restoration assertions against the real owners. No duplicate
symbols are suppressed and no failed cohort is published.

Adjacent source review also fixes CPDATA header/payload ambiguity as part of
the same constructor group. Both Find routines have only header consumers;
an embedded incoming header can numerically equal a still-live outer DATA
node's key. That is not the existing header mapping. Add a header-role bit
to the existing CPDATA.Flags and mark only the two original header producers;
the original list/search algorithm and allocation owner remain. Two exact
source cases require independent conversion of an embedded header in each
direction, then inner cleanup must leave the outer pair alive. The modified
filter-output negative separately requires original MSG preservation and
complete rollback. No new table, guessed identity or pointer-range test.
All24 model scenarios remain supplementary; actual guest Proc32 and complete
release gates still decide runtime acceptance. Initial nontrivial mirror
line upper-bound278 precedes these small guards, not a claimed final count;
the registered300-executable-line budget must be checked before delivery.

R5's complete24-case model, worker/provider build, actual original TLS thunk
scope, Hook scope and native indirect/identity/nested/seen/task-order all pass.
The first actual Proc32 Off case fails at native operation3, with normal
COPYDATA values/results and guest structures4/nested1 already reached. Preserve
that failure. Failure-only native-peer diagnostic r1 stops before runtime
at its own W4 local-name-shadow warning; r2 records two failed exact-full-HWND
comparisons on the sender thread, not a converter failure. Source confirms
walias.h USER32 is (HAND32)(INT)(SHORT), and pinned clmsg.c CallWindowProcAorW
ends at usercli.h CALLPROC_WOWCHECK, whose native branch calls the supplied
procedure with the unchanged hwnd argument. The probe incorrectly expected
a full native HWND here. Assert the exact sign-extended original USER16
carrier for each known endpoint instead (not a loose low-word match), keeping
payload, original-thread, fresh1/borrowed1, nesting and retirement checks.
No product normalization or converter change is made to satisfy that probe.
Failure-only logging preserves LastError and is absent from normal success.

copydata-closure-build-r6 completes0. All24 exact source/Hook fault-lifetime
cases,42 COMMDLG boundary scenarios plus direct native reference, actual
original TLS thunk restoration, Hook/context restoration, native indirect/
identity/nested/seen and original task order pass. Real USER16 Proc32 modes
Off/Win16/All each report fresh1/activeBorrow1, ordinary native structures6,
preserved original far nesting and both exact task retirements. Product source
is unchanged since r5's build; only the source-proven probe contract correction
and failure-only diagnostic changed. Prior r1--r5 outcomes remain retained.
This is whole focused-group verification, NOT release/S4/T closure.
copydata-closure-verification-r1 waits for that exact controller identity and
serially runs early strict WRITE3, fresh native/original/S3 foundations, the
existing15 message modes, application/DOS/modal/logon floors and final quality.
The existing full-gate script is mechanically derived with exact source/hash/
replacement evidence; performance experiments alone are omitted under the
owner's explicit freeze. No functional assertion/deadline is removed.
Installed/release stay P12 until complete reviewed publication succeeds.

Second-role source review holds the unstarted full-gate continuation, without
interrupting the running foundation tests. R6 is not delivery-ready: original
WOWGlobalAllocLock16 (wcall16.c) calls CallBack16, while the current forward
COPYDATA constructor first reads the native header after that reentry and
rereads its data key after another allocation. Capture that header's scalars
on the fresh-construction branch before the first guest allocation; borrowed
active mappings still require no header read. Preserve original payload copy,
allocation/list ownership and exception propagation, without another buffer.
The release-only no-access witness is insufficient: add a supplementary
allocation-boundary witness that revokes the native header on first guest
allocation while its independent payload stays alive. Separately, reject a
failed PostAppMessage conversion before both native posting and normal
unthunk, rather than merely skipping the native call. Successful HWND0 thread
messages remain valid. These are finite corrections within095's already
registered acquisition/caller contract, not new functionality or a waiver.
The queued-script hold retains its original bytes/hash and records that no
running test or production source was changed. Foundation outcomes remain
separate evidence; the corrected source requires a new frozen cohort.

The allocation-boundary negative control reproduces an access violation with
the r6 exact constructor (rollback_live0); it is supplementary fake-service
evidence, not an original guest crash. The corrected constructor passes that
witness. Exact WU32PostAppMessage extraction now additionally proves failed
conversion reaches neither native posting nor normal unthunk, while a valid
HWND0 opaque thread message reaches both once. Model denominator27 includes
these three new cases. R7 stops only at the test's SDK macro redefinition;
explicitly undefining the substituted PostThreadMessage macro fixes the model
declaration, not runtime code. R8's full27 cases pass before product builds.

The earlier full-foundation run fails, independently of the planned queued
review hold. Its graph parent passes managed-slot reuse, but the designated
window-cache-failure child fails at fixture line992: Windows does not provide
the demanded same-slot HWND reuse before the bounded loop exits. That unrelated
matrix prevents reaching the explicitly requested callback-restoration fault;
the event is unsignaled and child exit1, not the required c0000420. Preserve
this failed gate. Do not describe it as a passed negative or product regression.
The planned fixture correction keeps the complete reuse matrix in the normal
parent and removes that unrelated dependency only from explicit terminal-fault
children; all resource/owner setup and exact terminal marker/event/status
assertions remain. No allocator retry bound or runtime assertion is relaxed.

R8 completes0 after both finite source corrections: model27, COMMDLG42/native
reference, original TLS/context/scheduling/native execution group and actual
USER16 Proc32 Off/Win16/All all pass. WOW32 is
B761907DDE0B212F037C96B179328FB1E17326C1DF6EA38958CFA29472D768DF;
NTVDM remains0E924EF84528D05F0A6E2BF605973CEB4CC7ACCC9876057F4C1EA6035CE43CA3.
copydata-default-probe-identity-r1 independently assembles pinned HEAD/current
SEND/RECV in all five existing modes: all ten image hashes match with the new
Proc32 option off. These are independently authored tests, not rebuilt guests.
The conservative changed nontrivial mirror-line upper bound is285, within
the registered300-executable-line budget; declarations are included and
comment/empty/brace-only lines excluded. No selected mirror file is added.
The fixture's terminal-child dependency correction is test-only and applied
after r8's consuming native fixtures finish; no live compiler/test input is
changed. Full verification-r2 uses fresh evidence IDs and the complete current
fixture, without reusing r1's failed foundation gate. Installed stays P12.

R2's full native group finishes20/21: the intended graph terminal fault paths
now all pass, but class-procedure's unrelated managed-reuse precondition fails
at the same bounded allocation loop, before its own selected behavior. Native
source tracing identifies create_bound's SetWindowsHookExA(WH_CBT) before its
CreateWindowEx: Hook and HWND allocation can compete for the chosen dead USER
slot. The independent64-round native service control confirms63 Hook matches
and zero window matches on that exact source-shaped allocation sequence. This
is a measured test-precondition defect, not grounds for a product workaround
or marking the failed class-procedure row passed. R2 does not publish.

The corrected test offers four simultaneously live, projected, pinned native
windows before destroying them. It selects ONLY an actually reused window
slot with a different full generation, retaining every old/new backing, tag,
identity, invalid-generation and final-release assertion, plus cleanup checks
for all unmatched pins. The existing131072-attempt/15000ms limits are unchanged;
failure to obtain real reuse still fails. No fixed OS allocation order is
assumed, no synthetic HWND substitutes for real reuse, and no assertion is
deleted. Explicit terminal children still omit only the unrelated repeat
matrix; the parent retains the complete two-mode creation/null-class proof.
R3 tests the previously failed case first, then reruns the complete foundations
and release group against the unchanged r8 production input hashes. Source
and binary results remain distinct; an orchestration exit alone is no pass.

R3 passes native21/21, original controls30, resource/style guards, but the
subsequent menu-resource fixture again cannot obtain managed HWND reuse within
the existing bound. Its16 original resource/7 parser/64 invalid-repeat and6
original menu-thunk checks have errors0; its separate reuse precondition has
errors1, so the aggregate remains a failed gate. The retained first-miss trace
now identifies an actual IME HWND occupying a candidate's dead slot. Four
candidates do not cover this richer native allocation context. The bounded
test pool becomes16 candidates, with unchanged real same-slot/different-full-
generation requirement, all matched/unmatched pin releases and original
attempt/time limits. This changes only test setup capacity, not product
allocation, lifetime checks or a semantic boundary. R4 first runs that exact
failed menu-resource case: it passes (managed mode36 attempts, null-class1),
including the original resource/parser/thunk and final memory/retirement
assertions. The whole current fixture cohort must still pass before delivery;
neither earlier partial pass nor more candidates authorizes release.

R4 still fails the graph parent's acquisition precondition, despite the larger
pool. Therefore candidate-pool growth is rejected, not increased again or
used to waive real reuse. Source/observed roles require preserving the native
GUI lifetime and reserving the intervening Hook allocation separately. An
independent source-shaped control keeps a native BUTTON alive, creates a
thread-local pass-through CBT Hook before the old STATIC, destroys the old
window, releases that Hook last, then performs Hook→BUTTON creation. All64
rounds obtain the actual old HWND slot with a new generation; the original
unprepared control obtained zero window matches and63 Hook matches.

The final fixture returns to its original single-slot/generation algorithm
and original retry/time bound. It runs that matrix while its existing actual
managed test window is alive (no added anchor/helper window), and only managed
creation mode primes/frees the same-role test Hook. Null-class mode does not
allocate that extra Hook. All normal, retained-generation, table/type/tag,
invalid-generation and final-release assertions remain. The unnecessary
pool/helper code is removed: current fixture delta27 additions/4 removals.
This is entirely test-owned setup; no runtime Hook/window/allocator mechanism
is introduced. Terminal children omit only the duplicated unrelated matrix.
R5 first selects the three exact previously problematic graph/menu/class
groups as a fixed all-must-pass set, then completes the unchanged whole release
suite. No further production code or performance experiment is selected.

R5's fixed three-case prerequisite set completes3/3: graph-snapshot,
menu-resource and class-procedure all pass, with both reuse modes obtaining
actual same-slot/new-generation windows on attempt1. The graph test also
reaches every requested terminal fault event and exact c0000420 outcome.
The role/lifetime correction supersedes, and deletes from current source,
the unsuccessful candidate-pool attempts. All failures remain retained raw
evidence. The ordinary complete cohort and publication remain pending;
these prerequisite passes alone do not close S4 or authorize release.

### S4 P13 composed correctness delivery and second-role review

Question: complete the admitted095 constructor/provenance/caller/unwind group
and097 native execution gap while retaining096 correctness, with performance
frozen. Original conversion/list/Hook/task algorithms remain selected; USER
owns native resources and delivery/replies, and adapters supply scoped
ownership/unwind only. No original guest or user configuration changes.

Inputs: sealed P12/3ce9b7d0b, copydata-resume-seal-r1, corrected frozen
copydata-closure-build-r8 source/output inventories and canonical guest ZIP
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.
The complete r5 correctness cohort, not prior failed/partial cohorts, passes:

- source model27, COMMDLG42 plus native reference, original TLS/Hook context,
  original task order and actual native execution/lifetime supplements;
- actual USER16 Proc32 fresh1/active-borrow1 in each of Off/Win16/All, ordinary
  structured messages6, exact sender thread and both normal task retirements;
- native21/21, original controls30, menu/resource and both-width style guards,
  all12 real Win16 controls/dialog/common-dialog/Hook groups;
- all15 real default/extension/structures/early/timeout message policy groups,
  exact returned values and unchanged receiver/sender retirement assertions;
- WINMINE/SOL gameplay and normal exit3 each, strict WRITE startup3 (four
  samples each), Console17/Window17, native modal reference and actual modal3,
  complete-token-logon gameplay3 and final quality/governance;
- retained096 exact-body correctness refresh0 (generation/retirement/metadata/
  fallback/LastError), with no timing or performance comparison;
- native-queue-reference-r1 remains reusable with every recorded input hash
  identical; native A/W ordinary-before-quit assertions are not bypassed.

Exact roots are copydata-reuse-fixture-and-full-r5 (exit0),
copydata-closure-first-runtime-r5, copydata-closure-foundations-r5 and its
guest-floors, copydata-closure-all-gates-r5, applications-r5 and final-quality-r5.
Original-source/source-body doubles remain supplementary; actual guest facts
come only from their real CCPU/USER16 reports and lifecycle receipts. Default
probe bytes remain identical in all ten prior SEND/RECV modes.

copydata-closure-delivery-r5 completes0: recoverable coherent P12 backups,
13-image publication to assets/release and O:/winnt/system32, installed
both-width style, installed-logon/gameplay3 and final13-image hashes all pass.
stopped-processes.json is empty: no existing owner session was terminated.
Only NTVDM and WOW32 change. Their final hashes are respectively
0E924EF84528D05F0A6E2BF605973CEB4CC7ACCC9876057F4C1EA6035CE43CA3 and
B761907DDE0B212F037C96B179328FB1E17326C1DF6EA38958CFA29472D768DF.
Original guest/configuration/ZIP stay unchanged; no performance conclusion.

Second-role review: all12 modified host-mirror files retain their original
functions, with285 conservative nontrivial changed lines within344's300
executable-line budget. Selected278 files are183 byte-exact/95 registered
content differences/issues0 versus P12's192/86: nine additional differing
files, no new selected mirror or guest file. Registration/final-state README
and Current/working-plan/evidence updates after runtime delivery are metadata
only, not another runtime candidate; final documentation/source-register
checks must cover these administrative changes before the reviewed commit.

Scope remains finite and unfinished: other M4 families and real movement/
failure/task-death boundaries, cross-worker and external-native-process/both-
bitness interaction, then M1--M6 reconciliation stay S4 obligations. S5 owns
individual task termination/NT Monitor and S6 reconciliation, not displaced
S4 foundations. WRITE editing/save/full exit and physical-input/pixel-complete
claims remain unproved. Historical0102/WRITE startup failures and older S3
risks are retained, not declared fixed by current passes. P11 ad7da140e's091
failed cohort was not published; P12's earlier facts remain in this audit.
This is bounded P13 delivery, not S4/T or persistent-goal completion.

Final-review-r1 completes0 after administrative finalization: runtime/test
inputs and all outputs remain frozen, the sole superset-inventory difference
is the WOW32 README's delivery status, the real CCPU fixture hash is unchanged
since r5, all deployed13/ZIP hashes match, and fresh source-register plus
post-delivery quality/governance checks pass (278/183/95/issues0).

### Resumed S4: actual external-native process and bitness group

Question: does P13's existing message/conversion/execution closure work across
a real process boundary, rather than only its in-worker native peer thread?
Owner accepts/closes Td at aadde21e8; closure/resumption3b82e2a51 leaves the
published P13 runtime unchanged. Performance is frozen. No runtime repair,
new exception, original-source import, guest change or semantic waiver is
selected by this evidence group.

Source-led chain review before implementation:

| Chain | Actual selected owner and rule | Disposition |
| --- | --- | --- |
| HWND discovery | Original FindWindow/GETHWND16, wow_user_discovery discovered/publish_discovery, existing page-domain table and wow_user_window_native_handle | Native USER supplies actual HWND/PID/TID; publish the full identity, retain original16-bit carrier conversion. Do not infer a guest task from a foreign thread. |
| Send, callback and reply | Original wumsg WU32SendMessage/WU32InSendMessage/WU32ReplyMessage, wmsg16/wmdisp32, wow_user_task_lifecycle native_call_begin/end | Existing execution frames release/reacquire local WOW execution around native services; receiver identity/reply belong to actual USER callbacks, not the target HWND. Keep algorithms and approved modern USER contract. |
| Standard pointer data | Original MINMAXINFO/WINDOWPOS/COPYDATA converters and call-owned finalization | Native USER owns cross-process32/64 capture/marshalling. Use actual standard messages and retain original guest conversion; never serialize raw custom-message pointers or build another transport. |
| Resource/private state | wow_window_words_attach_target rejects foreign process attachment; borrow scope pins observed backing separately | Foreign observation is not a local window/property/task owner. No private foreign pointer or synthetic NT4 USER object is introduced. This does not prove every observed private field consumer. |
| Lifetime | Existing original task retirement and immutable caller scopes; native endpoint process owns its HWND | Hold actual process handles, query real target PID/TID, require normal exit and window destruction, retain exact once-per-task retirement assertions. Explicit fixture cleanup is not the normal-exit proof. |

Recovery ladder: directly retain the already selected original thunk/callback/
converter sources; their native services already use the bounded public USER
owner. No missing production dependency was found for these selected paths.
There is no reason to import private USER/server code, alter a mirror or author
a replacement algorithm. The necessary addition is an independent test endpoint,
not a product provider. Broader failure/unsupported paths remain unproved.

Implementation: compile the existing wow_message_peer.c handler both as an
independent32/64-bit EXE and as its previous same-worker control. A small
test-only generic-thunk DLL creates the selected EXE, holding its actual process
handle. An event name plus scalar HWND/PID bootstrap the test; standard message
payloads cross solely through USER. The ready HWND's queried PID must equal
CreateProcess's actual PID and differ from the worker. The endpoint reports
its actual sizeof(pointer), thread and process; the verifier checks PE machine,
distinct process, counters and normal exit. There is no shared-pointer channel.
The original same-worker report and authored NE bytes remain unchanged.

CMake's EXCLUDE_FROM_ALL fixture targets keep objects/libs/metadata in the
existing architecture trees. Only test media are copied to disposable roots;
the existing short physical-stage helper and exact-path process cleanup remain.
The optional NativeHost/ExternalNativePeer inputs keep previous probe modes;
external mode requires the complete normal structured group and excludes its
inapplicable same-process Proc32/borrowed-input and early/timeout variants.
No original guest or public run16 option is added. The separate process is
a test endpoint, not a production helper HWND/process or adapter mechanism.

Reproducer: hidden background pwsh -NoProfile -ExecutionPolicy Bypass -File
tests/observation/verify-wow-external-native.ps1 -BuildRoot
<repo>/build/t447-s4-external-native/group-r1. Supervisor PID47648 starts
2026-10-19T04:24:11.878Z; test starts04:24:12.709Z, finishes04:27:02.347Z,
exit0. Build commands PID30784/40280 configure both stable ccache presets
and build only the named test targets (exit0). Actual guest-case processes
and peer identities follow; every case exits0 with native errors0.

| Case | Case PID | WOW PID | External PID | Native bits |
| --- | ---: | ---: | ---: | ---: |
| local Off | 20824 | 47468 | n/a | worker32 |
| local Win16 | 49916 | 41056 | n/a | worker32 |
| local All | 22112 | 43744 | n/a | worker32 |
| external Off | 43004 | 49468 | 35668 | 32 |
| external Win16 | 20868 | 26768 | 48804 | 32 |
| external All | 46068 | 26892 | 16960 | 32 |
| external Off | 45152 | 5016 | 30832 | 64 |
| external Win16 | 49924 | 32400 | 5932 | 64 |
| external All | 19476 | 10220 | 11932 | 64 |

All9 retain original sender phases0/1/2/3/4/6 and receiver0/5/6 with errors0;
distinct actual guest tasks and threads, version030A/0300 retained across
callbacks, nested2/same2/early1/post1/filter1/threadpost1/text1 and native7
sender counters. Each guest has4 structured callbacks and exactly one normal
retirement. The native endpoint receives4 scalar requests, nested1/reply1/
text1 and6 structure checks, actual process/thread exit0 and windowAlive0.
The standard families are MINMAXINFO, WINDOWPOS and COPYDATA, in both native
directions in addition to the unchanged Win16/Win16 baseline. This proves
the named cross-process conversion/lifecycle cases, not all messages or waits.

All9 SEND/RECV image hashes are respectively
B0692BB2A0948B2AAE4C4E98E703CEF39200C8FFEEABFFF8AA9038DC9CDF86BB and
356AB86E785A1DF9F137B77D60B0D606CC450010F24F95729F44C99F9534DEF8.
These probes are independently authored; original guest media are not rebuilt.
The runtime in every case comes only from canonical assets/winnt.zip plus
the exact released13-image manifest, never the Td's build-only candidate.

Adversarial control: invoke verify-win16-message-probe.ps1 with its same
StructuredPayloads/ThreadClientView/P13 inputs but deliberately pair NativeHost
MSGHOSTLOCAL.dll with ExternalNativePeer=x64 MSGPEER.exe. The actual local
thread runs and returns its old52-byte report, so it cannot satisfy the60-byte
external report/identity contract. PID15596 starts04:28:09.898Z, returns1 with
the exact Missing native peer report assertion; the controlling expected-
failure check passes04:28:17.401Z. This is a proved test refusal, not a runtime
regression, and is not retried into a pass. Both argument paths refer to the
same stable message-probes fixture outputs as the positive group.

Result813AA633E2A0EA4EADA5ABACDDD03631C7252E1368C762CC6E08BC837743A69D;
steps80BEF6E58AD5B384AD45B54BFFA50D8D111B58C88734DB10D59933E57EF5EC44;
negative5B9DDE57D1FF2C4BAE5A3AC50430E10203093F95BB5A0213D784C6D5078BB97A.
Shared native sourceF02A364401982291BD78B1B49BECAC74BC5BDC6B6D11F55EF6AC95BF8F8A8965;
case runner24C9A9744FD45D6788F31B306881EE927271F31E8FD34C094F1BE8F761CACECB;
group runnerE8CB0464C4A8E04384A0135DFDAD3CC1250758442E899BEF6B0F3314D8775E73;
fixture CMakeEA5FB0AA6DA5BA2F36C9D008D628C5478E3246A632263220E5FC700CCE6D74A9.
Hashes/conclusions precede disposal of the owned raw evidence.

Second-role scope review: no production source, original mirror, guest or
release asset changed; mirror delta0. A runtime rebuild/publication and the
unrelated whole application/device sweep are not implied by a test-only group.
Do not reuse this as cross-worker Win16 proof: both guest tasks still share
one CCPU40 worker. External Post/ThreadPost, implicit receive, SMTO_BLOCK,
pending receiver death, other M4 conversion/movement/failure families and the
complete M1--M6 reconciliation remain S4. No performance experiment ran and
no historical0102/WRITE or older S3 failure is reclassified.

Final quality PID47100 runs04:31:43.251--04:31:57.283Z, exit0: all9 actual
runtime-input CSVs match the released13 manifest, every probe/source input
still matches its recorded hash, both installed/released sets and canonical
guest ZIP remain unchanged, both PowerShell scripts parse, source/asset diff0,
and fresh documentation/link/whitespace gates pass. Compilation targets are
EXCLUDE_FROM_ALL fixtures; no production rebuild/publication is claimed.

Before phase disposal, the sole cleanup owner inventories419 files/13450659
bytes. All85 sensitive entries are individually hash-reviewed:35 match the
canonical ZIP exactly,20 are identical tracked-probe NASM listings and30 are
the authored MSG6/NMSG result records. No user/configuration exception or Git
data is present. Inline ownership-manifest hash is
E0BB641D88E214AE15B26DBD733EB947EC869F08E32620E17B3AECC2DF16E474;
exact membership5CF0D06E24334A27B0747C2723E6870D937B4F2953194B38A861B6C3FCEAFA89.
These summaries/hashes precede deletion. Apply requires a fresh actual process
inspection and completed jobs; valid .ninja/.ccache remain untouched.

At04:33:56.150Z, preview Applied=false and fresh process inspection precede
successful Apply with that exact membership. No active case, external peer or
root consumer remains; no process is killed for disposal. Exactly419 files/
13450659 bytes of generated evidence/canonical duplicates are removed; the
same tool's final residual check returns only the two caches. Raw artifacts
are not promised recoverable; their results/hashes above, canonical media,
release images and tracked reusable test producers remain.

The containing reviewed commit/push delivers S4 P14 as a test-only checkpoint.
P13 remains the runtime/publication frontier; no untested binary is substituted
and no source/mirror/guest/runtime completion is inferred. Next fixed S4 work
remains cross-worker semantics and the remaining message/failure/reentry
inventory, not another performance experiment or an early S5 admission.

## Resumed S4 cross-worker message and local-task identity

Question: do the original HWND message/conversion paths work between actual
separate WOW workers, and is GetWindowTask a globally unique task identity?
Scope is the finite group admitted in the working plan after P14/7185ae4eb;
performance remains frozen. This is test-only P15, not S4 closure.

Source trace: original wuwind.c WU32GetWindowTask obtains native PID/TID then
calls walias.c's process-local ThreadProcID32toPTD lookup. A missing local
record falls back to gptdShell. wkman.c W32Thread
initializes that shell. PostAppMessage resolves a local task/thread; HWND
SendMessage/PostMessage may cross processes. Thus a remote task must be
identified by worker PID plus its own reported HTASK, not by assuming that
GetWindowTask(foreign HWND) returns the remote HTASK. No global registry or
replacement algorithm is warranted by this audit.

Implementation is entirely in tracked observation sources/CMake fixtures.
An independent native coordinator launched through Run16 receives the real
Hook and launches authored SEND/RECV NE images with the existing native
CREATE_SEPARATE_WOW_VDM flag. It pins HWND-derived PID/TID, exact staged
ntvdm image, creation time and actual process handles before a scalar ready
handshake. The sender skips its usual internal WinExec only in opt-in cross
mode. MSG7 adds remote-task/local-shell/ready fields; default MSG6 binaries
remain byte-identical to P14. The existing generic-thunk native endpoint
still runs inside the sender worker; this does not claim a new external
native endpoint. Original USER16 calls, converters and native USER perform
all actual message delivery. There is no test provider in production.

Reproducer: hidden owned pwsh -File tests/observation/verify-wow-external-native.ps1
-CrossWorkers -BuildRoot <repo>/build/t447-s4-cross-worker/group-r1.
Supervisor PID46596 starts 2026-10-19T04:46:16.286Z; group runs04:46:17.175Z
through04:48:47.163Z and returns0. Stable x86/x64 fixture builds PID19772/
37100 exit0. The six actual case processes all exit0:

| Style | Shared case PID | Cross case PID | Sender worker/task | Receiver worker/task | Foreign lookup: sender/receiver |
| --- | ---: | ---: | --- | --- | --- |
| Off | 2492 | 14596 | 25272/6071 | 15464/6079 | 935/927 |
| Win16 | 45576 | 27992 | 15716/6071 | 49032/6079 | 935/927 |
| All | 36936 | 47604 | 37664/6071 | 5840/6079 | 935/927 |

Each foreign lookup equals that worker's desktop/shell fallback, while the
explicit receiver/sender acknowledgement reports6079/6071 respectively.
Neither value is silently substituted for the other. Each cross case retains
sender phases0/1/2/3/4/6 and receiver0/5/6, errors0, exact scalar/nested/reply/
post/filter/local-thread-post/string results and client-version identity.
MINMAXINFO, WINDOWPOS and COPYDATA retain four structured callbacks per
guest and six native checks. Original last-app separate-WOW ExitKernel can
bypass shared SG_EXIT: normal retirement here is proved by pinned actual
process handles signaling exit0, completed guest reports and dead HWNDs,
before cleanup. Shared controls still require exactly one original retirement
marker per task in the shared worker. Cleanup is not this lifecycle proof.

Adversarial control: same inputs with -SharedWorkerControl deliberately omit
the separate flag. PID31968 starts04:52:34.049Z, returns1 by04:52:49.466Z;
both HWNDs resolve to PID34364 (different threads14644/47404), flag00000000,
and the coordinator records exact FAIL worker-not-distinct. The expected-
failure check passes. This refuses the false cross-process proof, not a
product regression. An earlier detached launch PID47440 produced no child
owner, result or output; it is unproved infrastructure, not a test pass.

Cross SEND SHA80150734BAD892AC034A02B3ED7BC08BDF597F83D6C5DDBB4ED2D7C046A3C5B9;
RECV9B25D272EC7B548E9EF05971196AD6A234ED23E32BFB336B2BD6AB05C3485BAD.
All shared controls retain P14's exact default pair. Original guest remains
canonical ZIP7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7;
all runtime inputs are released P13, not Td build-only outputs.
Group result CA193BB94D9FD4ACD7CB46474A4871FA88EBF412BDAC1ACF662EF235DF5C1069;
steps A10B77ECBBCAC70E5771D697554203AF489E1064A9A90C7E98767B89DD3C4153;
negative7878201D3E05541DB004568E6BB58740F10E7A02C0340065C685D32F6E76A40F.
Coordinator sourceFC3B564939AC2A05AD0C04ABA6553BB531DB68CB9F5728A1D8F6837E3039148D;
probe C2D99CA3D278ECD5B9A8FD63F57814E7D5827C13835A5124B0BCA9BB28EF13B1;
native source7557437F004A0D2CDACF6D249E40C6842EF4D7D4AD17D0F9A0FB3647A89741A7.

Second-role review: no production source, mirror, guest, installed or release
image change is intended; original-mirror diff increase/decrease0. No new
semantic exception, global identity mechanism, guest rebuilding, runtime
publication or performance claim. These six positive cases and one negative
close only the selected cross-worker evidence gap. Pending receiver death,
SMTO_BLOCK, implicit delivery, remaining M4 family/ownership/failure paths
and full M1--M6 reconciliation remain finite S4 obligations; S5/S6 remain
unstarted. Do not infer their completion from this group.

Final quality job PID1656 runs04:54:21.162--04:54:37.154Z, exit0:
all six runtime-input manifests match released13; source/probe/coordinator
hashes match; installed13/released13/canonical guest ZIP are unchanged;
both runner scripts parse and documentation, links and whitespace gates pass.
The earlier detached quality launch PID49440 produced no result; it is not
counted as verification. Held-handle execution supplies the actual result.

Before disposal the sole cleanup owner inventories334 files/12086582 bytes,
membership7659BB77ED100153626FDC94B1861751F1F4D19012F3D6AF24E7468DD4645015.
All69 sensitive entries are reviewed individually:35 exact canonical ZIP
duplicates,14 NASM listings tied to the tracked source hash and20 authored
MSG6/MSG7/NMSG reports. No Git data, user configuration exception or unknown
media is admitted. The six-case conclusions, failure control and hashes above
are retained before removal. Actual process inspection finds no listed job,
coordinator or product broker/worker alive; no process is killed for cleanup.
The existing remover must preview then apply this exact reviewed membership;
valid .ninja/.ccache are retained, no cache reset or Git prune is authorized.

Preview Applied=false and subsequent Apply both confirm that membership;
all334 generated/duplicate files (12086582 bytes) are removed. Final residual
inspection returns only .ccache and .ninja. Raw results are no longer promised
recoverable; tracked producers, canonical media and the summaries above
remain. The containing reviewed main/origin commit delivers test-only P15;
P13 remains the installed runtime frontier and S4 remains active.

## Pending-send blocking and receiver-window destruction

Source-selected M2/M3/M5 question after P15: can a real Win16 callback send
back into a native thread which has explicitly forbidden receive reentry,
and are destruction-time native results kept separate from HWND validity?
Original WU32SendMessage copies operands before ThunkMsg16, frees its argument
views before native dispatch, then invokes its original unthunker.
W32Win16WndProcEx/CallBack16 own the reverse callback. WU32InSendMessage and
WU32ReplyMessage directly use native receive/reply; no private SMS is inferred.
wow_user_task_lifecycle's native-call scope restores execution in finally;
SendMessageTimeout only captures output roots on native success. DestroyWindow
uses the same scope; actual USER owns destruction and the adapter withdraws
its projection. No replacement wait, queue, transport or scheduler is needed
for the selected calls. Existing native lifecycle fixtures cover nested
SMTO_BLOCK timeout/destruction, but not actual USER16 callbacks and handles.

The opt-in authored probe therefore retains the full ordinary message and
structure group, adding two scenarios as one acceptance unit. Native peer
receives a guest send, calls back with SMTO_BLOCK, and the guest sends back
to it. That reverse native callback must not execute before the blocked send
has timed out. After timeout USER can service the reverse call, the guest
continues and normal original cleanup must follow. An additional window
created by the real receiver task destroys itself inside another synchronous
callback. Native status/result/error and dead HWND are checked separately;
the surviving receiver verifies its original guest handle is invalid and a
send through it returns0. Both original main tasks must still retire normally.
These are disposable test windows, not hidden product helpers.

Failed first cohort is retained as failed, not retried without attribution:
supervisor PID12640 starts2026-10-19T05:00:46.622Z; group ends05:03:10.553Z,
exit1, after both builds/shared3 pass. pending-Off records blocked status0,
error1460, reverse-after-timeout1, but destruction status1/error0 with HWND
already dead. The initial test incorrectly required status0 for that final
send; native errors1 and sender errors818A prevent normal completion.
No shared-task/receiver normal-retirement pass is claimed for this failed case.
Result644046F2AC34AC93C5C583F8CE0FF32C3A00A64A7FC90A58500A6E07E8474CE5;
failed WAIT32F101CCDCB8BAF88D900604E0D4605E0C3AA48F571823E43396652630030FFF.
Its exact source inputs were probeD0380F85080A7702A2D32D88D29355A99CBE168293716456F221886D86FE943D,
native5D9CD8AB66C211BDF76E2A61B5969173FCDFD2153B7E155F439F6AD486EA7A3D,
runner5A38B5711E72720CD89B1F6685036F05DAE789767579DC0FE88571B7BEDEC4FA.

Attribution control: tracked wow_pending_send_native_control.c calls raw
USER32 only, with matching DestroyWindow-inside-WndProc/ERRORONEXIT/result,
both native widths and both same/cross-process targets. No Hook, WOW, test
provider, private layout or guest participates. Native-control-r1 supervisor
PID40148 runs05:05:14.126--05:06:42.063Z, exit0; all four cases return
status1/error0/result447A007C/destroyed1. This matches WOW, so the failed
expectation is not evidence of a runtime defect. Source hash
FA51D1B01C13F801BB50BEF3E71D76052A4C64E7A33A4517C63C47D84B7973FF;
resultF440FD4AB53A502C730188D61CF071C58C98C36D8EBFC5AB9FC39A1BB2078ED3.

The corrected group requires that exact paired native result, a separately
invalid HWND, guest stale-handle refusal and normal task retirement. It does
not merely accept either result to obtain a pass. Each final run repeats the
four native controls. A separately compiled no-SMTO_BLOCK control must expose
the forbidden reverse-before-return ordering and be rejected by the guest
group. No product change or semantic concession follows from correcting this
assertion. Receiver-thread/process death, HWND reuse, arbitrary failed sends,
implicit receive and remaining M4 families are not proved by window death.

Final reproducer: hidden owned pwsh -File
tests/observation/verify-wow-external-native.ps1 -PendingSend -BuildRoot
<repo>/build/t447-s4-pending-send/group-r2. Supervisor PID11572 starts
05:07:46.011Z; group05:07:46.970--05:10:08.588Z, exit0. x86/x64 builds
PID30036/12412 exit0, using the existing stable caches. Native controls
PID42196/47828 repeat all four exact matches. Guest results:

| Style | Shared case PID | Pending case PID | Worker | Sender/receiver task |
| --- | ---: | ---: | ---: | --- |
| Off | 48528 | 9124 | 35912 | 6079/6031 |
| Win16 | 31296 | 12900 | 30744 | 6079/6031 |
| All | 11260 | 48588 | 29120 | 6079/6031 |

Every pending case records errors0, blocked status0/error1460, one reverse
callback only after timeout, destruction status1/error0/result447A007C,
dead HWND and guest stale-result447A007E. Guest reports record one blocked
sender callback and one receiver destruction, retaining version030A/0300,
all ordinary scalar/nested/reply/post/filter/local-thread-post/text and three
structure-family assertions. Sender phases0/1/2/3/4/6 and receiver0/5/6 finish;
both original tasks retire exactly once before fixture cleanup. Shared3
retain the exact P14 default NE pair. No thread/process-death claim follows.

The separate no-block fixture PID14916 runs05:10:01.566--05:10:08.562Z,
actual exit1. It logs exact FAIL reverse callback before blocked send returned;
the verifier accepts only this attributed negative, not an arbitrary failure.
This demonstrates the ordering assertion distinguishes actual SMTO_BLOCK.
Its failure cleanup is not normal guest retirement.

Final resultF23D4ED5973D50EDDA4618C7CAD06FB5A346EDFFFFA4CC29C656C8BFC20AB158;
stepsA25E996B3388289A32EA6A996CE861B4C43A43E69BA92605F7EA5AB905C83C0A.
ProbeD0380F85080A7702A2D32D88D29355A99CBE168293716456F221886D86FE943D;
native23381445CDFF7A6030867C20BC55B56C0DC6887C3CA27374016AECFDE1ECBA20;
case runnerB5597E415DBF88E3C6290EB3A6A248382398F4C02A7D71B21C262EFE3BE6683A;
group54D24E740B540DCA1D5047354CBD4E97C12F7F49F33CCA22904A96C415D221A5;
CMakeCFA4ED5AC20DB976839744D0F185C7AC2CC9CEA810D0BFD60E8AEEE1FA5A9D91.
Quality PID25068 runs05:10:42.093--05:10:58.337Z, exit0: all six current
probe-input sets match; each runtime manifest and installed/released13 match
P13; canonical ZIP is unchanged; parsers, documentation, links and whitespace
pass. Failed-r1 source identities remain historical, not asserted current.

Second-role review finds no production change or new runtime dependency:
all new CMake targets are EXCLUDE_FROM_ALL fixtures, C outputs remain in
stable .ninja trees, scratch media are canonical copies. The control kills
only its own held child on failure and returns failure; its passing path
requires natural child exit0. No guest rebuild, new exception or image refresh
is justified. Original-mirror additions/deletions/diff delta0. The containing
commit is test-only P16, with P13 still the installed runtime frontier.

Read-only M4 follow-up constraints, not another implementation admission or
a proved defect: wmdisp32's explicit malloc16/stackalloc16/GlobalAllocLock16
sites span19 converter functions/26 calls. They group as follows:

| Existing original owners | Required ownership boundary still to reconcile |
| --- | --- |
| Create/MDICreate | Partial string/structure construction, nested create, rollback and original frees. |
| SetText/GetText/GetDlgCode/AskCBFormatName/DropObject | Borrowed parameter map versus owned heap/compatibility stack; output copyback and interrupted return. |
| GetMinMaxInfo/DrawItem/MeasureItem/DeleteItem/CompareItem/NCCalcSize/WindowPosChanging | Stack-frame lifetime, nested callbacks, native output/result and rollback. |
| SizeClipBoard/DDEAdvise/DDEExecute/WinHelp | Original shared-global/protocol ownership, asynchronous transfer versus call cleanup. |
| CopyData | Retain P13's specific ownership proof, not a universal finalizer for the other18 owners. |

This allocation-site inventory is not the complete M4 conversion inventory:
wmsg16's reverse/native allocations and delegated helpers also remain.
Original stackfree16 is intentionally empty; stackalloc16 advances vpCBStack,
and CallBack16 restores the original frame/stack. Do not invent an independent
stack allocator merely because that macro is empty. A callback returning
FALSE still reaches W32Win16WndProcEx's normal original unthunker before its
default path; an escaped exception/partial constructor is a distinct edge.
The COPYDATA-specific finally is not proof of generic string/structure cleanup.
No leak or output-after-reply fix is claimed without that source closure.

Retained artifact identities: pending SEND9A61DED24E7B6F6ECDA596D18E666A3E12CF9AD022494860557DCAEBA750C6EF,
RECVBA1E558FF3877C291C195A2A4175FAA30A11E77FE5C97DE0A62D0082C6932DE0,
hostD9F731C88E723CF73EA9E4BBA1DE2DD3FE661953D0053D8FAE7C07C9F15AE1B6.
Native control32 SHA2833F7FF166EBD848C54E4D09B08F68639E4FEE0F64C8D506017F8C74DEA4479;
control64 FCAFE1322B1E00F231A435EA5AECD45ED807AC5F09629E5A166A20923D693F5D.
QualityA0183FAA2D36C7797D856E102D72F445B108649787693F07BE6713F5EF74DDC7;
negative markerD3A43856EE22D99AAB145E9E079200CA4DEC05BF3436C495D6C2B74CA157DC3D.

Before disposal the sole remover inventories548 files/21489923 bytes,
membership322E6394DFE0CF4D0E3519360280A30C3DCBFF12BC36D5AEEBB343328E3A7E20.
All127 sensitive entries are individually hash-reviewed:70 exact canonical
ZIP duplicates,22 NASM listings tied to the unchanged tracked probe and35
authored MSG6/MSG8/NMSG/WAIT reports, including the failed first assertion.
No Git data, changed user configuration or unknown media is present. Actual
process inspection confirms all listed jobs/controllers/project workers are
gone, without killing processes for cleanup. These conclusions/hashes precede
the exact preview/Apply. Valid caches stay; no Git prune or cache reset.

Preview returns Applied=false; subsequent Apply removes exactly that reviewed
membership,548 files/21489923 bytes. The same remover's final residual check
returns only .ccache and .ninja. Raw failed/passing artifacts are disposed,
not promised recoverable; tracked producers, canonical media and the exact
results/hashes above remain. S4 stays active at the test-only P16 checkpoint.

## M4 native capture prerequisite

Source-first question after370134abb: WM32GetText/GetMinMaxInfo/
WindowPosChanging write their native output after CallBack16; ReplyMessage
may already have released the sender. Does that imply a dangling native
pointer, requiring a new early-finalization mechanism? This is a prerequisite
inside the admitted M4 whole-ownership audit, not a new product feature.

Actual source/build chain remains original: wumsg WU32SendMessage and
CallWindowProc use ThunkMsg16/selected apfnThunkMsg16 and original unthunk;
wmdisp32 W32Win16WndProcEx uses aw32Msg then original callback/unthunker;
walloc16 owns its guest message heap and callback stack; wparam owns parameter
map reference/alias state. The actual stable wow32 Ninja link contains
wmdisp32.obj/walloc16.obj/wparam.obj and their canonical source paths.
No autonomous replacement for these original owners was found or added.

Related original-state facts: WM32SetText GetParam16 increments a borrowed
parameter-map reference; its DeleteParamMap path avoids freeing borrowed
guest storage. Owned strings take malloc16 or the compatibility stack path.
WM32GetText retains original low-word result, touched-byte/zero-size quirks,
bounded copyback and matching release. W32LocalReAlloc updates the original
map after movement; task exit calls FreeParamMap only for that task. Original
stackalloc16 advances vpCBStack; CallBack16 restores vpStack, and stackfree16
is deliberately empty. These are source findings, not blanket runtime proof
of movement, exception cleanup or concurrent mapping lifetime.

Native evidence is necessary before changing normal output: the earlier
COPYDATA early-reply witness protects the sender's page but does not observe
USER's receiver capture. The new independently authored
tests/observation/wow_message_capture_control.c uses raw public USER32 only:
main-thread native WndProc plus held sender-thread handle, four standard
messages, early ReplyMessage6, sender-owned page protected NOACCESS and
verified unreadable, acknowledgement, then a second cross-thread scalar
send bracketing full WndProc return. No USER call occurs between receiver
acknowledgement and its return. Actual receiver address/protection are
observed separately from the sender allocation. For the three output
messages, a write is attempted only on a distinct writable capture; a caught
fault fails this diagnostic, never conceals a product exception. COPYDATA
has no post-reply input dereference. After the full-return fence only, the
sender restores/releases its own page. Destroy/unregister/thread exit are
required. The CMake targets are EXCLUDE_FROM_ALL, both widths, /O2 /W4 /WX;
no production source, Hook, WOW provider or guest is linked into the control.

First capture run PID47892 starts2026-10-19T05:20:44.590Z; actual x86 control
PID3596 ends05:21:22.525Z with exit1/errors2. It correctly observes distinct
GETTEXT receiver storage/protection4 but incorrectly requires output already
copied into the sender page at early reply. That unsupported assertion fails;
it is not a product defect or a successful eight-case group. Original source
D92A064C194B3DC963E7A35E89F812C737EDE30145A3A33609645A7832F7DF4D;
resultBB9CF801549AD54F20D643C913FCC28F26545B2155B307BDA8312A16F02119C7;
output7BA32E78E83EC50B970749F06F4AB87F434878DDB2CCB508A7B02EEDC79E1E3B.

Capture-r2 removes that presumption and records output_at_reply instead.
It passes all8 native observations with errors0, distinct writable captures
and output_at_reply0 for each output message. It does not yet demonstrate
a post-reply write; VirtualQuery alone is not promoted into that stronger
claim. SourceAAA4B30353277439C556D34DEDC11C5BA599F11AE5FECD1822C1295B4A16DCD3;
result68674B15B9EB01E0757EADDFC3313C6B46EFEA726257F20D241B24282C88BBCA.

Final capture-r3 sourceE74FFA8E55CCB38D0A8637D8DA4AD8E93374A279C4C8D7A694F02804C6FB3B91
adds actual guarded writes, not another product implementation. Supervisor
PID34500 starts05:24:37.928Z. Stable incremental x86/x64 fixture builds
PID43800/42836 exit0; native controls PID12212/33696 run05:24:45.673--
05:24:45.808Z and05:24:52.893--05:24:53.017Z, exit0/errors0. Group ends
05:24:53.046Z, result0. All eight exact observations:

| Message | Native32 receiver | Native64 receiver | Sender alias | Protection | Guarded output write |
| --- | --- | --- | ---: | --- | ---: |
| GETTEXT000D | EFEA34 | C8008FF0E0 | 0/0 | READWRITE4 | 1/1 |
| MINMAX0024 | EFFA5C | C8008FF990 | 0/0 | READWRITE4 | 1/1 |
| WINDOWPOS0046 | EFFA64 | C8008FF990 | 0/0 | READWRITE4 | 1/1 |
| COPYDATA004A | EFFA78 | C8008FF990 | 0/0 | Header/data READWRITE4 | not attempted |

Sender allocations1170000/native32 and23362C50000/native64 are NOACCESS
at observation/write time. COPYDATA data B3DDA0/native32 andC8008FF9C0/native64
also differ from their sender page+64. Every call returns status1/error0,
early result6; the later callback result447A0099 does not replace it. Output
at early reply is0 (not yet copied back), and all6 guarded output writes
succeed before full return. The owned supervisor explicitly requires four
rows/errors0 and three capture_write1 per width, not merely process exit0.
This is raw native same-process/cross-thread capture evidence; it is NOT
cross-process, actual USER16, every message or a portable private-USER contract.

Reproduce in owned background jobs: configure the existing
msvc-x86-release-ccache/msvc-x64-release-ccache presets, build target
wow_capture_native_control, then run the corresponding
build/.ninja/msvc-<arch>-release/message-probes/MSGCAPTURE.exe. Require exit0,
four message rows/errors0 and exactly three capture_write1 lines per width.
Observe per-build180-second and per-control90-second deadlines; do not clear
caches or use a new task-numbered build tree. capture-r3 needed no configure
because capture-r2 had already configured both exact trees.

Final result4EAEC5688E03CAD6BB42CA85B593CDE37AF7C261E1D2689513FC36B5A8153171;
stepsCFE937144B10732F9415F909656EDB84F03ABD30ACE063035E4FAC6B47C117C9;
CMake5F77214361D5F0A5411E7BAA92DE74D139046B448B8977453CD5D21C32C2B32E.

Conclusion/reviewer decision: the selected native output-pointer suspicion
does not justify rewriting original normal unthunkers, delaying ReplyMessage,
creating a conversion registry or guessing SMS identity. Preserve original
normal conversion and the already approved modern native reply boundary.
Actual Win16 early-output composition, all19 constructor/release owners,
reverse/delegated conversions, parameter-map movement, allocation-failure and
escaped-exception paths remain finite M4 work. A callback FALSE still runs
the normal original unthunker; partial construction/escaped exception is
different. No generic-finally correctness or M4/S4 closure is claimed here.

Final control32 SHA F4B854B9AC5C330CDBD0F73D89352797C409C32C3E0546534D7FEDA16FBC99BD;
control64 15F755DAA5EF177090D4B3D34FA63BAC4603418211FDA00158962C999DCC5E53.
Quality PID34664 runs05:27:17.201--05:27:31.329Z, exit0: source/artifact
hashes, actual PE widths, four distinct captures/three writes per width,
installed/released13, canonical ZIP, no src/assets diff and documentation/
links/whitespace all pass. Quality hash
10314C1C6C4495EEFAD4CEAD1C1AAB906EDAEFE05E128DD1A660FFEC37393513.
CURRENT compacts the delivered077/123/078 build-attempt chronology already
retained in this audit, preserving its facts and remaining obligations rather
than exceeding the control-plane size limit. No prior failure is erased.

Before disposal, the sole remover inventories45 files/15745 bytes, membership
8890071D44C3D70B32D7169D8EEB5CF8E6F90DEA7BD0C531ED7FFBC8E51D7DFD.
There are no sensitive/configuration/media/Git entries, only this phase's
owned generated commands/logs/JSON/CSV. Actual process inspection finds no
live listed owner or MSGCAPTURE controller; no process is killed for cleanup.
The necessary observations, first failed assertion, decisions and hashes are
retained above before preview/Apply. Stable .ninja/.ccache remain untouched.

Preview Applied=false precedes successful exact Apply:45 files/15745 bytes
removed; final residual returns only the two caches. Raw diagnostic logs are
disposed, not promised recoverable. The containing reviewed main/origin
commit is research/test-only S4 P17, not a product release. Original mirror
diff delta0, guest modification0; P13 remains installed and S4 remains open.

## M4 original construction and captured ownership cohort

After58fa75e55 the admitted MVDM-HOST-DIV-345 group retains the original
WM32Create/MDICreate/GetText/SetText/WinHelp converters, original parameter
map and allocators. It guards the absent GETTEXT allocation, propagates
nested constructor FALSE through existing rollback, captures the owned MDI
title/nested-structure rather than freeing guest-rewritten fields, preserves
the original unmapped-text fallback on optional map allocation failure and
initializes WinHelp's otherwise undefined failed-allocation handle. There is
no new message transport, finalizer registry, scheduling or guest change.
The complete source/alternatives/budget are in the working plan and local
345 registration; this cohort is not the complete M4 inventory or S4 closure.

Supplemental producer is
tests/observation/verify-wow-message-construction.ps1, using the exact selected
whole original converter bodies/types and six original parameter-map functions
with lower allocator/mapping/window services doubled. Baseline58fa75e55 and
candidate are compiled x86 /MT /O2 /W4 /WX. Model-r1 lacks test declarations;
model-r2 rejects the original cast-lvalue dialect warning. Those are failed
fixture builds, not product runtime failures. Model-r3 PID26600 begins
2026-10-19T05:51:13.6437194Z: baseline compile11656 exit0, run2720 exit1 with
13 failed assertions; candidate compile8688 exit0, run28224 exit0 with all36
scenarios. Six required old-negative families cover absent GETTEXT buffer,
MDI title ownership, nested rejection, optional-map text, NULL map leak and
dead-window rollback. Candidate additionally verifies mutation, borrowed map
references/movement and normal release; old WinHelp's coincidentally zero
uninitialized stack is not claimed a deterministic runtime negative.
Original allocator output-on-failure source establishes that defect.

Host-private WM32MSGPARAMEX is64->72 bytes, scratch offset56 unchanged,
scratch2->4 DWORDs, PARM16 remains16. The appended ownership slots have no
guest/wire/public export layout. Source search finds no assembly/inc layout
consumer: C callers instantiate the actual header, including nested local
frames. WM32Create explicitly initializes its new owner slot; the nested
MDICreate initializer writes its own title slot. Existing unrelated scratch
and converter algorithms remain. Original stackalloc16 is the unchanged
callback-stack subtraction, not an independently fallible heap allocator;
the injected allocation-stage evidence is explicitly heap failure, not
guest stack exhaustion. Escaped-exception/protocol-owner cases remain M4.

Model-r3 result SHA256
EFE359645F3C5F22059C9BF9413F250935BB468C0949C5EDC6E210CCD648EB3B;
fixture062872BFCE6FBFBA4CCAF02857CA5289CB321DFE3EBDAE78F7A4E3B04E220DFD;
producer63597C370D6A911DAA1A86D4396B252386EF050F8D3C47A49B9AAAA8B790CCAE.
Runtime source hashes wmdisp32.c
8EFE01255A083F7EDB11E24FCAA7AAD87CCED6D7C8B99DECD8D9BA3334031F6E,
wmdisp32.h DC153DA7665ED9C0B8C8E2607D8F188373860A7EB4C9B91AA610AEE3F2D7DE57,
wmsg16.c BE05F88C0D422049D7308E5A6711E8E65181C8331A5405547BD547E9B1676B2C.
Conservative raw changed-line bound, including comments, is51, below the60
executable-line budget; no new original file becomes different.

Coherent product-build-r1 supervisor9484 starts06:02:27.0805097Z and finishes
06:04:45.3540816Z, result passed: x86 child39108, x64 child7292 and x86
Hook/VDMREDIR child23640 all exit0. Stable .ninja/.ccache are retained.
Stage child49328 builds candidate13 from a fresh canonical assets/winnt.zip
extraction plus exact current host outputs; no installed-tree bulk copy.
The first real probe cannot create O:/tmp/n081 under restricted permissions;
it is not a runtime pass. Escalated r2 runs and fails assertion26 after its
first MDI creation, before text/destroy/help verification.

The failure is the probe's BOOL assumption: a pure modern USER control with
STATIC frame, real MDICLIENT, registered DefMDIChildProcA child and WM_MDICREATE
returns0 for successful WM_SETTEXT; GetWindowTextA returns15 and exact
"M4 native title". The first native control omitted CLIENTCREATESTRUCT and
fails0xC000041D; its corrected input supplies menu0/firstID1000 and exits0.
No product workaround follows. The Win16 probe now requires the native zero
return AND exact subsequent length/text, retaining rather than deleting the
behavior assertion. Failed controls and roots remain non-pass evidence.

Actual constructor acceptance uses independently authored MSG9 probes with
original USER16/KERNEL unchanged: normal, guest-replaced title and nested
creation-parameter cases; original DefFrameProc/DefMDIChildProc/text; private
WinHelp buffer delivery and post-return GlobalLock rejection. R3-Off passes
PID19436 at06:11:59.355--06:12:07.366Z. R3's unfinished Win16 leg has no
terminal result and is not counted; its process identity must be reconciled
separately from the valid Off row. R4-Win16 and r5-All pass: each sender final
has errors0, created3/destroyed3, titleMutation1/paramsMutation1/textChecks3,
helpCalls1/helpFreed1; ordinary message/identity/normal task retirement
assertions also remain. These are not allocation fault injection in a real
guest or full DDE/clipboard transfer coverage.

The fresh source-register gate passes278 selected/183 exact/95 registered
differences/issues0. Remaining retained native/original/S3/application/DOS
and coherent publication gates are in progress. No candidate has been
published, committed or presented as the current release; P13 remains the
installed baseline until every required gate and final identities pass.

### Full-floor identity failure and source-correct acceptance

Native21 PID48264 runs14:07:20.250--14:15:52.775Z,21/21; original30 PID28520
runs14:15:52.789--14:19:39.622Z,30/30. First full S3 PID35728 runs
14:19:39.637--14:21:14.660Z,11/12: common-dialog-Win16 fails in round1 with
phase0/1/1,0102, DestroyWindow AX1 and local USER16 IsWindow AX1. The launcher
exit0 is not success. Both queued successors refuse their failed prerequisite;
no application/publication success is inferred from those refusals.

The reusable unpublished observer is
tests/observation/new-wow-common-return-diagnostic.ps1 plus
wow_common_return_diagnostic.h. It compiles two generated copies of current
project-owned lifecycle/thunk sources and relinks the exact cached original
objects into a separate DLL. It never edits a product source/cache object,
guest, release or installed image. Initial diagnostic compilation includes a
report-only historical declaration header in the wrong translation unit;
the second encounters its unguarded duplicate type. Both builds fail and are
never run. R3 removes redundant historical includes and links successfully.
Diagnostic DLL112A3C544A82C4B99025B0185C7C6889FD6FDF1B5C0335BBDB45E93E2CECB2B7;
header316A7874A81463AB21ABCA80DC0DCA49B36D1F67B7B2DAA74BC8CDB6B7B6D8DE;
producerC45E3408E9C8904C982DC8A669477FB958A87D53A515B36F902DC4D8558453EB.
TLS records the completed DestroyWindow raw/full target/result without USER
queries or I/O before the guest's decision. Only its already-failed CDL2
file-write entry emits current native/copied identity, metadata and cache.

Diagnostic supervisor9512 runs14:30:58.179--14:31:24.556Z. D001 authored
negative20936 correctly returns1 and records full0057036E dead, current short
036E dead, metadata00010000 and cache0/0. One unchanged diagnostic case21052
and one prior-P13-provider case32036 both pass. These observations validate
placement, not a repair or green replacement for the failed row.

One bounded full-S3 contextual replay26992 runs14:36:43.037--14:38:19.224Z.
It again fails only common-dialog-Win16 and captures the real boundary:
CDL_FAILURE code0102/dialog0E90/raw00000E90/full0FBC0E90/result1/old_live0/
current0FBD0E90/current_live1/metadata0FBD0001/cache0,0/current_pid12336/
classCicLoaderWndClass. The trace's own worker PID is12336. This proves an
already-destroyed original HWND and a different live generation in its old
low16 slot, not stale publication or failed DestroyWindow. It is not merely
another successful replay. No name-based runtime exception or IME suppression
is introduced. The first uninstrumented0102 and older historical0102 cannot
be retrospectively attributed solely by their similar signatures.

Source: pinned OpenNT client/wow.c ValidateHwndNoRip/IsWindow and rtl/wow.c
HMValidateHandleNoRip; their SHA256s are
1ECE9DB0E35238434843BB73ECE92DD66000A44BC6B1148A08BCABF1329F8D9A and
71E5B523BD44077AB4E86FFC6093544922C815C759BB753C32B4155E3206691D.
Original usercli.asm passes HWND through pushINT_LONG; rtl accepts matching
generation OR high0/FFFF, then checks the current slot type. Therefore local
IsWindow(short0E90) may correctly accept the replacement. The retained
daytona wowrtl.asm (7EF0A72EDF8A29303F3B72D75E64DA1510CBDD3715E7F794AE295CDFDDCD9D81)
and usercli.asm (4240D6F792E39C8DB65158E626BFE0F127FEE09351DA476CDF5A280E9AFDFC20)
corroborate the exact client ABI. Their bodies/guest binaries remain unchanged.

The authored acceptance now optionally uses the separate test-only32-bit
CDLHOST.dll, not a product adapter or runtime diagnostic. Original GenericThunk
loads it. Public enumeration captures the exact live full HWND before every
original guest DestroyWindow; the actual guest IsWindow/raw AX remain in
CDL3. Every round requires the captured full identity dead. Guest TRUE is
accepted only with a different currently live full generation in that same
low16 slot. CID1 paired native records additionally verify exact process,
thread, input, result and native/guest counts. No missing/racing witness is
called success. All16 rounds,17 original hooks, original resource callbacks,
the final live-dialog task exit and exactly-once retirement stay required.

Native control's first capture fails because short-handle GetWindowThreadProcessId
is not reliable canonicalization. Public enumeration corrects only the test
precondition. Identity-build-r2 compiles DLL/reference and passes omitted
destruction/wrong-identity negatives; selected production WOW32 reports Ninja
no-work and remains39B7AC68011109E05465572C8BC56F1668EF8DAA6E68C2EE0A10F51EA51D5777,
equal to the sealed candidate. Test DLL
BEA263380F1E92B37FF77AE9066ADCA3139EDCDF46A5F150B6746553D508460F;
reference586EAEB064E6AB6D5082CA3CBBA62EDF54C1FBEBED2A146E1B20F1F6B67C1681.
The independent wow_common_dialog_identity_reuse.c loads that exact DLL and
rearms owned native windows, not a fixed dead-slot promise. Its first attempt
observes010D0544->010E0544; omitted destruction/stale TRUE are rejected and
the real new generation is accepted. This exercises the same DLL's TRUE arm,
not a fabricated handle or source-only predicate.

Fresh acceptance-r2 passes all12 S3 rows, each common-dialog policy16 identity
checks/0 reuse and17 hooks/normal retirement. The observed native TRUE arm
above independently covers the allowed reuse case. A subsequent test-only
FreeLibrary failure branch now writes its error rather than exiting after
the last successful report. Reversing only that branch reconstructs exactly
the prior S3 source SHA068BBCD685D305656A92639BD09072E1F30AD81BEF8868FCCB1CFA1094FD569C;
fresh common3 still required for the final source. The original failed rows
are retained, never relabelled as passes or waived by later retries.

Acceptance-r2 also passes all18 actual normal/extension/structured/early/
timeout/Proc32 message policies, WINMINE/SOL gameplay and normal exit3 each,
both-width style, native modal reference/real modal3 and ordinary-logon
gameplay3. Its WRITE/DOS aggregate fails before DOS: the task controller
mistakenly uses a long workspace TEMP, and WRITE reports that exact temporary
path cannot be created. A second valid-short-TEMP attempt omits the retained
observer's -WaitTarget and exits before a snapshot. Neither is product
acceptance or proof of a runtime regression. Correct the isolated inputs to
the standard physical-stage TEMP/TMP and explicit wait; do not patch/filter
guest, user global environment or product. Reuse the unchanged compiled
read-only window observer, not another build tree. Full supported Userenv
entries remain; only the normal test-owned TEMP/TMP are scoped.

Remaining fresh WRITE/DOS/common3/external/cross/pending gates and installed
publication are pending. Native21/original30 reuse is guarded by unchanged
source inventories and exact sealed-candidate artifact hashes, not elapsed
time. New CMake additions are only two EXCLUDE_FROM_ALL identity-test targets;
removing their exact block reconstructs the original frozen test-graph SHA
031B4A7BE2307BA77751FB28A7CE5910903DBE131185CA80E38014B39139A80A.
There is no relaxed product source/build-input check or implicit adoption of
newly relinked cache outputs. The coherent candidate13 stays immutable;
publication continues to require all finite gates and exact13 destination
hashes. M4/S4, all older unobserved risks and S5/S6 remain open as before.

### P18 bounded delivery

The pending statements above describe intermediate states, superseded only
by the following terminal evidence, all UTC on2026-10-19. The unchanged
candidate13 is the product-build-r1 package, not later mutable cache relinks.
MSVC x86/x64 Release /MT uses stable .ninja trees; original guest ZIP remains
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.

| Background group | PID/start--end UTC | Result and scope |
| --- | --- | --- |
| acceptance-r2 S3 |26336/14:53:07.579--14:54:42.854|12/12; controls/dialog/common/hook in Off/Win16/All; common16 rounds/17 hooks each. |
| acceptance-r2 remaining successful rows |14:54:42.884--15:01:17.089|message18, WINMINE3/SOL3 playable and normal exit, both-width style, native modal reference/guest modal3, complete-logon gameplay3.31 aggregate rows pass; failed WRITE row excluded. |
| final supervisor acceptance-r4 |45212/15:09:10.580--15:19:26.816|exit0, real10800s deadline; no observation timeout restart. |
| WRITE/DOS corrected inputs |15920/15:09:11.418--15:11:48.669|WRITE3 four startup samples each, Console17/Window17 text/interaction/exit checks. Not WRITE editing/save/full exit. |
| common final source |30436/15:11:48.731--15:11:56.087;23912/15:11:56.108--15:12:03.647;43552/15:12:03.678--15:12:11.309|Off/Win16/All pass16 rounds each with paired full identities, raw guest return and normal retirement. |
| external/local mixed-native |1352/15:12:11.322--15:14:48.595|9/9, local/native32/native64 across3 policies. |
| cross/shared worker |27964/15:14:48.609--15:17:06.087|6/6 across3 policies. |
| pending send |48988/15:17:06.099--15:19:26.285|6/6 local/pending across3 policies, native same/cross destruction4 and forbidden nonblock ordering rejected. |
| coherent publication |32220/15:21:37.457--15:22:16.291|exit0; exact13 backed up, copied to assets/release and O:/winnt/system32 and hash-verified. No active exact-path project processes needed closure. |
| installed style |43624/15:21:44.971--15:22:01.138|both-width boundary pass, installed destination. |
| installed ordinary logon |34796/15:22:01.156--15:22:16.172|complete Userenv, gameplay3 pass; final13 hashes unchanged. |

Reproduction entrypoints are verify-wow-message-construction.ps1 (old revision
58fa75e55498914dbf45ffa8630160f3f830b5df/current exact bodies),
verify-win16-message-probe.ps1 -Construction -StructuredPayloads -ThreadClientView with each
-Win16Style, verify-wow-s3-guest-contracts.ps1 with selected PackageRoot,
Observer, EvidenceRoot and ManifestPath, and verify-wow-external-native.ps1
with the same sealed PackageRoot/ManifestPath (ordinary, -CrossWorkers,
-PendingSend separately). Build CDLHOST/CDLCONTROL using the named CMake
test targets first. verify-wow-mine-gameplay.ps1, verify-wow-sol-gameplay.ps1,
verify-win16-native-modal-probe.ps1 and verify-wow-logon-gameplay.ps1 retain
the established3 policies. WRITE uses the retained read-only frontier observer
with -WaitTarget and physical-stage TEMP/TMP, not a reduced guest or product.
The two17-route groups use Verify-CommandExitStatus.ps1. The existing
verify-ntw16s-boundary.ps1 tests both widths; installed logon adds -Installed.
All invocations run in hidden owned processes with captured child identities
and real deadlines. Full original and native fixtures remain30/30 and21/21;
their unchanged source inventories and sealed artifact hashes are revalidated
by the final supervisor, not rerun merely to allocate a new evidence name.

SHA256 evidence summaries (raw disposable output is not a runtime dependency):

- sealed candidate manifest4754699FC4F0EDC60393758E09310DD045682F9A9950CA94848FCAEB5A51CB36;
  committed release manifestC32C4AE3269B69635CA363C6E8740ECB14D4E5C1058C2E75F15E4AF8B7CBCF66.
- acceptance-r2 steps30FB560C44D70B5EC4228C44F47F5143B429AECD070A4E55EF92803852C2EA76;
  final acceptance7A1F175E35C774E488349C5A8896475F543AE437268F2FE890D4FFAAE13F9333;
  native real-generation reference18DA411FACC4592B1E7F577E29424B5A0ADA469EC542CE8972F13F6A55CEC7E3.
- final publication0CE1001D5C4F33CA04D2364E08BD7EFB53F0F3BC0EF7694B170CC2475E299A24.
- runtime wmdisp32.c8EFE01255A083F7EDB11E24FCAA7AAD87CCED6D7C8B99DECD8D9BA3334031F6E;
  headerDC153DA7665ED9C0B8C8E2607D8F188373860A7EB4C9B91AA610AEE3F2D7DE57;
  wmsg16.cBE05F88C0D422049D7308E5A6711E8E65181C8331A5405547BD547E9B1676B2C.
- deployed NTVDMF615FD24F01B1CDDEF417588BEA39B6D00AD03174FBB6AC9677874252858BAA4;
  WOW3239B7AC68011109E05465572C8BC56F1668EF8DAA6E68C2EE0A10F51EA51D5777.
  All13 exact names/hashes are in the committed release-manifest.json; add-ons
  remain untouched and outside that host manifest.

Independent second-role review retains the original algorithms and sole native
USER identity authority: only absent allocation, failed nested construction,
captured ownership and optional metadata fallback change. No authored third
converter/registry, guest modification, private native-layout assumption,
name-based exception, environment filtering or new semantic concession.
DIV345 raw28 added/23 removed lines including comments is inside its60-line
budget. The source-register result remains278 selected/183 exact/95 different,
issues0: three already-different files change, no new differing-file owner.
The diagnostic DLL and CDLHOST are never product artifacts. Successful later
runs do not turn failed prerequisites, interrupted runs or prior0102 into
passes. Native reuse attribution applies only to the captured failure.

M4's remaining19-owner reconciliation, reverse-thunk ownership, escaped
exception cleanup, asynchronous DDE/clipboard transfer and full M1--M6
closure remain S4 obligations. S5 individual termination/NT Monitor and S6
final reconciliation stay unadmitted. Performance is frozen; no speedup is
claimed. Td is accepted/closed; this is one bounded S4 P18, not S4/T closure.

Final governance/relative-links/PowerShell parsing/diff and both13 destination
identities pass. The first disposable quality launcher failed before running
governance (reserved PowerShell args variable supplied no -File value, exit64);
it is not a failed documentation assertion or product result. Corrected
quality-r2 supervisor45992 starts15:24:47.342; governance20668 exits0 at
15:25:23.160, links30728 exits0 at15:25:32.331, six changed/new PowerShell
scripts parse and final result passes at15:25:33.522. No runtime input changes
follow; final added evidence text receives the same documentation checks.

Disposable-output preflight uses only Clear-BuildOutput.ps1, with actual
escalated process visibility and PID/creation-time matching: zero live owned
package/owner processes. The exact plan is5071 files/276075620 bytes under
build/t447-s4-message-construction and one28-byte authored G7.COM under
build/M0-T447 (no other contents). Inventory identities are respectively
9E8D4C6FDD675303286A2E29CEDC4AE1676C588165856FA3B66451E9A2352A3B and
74F18F69EEEB15186224C1DAFB07B647CEACEBC8FB688E7DC6D4570F5940B205.
The in-memory ownership manifest SHA is
C5AF2EABF824B1BC72472017B417C5060ACF534A635E7D56BABCAFCFB2FE55A1;
530 sensitive entries are individually hash-reviewed: original media/config
duplicates match canonical ZIP streams; authored binary reports/listings,
numeric PID/exit records, D001/G7 probes and isolated PowerShell generated
proxy/empty TEMP are task-owned outputs, not user data. No Git data/junction
is present. Default preview deletes nothing; final Apply must recheck the
same membership/metadata and all process identities before deleting these
two roots, followed by CheckResidual. Only .ninja/.ccache may remain; neither
is moved, cleared or treated as disposable evidence. Necessary results and
hashes are retained above; prior coherent13 recovery remains available from
the preceding committed release, not dependent on discarded backups. No prune.

Final Apply succeeds with5075+1 files/276077673 bytes (263.29MiB) removed.
The four added files are the final documentation-check script/logs/owner;
its PID25380 runs15:27:18.103--15:27:43.237, exit0. Frozen main inventory
EFCEBDAE14FA1669332867AA6D55922FCFA6195748C57D7F81F41E7740235619 matches
preview and Apply; G7 inventory is unchanged. Apply's in-memory manifest
serialization hash41340C5D686DF383DF853E3029128A81CE06E7D554A796552B0FA4BD6132F443
includes the same530 reviewed sensitive files. Per-process hashtable property
order makes its serialization hash unsuitable for cross-run membership
comparison; the cleanup tool's sorted inventory hashes are authoritative.
CheckResidual passes with only .ccache/.ninja. A final document-only check
uses an owned180-second/5MiB disposable root, then the same cleanup owner;
it cannot change product inputs or hide outputs in caches.

### Post-P18 complete allocation-owner disposition

Source baseline049ae27da6fdae9121ca8f11876b8be9dc0f7804, clean main/origin.
This is source/contract evidence, not a runtime pass. Read original selected
wmdisp32.c, wmsg16.c and all dispatch delegates wmsgbm/cb/em/lb/sbm, plus
wparam, walloc16, wcall16, wstruc, wumsg, wowhooks, wudlg and the selected
project callback/native-call/message-lifetime boundaries. No test is run to
select a defect. P18 is still both released/installed; no performance work.

The previously broad19-owner obligation now has the following finite
source dispositions. `Normal` means the actual original successful-call
release is connected, not all failures/exceptions experimentally verified.

| Forward original owner in wmdisp32 | State authority / normal release | Remaining fixed boundary |
| --- | --- | --- |
| WM32Create | call-owned class/name and nested MDI title/structure; original Error and WM32Create unthunk, P18 captures identities | Escaped exception during construction and success followed by absent callback; CBT owns another caller. |
| WM32SetText | existing wparam map borrow/refcount or malloc16/compatibility stack; DeleteParamMap or original free | Abort before normal unthunk must retire exactly one borrow, never a task-wide FreeParamMap. |
| WM32GetText | malloc16 or original DS-based stack; original output quirks and free | Escaped output copyback failure must not leak heap or fabricate completed output. Native capture prerequisite preserves early-reply normal logic. |
| WM32GetMinMaxInfo | callback stack POINT16[5]; original conversion back | No independent heap lease. Interrupted copyback and callback-frame restoration, not a new free table. |
| WM32GetDlgCode | malloc16 MSG16; original input-only free | Abort after acquisition, before normal unthunk. |
| WM32DrawItem | callback stack; original input-only stackfree macro | Callback frame, not heap. |
| WM32MeasureItem | callback stack and optional appended text; original compatibility padding and output copyback | Original padding/sign-extension/width quirks remain; interrupted output/frame. |
| WM32DeleteItem | callback stack with original private-item conversion | Original object conversion and frame; no independent heap free. |
| WM32CompareItem | callback stack and original result | Original frame lifetime. |
| WM32NCCalcSize | callback stack rects/optional WINDOWPOS; original nested layout/copyback | Captured source layout, moving guest memory and interrupted nested copyback. |
| WM32MDICreate | malloc16 class/title/structure, original partial Error and release; P18 captured title | Partial escaped exception and cleanup after successful construction without callback. |
| WM32SizeClipBoard | temporary GlobalAllocLock16/handle, normal original unlock/free | Original SIZE/PAINT call-owned global, distinct from clipboard data ownership. Abort must not free original clipboard data. |
| WM32AskCBFormatName | malloc16 output, original copyback/free | Interrupted output/free separation. |
| WM32DDEAdvise | original DDE pair/list owns transferred h16; original packet unthunk depends on hook mode/result | Transfer commit, rejection and peer/task death require DDE owner, not generic call-finally. |
| WM32DDEExecute | original DDE pair/list owns transferred command h16 | Same protocol distinction; normal callback return intentionally does not free it. |
| WM32DropObject | malloc16 input, original free and optional cursor-result conversion | Aborted resource release must not manufacture a successful cursor result. |
| WM32CopyData | original CPDATA list, captured acquisition flags and P13 finalizer | Existing proved synchronous group retained; does not supply ownership for other families. |
| WM32WindowPosChanging | callback stack WINDOWPOS16 with original output conversion | Frame/output, not heap. |
| WM32WinHelp | call-owned shared buffer; original GlobalUnlockFree16; P18 handle0 correction | Abort release is distinct from normal SetForegroundWindow side effect. |

Concrete anti-overreach findings: stackalloc16 only moves TD.vpCBStack;
stackfree16 is empty by original design. CallBack16's normal return restores
TD.vpStack from its original CBVDMFRAME. This alone does not prove escaped
host exceptions restore the frame: wcall16's selected body has no finally
around host_simulate. That is a frame/execution obligation, not permission
to add a second stack allocator. Most GETPSZPTR/GETVDMPTR reverse views are
direct bounded selected-x86 pointers; FREEPSZPTR/FREEVDMPTR are no-ops in
the free build, not omitted HeapFree calls. The independently implemented
COPYDATA finalizers intentionally cover only their acquisition flags.

Normal unthunk is not a generic abort API: GetText/MeasureItem/NCCalcSize/
WindowPos write outputs, WinHelp activates a window, DDE advances protocol
ownership, UnThunkEMMsg16(EM_SETSEL) can send EM_SCROLLCARET and create another
callback. Blindly invoking every original unthunker from finally would alter
behavior. Cleanup must stay paired to exact ownership/commit state, preserve
normal original algorithms and omit uncommitted results. The absence of a
universal unthunker is not itself a missing original control implementation.

The entire reverse native-allocation sweep yields five concrete unchecked
acquisition branches, selected together as DIV346:

| Original reverse branch | Actual old failure and existing rejection path |
| --- | --- |
| ThunkEMMsg16 / EM_REPLACESEL | LocalAlloc returns NULL, immediately used by RtlCopyMemory. Return FALSE before copy; normal LocalFree unchanged. |
| ThunkEMMsg16 / EM_SETTABSTOPS | STACKORHEAPALLOC returns NULL; getintarray16 quietly skips NULL destination, then TRUE permits native call with absent required array. Return FALSE at acquisition. |
| ThunkLBMsg16 / LB_GETSELITEMS | Same allocator returns NULL; TRUE permits output call without acquired buffer. Original result/count copyback and STACKORHEAPFREE remain. |
| ThunkLBMsg16 / LB_SETTABSTOPS | Same silent NULL conversion as EDIT tabs. Preserve original signed cItems>0 predicate; reject only actual attempted acquisition failure. |
| ThunkWMMsg16 / WM_WINHELP | malloc_w returns NULL and copy is skipped, but TRUE submits a lost payload; unlike adjacent ASKCBFORMATNAME's existing FALSE rule. Release view and return FALSE, retain original private layout and normal free. |

The macro STACKORHEAPALLOC already uses sizeof(MsgBuffer), an inline array
of one original MSGTHUNKBUFFER union, not sizeof a pointer. Preserve this
real capacity; no threshold or allocator rewrite. Both array conversion
helpers explicitly test their native pointer, so tabs are a false-success
bug, not a proved NULL write. Counts0/negative keep their original behavior.
Other reverse allocations: ASKCBFORMATNAME already rejects NULL; COPYDATA
uses P13 rollback; clipboard globals use original handle rules; DDE pairs
have their protocol owner. BM/SBM/ST/MN create no extra native allocation;
CB_DIR alone registers a compatibility parameter-map reference, whose abort
lifetime remains in the fixed map group. No extra buffer ownership is
invented for direct guest-pointer views.

Original dispatch remains the behavior owner. ThunkMsg16 initializes
fThunkSucceeded FALSE, records its delegate result and returns NULL on FALSE.
Existing WU32SendMessage/SendDlgItem/CallWindowProc/default/MDI/Post paths test
its returned target; HWND0-valid PostApp/Dispatch/IsDialogMessage and reverse
Hook paths use the already admitted explicit success field. getmsg16's
retained scalar route sets success itself; special original messages use
ThunkMsg16. The proposed five checks therefore use a pre-existing rejection
contract, not a new error protocol or delegated native control replacement.

Four-rung decision for346: original translation units already compose and
remain selected, no new import; a lower allocator/facade cannot fix original
consumers ignoring NULL; minimal local guards in those exact owners are
necessary, registered in README before source edits with20 executable-line
budget. No new adapter behavior, global flag, helper HWND, registry, guest
change, shell rule or semantic concession. Source-exact faults must prove all
five old failures independently; selected normal/inline/heap/zero/count and
one-release paths plus actual original control messages follow only after
the complete group is implemented. This is an implementation admission,
not another runtime pass or full M4 closure. All other table rows stay S4.

Adjacent signed-count review finds one additional paired edge within this
same array-constructor cohort, before product build: both EDIT/ListBox tabs
construct using signed INT32(wParam)>0, but unthunk uses unsigned WORD>0.
ForFFFF, no array is acquired and lParam still holds the original guest token;
normal release would pass that token to STACKORHEAPFREE/free_w. Match the
original signed construction predicate in both release sites. The submitted
native negative-count request and original result behavior remain unchanged;
only a free of storage never acquired is removed. Extend exact negatives to
both signed cases; zero/inline/heap cases stay mandatory. This is paired
ownership repair, not another control feature or a new semantic concession.

### Reverse-construction dependency implementation and first evidence

DIV346 is staged, not delivered. All five acquisition guards and both signed
release predicates are complete before the first product composition. Raw
MVDM delta14 additions/2 removals includes local provenance. The exact-source
test keeps original PARM16/MSGPARAMEX/MSGTHUNKBUFFER, STACKORHEAP macros,
complete Thunk/UnThunkEM/LB bodies and original integer array helpers; only
lower memory/allocation services are doubles. The private-help acquisition/
release case is an exact source slice, not claimed as a full WM dispatcher.
The original union is84 bytes and complete MSGPARAMEX140 on selected x86.
All normal inline/heap, zero count, signed negative count, text/array copy,
selection result narrowing and one-release checks retain original behavior.

The first fixture build incorrectly duplicates the public SDK DROPSTRUCT
with a guest declaration and failsC2371. Remove only the duplicate: native
DROPSTRUCT already comes from the same SDK as the product. R2 old bodies
fail all five missing-allocation assertions; candidate16 scenarios pass.
The signed-release sweep then extends the fixed group: r3 old9 assertions
fail/current18 scenarios pass. R4 repeats the complete18 after restoring
repository-native LF format for the two initially exact indexed sources;
there is no whole-file newline diff. The old raw working/upstream SHA versus
indexed normalized SHA is not misrepresented as a new semantic change.

First source-complete product build PID8788 runs15:43:57.754--15:44:14.191Z,
exit0, compiles all three changed MVDM units and links WOW32 through the
stable Release/MT/CCPU40 graph. Canonical ZIP plus exact named release13,
replacing only built WOW32, form the first isolated candidate, not installed
O:/winnt material. Original unrelated compiler/linker warnings remain visible.

Actual-r1 controls-Off PID17692 runs15:44:37.039--15:44:47.468. All9 phases,
new tab/reset and multiple-selection array assertions, final created11/
destroyed9/leaving2 and exactly-once retirement pass. The outer runner still
fails while removing its stage: access denied on O:/tmp/n07e/SYSTEM32/ntvdm.exe.
This is not aggregate acceptance. Later authoritative process inventory finds
zero live stage executables;44 remaining files/5603413 bytes individually
match canonical candidate or authored probe/report. The exact marked owned
stage is removed, retaining all failed-run evidence. No antivirus cause or
runtime regression is inferred from the temporary deletion error.

Actual-r2 controls-Off PID32612 supervisor child identity is retained in its
steps record (the observer report's root launcher is32116). It ends15:46:21.745
with launcher1067 after phase6, at the list's DestroyWindow call. Earlier
phases/errors0 and a prior normal return do not pass this destruction failure.
Do not retry the failing group into green or revert independently valid guards.

Source now proves the reached dependency, not guessed task scheduling:
original client/usercli.h UserLocalFree expands to HeapFree(pUserHeap,0,p),
BOOL nonzero on success. LBSetTabStops' zero-count reset in lboxrare.c tests
!UserLocalFree and therefore leaves iTabPixelPositions dangling after a
successful free; xxxDestroyLBox later frees that same member. Failed free
incorrectly clears ownership instead. Its original file is byte-identical
before correction, SHA E8914F4761ADE70A1A7357B5DC8C17744CE1BC49379D03A99470F815EF85F64D.
Sweep every selected conditional UserLocalFree consumer: the only other
one is EDMLSetTabStops, whose !UserLocalFree branches to FALSE before clearing,
and is already correct. An initial verbal two-control attribution is explicitly
corrected; no EDIT-native or shared allocator change follows. Bitmap-only
LocalFree bindings are not the control-client macro contract.

Register OPENNT-HOST-125 before changing only that ListBox predicate; preserve
the original native heap macro and complete control behavior. Raw delta2/1
including comment, under its6-line budget. No lower facade can fix this
consumer-specific inversion without breaking the shared correct BOOL contract.
This original host defect is independently source-proven and within the
standing minimal-correction authorization; it is not a guest mutation, a new
feature, a name-based exception or approval to rewrite control allocation.

The expanded same-source fixture now includes the complete original
LBSetTabStops, the exact original destructor's tab-resource slice and original
MultDiv macro. Its first r5 build lacks that arithmetic macro and fails under
WX before running; r6 adds the actual original macro, not an approximate
MulDiv replacement. R6 at15:54:04.154--15:54:18.011 compiles old19484/current39164
and runs old26420/current4532. Old returns1 with13 failed assertions (all named
allocation/signed-release cases plus successful-reset dangling ownership,
second destruction and failed-free loss); current returns0 with21 scenarios.
It preserves failed native free ownership, normal tab conversion, allocation
failure, repeated reset and normal destruction's one release. Neither model
is actual guest/runtime acceptance or an injected real allocation failure.

Model result24ABF7195FDB3DF71D5E3DBB69FA502E7A84DF63828BC0C5355A828D9067BFB9;
fixtureEA9B9C84395EF24C46046E0E39547DAAAED5510F051BDD029C54F0CBBF574668;
producerB3B428ED725C73092E2AA436036918D617C333499B22EEAF03D9DC93F37183F7.
Current runtime source hashes:
wmsgem251325307C9E698EE579E3493F15F93ABB6DAFE7D289F334AC3714C513566BE9;
wmsglb9F5D3549B2E3C1FCAA4342DA0A04D37F74477DDA07903071291E74539AF426A2;
wmsg16805C9E1958CDA9474838F0ADD263396C79F9AAC505F5606F671DE75D1C9F199F;
lboxrareF5D1D03E2C274A67450DB71122A83BE73607441080DFB379EBD6A86466DF6797.

Whole dependency build-r3 product24408 runs15:54:18.843--15:54:32.566, exit0;
only the newly changed original ListBox unit recompiles before final link.
Earlier candidates stay sealed. A fresh candidate-r3 also selects the matching
newly linked worker; unchanged other11 host images come from the exact P18
manifest, not mutable installed contents. Original ZIP/media/config stay
unchanged. New actual-r3 runs all controls and forward/reverse construction
under Off/Win16/All; full retained gates and publication are still required.
No claim that the entire M4/exception/DDE/clipboard inventory is complete.

Actual-r3 supervisor49876 runs15:55:06.455--15:56:20.141Z, exit0. Six complete
serial cases, not a selected later phase, pass:

| Case | PID/start--end UTC2026-10-19 | Required original-boundary result |
| --- | --- | --- |
| controls Off |46164/15:55:10.627--15:55:19.180|all9 phases; EDIT replacement/tabs, ListBox64 tabs/reset and64-capacity selection[1,3], all retained control checks; created11/destroyed9/leaving2 and one normal retirement. |
| construction Off |34684/15:55:19.202--15:55:32.725|original MDI3 create/destroy, captured title/param, text3, forward private-help buffer/free and reverse512-byte private-help payload/native result. |
| controls Win16 |36108/15:55:32.739--15:55:43.239|same strict control/lifecycle assertions. |
| construction Win16 |9492/15:55:43.252--15:55:56.713|same forward/reverse assertions. |
| controls All |44612/15:55:56.727--15:56:06.655|same strict control/lifecycle assertions. |
| construction All |33508/15:56:06.668--15:56:20.122|same forward/reverse assertions. |

Reverse-help assertions are in the authored source's ordered successful
construction path and peer's errors0, not an invented second helpCalls count:
the original forward report still reads helpCalls1/helpFreed1. The512-byte
reverse input exceeds the original84-byte inline buffer; native peer checks
size, leading/trailing payload sentinels and exact return447A007E. Normal
original UnThunkWMMsg16 frees this transient native buffer. Actual allocation
faults remain model-only. All six cases include strict stage cleanup success.
The earlier access-denied and1067 runs remain failed aggregates; the latter
has no saved native exception stack. Exact-source counterexamples prove the
bad ownership and required dependency, not every past similar exit's cause.

Sealed candidate-r3 NTVDM
304FA453A1A22728E143C99E1FD5549DC8AA3FB191AB27E988E9CD206310B35E,
WOW329A21275C4753F6D614F084CC7D148A1653E70C157370600312D1AB73422E9467,
13-image manifest530EE7BC46EF65D7AB859B3821B9A9240811621F464F8459EFF5EB55D118152D.
These do not yet replace the P18 release. Four runtime originals change:
MVDM14 additions/2 removals plus USER2 additions/1 removal,19 raw lines
including comments, no new authored runtime implementation or guest change.
Source-register gate passes278 selected/180 byte-exact/98 registered content
differences/issues0; three previously exact files become registered changes.
The retained acceptance supervisor41556 starts15:57:50.680 with an actual
10800-second ceiling and exact child phase deadlines; complete floors,
installed verification and coherent publication are still pending.

The frozen346/125 batch does not absorb another optional-map path. Within
the already listed CB_DIR/map obligation, source now narrows a separate
counterexample: when W32CheckThunkParamFlag is true and the guest pointer is0,
ThunkCBMsg16 calls AddParamMap(0,0); that creates a node, but FindParamMap
returns a numeric0 as its not-found sentinel, so normal DeleteParamMap cannot
retire it. FreeParamMap on task death is not per-call cleanup. This is the
remaining parameter-map cohort, not a direct native-buffer acquisition or
permission to modify wmsgcb while current acceptance inputs are frozen.
No new runtime code, branch waiver or full-map completion is claimed here.

Similarly, actual CallBack16 is wrapped by wow_user_native_extension.c, whose
finally restores callback execution/borrow ownership. It does not restore
original TD.vpStack/vpCBStack or captured CPU IP; original wcall16 performs
those normal-return writes itself. Therefore an outer scope's existence
cannot be cited as exception-frame proof. The next frame/abort reconciliation
must preserve the original W32TryCall/W32Exception disposition and task/stack
identity, not blindly restore a stale whole-machine snapshot. This remains
the existing fixed exception/frame row; no replacement exception policy is
selected, and ordinary callback FALSE already takes the normal unthunker.

The full retained-r1 supervisor41556 ends16:14:17.658Z exit1 at WRITE-Off:
four4/8/12/16-second samples retain worker3488 but no MSWRITE_MENU/DOC/PAGEINFO.
All preceding35 rows pass, including source registers, native21/original30,
345's36-case fixture, S3 twelve, message18, MINE3/SOL3, style, modal reference3
and complete-logon gameplay3. The failed WRITE aggregate is not acceptance;
DOS/external/cross/pending have not run in that supervisor.

Its controller incorrectly inherits the developer process environment rather
than the retained supported Userenv block. A fixed single P18 exact13 control
(33896,16:16:26--16:16:52Z) under the same inherited-environment policy also has no
WRITE windows in all four samples, worker38844 still alive. Thus this observed
startup failure is not unique to346/125. The control's overall exit1 also
includes a test-controller cleanup error: it replaced two images after the
physical-stage marker had sealed candidate identities. Normal cleanup correctly
refuses n321. At16:18, after verifying all13 against P18 and no live stage
process, the controller restores the recorded sealed candidate13 and uses
unchanged identity-checked removal. No cleanup predicate is relaxed.

This is consistent with the previously source-proven immutable COMMAND large-
initial-environment limitation, not a fresh observation of guest EnvSiz writes:
the inherited observer environment is not a supported Userenv launch proof.
The preceding read-only host shape is78 entries/6159 characters and the
baseline-control shape78/5982; no byte-identical inherited block is claimed.
Supported
CreateEnvironmentBlock current-token/inherit=false is45/3349, unfiltered.
Do not claim exact failing guest instruction attribution from this comparison.
The unchanged candidate's completion supervisor49044 starts16:18:20.712Z,
real5400s deadline, reuses only frozen passed rows and exact13 identities.
WRITE/DOS23088 starts16:18:22.965 with that complete Userenv, test-owned short
TEMP/TMP, the same observer and unchanged four-sample assertions. Then run
the unexecuted external/cross/pending groups. No source/guest or installed
package changes are made to obtain this input-corrected acceptance.

Completion-r3 is not a pass: WRITE-Off passes all four strict samples, but
Win16 has a visible Write/#32770 dialog and original IDPMTCantRunM text in
every sample, without the required document windows. Its23088 child exits1
at16:19:12.883, supervisor49044 ends1 at16:19:13.263. DOS/external groups are
still unexecuted. This second outcome is not explained away as the inherited-
environment failure. Research-only original initwin.c FInitWinInfo/InzFailed
uses this string for many failed startup operations; it does not itself
prove an allocator failure. No guest code is rebuilt or patched.

Predeclare one four-case comparison, P18/346-candidate under Win16, then
P18/346-candidate under All, with complete supported Userenv and the same
strict samples. Existing balanced dispatch/return, window and failed-file
observers are enabled at short stage-local paths; no new product diagnostic.
Supervisor41560 starts16:21:10.305Z with a600s actual deadline. P18 is sealed
as a separate reference before physical-stage marking, avoiding the previous
controller's identity mismatch. Preserve every outcome and first failing
original API/return; this diagnostic does not convert any earlier failed
aggregate to acceptance or authorize publication. No source change is made
between these declared cases.

All four declared diagnostic cases pass: P18-Win16 at16:21:19.243--16:21:42.747,
candidate-Win16 at16:21:43.642--16:22:05.918, P18-All at16:22:06.830--16:22:29.325,
candidate-All at16:22:30.195--16:22:52.536. Supervisor41560 exits0 at16:22:52.670.
This leaves the prior nontraced Win16 initialization failure unlocated; no
failed API can be inferred from traces of passing launches. Shared unsuccessful
optional WFWNET/OLECLI/COMMDLG/WINSPOOL search probes occur in both passing
versions and are not evidence of a new missing-file regression.

The declared final ordinary gate, supervisor15496 at16:23:23.953 with5400s
deadline, turns all diagnostic path variables off by selecting the complete
supported Userenv anew. Child28936 starts16:23:25.652. It reuses the exact
read-only observer and retains the4/8/12/16s main/document/pageinfo assertions
for every policy, then DOS17/17 and the still-unexecuted external groups.
All three ordinary WRITE policies now satisfy all four samples, without a
source/binary change. The earlier inherited and Win16 failures remain failed
records and existing unexplained WRITE risk, not repaired or waived assertions.

### P19 bounded delivery

All times below are UTC2026-10-19. Source/actual346/125 witnesses and failed
intermediate runs above remain part of this delivery, not discarded retries.
The final supervisor revalidates every frozen source/test input and sealed13
image after the following terminal rows. Earlier passed35 retained-r1 rows
are reused only with those unchanged identities, not elapsed-time arguments.

| Group | PID/start--end UTC | Result and boundary |
| --- | --- | --- |
| final retained supervisor |15496/16:23:23.953--16:33:48.582|exit0, real5400s deadline, frozen inventories unchanged. |
| ordinary WRITE/DOS |28936/16:23:25.652--16:26:13.279|complete unfiltered Userenv, diagnostic paths absent; WRITE3 four strict startup samples each; Console17/Window17 text/interaction/exit. Not WRITE editing/save/full exit. |
| external/native |42696/16:26:13.315--16:29:10.892|9/9 local/native32/native64 across3 policies. |
| cross/shared worker |41308/16:29:10.906--16:31:28.022|6/6 across3 policies. |
| pending send |13728/16:31:28.034--16:33:48.035|6/6, native destruction controls, nonblocking forbidden-order negative exits1 as required. |
| coherent publication |40792/16:34:33.609--16:35:11.053|exit0; backup exact P18, publish same13 to assets/release and O:/winnt/system32, final hashes equal; exact-path active process count0. |
| installed style |23840/16:34:39.512--16:34:56.239|both-width boundary pass. |
| installed ordinary logon |36804/16:34:56.259--16:35:10.916|three gameplay/normal-exit policies pass, full Userenv; no final identity drift. |

Reproduction uses verify-wow-reverse-construction.ps1 -BuildRoot (fresh
disposable directory) -CompilerDriver (stable wow32/msvc-x86.cmd), old049ae27da
versus current selected bodies; verify-win16-message-probe.ps1 -Construction
-StructuredPayloads -ThreadClientView and each -Win16Style, and the strengthened
verify-win16-hook-probe.ps1 controls mode through verify-wow-s3-guest-contracts.
Remaining entrypoints/arguments are the same retained native21/original30,
six message modes, MINE/SOL, style, modal, complete-logon, read-only WRITE
-WaitTarget and Verify-CommandExitStatus groups described in P18. External
verification selects the sealed PackageRoot/ManifestPath with ordinary,
-CrossWorkers and -PendingSend separately. Every invocation is hidden/owned;
interactive observation slices never set a test deadline.

Retained result A2728051D42A742FEF6F80AF6CFCE1C54332AF63CB0AA986846AF0405AC89813;
final step inventory AC6DE2FA1D33367198DF0E5886550BDFC24ACABFCC1E10AE4A99D868D8D9D1F1;
retained-r1 inventory D9AADC237F0EB793D6109A918E225C3A8F1E5FE4D5552AF7687C17D0E0386A52;
S3 twelve FF3DBFC71B05237432D490C17226724FB736296F8923158135BD6C3E529CEC3C;
WRITE/DOS 2B80C28A22C8DE6EFA7507EA98968C966269FD822B724B459EF92B56D51CBAD1;
fixed diagnostic pair3F9F05C636E3785FD36862DED645237784302ECCB797429632634DFFAA69175E;
external600A518C541B8EBF5C91C598B4DB4F1398920825F09F35F4AB88E4E9A7DFA67F;
cross3816ED63C55DDA9309BE9FB528A2FD2F43171BA2281D91725DBF7DADA1E0C77C;
pendingFF2B4A38060B16DC9C8ACDFB49B37F42B235A07D8F6422516E249F582E0E75EA.

Published NTVDM304FA453A1A22728E143C99E1FD5549DC8AA3FB191AB27E988E9CD206310B35E,
WOW329A21275C4753F6D614F084CC7D148A1653E70C157370600312D1AB73422E9467;
release manifest C3948242A475E38C987B71D86D83326065555B178061A6E42E101FDD9E2F9EB6.
Canonical guest ZIP remains7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.
Other11 host images and add-ons are unchanged from P18. Runtime original diff
is16 added/3 removed raw lines including comments, four original host files,
no new autonomous runtime implementation, import, guest edit or ABI change.
The registers are346/125; selected278 becomes180 exact/98 registered content
differences/issues0. This closes only that finite source group. Full M4/maps,
exception/abort/frame and other M1--M6 obligations remain; older0102, WRITE
and S3 risks are not retrospectively repaired. S5/S6 remain unadmitted.

Final review35844 runs16:36:17.601--16:36:54.032Z, exit0, actual600s ceiling:
documentation governance/links, changed and new PowerShell parsing, diff check,
candidate13 PE widths/hashes, frozen source/test inventories, both publication
destinations, unchanged guest ZIP and Current's32KiB ceiling all pass.
Publication result42000B921D40AC32DF12A77E33F1A93E398F2EF9066F8B00EC3B729D945980D6;
publication owner8BF8241D9316BFE5A4F88949DAAEB03264259FF5759A8ACEED8BB035668E2782.

Disposal preview joins all background owners and checks actual CIM executable/
command paths, not merely a TasksExited switch. Exact targets are only
build/t447-s4-reverse-construction (4617 files/274801771 bytes) and
build/M0-T447 (one independently authored28-byte G7.COM; no other contents).
The real sole cleanup tool passes default preview with Applied=false. Its473
sensitive-file reviews compare original copies to canonical ZIP entry SHA256,
assembler listings to owned probe producers, signed mode-tagged binary reports
to tracked runner formats and G7 to its exact authored bytes. Earlier
previews correctly stop before deletion when a raw report is misclassified
as an executable or an expected mode signature is too narrow; source identifies
MSG2/6/7/8/9, not a weakened unknown-file rule. No Git/link/configuration/user-
data exception remains. Ownership-manifest hash
8FFFBFFA2557EBBDF80FB5A55B211AFBCC0DEC989828217C4C91BBCF83F016BC;
exact tree digests9B5A884504F4CAD90C81F8101E8794742014675AC4441F59FC6DCA3A04A10C61
and30949138D3EDB88D91923B95DFF48C41CD3DCB7CBA50FB5CE3D169FB60A55AA7.
Necessary terminal evidence/source/artifact hashes are retained above before
Apply. Preserve both valid cache roots and published assets; no prune.

First Apply stops before any deletion: directory div346-g7 appears changed
inside preflight, although both full tree digests above remain identical and
CIM finds no writer. Direct experiment proves stale enumeration metadata:
Get-ChildItem reports639280238769757164, Get-Item reports639280238769834470;
DirectoryInfo.Refresh changes the former to the exact latter value. This is
not a permitted tolerance or an actual ownership change. Clear-BuildOutput's
existing enumeration loop now refreshes each item before link/Git/membership/
metadata inspection (one executable line plus comment), preserving all guards.
This bounded delivery dependency does not reopen compiler configuration or Td,
introduce another cleaner, or affect already frozen production/test inputs.

Existing Test-BuildHygiene.ps1 -CleanupOnly runs hidden47736 at16:41:23.504--
16:41:25.586, exit0:22 assertions, including preview, all-target atomic refusal,
new generated-looking content, ownership, media, Git, traversal, cache and
junction refusal, and legal deletion. Only its deliberately authored refusal
fixtures are created/removed. Extra build/td-build-audit contains only two
generated JSON records (1781 bytes), now added to the exact disposal inventory.
Re-preview against fresh actual metadata before Apply; no safety bypass or
force-remove is used to get an empty build listing.

Final dependency-inclusive review31628 runs16:41:55.355--16:42:18.441Z exit0:
the same governance/links/PowerShell/diff/PE/source/dual-publication/guest gates
pass. Cleaner sourceFBA473D06ED5A74598AAFDDD2F4B317E85B9AF98BF7CF4DB13EB6AFE03895D69;
CleanupOnly resultABFEA5069B9CCDB80F3A2E1F8ADBE14969BCE616B09E2C5344F9E9CC9F8F2325.
The new preview reviews the same473 sensitive copies/reports and all three
exact roots. Ownership manifest879374BB424002D40433A41373722BD4DCA1137040BF95EA3E6974E74B3AAA80;
final main-tree digestCF7836318DCBD5736ABEE06873950445F1C918C902C3A2EA404E12389C28C199;
G7 digest unchanged above; safety-output digest9D1629E816689F7DAD8C4AB90C9B6DDBBF641C434F811AA355CCE2F964F371B7.

After a fresh actual process-quiescence check, Apply succeeds at16:42:56.761Z:
main4623 files/274804951 bytes, G7 one/28, safety records two/1781. The generated
review manifest is separately previewed then removed by the same real tool,
one file/148089 bytes, digest812179B1D8F909937AA197A147A3C9101D02D98B909BFF454919427965C3128C.
Total4627 files/274954849 bytes (about262.22MiB) removed; final CheckResidual
lists only .ninja/.ccache. No process is killed for cleanup, no cache/global
cache/Git pruning or user/guest deletion occurs. Disposable raw evidence is
removed only after the above necessary results/hashes are archived; published
images, immutable ZIP and recoverable P18 Git baseline remain. No other build
tree is retained as another task's unexplained exception.

Post-disposal hidden review24072 runs16:43:27.795--16:43:54.454Z, exit0,
180s real ceiling, stdout/stderr captured through owned in-memory pipes:
governance/links/diff, actual final residual inspection, both published13 and
immutable ZIP pass without creating another disposable tree. Single-person
second-role review checks the complete changed files, original budgets,
test-only provenance, unproved branches and unchanged guest against the owner
contract; there is no S4/T closure or new semantic concession. Reviewed source,
tests, tool dependency, authorities and coherent assets form sequential P19.

### Post-P19 complete parameter-map ownership cohort

Source baseline18d0ec060024912e6b03aec69bdded6c9181af03, clean main/origin;
P19 remains released/installed. This is source-led audit/design, not a test
result. No compiler or diagnostic runtime was used to select these defects.
One original worker-local gParamMap owns references/aliases; there is no
project substitute to retain or delete. Native USER owns actual delivery,
original converters own representations, existing scopes own execution and
the new per-call bits only record acquired cleanup work. No second authority.

| Complete selected entry/caller | Current fact / disposition |
| --- | --- |
| InitParamMap / wow32 initialization | Original once-per-worker cache initialization; keep. Failed cache allocation already falls back to original malloc_w. |
| AddParamMap: ThunkWMMsg16 text family | P18 preserves the direct view on optional allocation failure, but normal DeleteParamMap by native pointer can still consume a different active alias node when this call acquired no reference. Record successful acquisition; release by captured guest key, not merely matching a flat pointer. |
| AddParamMap: ThunkCBMsg16 CB_DIR | Compatibility flag controls identity optimization; current code inserts zero pairs and ignores allocation failure. Zero return is the map API's not-found sentinel, so Add(0,0) creates an unretirable per-call node. Central zero-pair rejection plus actual-acquisition progress fixes all producers, not a filename/command exception. |
| AddParamMap: WU32SetWindowText | Guarded non-NULL view but ignored map outcome; unconditional normal flat-key delete and no local exceptional release. Capture f2 before native reentry; finally release only acquired guest-key reference. Keep native SetWindowText and void guest result. |
| GetParam16 / WM32SetText | Original mapped ref borrow, otherwise malloc16 or DS-compatible stackalloc16 and copy. Capture mode immediately after acquisition, before putstr16; normal and exceptional release must consume it once. Keep original conversion and compatibility selection. |
| GetParam16 / WM32CBDir | Same borrow or owned heap fallback, no DS-stack branch; keep flag-controlled lookup and original CB_DIR translation. Input-only unthunk is a safe specific cleanup owner, not evidence other unthunkers are abort-safe. |
| FindParamMap / GetParam16 / DeleteParamMap | Original exact/current/alias matching and refcounts remain. NULL input must not resolve to an unavailable selector's NULL current address. Actual callers pass FINDPARAM storage; the unused documented NULL-output form is not selected and needs no speculative ABI repair. |
| Add existing-node refresh | Refcount increments before ParamMapUpdateNode, which can enter guest before returning. Preserve this pin; rollback through original DeleteParamMap on failed/abnormal refresh. Reordering increment after the callback would remove the pin and is rejected. New-node publication has no guest callback after linking. |
| ParamMapUpdateNode / W32LocalReAlloc | Retain original selector refresh and alias history, including its16-alias policy. Reject absent current view before changing native identity/alias history; propagate NULL, never invent a valid old pointer as a successful refreshed result. |
| GetPModeVDMPointerAssert | Original nonpresent selector calls RET_FORCESEGMENTFAULT. wow16cal invokes IsBadReadPtr; gpfix probes selector/data under the original fault trap. This is a real kernel callback, not pure lookup. Original CallBack16/scopes still own scheduling/frame behavior; no new lock held across it and no claim that outer borrow-finally restores CPU frames. |
| FreeParamMap / WK32 task exit | Original task-owned residual purge, not normal per-call release. Keep exactly this owner and original call. Individual-task forced termination belongs to S5; do not claim new survival guarantees for an application invalidating/freing an active borrowed input. |
| CacheBlockInit/Allocate/Free/IsCacheBlock | Keep complete original allocator. Descending free list, split/coalesce and malloc fallback are dependencies to exercise, not permission to replace it. The apparently suspicious right-merge/previous-neighbor case is unreachable for a valid ordered nonoverlapping list: the preceding iteration would already take its left-adjacency merge. Do not patch an invented reachable defect. |
| AddParamMapEx / MAPPARAM_EXTRA | Compile-time disabled and no selected caller; retain original inactive source, no new extra-buffer feature. |
| Alias/duplicate flat identities | Preserve original first-match native-to-guest selection. Cleanup uses this call's actual acquired guest key, so optional-map failure cannot remove an outer equal-flat node. No new identity preference or global generation manager. |

Original flag producer sweep finds dwWOWCompatFlags assigned at WK32InitTask;
normal reentry restores the calling task. Capturing acquisition mode is still
required: rechecking a flag cannot prove allocation/map success. Reverse
text and CB_DIR use flags/release address in existing MSGTHUNKBUFFER (84-byte capacity
unchanged); forward records reuse original dwParam and their captured Parm16
key, not a new struct field. Family identification follows the selected
original unthunker/function, never an arbitrary numeric message alone.
Clear progress before Delete/free to make the normal-plus-finally composition
idempotent and safe against nested release callbacks. Heap versus DS-stack
fallback release retains the original allocation kind; direct selected-x86
views retain original FREEPSZPTR semantics, not fabricated heap ownership.

Complete caller closure: existing wmdisp32 W32Win16WndProcEx, wudlg dialog
gateway and CWP/CBT/WH_CALLWNDPROC/message-filter/send hooks already finally
call wow_message_unwind32; wumsg send/dispatch/default/MDI/filter/callproc and
wowhooks reverse paths already finally call wow_message_unwind16. Extend
these existing finalizers only for WM32SetText/WM32CBDir and their reverse
input-only counterparts; normal callback FALSE already reaches unthunk.
Do not run GetText/output, EM_SETSEL, WinHelp/foreground or DDE unthunk on
abort. Direct WU32SetWindowText lacks a message record, so local original-
owner finally is smaller than constructing a fake message or adding an
adapter registry. Original W32TryCall/W32Exception remains exception policy;
service-abort cleanup does not claim every modern native WNDPROC exception
propagates, nor solve the separate original CPU/frame reconciliation row.

Four-rung disposition: direct original table/cache/conversion is selected;
existing same-shaped adapter finally supplies only scope mechanics. A lower
allocator/adapter cannot know which ignored map result this original call
owns, and a normal unthunker lacks captured acquisition state. Thus register
347 before minimal owner edits, and098 before extending the existing finalizer.
Do not copy original mapping/conversion algorithms into a new adapter. No
new guest/import/public export or message transport. Budget180 executable
added/removed mirror lines across the seven registered files; target is a
small conditional/progress/finally seam, not an allocator or mapper rewrite.

P19 baseline SHA256s: walias85E9F0B98E2A00D6B1011CCF36F51EDBF636B8B41467B78021D85DAAB5327978;
wmsg16805C9E1958CDA9474838F0ADD263396C79F9AAC505F5606F671DE75D1C9F199F;
wmsgcbCA644A033B4ED5974F70FAD75615EF18C80466F3ED6D987554CE031CACAB1E13;
wmdisp328EFE01255A083F7EDB11E24FCAA7AAD87CCED6D7C8B99DECD8D9BA3334031F6E;
wcntl3236C8CB9A46F13998D0459BC4869DC6DD4191423C0429A7335EBD26F68C804B59;
wuwind010E533C8CA043469E1BAB389D39ACD89DFAEB6A50F9926F3E07815C5A4B9F7A;
wparam939785D0AEE348459E02136EFAFDCBDAC7AA1D7612F4BFCF8C4F21ACF677271A.

Verification is designed before edits: exact complete original map/cache and
text/CB_DIR bodies plus actual finalizer, old18d0ec060 versus current, with
NULL, all acquisition failures, equal-flat outer reference survival, nested
borrows, moved pointers/aliases, refresh FALSE/SEH, heap/stack fallback,
interrupted input copy, repeated finalization, selective task purge and
cache out-of-order release/fallback. Fault doubles supplement, not impersonate
real guest acceptance. Update the existing345 fixture to consume the real
MSGPARAMEX union rather than its old abbreviated test-only struct. Retain
its36 cases and346's21, extend actual control/text/construction probes as
appropriate, then run the complete retained/installed13 floors. Compatibility
flag branches require exact-source proof with both flag values; do not claim
actual flagged guest execution without an actual task/flag witness. No host
global registry mutation is authorized for a probe. The completed source group
and all probes precede one background build cohort; no partial runtime is
published. Unmet real-boundary rows stay S4 obligations, not manufactured
closure or new guest limitations. Scratch/budget/disposal are in the plan.

The entire first implementation and model precede the first product build.
Intermediate group evidence is retained, not relabelled as a passing release:
r1's model compile lacks VPPROC; r2's WX compile rejects original WORD-to-HANDLE
C4306 (tdb16.h confirms WORD, so retain that field and exclude only4306);
r3 executes old16 failed assertions/current1 (UPDATE_FALSE_ROLLBACK). The new
rollback returned inside try, then AbnormalTermination cleanup released the
pin again. Move normal FALSE rollback outside try; finally handles only an
escaped unwind. R4's WX catches updated possibly uninitialized across SEH;
initialize FALSE. R5 executes current50/0 but cache equality detects Git's
line-array transport dropping a terminal newline. Select the complete original
cache through IsCacheBlock's closing brace, not transport-dependent EOF.
No original cache algorithm or acceptance assertion is weakened.

Group-r6 supervisor4336 runs17:13:52.734--17:15:24.492Z exit0: map50/0,
retained345/346 and affected86-step MSVC Release /O2 /MT link pass. Its WOW32
6568C7F6F7E8FF1AEDFAE96D36BC18E66D456D8454C6BA2477D11CADA8980671 is
an internal pre-final-review build, not released or actual-guest acceptance.

Final source closure reads every original normal/abnormal caller, including
PostApp/PostMessage's normal unthunk and wowhooks' changed-message copyback.
The native filter may overwrite reverse mpex.uMsg/lParam before unthunk, but
its captured Parm16 remains original; the forward original wm32mpex record is
not replaced by pMsg16 (a distinct mpex constructs modified output). Thus a
reverse release must retain its acquisition address, and CB_DIR cleanup must
select original OLDCB_DIR, never a mutable native message. Otherwise changed
CB_DIR->CB_GETLBTEXT can interpret progress bits as output state or leave its
map reference. Capture native release address in the already admitted union,
keep its84-byte size, and move only original CB_DIR's pure release before the
mutable-message switch. Other combo output branches retain original behavior.
The adapter matches original unthunker plus captured original message before
inspecting its progress; a RECT/owner-draw value is never assumed to be a lease.
Add exact-source normal/abort mutation counterexamples, including preservation
of an outer equal-flat node and rejection of another combo output as text.
This is the same ownership cohort, not a new Hook routing/identity policy or
a reason to retain a second allocation table. R6 inputs are superseded; rerun
the completed group before sealing/actual acceptance. No partial publication.

The real controls probe adds invalid-drive-string and NULL CB_DIR requests
for all three combo forms, asserting original CB_ERR and unchanged item count
before retaining every existing selection/drop/capture check. Source chain is
combo.c -> xxxCBDir/combodir.c -> xxxLbDir/lboxctl3.c: the original guarded copy
rejects NULL, and FindFirstFile rejects invalid drive syntax without filesystem
mutation. This exercises actual caller/provider/error retirement, not proof of
an activated compatibility flag, successful directory enumeration or every
original control branch. The mapped/flagged paths remain exact-source witnesses.

The added mutation negatives fail the pre-final r7 candidate twice: its capture
was mistakenly inserted in the adjacent CB_GETLBTEXT branch. Restore that
output branch and capture only CB_DIR's acquisition. R7 remains a failed
internal control, never a published candidate. Frozen group-r8 supervisor49224
runs17:20:58.010--17:22:04.025Z exit0. Exact-source map54 scenarios have zero
failures; immutable P19 produces19 failed assertions, including all17 named
required negatives. Actual structures remain reverse140/buffer84/forward72
bytes. Retained construction345's36 and reverse-buffer346's21 scenarios pass;
the affected production MSVC x86 /O2 /MT build also passes. These are not
actual guest or native USER exception proofs. Foundations supervisor46340,
started17:22:58.620Z, owns the continuing register/native21/original30 group;
registration reports278 selected/177 exact/101 registered differences/issues0.
P19's coherent installed/released13 remains unchanged pending actual acceptance.

Frozen foundations46340 completes17:31:29.528Z exit0. Its native21 supervisor
29372 completes17:31:16.794Z with21/21, then original30 supervisor26916
completes17:31:29.439Z exit0; frozen source and artifact inventories match.
Candidate actual supervisor43976 runs17:31:47.406--17:33:09.521Z exit0,
all six actual control/construction groups, with the same tested
WOW3290568B581F97821DB12A04E51661093B8E9DBE1744BD27EE3FBE6D8FDE189AD8
and NTVDM76C739495D83A555702F43676FE563C8E517038CD5E76EB2F3AD9D7545EB5B2B.
The other11 images are exact verified P19 reuse; staging derives only from
the unchanged canonical ZIP. This is sealing, not publication or S4 closure.

Immutable group-r8 result6DCB3D91396A1895A113345557B3924A31F64D3F041A9D3429D5C2073085BC95;
frozen source/test inventory0EB1BA387B3E7EDA9173C1940EB8B421B957718DD50E1E31E083808973D1C6B8;
map result17DE60BB1376CF10533463725B7169366A5BF609814FA7FF8DFB8A0DC2E1BE43;
current54-scenario outputAA33544B969B12DF8D1C4A00A1EDC182BC58852AC151490C4B304F2DA9FE1A13;
foundation resultA48623F5EE3B9D31113175CEE64B95BF5949F933C6ED52F2DB839C7BA989B04D;
native21 summary2210112433A82DAEE66E0931A2FDB8C7699FC49B7C3F6F65355F02EBD825400F;
actual6 resultDEB04AD4C9B1D6C02F8612951797C43EF5758A9A092D8E6E2539F8F57BF2C5BF;
sealed candidate manifest3EC51D4E9618F8BF82DB0651FA059993EE67B462E306637ED55BCE0345CD340B.
Retained supervisor25800 continues the unchanged admitted floors; no failure
can be promoted by a later partial pass and no release occurs before its gate.

Second-role source review: original AddParamMap's reference increment must
precede the potentially faulting selector refresh, otherwise guest reentry can
retire its node. Normal FALSE rollback is outside try; escaped unwind alone
runs the finally rollback. Neither path duplicates the old reference. Optional
map allocation failure must not delete an outer equal-flat node. Reverse
release selects the original guest key and captured native address, not Hook
copyback; forward release records borrowed/heap/DS-stack mode before fallible
copy and consumes it before release. The original cache, alias cap, first-match
lookup, task purge and stack-free convention are retained. Finalizers call
only input-only text/CB_DIR original release bodies; there is no generic
unthunk-on-abort, second allocator, table, mutex or stale CPU restore.
Raw original runtime delta is101 additions/29 deletions including comments,
seven files, below the admitted180-line budget; the adapter adds18 lines in
its existing header/finalizer. No public export, wire or record-size change.
Fault doubles prove the specified body/ownership transitions, not actual
native USER exceptions, physical segment movement or complete callback-frame
restoration. Those retained M4 boundaries are not closed by this review.

### P20 bounded delivery

All times UTC2026-10-19. Frozen retained supervisor25800 runs17:33:28.549--
17:51:33.087Z exit0 with35/35 terminal rows. S3 twelve; eighteen message groups
(normal/extension/structured/early-reply/timeout/Proc32 across3 policies);
WINMINE/SOL gameplay and normal exit3 each; both-width style; pure-native modal
reference and actual guest modal3; complete-logon3; WRITE four startup samples
per policy and Console17/Window17 text/interaction/exit all pass. WRITE uses
complete unfiltered Userenv, physical short TEMP/TMP and -WaitTarget, with no
diagnostic runtime. It proves startup, not editing/save/full exit. External
27344 runs17:44:14.134--17:46:57.372Z9/9; cross/shared45392 runs17:46:57.384--
17:49:12.565Z6/6; pending22052 runs17:49:12.576--17:51:32.945Z6/6, including
native destruction and the required nonblocking refusal (negative exits1).
The original assertions and deadlines remain; no failed runtime aggregate
is promoted, no runtime inputs change during acceptance and no performance
claim follows. Historical0102/WRITE and original-guest limitations remain.

Retained result024344A34447EFAEE33A9F5F5FBFD44EFD9DF4FCB72D38024F2CE76CFE8DF0F5;
step inventoryB985FBFAE31ACD6A71E6634E0EA211D531B56E72F47816A8E46AAB1FE6D2EF38;
S3 twelveEA79D94DE105F498928A3B915AA4CA741DB94F82C17A3CD8652B0E9EECE1C7E7;
WRITE/DOSF7991A82C76DB960F21EB34BBDAB1A9BAB442EDC8B0F22BF69F27A6ED11A9F7E;
external337B43BD9A102FE4D49AE7E3DCABD1151234EA09716CC102B9C5364E6819E563;
crossD8415608493577F558B758B919B6EB240343956538FE0507A9859CCF17234200;
pendingA833F53C8DD5754C1D88F9E0D9C1C5C6F17528746449E71240045BD6D06FEC4E.
Reproduction entrypoints/arguments are the same P19 retained matrix above;
new verify-wow-parameter-map.ps1 selects immutable18d0ec060 and current source,
and actual controls include invalid/NULL CB_DIR in all3 combo forms. These
named real boundaries and fault doubles do not close every M4 flag/protocol
branch or callback-frame restoration; the complete M1--M6/S5/S6 stays open.

Publication37688 starts17:52:00.336Z and completes17:52:35.770Z exit0. It verifies
and backs up exact P19, publishes the same sealed13 to assets/release and
O:/winnt/system32, then passes installed both-width style14772 (17:52:05.138--
17:52:21.154Z) and complete-logon/gameplay3 13416 (17:52:21.177--17:52:35.687Z).
Exact-path process closure count0; no owner session was ended. Final13 hashes
match both destinations; recoverable rollback was available but not needed.
NTVDM76C739495D83A555702F43676FE563C8E517038CD5E76EB2F3AD9D7545EB5B2B;
WOW3290568B581F97821DB12A04E51661093B8E9DBE1744BD27EE3FBE6D8FDE189AD8;
release manifestF9A5A315755919470F8C860A61A6F352EB9DB8E8FD1194DAEBC447E592C8B3F6.
Other11 host images/add-ons and canonical ZIP7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7
are unchanged. No original guest edit/rebuild, new import, semantic concession
or public ABI change. Mirrors278 selected/177 exact/101 registered/issues0.
This is a bounded P20 correctness baseline, not S4 or persistent-goal closure.

Final second-role review40276 runs17:53:11.951--17:53:48.657Z exit0:
documentation/relative links, diff whitespace, changed/new PowerShell parsing,
frozen source inputs, all13 PE widths and both destination hashes, immutable
ZIP and Current's32KiB ceiling pass. Publication result is
20356ADB845C2A8AACA32E43F2DB3D5B09F6FDD2E42519DD193EC92764A51EF0.

Disposal preview12424 runs17:54:06.613--17:54:12.884Z exit0, Applied=false.
All owned supervisors have exited; actual CIM path/creation checks find no
target users, not merely a TasksExited attestation. Exact targets are
build/t447-s4-parameter-map (4423 files/250069066 bytes) and build/M0-T447
(only the independently authored28-byte G7.COM and its empty parent tree).
Their inventory digests are
FDF6D9452D4C1DE9605FA885C5BBA9A956B519360405352F47D40CB5FDA33652 and
A8798F0EB448A97B0E941614690B4E585468246E171D4A096191696B0AC58D44;
360 sensitive/unknown-extension files are reviewed:70 byte-exact canonical
ZIP copies,104 generated authored-probe listings,164 signed-format observation
reports,21 terminal exit/PID markers and one authored COM. Review digest
EC1A85FA2B7468B7C894F3505CF834723AE9BC079658463B6D231AE3FF4242E2.
Necessary results, source/artifact hashes and failed-attempt interpretations
above precede disposal. The sole Clear-BuildOutput tool owns preview/Apply/
residual checks; do not duplicate its safety rules or delete the build root.
Preserve both valid caches and published assets. The complete coherent P19
recovery package remains in18d0ec060; no user configuration, unique media,
Git data or other task output is approved for deletion. No prune/cache clear.

First Apply16164 at17:54:44.512--17:54:48.414Z refuses before deletion:
the scratch caller hashed unordered Hashtable JSON. Independent preview14200
at17:55:01.987--17:55:07.885Z proves identical target membership/metadata hashes
but a different review serialization digest. Fix only that disposable caller
to sort fixed path/hash/reason lines, not the real cleaner or its guards.
Canonical preview44296 runs17:55:20.335--17:55:26.205Z exit0, Applied=false;
the59-byte caller correction makes the first target250069125 bytes, still4423
files; G7 remains28 bytes. New first inventory digest
F30DAE7BC3CABD4540A904C1AB94DDB3771F0D1A8B3507FA725BA51CA5296DA4;
G7 digest remains A8798F0EB448A97B0E941614690B4E585468246E171D4A096191696B0AC58D44.
The same360 exact reviews now canonically digest to
843B20DA4DCCAF1C541B9491032C4EA57EA6887E45B6E671CE41FA808163169F.
No product source, evidence assertion or publication input changed.

Apply34980 runs17:55:41.045--17:55:52.489Z exit0 with those exact final digests.
The real cleaner rechecks all targets before deleting4424 files/250069153
bytes (238.48MiB), then CheckResidual reports only build/.ccache and .ninja.
No process is killed for disposal; only generated copies/evidence are removed,
including the temporary caller itself. User data, valid caches, Git and both
published13 sets are retained. Final review result before disposal is
5D294168ACAF4F10B59706A7E6D7BDFD777FE486E5FBFE9B39D3E47AFF632A76.
Final gate32248 runs17:56:18.136--17:56:37.732Z exit0 after the recorded
disposal facts: documentation/links/diff, actual two-cache residual and all13
both-destination hashes plus immutable ZIP pass. Only recorded review/cleanup
facts change afterwards; runtime/source/test/artifact inputs remain frozen.
Recheck documentation for this terminal record before reviewed commit/push.

### Post-P20 original-frame and simulation-owner audit

Question: what actually owns an interrupted original callback, beyond the
execution/borrow finally already delivered? Baseline1122ab55d55b4d15699715ce1416b092b5887772
is clean main/origin; P20 remains installed. This is new source attribution,
not another runtime pass or a claimed completed repair. No compiler, running
guest, diagnostic image or random test selected the following findings.

| Actual chain / source | Established fact and consequence |
| --- | --- |
| wow_user_native_extension.c CallBack16 -> wow_original_CallBack16 | The public facade finally pairs execution/borrows only. It does not own TD frame fields, interpreter IP or simulator jump buffers. Keep that owner separation. |
| wcall16.c original frame preparation | GETFRAMEPTR may itself enter RET_FORCESEGMENTFAULT. DIV178 snapshots only wTDB/wLocalBP before the second resolution; do not write that VDMFRAME snapshot over guest ES changes. After it, callback cursor/INIT state changes before CBFRAMEPTR can fail. A scope beginning only at host_simulate misses this acquired state. |
| walloc16.c stackalloc16 / CallBack16 | INIT set means derive from vpStack; allocation clears it and reserves vpCBStack. CallBack16 sets it and pushes its own CBVDMFRAME. stackfree16 is intentionally empty. A blanket reset loses outer reserved arguments or changes the next callback's allocation base. |
| wuser.c WU32SignalProc SG_EXIT/SG_GP_FAULT | Original teardown deliberately sets INITCALLBACKSTACK together with IGNOREINPUT before resource callbacks. Reinstating the entry flags wholesale would erase this legitimate teardown transition. Only actual call-owned progress can be reversed. |
| wcall16.c normal simulation and return resolution | host_simulate precedes setIP(SaveIp), TD.vpStack=VDMSTACK, returned CBFRAMEPTR and output extraction. The third resolver can fail after simulation has already returned. Preparation failure, live simulation escape and returned-frame failure are different states; AbnormalTermination alone does not distinguish them. Normal result/guest writes stay original. |
| nt_cprgs.c load_sw_cpu_access_functions | The selected host_simulate_func is c_cpu_simulate. nt_cpu.c's external wrapper delegates to the same selected body, not another executor or a wrapper with hidden exception cleanup. |
| c_main.c c_cpu_simulate -> ntthread.c ccpu386SimulatePtr | NTVDM's TLS ThreadSimBuf.level increments when choosing the original jmp_buf. Only ccpu386Unsimulate decrements before longjmp. Escaping native SEH does not execute that ordinary pop. Fixing just TD/IP would leave a stale native return target in the original TLS owner. No adapter has the authority to recreate or reinterpret that private array. |
| CCPU ccpu386ThrdExptnPtr / GotoThrdExptnPt | Exception/restart slots select level-1 from that same TLS level. An abandoned level affects subsequent original CPU exception handling too, not merely an observer's depth counter. Preserve the actual setjmp function's live native frame; a helper cannot call setjmp and return its stack frame. |
| W32Dispatch / W32Exception | Windows searches filters before finally unwind. W32Exception CLOSE writes RET_FORCETASKEXIT through current TD.vpStack; IGNORE executes a handler, while invalid handles/alignment/debugger paths can continue execution/search. Restoring a callback cursor afterwards can discard the frame containing the already selected termination signal. Preserve the original decision on the API frame whose handler will actually resume; do not replace the policy or infer it from arbitrary native message fields. |
| FastWOW comparison | Its PostExceptionHandler/W32SetExceptionContext explicitly targets ApiReturnAddress and the saved native monitor stack. It confirms that exception continuation belongs to a known API boundary, but depends on native VDM/private monitor context and is not an executable CCPU provider. Do not import that assembly as a shortcut. |
| c_getset.c c_getCpuState/c_setCpuState | Full restoration changes CR0/CR2/CR3, descriptor tables, video latches, A20 wrapping and NPX-state ownership in addition to registers. It is not a callback-only rollback and can undo legitimate guest changes. Reject full-machine snapshot restore, even though original source exists. |
| wkman.c remote callback context | Original source separately saves/restores bounded registers and selectors for its explicit remote continuation. This is relevant source precedent, not permission to copy its globals or treat an interrupted, possibly different-mode interpreter as an ordinary remote return. Segment setters call the actual CCPU segment loaders; selector identity alone is not proof of valid restored control state. |
| session_terminate_current / mvdm_softpc_terminate_current_session | Whole-worker escape closes the original host, marks the session COMPLETED, then longjmps. It is not an API exception eligible for guest resumption. Retain terminal abandonment and existing lifecycle authority, with no access to withdrawn guest state. |
| host_ExitThread / opennt_ccpu_exit_thread | Existing original CPU TLS retirement is followed by native ExitThread, not a recoverable callback return. Microsoft documents that ExitThread/ExitProcess bypass finally; do not fabricate missing callback returns or add manual task retirement from a new finally. |
| Existing original callback fixture | Four phases test resolver failure1/2/3, propagation and output preservation. They do not assert restored TD cursor/flags, abandoned simulation TLS, W32Exception target or real-mode/protected continuation. The real CCPU fixture proves normal BOP/FE order only. Keep both, extend their actual assertions rather than counting their old PASS as full frame proof. |

The current production graph is MSVC x86 /MT /O2 with CCPU/CPU_40_STYLE/NTVDM;
the host vector selects the original TLS simulation branch. The exact graph
and original headers, not the host architecture name, select non-fast WOW.
No FASTWOW/native-monitor profile, guest change or CPU-instruction modification
is admitted by this investigation.

The findings rule out two tempting incomplete implementations: (1) a generic
CallBack16 finally that restores entry pointers/IP, because it misses simulator
ownership and exception target selection; (2) c_getCpuState restoration, because
it rolls back unrelated authoritative state. The required architecture remains
owner-paired cleanup: original CallBack16 owns its call record, original CCPU
owns its exact jump-frame stack, original W32Exception owns disposition, existing
scopes own execution/borrows. No second scheduler, TLS-frame registry, hidden
window or generic CPU state manager is warranted.

Implementation is not yet admitted merely by those negative conclusions.
Three finite coupled design obligations remain in this same frame cohort:

1. Define the caller continuation for preparation, active simulation and
   returned-frame failures, including a legitimate moved backing and terminal
   escape. Establish exactly which control fields may be restored and which
   guest/shared changes must survive. Do not presume saved IP alone is enough.
2. Preserve the selected original filter result/kill signal at the actual
   resuming API boundary. Prefer that boundary's existing call-local identity
   over another TLS manager; continue-execution/search must not run a synthetic
   handler or prematurely roll back a live callback.
3. Pair only the abandoned original TLS jump level, preserving ordinary FE,
   same-level CPU exception continuation, nested callbacks and thread/session
   retirement. The original owner must perform this operation, not an adapter
   overlay of ThreadSimBuf. No new semantic concession has been chosen.

These are concrete pre-implementation dependencies, not a request for owner
approval or an excuse to absorb unrelated CPU repair. All selected owners and
their combined normal/negative/lifetime witnesses must be completed before
the next affected runtime build. Existing output/protocol rows remain S4 and
are not silently closed or transferred to S5/S6. No runtime repair, build,
publication or speedup is claimed by this audit-only progress record.

Baseline SHA256s: wcall16 C44A3AE33014624B30A506A7620A06BB9AFF8B3B0611DC7224CF39C0A519776A;
wow32 E00F224D9353C99F5884A8BC1B7ADA6CF797F5C67AC8FB2F056F6D0069DA8A24;
wkman7F0E802CE413CF76235BBA8F2296B2DFFF4C4A69ED94CDD0216508DB88A214BF;
walloc16 6A1111899ADF5351E03F9989325D2412EF0716DA307A36793DC35ABF585C81F2;
wuser5BA9B920F7C5571CB17171D5DE123CC8FD27A2338B11B600B2057A72F23E3D96;
c_main6039373F8614B77F66DFB8431A59957299048970A25D8B3E2DA5C05070CBDC4C;
ntthread.c EE3EA7B12E544C9C34504D726256167DCE13E7A335F793250824F35D5C71E19D;
ntthread.h A30D41E57C90BC0B27366DEBDAA1E6A23C214CC07D757E9F46C7B43FA636328E;
c_getset02783213DEBC6B9A712ADECF37941B4FEA01C60C0CC86C0D3CDF526B1FF43374;
immutable wow16cal441FCE7B100A5E4ECAAC173F190A02CE33CEE08340EA047037B0FAD3A681147C;
native-extension93D840A9ECE0BE6FE0AFC6FA0597E23CFC2182359CE2BA4BE6E4848D97B6FA54;
thunk-scope371CB5FE8AA52273A01085BADC3174F00CF0321814A62A1266A55D7F21E2A2F0.
Original OpenNT wcall16 is4875844ABFB8D22025480C14174481A194779D83759CE3A696E978B68C55A9F0;
wow32 1DFE9900E6DA3BF0E2984C3DF42BDD155B177CFE7707BB24E1324037147D292A;
ntthread3FD33DC3055AC59E0958A8335C9F3D3155D36D76AC1E178C7DD1E4232DEB5528;
c_main F6CD45A30B6575D38EF95083601594F1403A34EBEF1278DBB8362C19F4A14700.
They retain these ordinary-return-only paths. The pinned comparison search
finds no matching replacement body; this is not a claim that no historical
patch has ever existed or admission to import an unaudited substitute.

Public compiler/host-contract references, consulted2026-10-19:
[termination-handler syntax](https://learn.microsoft.com/en-us/windows/win32/debug/termination-handler-syntax)
documents native thread/process termination bypass;
[exception timing](https://learn.microsoft.com/en-us/cpp/cpp/timing-of-exception-handling-a-summary?view=msvc-170)
distinguishes filter selection and unwind; and
[longjmp](https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/longjmp?view=msvc-170)
describes the separate nonlocal transfer. Installed MSVC14.43.34808/setjmp.h
maps ordinary C setjmp to _setjmp unless setjmpex overrides it. Do not infer
every callback exit's unwind behavior from a C++ example or the word exception.
No external implementation or SDK text is imported into runtime source.

Second-role review accepts these owner/dependency findings, not a completed
continuation design or runtime correction. The source audit changes three
documents only: zero original-host diff, zero guest diff, no build/runtime
invocation or publication. P20 manifest/ZIP hashes remain unchanged. Mandatory
documentation/links/diff, coherent installed13 and two-cache residual checks
precede the audit commit; retained source findings do not substitute for the
eventual whole-group normal, failure, nesting and terminal acceptance.

### Admitted original callback-abort continuation group

Successor to the three finite design obligations above, not a claim that their
implementation has passed.348/099 is admitted in Current and the working plan;
the P20 release and immutable guest remain the acceptance baseline.

The continuation is the known original API frame, not a machine snapshot.
CallBack16 retains call-local TD stack/callback cursor/INIT progress and a
native20-byte CS/IP/PE/VM/CPL record. Capture precedes its CPU stack selection.
On abnormal unwind, restore these control fields and caller SS:SP using the
current original CCPU selector loaders. Current descriptor tables, CR3,
device/video/A20/NPX state, guest writes and unrelated TD flags survive. In
particular do not copy callerFrame back over WOW16Done's saved ES mutation.
An original SignalProc IGNOREINPUT/INIT teardown transition is not rolled back.
Before inspecting the control record or TD, existing wow_user_worker_active
rejects terminal resumption. A rejected selector reload uses the already
established no-resume adaptation-failure boundary, not a successful callback.

The selected original c_reg.c exposes the required operations. The legacy
softpc.h setCS/setSS declarations discard their status, whereas the actual
c_setCS/c_setSS definitions return the original call_cpu_function status.
The worker's private import library therefore exposes those two unchanged
bodies plus six unchanged PE/VM/CPL accessors. No new CPU algorithm, published
WOW export, guest/wire record, snapshot manager or private-layout cast is added.
The earlier remote-continuation source is precedent for bounded selectors,
not another state owner. Full c_getCpuState restoration remains rejected for
the concrete shared-state rollback reasons above.

Windows selects the filter before unwind. A private adapter temporarily binds
the already known W32Dispatch entry to TD.vpStack only while the unchanged
W32Exception evaluates its disposition; finally it restores the pending inner
identity. CLOSE consequently writes RET_FORCETASKEXIT on the actual resuming
API frame. Continue-execution/search do not run handler cleanup. After a
handled disposition, W32Dispatch resumes that same entry. The filter does not
invent a reply, translate an exception into success or replace original policy.

CCPU owns its own exact simulation jmp_buf identity. Its original TLS owner
removes an abandoned level only if that exact native frame is still the top;
ordinary FE has already popped it and must not pop the parent again. The
actual c_cpu_simulate setjmp stays in its live native frame. The same-level
exception restart retains the original excepts[level-1] owner. An additional
reached closure, c_reg selector loaders -> c_xtrn call_cpu_function, owns an
interface_active flag and interface_abort jmp_buf. Its existing flag clear
must also execute on native SEH, not just the ordinary/setjmp-return path.
That finally is included in348's same six-file180-line budget; the original
owner must clear its private state rather than exposing its layout to an
adapter. These are registered original-owner correction seams under the
standing source-proven host-correction authority, not newly imported files.

Recovery ladder: compose the actual existing original CallBack16, W32Dispatch,
CCPU simulation and selector/interface bodies; use the existing control and
thunk adapters only for bounded host continuation/filter binding. An outer
facade alone cannot repair the original private jump stack or original handler
target. A registered owner-local correction is required at those exact state
owners. No independently authored execution engine or global restoration
registry is selected. No native/guest exception policy concession is added.

Whole-group proof sources are prepared before the affected build:

- verify-wow-original-callback-frame retains the prior four assertions and
  adds nine preparation/reserved-stack/returned/live/teardown/terminal/selector
  cases through the whole original body and actual control adapter. Service
  doubles make these supplementary evidence, not actual CPU or guest proof.
  Terminal PAGE_NOACCESS proves that the control record is not read; it does
  not independently prove the lifetime of every possible TD allocation.
- verify-wow-dispatch-exception keeps all prior dispositions/debugger controls,
  adding nested logical-frame Ignore/Close with actual original W32Exception.
  Immutable P20 full-TU controls must fail the new causal assertions.
- verify-wow-callback-abort-ccpu executes actual BOP/FE and native escape,
  nested same-level continuation, normal reuse and serial native-thread TLS
  independence. Tests inspect the original owner by including its complete
  source only in their TU, not by adding a production getter or copied layout.
  Actual selector reload uses changed current GDT backing and preserves CR3;
  this does not claim a test of every device or NPX mutation.
- The diagnostic-only callback provider includes the whole selected wcall16
  TU and injects at its Preparation or Returned resolver edge. The existing
  independent SHLPROBE/observer tests original Ignore continuation/next API,
  Close/no continuation, exact-once retirement and surviving WINMINE under
  all three styles. Its images are isolated, hash-bound and never publishable.
  Normal generic recursion and production guest floors remain mandatory.

No build/runtime result is asserted by this source-design entry. Complete
source freezes, old/current controls, actual CPU/guest failures and all retained
floors must be recorded before publication. Arbitrary native catch-and-resume
inside an outer provider remains the previously excluded boundary; this group
does not silently broaden it. M1--M6/output/protocol/S5/S6 stay open.

### Callback-abort build and boundary evidence

All times UTC2026-10-19; source baseline remains92a60fe38 and runtime P20.
Owned supervisor39712 starts18:34:36.306 with a3600s deadline. Its configure
43300 starts18:34:38.111, and runtime40420 starts18:35:23.892 with a2400s
deadline: stable actual CMake/Ninja/ccache x86 Release worker/WOW32 builds
exit0. No task-specific Ninja tree, cache clear, guest mutation or publication.
Candidate NTVDM5E47C078DC1456D71168CBD73A2866AC0ADBD71A97E366B6602006560104F374;
WOW32 6639B7BEA89371F53F15CAF7A20751E94F2C21F351B371E5C27E39AAB80672F5;
private import library64F399A6EDF054846817FEB241A8069BD8A992674D6D562707CE0CAD41D6ADF8;
provider definition53B02FC055160E19492E23B59E357D17855677F7D10D3E8954B7B176594A233E.
P20's13 and canonical ZIP remain the installed/release baseline, not this
unaccepted candidate.11 image inputs are retained from verified P20 copies.

Whole-group scripts retain r1--r8 failures/successes separately; no failed
aggregate is renamed a pass. Runtime sources never change after this build.
Initial frame link fails on three unrelated resource-copy dependencies of the
complete control TU; its original CallBack16 path uses the protected resolver.
The test supplies only exact fail-closed acquire/release/address edges, never
fake success. R2 actual whole-body current retains the original four phases
and passes nine abort scenarios; abort17500 starts18:37:35.591, exit0. All
seven immutable P20 preparation/reserved/returned/live/teardown counterexamples
fail their causal assertions (exit1), while terminal bypass remains unchanged.

R2 dispatch link additionally requires the complete adapter's unrelated thunk
borrow closure and two TLS flags. R3 supplies fail-closed test edges; its map
gate requires them and the unused thunk facade to disappear. Actual original
W32Dispatch/W32Exception plus the production filter helper12572 starts
18:39:51.918, exit0: six phase assertions and the original debugger child pass.
P20 Ignore leaves the inner logical/CPU stack; P20 Close writes its kill signal
to that inner frame. Both new old-body assertions exit1; current targets the
actual API entry. These are controlled-service original-body proofs, not guest
acceptance or a substitute for the debugger/continue-execution cases.

R3 actual-CPU setup fails before compilation because its fixture seam object
was not a product-link prerequisite. The producer now builds its exact existing
Ninja dependencies, not a new graph. R4 then correctly rejects repeated inclusion
of original host_def.h's unguarded anonymous extern type. Keep ntthread and
c_xtrn in separate test TUs, each including its complete original owner once;
do not modify the original header or copy ThreadSimBuf into production. R5's
CPU executable exits0 but required output is absent: the retained full host
closure resolves printf/puts to original fprt.c's debugger output. This is a
failed evidence gate, not a runtime pass. R6 writes test markers through its
owned stdout HANDLE instead; no product output provider is replaced.

R6 actual CCPU12212 starts18:45:20.040, exit0. It proves native escape level
cleanup, nested/same-level CPU continuation, normal FE reuse, main plus two
serial native threads, interface guard/architectural error13/normal reuse,
current relocated GDT selectors, preserved CR3 and LastError. The test's TLS
observation itself preserves LastError. Old full-owner controls8788 and37100
start18:46:02.557/.712 and exit1 with escaped-level/stale-interface markers;
they refuse to longjmp to the known dead frame. Actual symbol/MAP checks bind
c_cpu_simulate to cpu.obj, ccpu386SimulatePtr to tls.obj and call_cpu_function
to the inspected original fixture TU. These are real CPU/control observations,
not claims about every device, paging mutation or immutable WOW16 trampoline.

R6 extension/MM/rejection and repeated original-frame groups pass. Its
diagnostic DLL5ABD9906EC207A894C4B55D02EE013CE188822C8A29A6A4E886DD23F64604B5C
retains the complete selected wcall16 body and actual production cleanup; only
resolver Preparation/Returned injection and copied-scalar logging differ. It
is explicitly diagnostic-only and never publishable. R6's final source gate
fails only because its sandbox cannot open the external pinned original Git
root. Read-only reconciliation15556 and source gate7548 (18:48:51.386) run
with that access and pass:278 selected/177 exact/101 registered/issues0.
Reconciliation rechecks frozen current inputs, old counterexamples and actual
compiled images; it does not rerun or promote the failed aggregates.

The separately owned six-file paired source audit35672 passes: original
provenance hashes match, no format-only difference,348 delta64 added/6 removed
raw lines (70 total, including comments) within the180 executable-line budget.
This adds no original file, guest diff or instruction/decoder algorithm. The
278 provider-selection inventory does not pretend to include CCPU; this
additional six-owner comparison covers that explicit boundary.

Guest supervisor30060 starts18:49:09.906, real2700s deadline. It stages only
canonical assets/winnt.zip and exact coherent13, then the diagnostic provider
on a separately marked copy; installed files are untouched. Its first case
fails before guest launch: the independent CALLBACK_ABORT probe omitted three
call sites but retained their NE relocations. The corrected diagnostic-only
count/table is recorded by r8 as the sole changed test input; all product and
prior boundary inputs remain identical. No original guest is assembled.

The next actual Preparation/Close/Off case is a real non-pass: WINMINE reaches
its board, SHLPROBE enters actual RET_WINEXEC, and worker9056/thread26012/task5943
logs entry170F53BC, CPU170F53BC, CS021F/IPAEB7. The original exception dialog
appears and observer posts its offered Close response, but no unwind record
arrives; the sibling receipt remains live at the ten-second deadline. Probe
exit17 and aggregate exit1 are preserved. Cleanup is not normal retirement.
No Ignore/Returned/other-style success or full retained floor is inferred.
Source tracing now follows original WOWSysErrorBox -> NT4-shaped NtRaiseHardError
-> the existing in-process native hard-error dialog, and its execution/wait
ownership during first-pass filter evaluation. A bounded wait/IP observation
using the existing worker_thread_snapshot is admitted before selecting a fix;
an execution-ownership deadlock is a hypothesis until that observation/source
chain proves it. Do not patch the dialog, weaken the timeout, replace original
exception policy or claim the frame cohort delivered from unit/CPU results.
All full S4 obligations and the retained0102/WRITE limitations remain open.

### Native error initialization dependency within the frame cohort

One bounded instrumented repeat, owned21816 from18:54:52.185, uses the
existing worker_thread_snapshot, not a new window/control facility. It
validates fresh worker41676's exact O:/tmp/n51f/system32/ntvdm.exe identity,
creation18:55:06.757 and stage source before two short suspend/read/resume
IP/stack/public-WCT observations. The case again fails exit1; capture success
is not acceptance. WCT returns single-thread blocked nodes, no cycle: it does
not itself prove a lock graph.

Actual stacks and selected MAPs identify the missing boundary. SHLPROBE
thread39256 is identically blocked at win32u+165Ch; its return010919B4 rebases
with NTVDM00F20000 to preferred005719B4. Owned dumpbin7256 proves that address
follows the SetFocus import005719AE, in the WM_INITDIALOG default-button branch
of mvdm_wow_hard_error_dialog_proc. It is not EndDialog or an unrecognized
Close ID: the queued Close has not been processed. WINMINE35596 waits at
original xxxSleepTask/xxxDirectedYield from callback_enter, with captured
6A1DF99F/6A1D2748/6A1D24C6/6A1DFCC3/6A1E478B, diagnostic WOW32 base6A180000
and its exact MAP. Its trace ends on WM_NCACTIVATE for owned window0A62084C.
The offered41001 Close is posted while initialization has not returned to its
message pump. Both snapshots and unchanged images stay sealed.

Source completes the explanation: callback_enter pushes ready execution and
obtains original CSOwningThread; first-pass filters precede its finally/pop.
WOWSysErrorBox -> WOWpSysErrorBox -> NtRaiseHardError reaches the existing
in-process dialog without a blocking WOW native-call scope. The other task's
park_native_owner cannot park a still-ready owner; xxxDirectedYield waits.
The synchronous native focus operation waits for that callback. NT4's external
hard-error server is a different resource boundary; the rehosted UI lacks its
execution handoff. Earlier after-callback exception tests do not exercise this
still-active ready scope.

100 reuses blocking wow_user_native_call_begin/end around WOWSysErrorBox in
the existing thunk-scope adapter, not around all W32Exception, which must read
guest state while owned. Source-owner generator binding selects exactly three
WOW consumers; original nt_error.c, worker UI, DOS and guest stay unchanged.
Original offered responses/parameters/errors, selection and nTaskLock/native
policy remain authoritative. No new task-lock or exception-policy waiver.
Return reacquires ownership before W32Exception uses its disposition/kill
frame. Move348 capture to whole-call entry before its first resolver: native
error UI can dispatch another task even on preparation failure. Uncaptured
preparation no longer establishes unchanged shared CPU state.

Recovery ladder keeps original exception/frame/CCPU algorithms and uses an
existing bounded native-service adapter, so no new engine or original UI
import is justified. Rejected scope entry uses the established no-resume
invariant boundary, not fake success. Whole service/LastError/throw/rejected
entry, first/later resolver/cursor and real guest Close/Ignore/style/survivor
proofs precede the next composed build. This design is admitted in the same
finite cohort; no repaired runtime/guest pass is claimed by this entry.

### Composed callback and native-error acceptance

Fresh r9 supervisor47140 completes exit0 after the full100/348 change,
including actual configure/affected build, original callback four plus11
abort phases, original dispatcher six plus debugger and five native-error
scope outcomes, real CCPU simulate/interface/current-descriptor tests,
old-body counterexamples, extension boundaries and source registers. Native
scope service doubles prove pairing/LastError/rejection, not a real scheduler
handoff; actual guest evidence below supplies that separate boundary.

Normal NTVDM SHA256 AB9D1B8ABC54C47FE6AB16DA2912088ED8AEF28BEBB6CBBC2FAA1AE013F42DBF;
normal WOW32 SHA256 59E008EC1F09A2228FCF87FDB346F205473EB7B6DEE55ED39983C866A96C263C.
The unchanged guest ZIP remains7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7.
The coherent normal13 uses precisely these two and the sealed P20 other11;
the independently marked resolver-injection DLL is never publishable.

Guest supervisor46476 starts19:10:43.662UTC with2700s deadline and completes
exit0: Preparation/Returned x Close/Ignore x Off/Win16/All, all12. Every case
checks actual original CallBack16 entry/unwind TD and CPU return identity,
exact-once task retirement, no post-retirement callback, and surviving
WINMINE flag/reset/open/normal close. For the previously failing
Preparation/Close/Off, worker17532/thread31376/task5943 logs matching
entry/logical/CPU170F53BC and CS021F/IPAEB7 before and after resolver2's
exception. The original Close path returns from the dialog, unwinds, and
retires the sibling. This is the causal runtime counterexample's repair,
not merely an observer cleanup or a native mocked pass. Returned uses
resolver3; the preparation-only missing optional before-trace is not
misclassified as a missing actual entry/unwind.

Review confirms all three original WOWSysErrorBox consumers (wow32.c,
wuser.c,wudlg.c) receive the same compile-time service binding; raw worker
presentation and DOS calls remain unchanged. Only the native modal service
releases/reacquires existing execution ownership. Original filter policy,
KillTask re-resolution and original nTaskLock remain. Whole-filter/native
UI replacement, rollback of shared CPU state, guest changes and performance
changes are expressly absent. S4's full retained regression/publication and
remaining M1--M6 obligations are not discharged by these12 outcomes.

Supplementary closure review finds the existing pure thunk-trace test links
the complete modified thunk-scope TU. Owned38476 exits1 at link1120: four
new filter/modal dependencies and two earlier TLS dependencies are unresolved;
no test executable ran. Refresh only its fixture with exact fail-closed service
signatures and original TLS declarations. Explicit function packaging,
OPT:REF and a map gate must discard all unrelated invocation/filter/modal/
borrow/failure owners. Do not derive aliases from unresolved symbols or link
a success-only execution substitute. Current supplementary25796 exits0:
WOW_THUNK_TRACE errors0, disabled1/enabled2, failed output, exact text,
LastError and no-scope all pass; the public ABI compile and discard gate pass.
The first supplementary supervisor44600 failed to parse a scratch foreach
pipeline before any test; it is retained, not a runtime failure or a pass.
Documentation passes. Neither diagnostic test repair changes a product input.

The final six-owner pairing48368 exits0: original provenance unchanged,
65 added/6 removed raw lines,71 total within180, no format-only changes,
zero guest changes. Source-register r9 retains278 selected/177 byte-exact/
101 registered differences/issues0. Native21 and original30 floors complete
exit0; S3's twelve control/dialog/common-dialog/hook groups complete exit0.
The later trace-fixture-only source edit is not a compiled input to these
floor rows; their production/provider and selected test owners are unchanged.
Full actual message/external/application/DOS and publication gates remain
pending at this evidence point. Do not promote the candidate from these
partial retained-floor results or erase the old unlocated0102/WRITE risks.

### P21 bounded callback-abort delivery

Full floor supervisor30916 starts19:15:52UTC and completes exit0. Actual
recorded child entrypoints, frozen r9 inputs and results cover native21,
original control30, S3 twelve, six message modes x three policies (normal,
native extension, construction, reply lifetime, timeout lifetime and Proc32),
external9, cross-worker6, pending-send6, WINMINE3/SOL3 gameplay/normal exit,
native modal reference plus guest3, both-width NTW16S boundaries, and complete
Userenv current-token/non-inherited logon3. No test substitutes another thread
for the external-process rows. Input pacing, assertions and deadlines remain.

Supplementary frontier33492 passes the old after-Shell-callback Close/Ignore
x three policies (6), and WRITE's four read-only startup observations in each
policy. Its aggregate exits1 at DOS preflight: the orchestration omitted
GuestFixturePath, so no DOS case launched. Preserve that failure. It is not
evidence of product failure or permission to relax a gate. DOS supervisor39328
then supplies the existing verifier's independent G7 COM input, ordinary
frontend and canonical observed-input mode. Console17 and Window17 both pass,
all actual exit codes equal their expected values. This uses only a28-byte
authored artifact in the expressly owned legacy-compatible disposable path,
not another compiler/cache tree. No verifier/product change was needed.
Retain the individually successful Shell/WRITE rows, not a promoted failed
frontier aggregate. The first continuation observer also rejected an incorrectly
coerced UTC process timestamp before starting anything; reading the raw JSON
stamp restores exact identity checking, not a PID-check waiver.

Publication40296 verifies every prerequisite separately, preserves coherent
P20 backups for both destinations, then copies the exact normal13 to
assets/release and O:/winnt/system32. Installed both-width style and complete
logon WINMINE3 pass; final13 hashes match the frozen candidate. No existing
project owner had to be closed for this publication. Only NTVDM/WOW32 and
release-manifest bytes change; other11 retain P20 identity. No diagnostic DLL
is published. Manifest SHA256
4EBD672E30F1606F3F02639AB6671E95DC5051BB949130B5C8628CED86FCE330.
Normal NTVDM/WOW32 hashes are the r9 identities above. Canonical guest ZIP
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7 and
installed SYSTEM.INI remain unchanged; absent NTVDM.REG paths are recorded,
not created or replaced. Source registers retain278/177/101/issues0;
mirror348's six-file65+/6- delta and099/100 are registered before delivery.

Second-role review keeps original callback/exception/taskman/CCPU algorithms
and policy, host USER resource/delivery authority and the existing adapter
execution/data scopes. The new native UI edge pairs the established blocking
service contract; no helper window, parallel scheduler, full CPU rollback,
private modern-USER overlay, new public/wire ABI or guest change is introduced.
Original normal returns, FE and same-level CPU continuation remain covered.
Mocked failure endpoints, terminal guard-page evidence and real CPU/guest
tests retain their distinct limits; no claim covers every device/NPX mutation,
every task-lock/native-error combination or every possible terminal allocation.
WRITE editing/save/normal exit and old unlocated0102/common-dialog risks remain
unproved. This P closes the finite callback-abort cohort, not all S4: output
conversion/protocol-owner obligations and exact M1--M6 reconciliation remain.
S5 single-task termination/NT Monitor and S6 final-T audit are not admitted.

Final quality38400 exits0: frozen runtime/test inputs, both installed/released
13, unchanged canonical ZIP, PowerShell parsers, documentation, relative links
and whitespace all pass. Six-source before/after normalized content comparison
confirms four were already differing; c_xtrn.c and ntthread.h are two newly
content-different original host files, not new imports. The additional native
adapter/test changes do not alter a public/wire or guest ABI.

Before disposal, retain SHA256 receipts: r9 source freeze
ABE084E3C6AF3A5BA60F863FC57723618BD0C19C789613283721E76D0932FABD;
full-floor PID/arguments/results
4E0558F6A153D11D96F2AC54246043B7CD672FF53FBE3C022AB77A9DF3C99FF6;
repaired Preparation/Close entry/unwind
5DA87E79794C90776911689931970A78B05F7E8DE6F9B05A5E40FC43CA639A8E;
six-owner before/after
90723F299AA3D4BB956C1608770A74DA1E66B20BC1D74A60604D7B7B6016E420;
pre-disposal final input inventory
8AE50E798C9B7905C35B5C6850A0EBF05D6A40121255B506BF6D2D5C33D826F9.
The corresponding assertions, commands/entrypoints, exact artifact identities,
failed outcomes and limits remain above and in committed reproducible tests.
This is a concise evidence retention policy, not a claim deleted raw logs
remain available. P20's coherent recovery images remain recoverable from
committed1122ab55d55b4d15699715ce1416b092b5887772 assets.

Disposal20732 starts19:49:47.791UTC, exit0 after actual owner quiescence,
read-only inventory and exact-source review. The unique Clear-BuildOutput
preview and explicit Apply agree: callback-abort6100 files/336774197 bytes,
and the G7 parent1 file/28 bytes, total6101 files/336774225 bytes (321.17MiB).
Inventory digests respectively
EAD8D7094A732685E1B3924DBE56B1CF5D6F3FEC35440915C081F95BCAD05C74 and
DF9779C4E349214CA5BEB7A7F1107052F9423178E37C02C8C92978D62B6EE1FE.
All866 sensitive entries have explicit hash/provenance review:245 canonical
ZIP byte matches,26 verified P20 backup images,173 independent binary probe
reports,421 owned receipts/traces/assembly listings, and the authored28-byte
G7 witness86EBA64D5173C29B4749FE0F2472FC88AEF5CBF3C62FF9067DFC7CEF8EDD56C0.
No unknown/user/Git data or link is discarded. No process is killed for
cleanup. Final CheckResidual returns only build/.ccache and build/.ninja;
neither cache, installed package, canonical media nor user configuration is
removed. Disposable evidence is deleted, not recoverable raw evidence; source
tests, the retained conclusions/hashes and committed runtime baselines remain.

### Post-P21 complete output and call-owned transaction source review

Baseline is clean4b8a7c8ae6eaaca3350d640650dd9d57201fd581, with P21's exact
installed13 and immutable guest ZIP. The preceding goal turn is verified
progress, not a wait: the callback/CCPU/error-UI cohort was implemented,
accepted, published and pushed. This continuation is source-first; no product
test is run to discover the next defect and no new production code is selected
merely because the old broad floors pass.

Read actual wmdisp32/wmdisp32.h/wowhooks/walloc16/wcall16/wmsg16, the selected
aw32Msg table, current message-lifetime/native-extension adapters, and the
immutable wow16cal GlobalUnlockFree body. Source establishes these finite
coupled obligations:

| Original call-owned group | Concrete retained state and failure boundary |
| --- | --- |
| CREATE/MDICREATE | Class/title keys are published after copying; a throwing putstr16 can escape before the key is retained. Error labels cover ordinary NULL failures, not every escaped partial construction. Nested MDI needs ownership transfer and original title/class/record release, not another allocator. |
| GETTEXT/ASKCBFORMATNAME/GETDLGCODE/DROPOBJECT | The returned malloc16 token is call-owned; normal unthunk frees it, but a constructor/copyback/callback escape or constructed request without a callback skips that release. GETTEXT's DS-based stack route must not be mistaken for heap. |
| SIZECLIPBOARD/PAINTCLIPBOARD and WINHELP | Temporary original global handles/pointers are call-owned, unlike clipboard data or DDE transfers. Capture completed acquisition before copy/lock can fail; preserve normal lock/unlock and WinHelp foreground behavior. Abort cleanup must not rerun foreground activation. |
| MINMAXINFO/DRAWITEM/MEASUREITEM/DELETEITEM/COMPAREITEM/NCCALCSIZE/WINDOWPOS | stackfree16 is deliberately empty; stackalloc16 changes TD.vpCBStack and INIT before CallBack16. P21 captures at callback entry, after converter allocations, so it cannot by itself roll back a constructor that never reaches that entry. Normal conversions, sign extension, padding and partial output writes remain original. |
| Actual direct caller closure | W32Win16WndProcEx already has the selected finally; ThunkCbtHook's independent WM32Create record is not zero-initialized or paired by it. Nested WM32Create constructs another local MDI record. Both direct/nested call paths must be included, not only the table-dispatched path. |
| DDE Advise/Execute and related packet owners | DDEFindPair16/DDEAddhandle transfers ownership into the original protocol list. A universal call-finally free is wrong; retain this separately enumerated protocol-owner obligation inside S4. |

The guest GlobalUnlockFree entry resolves GlobalHandle from the selector,
unlocks, and frees only at zero lock count. Thus an aborted global conversion
cannot add an unconditional extra GlobalLock and expect one UnlockFree to
release a still-locked allocation. This original kernel source is read-only
behavior evidence, never rebuilt or patched. Original free16's pool versus
global fallback and completed acquisition boundary stay authoritative.

The smallest viable repair direction is call-local progress in the existing
message record, immediate capture of original release keys, consume-before-
release through original free16/global operations, and a separate failed-call
callback-stack rollback. Do not add a per-task resource registry, replay
normal output unthunkers on abort, roll back already-written native outputs,
or copy a full CPU state. Pure input, output, temporary global and transferred
protocol objects remain distinct. The original BlockWOWIdle owner is
softpc.new/host/src/nt_eoi.c, not a native UI no-op: its bit affects interrupt
wakeup. Do not insert a leaf idle toggle into one converter while ignoring
the enclosing native-call pair; existing M3 reconciliation retains that owner.

An additional prerequisite is now source-proven, not a guessed buffer bug:
the full forward table has native output stores outside P17's three output
controls. MEASUREITEM writes its native structure; NCCALCSIZE writes a RECT
or three RECTs plus pointed WINDOWPOS; ASKCBFORMATNAME writes a string;
MDIGETACTIVE writes native BOOL; NEXTMENU writes menu/window results. The
last two have no malloc16 site and would be missed by an allocation-only
inventory. The reverse original unthunker consumes the corresponding data.
Do not infer their actual modern USER capture lifetime from GETTEXT alone.

Extend only the existing native capture control and its opt-in ten-case
matrix in both widths, preserving the old four-case default. Keep the same
early reply, sender-page NOACCESS, explicit acknowledgement and full-return
fence; observe the nested lppos identity independently. Attempt writes only
to separately writable receiver storage. A noncaptured alias is an observed
boundary requiring design, not a passing capture or a reason to dereference
the protected sender. No product rewrite, new semantic waiver or runtime
publication follows merely from this design-prerequisite observation.

### Complete native output prerequisite and finite transaction boundary

This is a test/source-only S4 checkpoint after P21, not a runtime P or S4
closure. No production source, mirror, public/wire ABI, release binary,
guest ZIP or configuration changes. The native observation supplies a missing
input to the whole conversion design, not a source of trace-derived patches.

Reproducer: tests/observation/verify-wow-message-capture.ps1 -EvidenceRoot
<fresh build child>, launched hidden with recorded PID/arguments and a3000s
outer budget. It builds the existing wow_capture_native_control target in
each stable MSVC Release tree after explicit VsDevCmd -arch=x86/x64, proves
actual /O2, /MT or -MT, /W4 /WX native-only commands and PE machine14c/8664,
then runs MSGCAPTURE with no arguments and --extended. Each build has1200s,
each native observation180s; all complete normally. No product provider,
guest, style DLL or external-source executable is linked into this probe.

The initial r1 exits1 before compilation: the new verifier recognized /MT
but not CMake's equivalent -MT spelling. This is a harness failure, not a
product or USER failure. Correcting only that check gives r2's complete
passing group. After adding retained host and early-copy identities, frozen
r3 repeats the same full group successfully. Both r3 incremental builds say
no work to do; r2 compiled the changed probe in both stable trees. Do not
count r1 as a passing test or treat the repeated build as a cold build.

| Case per width | Native output contract observed after early reply |
| --- | --- |
| GETTEXT, GETMINMAXINFO, WINDOWPOSCHANGING | Old strict positives retained: distinct capture, actual output write after sender-page protection, normal full-return fence. |
| COPYDATA | Old input-only header and nested-byte captures retained; no output write or output-copy claim. |
| MEASUREITEM | Distinct captured MEASUREITEMSTRUCT; receiver writes itemWidth after sender protects its page. |
| NCCALCSIZE, wParam0 | Distinct RECT capture; receiver writes left after protection. |
| NCCALCSIZE, wParam1 | Distinct outer parameters and distinct nested lppos; receiver writes both after protection. Neither pointer aliases the corresponding sender storage. |
| ASKCBFORMATNAME | Distinct writable string capture. |
| MDIGETACTIVE | Distinct writable native BOOL, despite no original forward allocation site. |
| NEXTMENU | Distinct writable MDINEXTMENU result fields, despite no original forward allocation site. |

r3: build x86 PID22016 starts20:08:50.185Z,6.560s; x86 default26276,
0.140s and extended49436,0.139s; build x64 PID38508 starts20:08:57.256Z,
5.659s; x64 default43136,0.121s and extended4852,0.135s. All six exit0.
Default8 plus extended20 gives28 protocol observations,24 actual outer
output writes and two nested WINDOWPOS writes; all four executable runs
report errors0. The extended set has18 outputs and two input-only COPYDATA
cases, not20 output buffers. Each sender waits for ReplyMessage result6,
protects its allocated page NOACCESS and proves ReadProcessMemory rejection,
acknowledges, then fences full receiver return with a second synchronous
message/result. The receiver queries protection before dereferencing and
never writes a protected alias. A fault remains diagnostic failure.

All24 output observations report output_at_reply0: the original before-reply
mutation has not been copied to the sender at early return. This is evidence
for the already approved modern USER delivery/reply boundary, not a restored
NT4 early-copy/ordering guarantee. It does not justify delaying reply, adding
a buffer engine, writing directly to sender memory, host-version gating or
another semantic concession. Noncaptured aliases would remain explicitly
unproved results, not silently safe or product passes.

Recorded OS10.0.26300.0; native and WOW USER32 report10.0.26100.8117.
Native USER32 SHA256 F285937030BCB325CAD39EE5BE4818F6666241229ADEADEA4FF1AE469DFD7EF3;
WOW USER32 56F3BC77020E21EBEEF825965929DC0D0BD87F9CAFBB08DBCD477043E4342D2E.
Current host/build only: other OS versions, cross-process capture, actual
Win16 converters, ANSI/Unicode edge cases and abnormal guest return are not
proved by these native same-process/cross-thread observations.

Frozen C source CB537DDDA30EAF77815CB6F79146DA4F04509080D1A2963CD419A70581ECF429;
producer D0CAAE0877FD0ACAF097B6E7EBABCA1CA36F8E96B92D89D6F2F6B7E8AD550DD8.
Native x86 image0F95A0F355E893A1DA55347498B03E5C2488F2F7D07DF79812B8828DFC4C277C;
x64 image711AEE7C56F49344CB96387CD442628F8D9F2DA2F1DA48F35601A65739ED2892.
r3 result D5F6D21FF0B0A49FCDBC762BBA08B8F13480CB667611D93564390580D4012048;
steps3AA7916049F47F1A28F928202CAB56C7C63E82A0EDEC0E225CB0C7C1A305DF5B;
observations F8E2DDC9D300A61561F12060941C4AA9D644737ADCF42E9189BF4FFCC91C14DD.

The finite production repair ledger remains these four coupled obligations,
with source locations and causes in the preceding table, not an indefinite
"foundation incomplete" category:

1. Completed call-owned heap/global acquisitions must publish their original
   release keys before any putstr16/GETVDMPTR/copy that can escape. Cover
   CREATE, MDICREATE, GETTEXT's heap variant, GETDLGCODE, ASKCBFORMATNAME,
   DROPOBJECT, SIZE/PAINTCLIPBOARD and WINHELP. Existing SETTEXT/CBDIR and
   COPYDATA progress remain composed, not replaced. A failed acquisition
   creates no key; class atoms are not allocations. Original allocators and
   pool/global distinctions remain authoritative. Native GlobalLock success
   must pair its own unlock on copy failure; it is not a guest global lock.
2. Normal releases and abort cleanup consume those same keys exactly once,
   before calling the original release operation. A callback FALSE still
   executes normal original unthunk. Abnormal cleanup performs no output
   copyback, HWND/HCURSOR return conversion or WinHelp foreground activation;
   it does not roll back partial native writes. Preserve the global lock
   count: do not add a fresh guest GlobalLock to an already locked temporary.
3. Pair every actual owner: W32Win16WndProcEx, direct ThunkCbtHook CREATE and
   nested WM32Create/WM32MDICreate construction/transfer. The nested owner
   retains keys until complete transfer; an outer copy failure must release
   the transferred keys too. Zero initialization alone cannot supply this
   transaction, and normal unthunk cannot be replayed as an abort finalizer.
4. Capture the original callback-stack cursor and INIT bit before converter
   stackalloc16, restoring them only after failed-call cleanup while the
   same original task/execution boundary remains live. Cover stack-only
   MINMAXINFO/DRAWITEM/MEASUREITEM/DELETEITEM/COMPAREITEM/NCCALCSIZE/WINDOWPOS,
   DS-based GETTEXT/SETTEXT and CBT stack temporaries. Preserve normal
   stackfree16's deliberate no-op, other TD flags, SignalProc teardown and
   P21's callback/CCPU continuation recovery. Never touch a retired TD or
   enable application reentry after terminal withdrawal.

Original-body acceptance must cover failure before acquisition, after each
published key, nested transfer, missing/rejected callback, callback escape,
normal output conversion failure, repeated cleanup, normal callback FALSE,
global lock counts, DS/heap policy, stack cursor/INIT and terminal admission.
Actual guest tests must exercise normal and escaped callbacks with surviving
tasks and normal retirement; every-P retained floors/publication follow only
after that entire cohort is implemented. These are required future gates,
not passes supplied by this native probe. DDE Advise/Execute list ownership,
other protocol packets and M3 BlockWOWIdle pairing remain separately named
S4 obligations; they are not temporary objects to blanket-free, silently
transferred to another S or closed here. No new exception ID is reserved or
registered by this test/source-only change; any production seam is registered
before implementation with its precise mirror budget.

Second-role review compares the actual five-file diff and the original
callers, not just the native exit codes. The no-argument four-case contract
is retained; opt-in cases use fixed known structures, no automatic payload
inference, protected-alias dereference or product exception swallowing.
The new runner reports observation completion separately from runtime
acceptance and unproved captures. Host capture observations do not discharge
the four transaction obligations above. Added source is independently authored
test code only; mirror diff increase0/decrease0, no new source import or ABI.

Quality PID5548 starts20:10:14.882Z with240s deadline, exits0: documentation
governance and relative links, PowerShell parse, whitespace, frozen native
inputs, unchanged13 release/installed hashes and original guest ZIP all pass.
Its retained stdout SHA256
32C59BFD39560CFCEADE7C05449529430EDE89DE28FDBD00584A480F9B1701F7;
owner/command receipt AE5E7899F39E43F0611D89DF3B08C16FC61164B44200FB7AF5028E21B894098A.
No product rebuild, deployment, process kill, prune or cache clearing occurs.

After actual owner/child start-identity quiescence checks, the sole cleanup
tool's read-only inventory, preview and explicit Apply agree on62 generated
files/51044 bytes (49.85KiB), membership
B3AFD628846680E5E77EB9EBEEE4C2C06DE6BF799D712483FBBB02FC1D551D71.
No media, user configuration, Git or unknown item requires an exception.
The concise results/hashes and reproducible tests above replace disposable
raw receipts; deleted raw output is not recoverable evidence. CheckResidual
returns only build/.ccache and build/.ninja, both preserved. T447 S4 remains
active with the finite transaction cohort next; S5/S6 and Queue unchanged.

### Call-owned conversion transaction349/101 — implementation and acceptance

Baseline860785e88/P21 remains published. Register349/101 before editing;
the implementation retains original converters/allocators and the existing
72-byte message record. Existing scratch captures completed heap/global keys
before copying; normal and abort paths consume the same keys before release.
No output unthunker, result translation or WinHelp foreground operation is
replayed on abort. Clipboard distinguishes its still-locked pointer from an
already-unlocked handle. Native GlobalLock is separately paired with Unlock.
CREATE/MDICREATE retain nested class/title/record transfer, class atoms and
original normal release/result ordering. No second allocator or registry.

The whole caller sweep includes W32Win16WndProcEx, W32DialogFunc,
WCD32CommonDialogProc, CallWndProc/MsgFilter/CBT and stack-only Journal/Mouse
hooks; delegated control converters supply14 more heap owners and common
dialog SHAREVIOLATION supplies one. putmsg16 selects only COMMAND/DDE/
DROPFILES, not these ordinary heap producers; its protocol ownership is not
blanket-freed. Existing COPYDATA and SETTEXT/CBDIR progress remain composed.
Before-conversion native-call-local TD/cursor/INIT capture restores only an
incomplete invocation with matching live task identity, preserving terminal
withdrawal, IGNOREINPUT and the normal stackfree16 no-op. No guest change,
modern private USER structure, idle/scheduler change or public/wire ABI.

Recovery ladder: reuse original algorithms and allocation/release services;
their callers must capture acquisitions because a lower allocator cannot
know nested transfer or whether output conversion completed. The minimal
registered six-original seam records progress and composes owned finalizers;
it does not import another USER engine. Final raw six-file delta is274 lines
(169 added/105 removed), within the registered300 budget. Exact source-register
comparison remains an independent gate, not inferred from that budget.

Source producer verify-wow-message-construction.ps1 -Transaction compares
immutable P21 selected full bodies with current selected bodies, the actual
stackalloc16 and the full owned lifetime adapter. Service doubles explicitly
stop at allocator/map/native-service boundaries. Source-r1 cannot compile
because fixture dependencies omitted an original constant and exposed an
unchanged EM_GETLINE narrowing warning; add the constant and suppress that
warning only around its extracted unchanged body. Source-r2 catches a new
adapter local shadowing a fixture global; rename the local. Source-r3 executes
and fails12 assertions: MSVC AbnormalTermination is true for return/goto out
of a guarded body as well as an exception. Use explicit completion flags,
not that predicate, for normal MDI ownership transfer and failed-call cleanup.
This candidate defect was caught before publication. The original documented
[SEH handler contract](https://learn.microsoft.com/en-us/windows/win32/debug/handler-syntax)
explains why normal returns cannot select abort cleanup.

Source-r4 and final r5 pass163 scenarios/errors0; P21 fails71 assertions
including the six required named negatives. Nineteen heap families cover
normal/OOM/abort/idempotence/copy failures; nested transfer, original CBT,
global lock counts, failed/nested/other-task/retired stack owners also pass.
These are original-body service-double proofs, not all actual guest paths
or proof of partial output writes. The final owned adapter declares exact
WINAPI/WORD HMEM16 original global-service prototypes, matching wcall16.c;
the fixture mocks use that ABI too. This removes the new implicit-prototype
warnings rather than hiding them. Earlier legacy original warnings remain.

Source-r6 adds the missing partial-output distinction: allow the original
GETTEXT copy to write its first byte, then raise, and assert the remaining
output remains unchanged while the captured allocation is released. Current
passes164 scenarios/errors0; immutable P21 fails72 assertions. This proves
release-only cleanup for that partial write through service doubles, not
all possible native output faults. No product input changes after r2 build.

Final retained-r3 supervisor runs the old six-negative construction suite,
parameter mapping and COPYDATA27 after that ABI strengthening; all three
exit0. Its child PIDs49068/15764/592 start21:05:52/21:06:09/21:06:25 UTC.
The earlier parameter-r1 fixture compilation failed on an unreachable return
after ExitProcess; remove only that dead fixture return, retain the failure.
The malformed first foundation supervisor and sandbox-denied original-source
read are orchestration failures before product acceptance, not passing gates.

Release product-r1 and final-r2 affected builds exit0. Final normal WOW32
84345EF27EFE5FC5A337DE218D0B9063877921B8276CB1C7E9B768B08FF3454C;
NTVDM1FC1F21AD1FBDC87151E4F836670A931BBEA1591E75AE4A4C0B7A0CDB5E641F2.
The worker graph regenerated base_vdm_config.c/relinked in r1 and reports no
work in r2; this is not a claim of unchanged worker bytes. Both remain only
in the stable cache/canonical build-owned stage, not assets or installed.

The reproducible diagnostic producer replaces only the complete wmdisp32 TU
in a separately linked diagnostic DLL, never normal objects or exports. It
retains the selected full body, original allocator and actual CallBack16.
Seal P21 control before rebuilding the normal closure: diagnostic SHA256
3D78513851DE5F80D5594B1FC798B2CC5D02ECE5A831C31EA8C488C61FF3BE74;
candidate73996E2886EA850BCE7762526A9E938D8A64CF619F01FECD565E83302DCEDC25.
Both manifests explicitly forbid publication. The native peer uses public
CallWindowProc on its actual owning task, exact thread/HWND arm identity and
a test-owned SEH catch; the diagnostic only reads original vahdr allocation
bits. It does not test original hard-error UI policy or inject guest changes.

Off-policy actual BeforeCallback and AfterCallback controls both complete
the whole ordinary two-task construction/message group and normal retirement,
while retaining the allocated original heap bit1. The candidate's corresponding
two cases complete with heap bit0 and matching pre/post TD cursor/INIT.
Candidate identities: worker25412/thread30448 and46516/thread4876, task17BF;
original slot14, key17E70E00. Before-callback sandbox attempt cannot create
O:/tmp/nc8b and never starts product acceptance; the authorized physical-stage
run supplies the actual control, not a promoted retry of a runtime failure.

The first styled fault case fails on both P21 and candidate with launcher
1067 and no diagnostic transaction record. This is an unresolved test/entry
boundary, not a repaired-style pass or evidence that349 introduced it.
Retain the failed rows. Normal styled construction and source tracing must
separate product behavior from the diagnostic subclass entry before any
runtime publication. Whole every-P floors, coherent13 publication and final
source review remain pending; S4/M1--M6, DDE protocols and M3 BlockWOWIdle
remain open. No unchanged guest or installed baseline is replaced here.

An attempted public GetClassLongPtr entry is a failed probe, not a fix:
OpenNT-shaped creation installs the actual guest entry per HWND while native
class registration retains wow_window_class_creation_proc, a no-WOW creation
placeholder. The altered probe finishes normally but cannot inject and fails
its guest assertion33164. Revert that test-only attempt. Retain only the
actual GetWindowLongPtr/CallWindowProc Off-policy positive/negative results.
The normal non-diagnostic candidate passes the entire Win16-styled construction
group; this separates ordinary styled behavior from the fault harness but
does not explain or pass the styled injection failure. No style product code
or boundary is changed to accommodate this diagnostic.

Register audit r3 passes278 selected/177 byte-exact/101 registered differences,
issues0. All six originals were already content-different; this cohort adds
no newly differing mirror file. Source ladder and runtime semantics remain
separate obligations. S3's twelve actual controls/dialog/common-dialog/hook
cases and normal message three-policy groups pass in the continuing normal
candidate floor run. Its other rows are pending, not presumed passes.

Native foundation r2 executes21 cases,20 pass and standard-class exitsC0000005.
Do not promote that aggregate. The unchanged sealed failing executable under
the existing wow_exception_observer reproduces an execute-at-NULL fault:
fixture base10000, _WOWCleanup+238 returns from context->destroy_class at
runtime42F58, enclosing verify_original_cleanup+411 at26C61. Source/disassembly
agree: that test hardcodes unrelated hInstance1 while a private BUTTON's
native image10000 has HIWORD(hModule)1. Original cleanup correctly selects
module destruction, but the selector-only fixture left that callback NULL.
This is a demonstrated fixture identity collision, not a guessed349 defect.
The two earlier sandboxed debugger attempts exit2 before a private desktop
can start and cannot diagnose the original failure; only the authorized
desktop run supplies the above exception/addresses. Debugger exit0 alone
never means the fixture passed.

Fix only wow_standard_class_fixture.c: choose a WORD absent from both actual
class lists and fail closed/report an unexpected destruction branch. Start
with the observed occupied value, so conflict avoidance is exercised without
relying on random ASLR. Final rebuilt standard-class passes all unchanged
private/public restoration assertions; a separate debugger run exits0 with
image7B0000/excluded124 and original cleanup restored9/errors0. It changes no
product, imported source, ABI or cleanup policy. The original failure remains
retained. Original30 also passes in foundation-finish.

Dependency-selected reuse is explicit: native21-r2's frozen source inventory
differs only at that one standard fixture; all20 successful rows' actual
compiler command receipts exclude its source, macro and object. All frozen
provider/worker/style hashes still match. native-floor-reuse verifies these
facts and the final affected row's exit0. These21 satisfied contracts are
20 retained successes plus the separately repaired case, not a rewritten
21/21 result for the failed aggregate. Future whole-group runs use the fixed
fixture. No unrelated case or failing assertion is deleted for throughput.

Worker PE comparison confirms the relink changes exactly four bytes: file
offsets296/297 in the COFF timestamp and1668108/1668109 in the IMAGE_DEBUG_DIRECTORY
timestamp (directory begins1668104). All other bytes, including executable
code, data, resources and relocations, match P21. Export time isFFFFFFFF;
an earlier diagnostic's wrong hypothesis that the second timestamp was the
export directory fails its masked-hash assertion and remains a failure. The
complete byte-diff and actual debug-directory mapping, not that hypothesis,
establish timestamp-only relink. No artifact is patched or reused as though
its full SHA256 were unchanged.

Final eight-caller source review catches two normal bypasses in the new guard:
EM_SCROLLCARET returns after a native-only EditWndProc notification, and the
common-dialog private ChooseFont message intentionally returns FALSE without
calling a guest hook. Neither is a failed invocation. Mark each completed
before its original return, preserving any preceding/nested native call's
normal stack effect. Do not change the original result, routing or algorithm.
All other guarded early returns are actual admission/construction failures;
normal callback FALSE is distinct from CallBack16 invocation failure.

Source-r8 passes167 scenarios/errors0, including the exact original native-only
branch with a service double that changes the callback cursor: normal return
preserves that effect, a deliberately omitted completion marker demonstrates
the wrong restoration, and an exception restores the entry cursor. A source
guard checks the intentional common-dialog bypass marker. Source-r7 could
not compile its new authored wrapper's unused uParam/lParam under /WX; add
explicit void casts in that wrapper, not a warning suppression or original
body edit. P21's required72 negative assertions remain. This is the final
candidate source; the previously tested84345E provider is not publishable as
the final result, and affected build/floors must be repeated against the seal.

The first supplementary runtime run passes all12 actual resolver abort/error
UI cases, then fails inherited-environment WRITE-Off: illegal instruction
05D8:21E0 with ASCII63/68/61/69/6E, missing all required startup classes.
Keep that failure and do not call the timeout observer's exit0 a pass. P21
under the same developer environment also fails its startup-class assertion,
but has no application windows, not the same illegal-instruction evidence.
The first environment supervisor's stronger same-symptom hypothesis therefore
fails; it does not establish the candidate's specific failure cause. Its
steps file was subsequently reused by the clean comparison; original owner,
failure and raw window/observer reports remain, not the old per-child table.

The full current-token/non-inherited Userenv control then passes P21 and that
prior candidate in all3 policies, each with the same four strict WRITE startup
observations. This preserves the supported logon acceptance condition, not a
repair or source attribution of the inherited-development-environment failure.
The guard's two normal-return corrections are source-proven independently,
not asserted to cause that WRITE symptom. The final sealed candidate still
requires its own full Userenv WRITE floor. WRITE editing/save/normal exit,
old0102 and inherited-environment failures remain explicitly unproved risks.

The styled raw-SEH diagnostic must not be confused with original WOW error
disposition: its fault is outside CallBack16's original handler and crosses
an optional native subclass/CallProc path before reaching the test-owned
catch. Microsoft's [WNDPROC contract](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nc-winuser-wndproc)
and [WOW64 exception documentation](https://learn.microsoft.com/en-us/windows/win32/winprog64/exception-handling-under-wow64)
do not promise that an uncaught window-callback exception reaches an outer
application handler on every such native boundary. That supports the probe's
limited portability, not attribution of its particular1067 exit: without a
pre-fault trace that exact failure remains unproved. Keep its failed result,
do not introduce a private USER reader/host-version gate, and separately test
original handled error UI/retirement in all3 policies. No new product semantic
concession or claimed styled raw-exception recovery follows from these docs.

Final source-r9 strengthens the167-case fixture: extract the production
WndProc's complete nested finally, not a simplified call to stack_leave.
Normal return, deliberately omitted completion and exceptional native-only
notification all pass with that exact finalizer; P21 still fails72 required
negative assertions. AbnormalTermination in the inner release-finally refers
to that inner cleanup, not the enclosing successful return. No product edit
or artifact change follows from this additional source check.

The final normal seal is WOW32
1983C92351D0503B4FC15A10C7AE97A4EFB48EA1A658BA11B637C690207541E5 and NTVDM
1FC1F21AD1FBDC87151E4F836670A931BBEA1591E75AE4A4C0B7A0CDB5E641F2.
Final-r1's fresh foundations pass21/21, original30 and source registers
278 selected/177 exact/101 registered/issues0. Its34 main orchestration rows
all pass: S3 twelve, six real message modes under3 styles, external9,
cross6, pending6 including required refusal, WINMINE/SOL gameplay and normal
exit3, native modal reference plus guest3, both-width NTW16S and full-logon3.
The retained legacy construction/parameter-map/COPYDATA27 group also passes.
These are separate actual entrypoints, not one denominator for all S4.

The final supplement passes both actual callback-boundary injection phases
and all12 original handled callback/error-UI cases. It then fails before
starting WRITE: PowerShell's case-insensitive switch parameter Logon shadows
the orchestration environment array named logon. That aggregate remains
exit1, not a success. Rename only the disposable runner's array to
logonEntries, validate/reuse its14 completed successful child receipts, and
resume the unstarted WRITE/DOS checks under frontier-final. Source/artifact
identities do not change. Publication is gated on those independent results
and the six retained Shell error-UI cases, not on the failed aggregate.
The waiting publication supervisor is explicitly stopped and replaced to add
source-r9 as a prerequisite; no test is interrupted and P21 remains installed
at that point. Non-elevated attempted child supervision cannot see these
authorized processes; actual PID/start checks and replacement use the same
authorized execution context, not a missing-process assumption.

The resumed frontier completes all8 child rows, exit0: WRITE startup/strict
assertion3 under complete Userenv, DOS Console17 and Window17. The next Shell
supervisor initially names a nonexistent outer diagnostic-shell-exit receipt;
it refuses before any Shell case starts, as does the waiting publisher.
Correct the disposable prerequisite to diagnostic-shell/exit.txt. This is
an orchestration failure, not an application failure or a passing aggregate.

The actual final Shell cohort passes Close under all3 policies, then fails
Ignore-Off: probe exit17, sibling launcher1067, worker8896 exits1067 and
its WINMINE is lost. The final trace reports TaskRetirement on tid39208,
task00000000, stage publish-native-return/status13. The preceding Shell
exception is original-return on task1737/tid37960; native error UI runs on
tid30204 and the final activation/text callbacks are WINMINE task17B7.
No source attribution to349 is established by this ordering. Keep the failed
aggregate and stop publication; neither callback12 nor the successful normal
floors can substitute for this distinct after-Shell-return boundary.

Source narrows publish_guest_return's failure to ordered window refresh,
borrowed graph projection or menu refresh. A diagnostic-only lifecycle TU
wraps these three results, preserving arguments/results/LastError and the
normal image; its first actual Ignore-Off passes, so it does not attribute
the earlier failure. The first diagnostic link script's mixed-slash output
replacement failed and relinked only the disposable sealed Shell diagnostic,
not normal Ninja, assets or installed images. Retain the overwritten files,
restore its exact0A720F DLL from package-shell, and explicitly record that the
old MAP was overwritten and is not evidence. Correct all output options with
exact fresh-path guards. R2 links and executes the diagnostic normally.

R3 adds read-only error13 source-line attribution inside the existing window
observation TU and a fixed eight-case Ignore-Off diagnostic, retaining all
results rather than retrying failed rows into a passing aggregate. It does
not change normal control flow, sample policy, private layouts or guest.
Native-chain normalization's local-only-cycle rejection and host graph/menu
validation are source leads, not established causes. A single passing
instrumented run is not a repair and cannot justify P22 publication.

The fixed R3 current diagnostic passes8/8 with no captured publication or
observation failure. A source counterfactual replaces all six349/101-modified
production TUs with immutable P21 bodies, retaining separate objects and the
unchanged current diagnostic observers. Its eight Ignore-Off controls also
pass. This is not the byte-identical P21 executable, nor attribution or
reproduction of the earlier failure. Its first link exceeds cmd's8191-character
limit before execution; the six completed compiles are retained and the exact
link is resumed through a response file, with separate failed/success receipts.
Normal Ninja/installed/P21 media identities remain unchanged by these controls.

### Same-cohort public native capture dependency102

Source establishes an independent finite acquisition defect: the adapter's
normalize_observation_chains assumes that a same-PID cycle means integration
corruption, although capture_window reads public HWND relations without NT4's
USER lock. A pure-native private-desktop control proves all three cases:
read A's next/parent/owner as B; legally unlink/reorder the native windows;
read B's corresponding relation as A. Both individual observations are true,
and the actual current native relation is acyclic. The exact production
normalizer rejects the mixed copy with13. This is a real public-query
interleaving counterexample, not a malformed-window service double. It proves
the rejection class exists on P21 and current source; it does not prove the
old Shell failure took this branch. No new snapshot semantic waiver follows.

Register ADAPTER-WOW-102 before editing. The entire private acquisition caller
closure shares project_graph: graph refresh, payload/return refresh and the
existing public project helper. Separate its unchanged capture/validation/
commit body into project_graph_attempt. Only the local-cycle normalization
outcome marks a call-local capture_raced flag. It occurs before object/graph
publication, so all captured rows/indexes are discarded through existing
cleanup. The wrapper permits one fresh acquisition after restoring entry
LastError. It never repeats native mutation, guest execution, failed commit,
invalid identity/root, allocation failure or arbitrary error13; no existing
local snapshot substitutes for fresh fields. A second raced acquisition
retains the original strict failure. No sleep, manager, retained state,
private USER access, ABI, mirror or guest change. Continuous native mutation
is not claimed to guarantee acquisition progress.

The formal source entrypoint is tests/observation/verify-wow-native-chain-capture.ps1,
with a fresh build child and the stable x86 compiler driver; -Reacquire selects
the full transaction matrix. The raw three native witnesses additionally
assert that existing foreign-cycle cutting and allocation failure never mark
the local-race flag. The extended fixture extracts the complete actual public
capture/put/normalize/acquisition bodies plus immutable P21 counterparts.
It uses real HWNDs and real Win32 getters; only initial page-domain copy and
final commit are service doubles. All3 relation families test P21 normal and
interleaved failure, current normal and recovered first race, persistent
second race, unrelated copy-error13 and commit-error13:21/21 pass, entry
error447A retained on success and exactly one complete commit after recovery.
Persistent races perform exactly two acquisitions/no commit; non-race and
commit failures perform one. Original six no-reacquire controls are not
deleted or weakened. The first extended compile fails /WX because the
authored fixture's global edge shadows the extracted body's local edge;
rename only the fixture variable, not the source or warning policy. R2 passes.

Normal frozen rebuild102 compiles only wow_user_object_bindings.c and relinks
WOW32; the second Ninja invocation is no work to do. Final-r2 retains the exact
prior11 host images and unchanged worker, records source/artifact seals and
rebuilds all three diagnostic providers separately. Its owned supervisor44876
starts22:24:46.790Z with4800s budget: raw classification, normal build2,
coherent stage13, six actual Shell cases first, then the full34 main rows.
Foundation supervision separately refreshes source registers/native21/original30
without BaseSrv. These fresh runtime results are pending, not implied by the
21-case native/service proof; all older final-r1 outcomes and failed Shell
aggregate remain unchanged. P21 stays installed until the full new seal passes.

### P22 bounded conversion ownership delivery

Final-r2 completes the whole349/101/102 cohort, not S4/T447. Normal WOW32 is
ACD234099C6B9B58A9AE5846181CE85E6AA9E2E58AB88BB8D07C1F554C907F0F;
worker is1FC1F21AD1FBDC87151E4F836670A931BBEA1591E75AE4A4C0B7A0CDB5E641F2.
The worker's executable bytes differ from P21 only at the previously proved
PE timestamps, not CPU/guest code. The other11 exact P21 host images are
unchanged. Release manifest B710548CA85F58FE01970CAD92A8FEB792A961ACBCC7A4EDE7B3B1391DA7B9AF
matches all13 in assets/release and O:/winnt/system32. Installed both-width
NTW16S and full-token logon WINMINE gameplay/normal exit3 pass. No existing
installed project process needed closure. The protected SYSTEM.INI/NTVDM.REG
presence/hashes remain unchanged. Canonical guest ZIP remains
7A0534DEC9940B2D8366C8EF71DD536ED94BE7817D7C4E1E19BED05F6E60B8F7;
no guest is patched or compiled, and no diagnostic image is published.

All groups use the exact final normal seal, with separate diagnostic images
and declared service-proof limits. Source-r9's selected constructor inputs
are compared byte-for-byte before reusing its167-case proof/P21 required72
negative assertions (result49041989F30C50A8ADF9ED6F9E6DE0C0E463F84B156E21C898E0E44DA7054F27).
The new actual-source native controls pass3 relation witnesses plus foreign/
allocation classification, and all21 complete capture transactions. The first
102 native fixture's global-name compile reject remains a non-pass, not a
suppressed warning. Native21/21, original30 and source registers278 selected,
177 exact/101 registered/issues0 pass freshly after the102 seal.

| Final-r2 execution receipt | Actual required assertions | SHA-256 |
| --- | --- | --- |
| floors-steps,34/34 | S3 twelve, six message modes under3 styles, external9/cross6/pending6 (including refusal), WINMINE/SOL gameplay and normal exit3, native modal reference/guest3, style both widths, complete logon3 | 4E9C1898BCBDBB279C953C8EF24C662235C85D167B3356B3FDB633DA746D64E1 |
| shell-first-steps,6/6 | Separate actual after-Shell-return Close/Ignore under3 policies, surviving worker/WINMINE and normal retirement | B8E58EF010D2A4D614790DD956F85318238A1206240DDC851D78FAAA9E259C82 |
| supplement-steps,22/22 | Actual allocator/callback faults before/after, resolver Preparation/Returned × Close/Ignore ×3, strict WRITE startup3, DOS Console17/Window17 | 283A564CF44A18E15398CC49942D28C3EE5F9D444D4811AC0CE6EDFC9D50A91A |
| retained-units,3/3 | Legacy construction/six earlier negatives, complete parameter-map, COPYDATA27 | 49308842E0704F241F82F3630272A1A4788F0AC1894A986B4DBE5391F6EF1EB4 |
| foundations,3/3 | Fresh native21, original30, source registration; not actual guest proof | 926A00B3CF6016280368D1BA0BAAAD55997EA7D64F555781D294F243D940744E |

Main actual rows run22:27:10.142Z through the last launch22:43:14.721Z;
supplement launches22:43:36.171Z--22:48:22.411Z. Every child has its actual
PID/start/arguments/deadline/exit; real BaseSrv packages run serially. Foundation
service/CCPU work uses separate processes with no global BaseSrv endpoint.
The waiting-only rest supervisor is replaced before any test begins to freeze
its final input-proof variable name; no running build/test is interrupted.

Diagnostic-only hashes are A63902D706F530EB9BF2997DCEB223F8267DD282F07225486E1DBB03B66D889F
(message transaction),850027E3CA609710D85F2D5F7BE4B583BBFE2340B3BDCCFA17CAD4DBAFA56546
(callback abort),B0903485572DBFD447480F74AAAEAA6EEC3E3BA90F4869FCE8567A1B48426486
(after-Shell-return). Each manifest binds normal ACD23409 and publishable:false.
The actual normal provider is built from the stable MSVC x86 Release/MT
CCPU40 closure;102 rebuilds one object plus link, then Ninja is no-op. No
cache clear, extra Ninja tree, pruning or global ccache mutation occurs.

Second-role review preserves the fixed eight caller/19 heap-family closure,
original allocator/output/result/lock ordering and protocol owners. New code
records acquisition/consumption and failure recovery, not a third behavior
engine.102 permits only a precommit fresh reacquisition, never stale local
fields or arbitrary error/commit replay. All six changed originals were
already registered content differences;349 raw delta is169 additions/
105 deletions =274 against budget300.102 adds zero mirror changes. There is
no new mirrored file, import, public/wire/record layout or guest difference.
The standard-class ASLR correction affects only the test's unrelated-module
selection and preserves its original assertions, not product cleanup.

Do not infer more: the first final-r1 Shell Ignore/error13 remains unlocated,
and neither later passes nor the independent native-query counterexample
prove its exact cause. Old0102/inherited-environment/WRITE risks also remain;
WRITE editing/save/normal exit and styled raw outer-SEH recovery are unproved.
Full M1--M6, protocol-owned DDE/packet retirement and M3 idle reconciliation
remain S4 obligations. S5 termination/NT Monitor and S6 whole-T review remain
unadmitted. No semantic waiver, performance claim or whole-S4 closure follows.

Pre-cleanup source seal7D0E00D45FD63D4E716C0679CBFA06CB5261F8B4DD5A40774E7B7C2ABB3EFA71,
artifact seal FC5AE18721E6439654B4A867572BEB6A11100A0CBD203BA8CB8FA083B88F788A,
publication receipt DC63E56702ACE29EB168BFA0B3CD3BA4DBC3FA0463278E017AEA5C5DC5E9D028.
Publisher11944 starts22:48:07.046Z and completes both installed gates/exit0.
The old unlocated Shell failure's lifetime/probe/stack hashes are respectively
2CF1EEA1E7BBB6EA9891A771176FEB72F06055968D101C30107C489F791F3932,
6F9B880218CB524CE9261D36EF70C63B22BF06A45B08E2A0D7CC78792F12DBB0,
57C1B0EBC0760C1D4238C275C0265D02EB0E26027067E5851ED333274682F0E4.
The preceding source/phase/terminal observations above are retained before
disposing raw files; their removal supplies no new diagnosis or repair claim.

Authorized CIM inspection at22:55:38Z finds zero live task-root, physical-test
or installed project processes. Cleanup preview29404 first refuses20 unknown
child-report.bin files without deletion. Inspection of the formal gameplay
producer and all identical12-byte SHL1/phase3/errors0 reports establishes
469B36BC4EA192DD49B1BC294A714B9B94D1107FF73F841CF9335C6E85B2DCCD.
Preview42516 at22:57:09.276Z then passes:15700 files/904703804 bytes under
build/t447-s4-message-transaction, plus the sole28-byte G7 input under
build/M0-T447;1711 sensitive files are individually reviewed. Original media/
configuration copies match the pinned ZIP by leaf name and hash; independent
probe/results/listings/diagnostic data have their specific producer ownership.
No Git checkout, junction, user original or unknown content is accepted.
Manifest71D50A4515460D48A00CCE4E382D6E97CBA64B8F493211F8B69E5D15454F0CB0
is a567555-byte disposable build-root file, not hidden in either cache. Apply
will recheck quiescence and all target safety first, add that manifest's own
unchanged inventory only in memory, and use the sole cleanup tool for all
three exact targets together. Preview performs no deletion.

Cleanup27148 starts22:59:11.633Z, rechecks actual process quiescence and every
reviewed membership/metadata/hash, preflights all three targets before deletion,
then completes Apply/CheckResidual with exit0. It removes15702 generated files/
905271387 bytes (about863MiB), including the temporary manifest; build now
contains only .ninja and .ccache. No cache, global ccache, original archive,
installed configuration or Git object is removed/pruned. Raw diagnostic
outputs are intentionally discarded, not recoverable logs masquerading as
retained evidence; the concise observations/hashes and reproducible formal
producers above remain. Prior published13 is recoverable from P21's Git
baseline, not from another permanently retained disposable build tree.
