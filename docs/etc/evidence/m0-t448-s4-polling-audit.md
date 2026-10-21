# T448 S4 owned polling audit

## Scope and baseline

Admitted after S3 P21 6ad0a4ba6, with the unchanged published P20 protocol64
package. Ordinary single-person implementer/reviewer. No guest changes,
original internal wait rewrite, third transport or recovery/retry framework.
This initial inventory is source evidence, not implementation or acceptance.

Read actual Sleep/SleepEx, timed waits, timers, condition waits and wrapper
aliases across the owned launch/service/worker/frontend paths, then adjacent
NTMON, WOW32, NTW16S and NTVDM adapters. Exclude original mvdm/opennt-host and
declaration-only opennt-abi from autonomous-code removal decisions. A zero-time
read within an event-woken operation is not a polling loop; a condition-variable
predicate loop is not polling when its wait is INFINITE.

## Finite disposition inventory

| ID | Actual owner/call chain | Finding and bounded action |
| --- | --- | --- |
| Q01 | run16/main.c connect_broker | Absent endpoint bootstrap retries connection every at most50ms inside the single approved5s budget. No task replay. Retain provisionally: prove whether existing RPC startup can notify readiness without inventing a new IPC mechanism before replacing. |
| Q02 | ntsrv/main.c idle timer | Existing one-shot empty-service5s policy, rearmed/cancelled by actual membership changes. Retain; not periodic polling. |
| Q03 | worker-base shutdown_close | Actual thread wait, either original Console grace5s or INFINITE. Retain; no sampling loop. |
| Q04 | NTVWM presentation_loop | Active output sampled20ms; inactive/route/input/close waits event-driven. Existing TODO records missing reliable Console output-change notifications. Retain disclosed output sampling, not input/lifecycle polling. |
| Q05 | NTVWM release_console_carrier -> console_close | Detaches and posts WM_CLOSE, then Sleep10/IsWindow until exact HWND identity disappears. Removal candidate: evaluate destroy notification using existing carrier WinEvent ownership; cannot replace with an assumed conhost-process lifetime or report success at PostMessage. |
| Q06 | NTVWM presentation final_capture | Eight ERROR_RETRY snapshots with Sleep10 under presentation lock. Audit actual instability producer and final I/O ownership before selecting removal; never drop final output or pretend capture failure succeeded. |
| Q07 | NTVDM console_compat read_pc_input | Mouse queue full returns after Sleep1 so original event loop can process suspension/APC. Audit bridge dequeue/ICA ownership and possible existing readiness notification; do not block holding ICA or consume input without capacity. |
| Q08 | NTVDM CCPU wait_for_event | Event-driven HLT; Sleep0 only when no initialized wake event. Trace selected call/lifetime and fixture paths before removing fallback; not a periodic normal HLT wait. |
| Q09 | NTVDM WOW page-domain clock_publisher |10ms publication of guest-readable tick values, stopped by event and joined before unmapping. Time itself changes without input; preserve guest-readable clock semantics. Inspect admitted clock mapping before any optimization. |
| Q10 | worker-base publication publish_thread | Dirty-event wake plus one-shot50Hz rate limiter, cancellable by stop/close; no timer while clean. Retain coalescing, not lifecycle timeout. |
| Q11 | NTMON main refresh loop |750ms management/elapsed-time display refresh plus keyboard wait. Audit snapshot versus displayed time needs and existing notification API; no new service transport just to avoid a UI refresh. |
| Q12 | WOW EDIT/NTW16S control timers | Source-shaped caret/autoscroll/repeat behavior. Retain behavioral timers; not lifecycle monitoring. |
| Q13 | NTSRV predicate scans, common async/pipe, worker-base input/task watch, NTCON present/service | Indefinite waits wake on actual process/event/I/O/condition changes; zero-time checks select priority or validate pinned identity. Retain, inspect for missed signals/hidden wrapper polling rather than mechanically deleting checks. |

## Required proof before implementation

