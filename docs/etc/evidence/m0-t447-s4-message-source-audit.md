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
