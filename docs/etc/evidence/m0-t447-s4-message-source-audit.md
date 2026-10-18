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