For Q05 the existing carrier guard already owns an OUTOFCONTEXT event thread,
but current release explicitly stops it before detach. Any reuse must subscribe
before destruction, pin exact window identity, survive destruction before wait,
and join callback storage safely; changing only Sleep to a process wait is not
proved equivalent. No implementation is selected yet.

For Q06/Q07, read the producer and consumer transitions as a whole before changing
the wait. Required tests retain final frame/input ordering, error propagation,
close/service-loss interruption and no lost wakeup. Retained items need evidence
and explicit rationale, not an assumption that every Sleep is necessary.

Remaining finite audit work is wrapper-alias/finite-timeout coverage and these
producer boundaries. Then select the grouped implementation, run affected
deterministic tests and every-P full package gates if product code changes.
No new runtime code or release has been produced by this inventory.

## Producer-boundary trace

Source review against the P21 worktree establishes these additional facts;
these are not runtime test results:

- Q06: `console_state.c::ntvwm_capture_begin_output` compares geometry before
  and after acquisition; `ntvwm_capture_read` checks geometry before and after
  each tile and rejects changed/clipped rectangles. Thus ERROR_RETRY means a
  torn snapshot, not a disconnected frontend. `main.c::end_io` calls flush
  even when the service says KEEP, so final capture is not restricted to a
  Console whose last writer has died. Deleting retries on the assumption of
  a quiescent producer would be incorrect. Any removal must retain the final
  coherent-frame obligation and error propagation, not accept a partial frame.
- Q07: capacity is measured under ICA in `mvdm_softpc_mouse_capacity`;
  `mvdm_softpc_mouse_submit` commits producer route state only after enqueue.
  The CPU-side `mvdm_softpc_mouse_apply` consumes through `mouse_next` and
  `mvdm_mouse_input_take`. A large relative displacement can require several
  deliveries before a queue slot is actually freed. No capacity notification
  exists in this bridge today. A replacement wake must follow actual count
  reduction or cancellation, not every mouse delivery, and preserve original
  input-thread suspension/APC handling. Simply deleting Sleep creates a busy
  loop; an unconditional blocking wait is not yet justified.
- Q08: the production standalone worker explicitly initializes the CCPU wake
  event; both identified CCPU fixtures do likewise. The original CCPU call is
  in `c_main.c`. The uninitialized Sleep(0) fallback is separate from the
  correctly initialized infinite event wait; lifecycle review remains needed
  before changing its failure contract.
- Q13 wrapper sweep: `base_sync_sleep_milliseconds` and `base_sync_yield`
  have definitions/declarations but no callers in src or tests. Their Win32
  wrappers are not an active polling path. Removing this unused API is a
  possible code reduction, not evidence of reduced runtime wake frequency.

Procedure: rg over src/tests for the named symbols, followed by direct reads
of the bridge, queue, guest consumer, capture provider and main handoff callers.
The remaining implementation decisions stay within Q05-Q08; no additional
feature or transport is admitted by these findings.

## Q05 rejected destroy-notification candidate

An uncommitted candidate replaced Console-close Sleep(10) with a caller-thread
OUTOFCONTEXT EVENT_OBJECT_DESTROY hook, installed before FreeConsole and
filtered by the captured Console PID/TID/HWND. It retained the existing hide
guard and used a message wait, without adding a thread, IPC or product deadline.
Actual `ntvwm-close-test.exe` built through the stable x86 Ninja/ccache tree,
but its live-client owner did not complete within the fixture's 15-second
diagnostic budget: `NTVWM-CLOSE error=13 ... FAIL`. This does not prove that
the window remained alive or identify why notification was absent; it proves
that this implementation cannot yet replace the existing completion check.
The candidate source was reverted before publication. No guest/mirror changes.

Evidence: build/t448-s4/close-r2-result.json, close-r2-test.out and close-r2.out;
controller34552, 2026-10-21 19:09:10.859 through19:09:31.634 UTC. Elevated CIM
subsequently confirmed that controller and fixture processes were absent.
The prior r1/controller33484 failed before compilation because the runner name
was wrong (run-ninja.cmd instead of run-ninja-parallel.cmd); retained as failed,
not a runtime result. The compiled disposable fixture contains the rejected
candidate until rebuilt and must not be used as a release input.

