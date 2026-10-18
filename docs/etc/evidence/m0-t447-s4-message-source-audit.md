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