Baseline comparison r3/controller19712 rebuilt the restored source and passed
both live-client normal close and empty-Console close on the same host,
2026-10-21 19:10:31.395 through19:10:37.529 UTC. The result is in
build/t448-s4/close-r3-result.json and close-r3-test.out. This restores the
fixture to baseline and rules out a general failure of this host's existing
close path as the explanation for r2; it still does not prove the precise
missing-event cause. git diff confirms no retained production change.

Next bounded proof for Q05: distinguish silent process-owned window teardown
from an ordinary explicit DestroyWindow notification, and determine whether
pinning the actual Console owner plus exact window events provides both
completion paths without assuming every conhost process is window-exclusive.
No runtime-equivalence claim or new lifecycle exception is made.

## Q05 event identity proof and revised candidate

The existing real close fixture now optionally records WinEvents plus pinned
PID/TID wait states (`NTVWM_CLOSE_OBSERVATION`, test-only). r4/controller21924
and r5/controller35340 show that filtering by the reported window thread, or
even just its process, misses EVENT_OBJECT_DESTROY (0x8001); an unfiltered hook
receives it for the exact captured HWND. Both baseline close cases pass.

r6/controller7632 adds actual self and event-source identities. The reported
window PID/TID equals the fixture client's own PID/TID, whereas Console events
originate in another process. Waiting on the former would wait on the caller
itself. That candidate also logs one `closed error=0 window=1`: the pre-existing
PID/TID-mismatch shortcut can return before the window disappears. Its later
diagnostic drain concealed this from the old post-helper check. r6 is therefore
NOT sufficient closure proof despite its aggregate pass. The observation path
now preserves the immediate IsWindow result and asserts it after the drain.
Without observation, r7/controller35804 passes the original immediate checks;
this one pass does not negate the observed race.

Revised source uses a temporary caller-thread OUTOFCONTEXT destroy hook with
exact HWND/object filtering (as the existing hide guard already does), installed
before detach. It removes the unsound reported-PID/TID identity shortcuts and
Sleep(10), pumps notifications with an infinite message wait and unhooks on
every exit. No new thread, IPC or product deadline. Callback TLS is restored
after unhook; successful posting alone is never completion.

r8/controller36380, 2026-10-21 19:15:51.298 through19:15:58.214 UTC, rebuilds
and passes both actual live-client/empty close cases including immediate
window-disappearance assertions. Example: window PID10988/TID20880 equals
self10988/20880, event source PID26416/TID22928; destroy notification is seen
before `closed error=0 window=0`. All logs/results use matching close-rN names
under build/t448-s4. These are focused tests, not product delivery or S closure.
Remaining Q05 proof: hook/detach/post/wait failure cleanup, repeated closure,
unrelated notifications and broader carrier/lifecycle regression. Q06-Q08 and
the full coherent publication gates remain open.

## Q05 deterministic failure coverage

`tests/app/ntvwm_close_events_test.c` includes the actual Console-state source
with only host API boundaries substituted. Eleven cases repeated16 times
assert exact failure returns, detach/post/wait/unhook counts, preservation of
WM_QUIT, restoration of a pre-existing callback TLS binding, and rejection of
wrong-HWND, wrong-object and nonzero-child destroy notifications. It asserts
that notification installation precedes detach and that waits remain infinite.
It does not replace real conhost acceptance or prove host API behavior.

r9/controller38796 stopped during graph generation because NodeExecutable was
not supplied; no test pass is claimed. r10/controller27216 explicitly uses the
same Node runtime already selected by CMake, regenerates the stable cached
graph and passes `NTVWM-CLOSE-EVENTS PASS cases=176` plus both real Console
cases. Times:2026-10-21 19:19:38.280 through19:20:16.141 UTC. Results are in
build/t448-s4/events-r10-result.json and r10-ntvwm-close*-test.out.

The generator now exposes `ntvwm-close-events-test.exe` as a normal fixture
target; objects remain in the existing .ninja tree. Full thirteen-component
build and second-incremental verification started as r11/controller21016,
followed by four repetitions of both fixtures. Its result is not yet known
at this record entry. This remains local candidate work, not a published P.

r11/controller21016 completed successfully at2026-10-21 19:24:22.859 UTC:
all13 images built; second incremental preserved1,419 intermediates; four
repetitions of both close fixtures passed. This validates the Q05 candidate
inputs, not subsequent mouse-bridge edits or the full product runtime gate.

## Q07 existing input readiness reuse

`worker-base/input_watch.c::input_thread` sets its manual wake once, then
removes source readiness from its wait set until consumption acknowledgement.
When the mouse queue is full, read_pc_input consumes nothing and issues no
ack. Therefore the existing latched wake can be cleared under ICA and signalled
again only when a CPU dequeue actually frees a slot; no second wait handle,
thread, timer, transport or original-loop change is required. ICA serializes
capacity/reset against dequeue/signal to prevent a lost wake. Original
nt_event_loop continues to wait alertably on suspend first, then input, so
its existing suspend and APC termination paths remain authoritative.

The local candidate borrows that input-watch handle in the bridge, removes
Sleep(1), signals full-to-not-full transitions and full-queue cancellation,
and preserves the borrowed handle while clearing route state at a quiesced
handoff. The watch remains the sole handle owner; bound producer/CPU threads
must already be joined before client teardown, as before. Fragmented large
displacement does not wake while its slot remains occupied. No guest/mirror
change. Existing bridge and Console-client fixtures are extended rather than
adding a parallel queue implementation. r12/controller37448 builds and checks
these candidates; its result is pending at this entry. Integrated input/loss
and original DOS execution evidence are still required before publication.

r12/controller37448 completed2026-10-21 19:27:26.680 UTC, passing the real
bridge/queue fixture with actual manual-event reset/wake checks, including
partial displacement, full cancellation and borrowed-handle preservation.
Console-client fixture compilation also passes. r13/controller27940's runner
incorrectly expected zero from that client's intentional close callback; its
actual result0x49 and PASS callback log match the established contract in
tests/observation/verify-console-close-contract.ps1. Preserve r13 as a failed
harness assertion, not a product regression or a silently rewritten pass.
The corrected r14 uses that existing0x49 contract with fresh evidence.

## Remaining-category review and grouped validation

Q08 disposition: retain the existing CCPU wake binding, including its
uninitialized yield fallback; do not replace it with a failing null-handle wait.
The selected standalone worker requires `mvdm_softpc_ccpu_wait_begin` before
execution, and teardown closes it after the execution/producer boundary. Both
`ccpu_halt_reset_test` and `wow_callback_abort_ccpu_fixture` explicitly initialize
the same carrier. The sole original caller in `c_main.c` uses the infinite
event wait in this admitted lifetime. This source review finds no normal
periodic HLT wake to remove; it is not proof for unsupported uninitialized
callers, and does not change DIV-330 or claim new runtime coverage.

Q11 disposition: retain NTMON's750ms display sampling. Its `refresh` calls the
existing copied task-snapshot RPC; `render` also displays elapsed time, which
changes without a task membership event. The management API supplies snapshots
and management actions, not a change subscription. Making this wait infinite
would leave both displayed membership and elapsed time stale until keyboard
input. No new service notification protocol or UI scheduler is introduced just
to remove this necessary observation sampling. It is not a lifecycle deadline.

Q07 failure review: `console_input_failed` also latches transport failure through
`ntcon_worker_client_fail`; watch readiness is not the sole failure indication.
The existing worker failure/lifecycle route remains separate from mouse-slot
readiness. Capacity reset and real dequeue are serialized by ICA; cancellation
and quiesced handoff preserve the borrowed event, never its ownership. Integrated
service-loss/input tests are still needed; the bridge fixture alone cannot prove
the complete original event loop.

r15/controller25620 is the grouped candidate run. The full thirteen-image build
and second incremental pass with1,420 unchanged intermediates; service40,
frontend-scope and nine lifecycle-owner cases pass before package staging.
The controller remains active at this entry, so downstream runtime gates,
publication and S4 closure are not yet claimed. Source inputs remain frozen
during this run. Q06 final-capture retry remains an explicit unresolved item;
no final-frame semantics have been weakened to force a passing disposition.

### Q06 selected minimal change, pending implementation

Keep the existing maximum of eight optimistic acquisition attempts, but remove
the inter-attempt Sleep(10). This is bounded consistency acquisition, not a
wait for a task to finish or permission to retry a submitted command. Every
failed acquisition is discarded before publication; non-ERROR_RETRY failures
still return immediately, and exhausted instability still returns ERROR_RETRY.
Do not substitute the last previously captured image or acknowledge a barrier
after failure. No new timer, wait handle, thread or transport is necessary.

The old seven sleeps offered up to70ms of settling opportunities, not an atomic
Console snapshot or a guarantee that a concurrent writer would settle. Removing
them intentionally removes that scheduling delay; it does not promise equal
success probability for every concurrently resizing writer. Geometry validation
and bounded failure remain the contract. Existing ordinary20ms output sampling
is separate and unchanged. Capture validates geometry, not an atomic snapshot
of all concurrently writable character cells.

Before delivery, inject repeated capture instability and a non-retry API error
at the capture-provider boundary; assert attempt counts, unchanged publication
on failure, eventual complete-frame publication after transient instability,
and actual real-buffer/pipe behavior. Keep current r15 inputs frozen until its
controller is terminal; r15 cannot serve as evidence for this subsequent change.

## S4 P1 progress delivery

r15/controller25620 completed runtime gates: full13 and unchanged1,420
intermediates, service40 and nine owner cases, close176 and mouse bridge,
real RPC/migration/wire, WRITE's retained startup frontier, DOS17, WINMINE
gameplay, CAF-three before-Enter echo and negative native-completion/I/O cases.
SOL retains owner-confirmed startup, not fresh full-gameplay acceptance.
At19:39:26 UTC the aggregate failed the CURRENT32KiB documentation limit;
its failed all-r15/accept-r15 records are preserved. No product test failure
was waived. CURRENT's redundant S3 history was condensed to its existing ledger.

Controller13332 explicitly validated the preceding individual runtime gates and
the exact governance-only failure before continuing the unexecuted post checks.
At19:42:06 UTC these pass: common-native142 checks/zero handle delta,
presentation509 checks/zero failures, governance, whitespace and original-source
hash review. See post-r15-result.json, presentation-r15.txt and review-r15-result.
The candidate inputs did not change between runtime and post verification.

Publication37912 then backed up the coherent P20 set and copied all13 tested
images to assets/release and O:/winnt/system32, verifying every SHA-256 against
the candidate manifest. publication-r15.json records time/hashes; recovery is
publication-r15-backup. No installed project process was found by exact-path
CIM inspection before replacement. Q06 remains unimplemented and is not part
of this delivered candidate. This is P1 progress, not S4/T closure.

Self-review: Q05 subscribes before detach, rejects unrelated notifications,
restores TLS and removes the invalid Console-reported PID/TID shortcut; real
close and mocked error paths complement each other. Q07's borrowed signal
remains watch-owned, full-to-free transitions are ICA-serialized, and partial
mouse displacement does not signal a nonexistent slot. The integrated tests
do not claim exhaustive host scheduling or HWND-reuse proof. Production diff
is +65/-24 (net+41); tests +203/-6 (net+197, including107 new fixture lines);
build selection +6. This progress delivery increases owned code to replace
polling and prove failures; it does not falsely claim a net reduction. Original
mirror diff and immutable guest delta are both zero relative to P21. Q06 and
final retained-category reconciliation are the remaining S4 work.
