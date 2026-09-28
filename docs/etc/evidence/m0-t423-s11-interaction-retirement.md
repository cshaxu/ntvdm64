# T423 S11 Interaction And Retirement

## Latest scope and mouse repair, 2026-09-28

Owner requires mouse completion in S11, followed by commit/push and a stop
for owner validation. Only native continuity/retirement restructuring moves
to the approved S12 NTCON plan; RDP capture moves to S13. Neither transfer
means those capabilities passed. Historical sections below retain the rejected
alternatives and their evidence. Do not automatically begin S12.

The mouse candidate restores original five-record consumption and moves the
existing scan-less keyboard conversion unchanged out of the mirror into the
worker adapter. Maximum expansion is forty stack records, not another queue.
Owner-supplied removal of the packing Sleep(10) and suspend-first wait order
are retained. Original keyboard backpressure, mouse IRQ delay, EOI and guest
callbacks remain unchanged. nt_event.c uses the original CRLF line endings.
Its pending diff is +15/-148 lines against the committed baseline; relocation
of normalization is mirror reduction, not deletion of that required behavior.

Removing throttling exposed a bounded-queue problem: the adapter could remove
more relative input than its 256-slot downstream FIFO could accept. The single
event producer now bounds raw removal by free slots under ICA; at full capacity
it leaves all records upstream and returns to the alertable suspend wait after
a 1ms pressure-only yield. No direction/button merging, unbounded allocation,
guest patch or changed original IRQ cadence is used. The capacity invariant
depends on the existing single event producer; the CPU consumer only frees slots.

A separate real pressure/source-retirement failure reached DisplayErrorTerm.
Temporary error-only instrumentation recorded error=5023 (INVALID_STATE),
action=3 (LEAVE), submitted=0, active=0, count=0 in
Logs2/t423-s11-mouse-error-r1.txt. Original FlushMouseEvents also invokes this
adapter on IRQ cancellation/suspension: its previous whole-state zeroing erased
frontend route ownership although upstream records still belonged to it.
The adapter now clears pending samples/buttons/remainders but retains the
last accepted ENTER/LEAVE state. Cancellation is not frontend disconnection.
Late MOVE/LEAVE is accepted without fabricating a new frontend ENTER. Explicit
LEAVE still retires the route, duplicate ENTER and malformed input still fail.
Temporary instrumentation was removed before the final build.

### Current verification identities

MSVC x86 /MT incremental worker build uses
build/M0-T423/S1/restart-formal-x86. The final mouse-only candidate is
build/M0-T423/S11/mouse-delivery-r1/p, mapped as L:. Its other six binaries
are the preserved S10 candidate; it excludes uncommitted native branch code.
Current worker SHA256: 205281811C412A8DFB62150E2B66135DB2616848EE8A238CEE7060A11A5CDA81.
This is a tested candidate identity, not a publication claim.

The preceding 3CCCC54C4673D5F9F9C41A8E52B4472896B4F60E719A08DD2CB906F7A80B766B
candidate passed mouse stress but failed Window DOS handoff regression. Its
Console17 all passed; seven Window cases reached INVALID_STATE after route
re-entry. This revealed the second cancellation boundary: frontend
apply_dos_binding(FALSE) discards DOS mouse records and resets its source;
worker IRQ cancellation alone must not reset a live source, but successful
CONSOLE_IO_DOS_ACTIVE(FALSE) must. console_client now clears local route state
only after that acknowledged handoff, with original input/timer threads already
quiesced. No frontend/ABI change or permissive duplicate-ENTER exception is used.
The failed full matrix remains Logs2/t423-s11-final-matrix-r1.json.

The real Console-client transport fixture checks ERROR_BUSY preserves the
existing route and successful deactivate clears it before resume. It passed
with the expected original-shape callback exit 73 in
Logs2/t423-s11-client-retire-r1.txt. Production-path Window MEM, three nested
MEM invocations and EDIT-to-MEM pass in t423-s11-handoff-r2-summary.json.
Full final matrix-r2 and final pressure rerun remain required below.

- Batch production-adapter fixture passes pressure/nonconsumption, bounded
  recovery, 1000 absolute and 1000 relative records in 200 reads, signed motion,
  exact button/order preservation, key expansion and shortcut filtering:
  Logs2/t423-s11-final-batch-r1.log.
- Relative bridge fixture passes under observed ICA/IRQ bindings, including
  full-queue atomic rejection, cancellation followed by MOVE/LEAVE, duplicate
  entry and independent state. This is unit evidence, not a guest run.
- Real guest stress passes 1000 alternating moves plus click/release,
  1000 moves plus source retirement, and the 200-event control:
  Logs2/t423-s11-final-pressure-r1-{burst,retire,latency}.txt. These runs used
  identical semantic source before the mechanical CRLF normalization.
- Final binary passes all five existing real guest cursor tests:
  Logs2/t423-s11-final-mouse-r2-{count,video,text,move,retire}.txt.
- Pressure-r3 also passed after the cancellation fix; the earlier failing
  pressure-r2-retire and diagnostic-r1 runs remain available, not overwritten.
- Inputs are injected into the real private-desktop Window event path and
  asserted by an actual guest callback. They do not prove physical RDP capture
  or desktop focus; those retain their prior explicit disposition.

The corrected mouse-only candidate passed all 34 strict Console17/Window17
cases in Logs2/t423-s11-final-matrix-r2.json, including native-zero/missing.
This does not prove general shared-buffer/native retirement correctness; that
counterexample and its S12 receiver remain unchanged.

### Sealed seven-file publication candidate

To exclude S12 WIP without reverting it, the final graph uses a read-only
git archive of HEAD 44d465bf8 src/ntkvm-exe under
build/M0-T423/S11/pressure-r1/head-source. Its archive SHA256 is
2AA363732852A77899B668AACD1E939B216903294AA22B7E6CA703F00D6E9686.
The generated s11-delivery.ninja graph redirects frontend sources/includes
there; other sources are current HEAD plus the S11 worker patch. The seven
MSVC x86 /MT formal targets all build successfully. The changed graph caused
582 compile/link steps; subsequent work retains that cache. WOW32's original
cache reports no work required. The first target-name case typo is retained
as package-target-case-failed.log, not a compilation or runtime pass.

Final package: build/M0-T423/S11/production-r1/p, short drive M:.
Its source is committed frontend code plus this S11 worker patch, never the
uncommitted native-branch prototype. Earlier L: candidates remain preserved.

| File | SHA256 |
| --- | --- |
| run16.exe | 26763B94285E6AA1E9D790AC86807E5C6426A1F13EC0784B716C4F13E6FB3CBA |
| ntsrv.exe | FF7624F46496A4D179E20AF41367A5E4EFF5FCB2FC9D7C5CA3E223AA5A63F00C |
| ntvdm.exe | CED9E83E052FD8123F95560F1890F64EAE3E9976143C6726ED1FB8F53274C981 |
| ntkvm.exe | D9C4A0E5A66C99E3993F85335F31DA42BDD3D2D1086A4A3A25A1603F78E0763C |
| ntmon.exe | 721A6592C13AD2931249C47C7BBCC0BC6F0EBDD59EF7D8A63A9B46D6840114E1 |
| WOW32.DLL | 0D2AE60264B03A8040D98AA86BCF80455127064084E2217D318D5E13F90FA94A |
| VDMREDIR.DLL | 2F643878A91B62E58584AE02C1D26852E28B30BB4267CAF46E7F5A741F8BD3C4 |

The sealed combination passes all 34 strict cases independently in
Logs2/t423-s11-sealed-matrix-r2.json. A prior denied drive-mapping attempt
started no product; its environment failures are saved separately as
t423-s11-sealed-matrix-drive-rejection.json. No assertion was weakened.
Final adapter fixtures pass in t423-s11-sealed-batch-r1.log. The same sealed
package also passes:

| Gate | Evidence under O:/winnt/Logs2 unless noted |
| --- | --- |
| Three real guest pressure cases, 1000/1000-retire/200 | t423-s11-sealed-pressure-r2-* |
| Five real guest cursor/input/retirement cases | t423-s11-sealed-mouse-r3-* |
| Console and Window five-case lifecycle/faults | t423-s11-sealed-fault-console-r1-* and fault-window-r1-* |
| GGGWDWGGGDWD and GGGDDWGGGWWD | t423-s11-sealed-chain-r1-*; build/M0-T423/S11/chain-sealed-r1/summary.json |
| Actual NTMon Console/Window title, F3 and result | t423-s11-sealed-monitor-r3-* |
| Separate WINMINE, SOL and WRITE frontiers | t423-s11-sealed-wow-r1-* |

The WOW gate preserves a headless WINMINE main window and the original
SOL/WRITE out-of-memory dialogs, not playable acceptance of those applications.
Physical desktop focus/RDP capture remains explicitly unverified/owner-waived.
No general ConPTY screen equivalence or native-member retirement is claimed.

NTMon's first run inherited a 39-column buffer, wrapping its title across
rows; the unchanged readiness marker could not match, so no F3 was sent and
the test timed out. That failure remains t423-s11-sealed-monitor-r1-console.
The observer now supports an explicit test-only buffer width. The monitor
suite reproduces the prior 120-column buffer while keeping the narrow physical
viewport and font unchanged (scrollbars, no scaling). Both modes pass without
weakening title/input/result assertions. This does not certify a 39-column
NTMon layout. No product or shared-lib geometry change was made.

The sealed seven-file package is now published at O:/winnt. Its complete old
seven-file backup and before/after hash manifest are in
build/M0-T423/S11/publication-backup-r1. Guest and configuration were not
overwritten; system.ini, config.nt and autoexec.nt matched the candidate.
Published-path verification passed: direct MEM and COMMAND /c, Window MEM,
nested MEM and EDIT, all three pressure cases and five mouse cases, and the
three inherited WOW frontiers. Evidence prefixes under O:/winnt/Logs2 are
t423-s11-published-{console,window,pressure,mouse,wow}-r1. The publication
verification script exited zero and all seven hashes matched the sealed set.
S11 is closed for the approved mouse scope; stop for owner verification.
S12 remains subsequent, not active, and T423 remains open. Native continuity
and retirement are not included in this closure.
Native WIP and side-chat Queue/WOW changes are preserved, not silently reverted
or mixed into the production P. In particular, the four native branch source
files remain working-tree research, and side-chat proposals remain unstaged.

## Historical Admission (superseded by latest scope above)

Owner admits two repairs after S10 bounded cleanup closure f98825653;
e59e63d74 records the sole active S11 packet. T423 remains open. RDP pointer
escape is assigned to S12, not waived or included in S11 completion.
O:/winnt still contains the S9 publication. No S11 candidate is published.

## Mouse Source Findings

The owner reports delayed block-cursor motion in Window EDIT. Queue residence
time and total latency have not yet been measured; source ceilings alone do
not establish the only cause.

- Original OpenNT base/mvdm/softpc.new/host/src/nt_event.c reads up to five
  records per event-loop iteration and retains Sleep(10) after processing.
- Current mirror reads one raw record, expands a scan-less key into at most
  eight records, and retains that sleep. This limits raw-event consumption.
- Current src/ntvdm-exe/win32/console_compat.c::ReadConsoleInputExW also reads
  only one record, even when its caller offers a larger buffer. Restoring
  only the event-loop request count will therefore not restore batching.
- Preserve scan-less key expansion, ordering, history/returned-input origins,
  host-shortcut filtering and mouse button transitions. Do not change the
  original mouse IRQ delay or move host I/O back into the worker.

## Retirement Source Findings

src/ntkvm-exe/native_console_backend.c::run16_native_backend_members returns
retained ConPTY liveness, not attached-client count. session_service.c tests
this value before OpenNtBaseClientRetireFrontend, so retained idle ConPTY
prevents retirement. Do not remove the guard without replacing its contract.

Required owner predicate: no pending startup, active DOS task, unfinished
direct Win32 request or other attached Console user; then atomically close
admission and retire. Retain one ConPTY during CMD-to-DOS transitions.

Existing S9 evidence already distinguishes Job/process ancestry from Console
attachment and proves early release breaks future native I/O reuse. Reuse
that evidence, not a new speculative Job or observer implementation.

Public system API review:

- [GetConsoleProcessList](https://learn.microsoft.com/en-us/windows/console/getconsoleprocesslist)
  queries the caller's current Console, not an arbitrary HPCON. ntkvm is
  attached to its visible Console, so this is not the requested query.
- [ReleasePseudoConsole](https://learn.microsoft.com/en-us/windows/console/releasepseudoconsole)
  releases the keepalive reference; remaining clients may continue until
  the last disconnect, then the output ends. It is not a read-only query.
- [Microsoft's lifecycle change](https://github.com/microsoft/terminal/pull/14544)
  explains native client lifetime and the loss of subsequent launch support.
  The local S9 admission tests are stronger evidence than assuming a successful
  later CreateProcess also has working Console I/O.

An alternative final-drain protocol could close new frontend admissions once
broker/direct work is empty, release keepalive and preserve existing client
I/O until clean EOF. This changes the owner's required ordering: surviving
clients could no longer submit new work into that same frontend. It therefore
requires an explicit owner choice and is not selected or implemented here.
The owner selected the original ordering: keep accepting new requests while
any attached client remains. The early final-drain alternative is rejected.
Continue researching exact membership without adding a helper/Job, changing
frontend ownership or pretending process ancestry is Console attachment.

## Historical Candidate And Acceptance Record

### Candidate And SoftPC Comparison

Candidate restores five-record consuming batches through both adapter layers.
The 125-line existing scan-less normalizer is moved out of the mirror into
console_compat.c without algorithm changes. Forty stack slots accommodate
five eight-transition keys; validation precedes consumption, no carry queue.
Original key history, dispatch, Sleep(10), IRQ delay and guest remain unchanged.

tests/observation/verify-console-input-batch.ps1, BuildRoot
build/M0-T423/S11/input-r3, LogPath
O:/winnt/Logs2/t423-s11-input-batch-r3.log passes controlled-queue assertions:
1000 mouse records in 200 reads with exact order/buttons, mixed focus/key/
mouse ordering, scan-less make/break, pre-consumption capacity rejection and
Alt+Enter filtering. This is adapter mock evidence, not measured EDIT latency.
The first build omitted two no-frontend fixture bindings; the first runnable
test incorrectly compared INPUT_RECORD padding. Both failures are retained
under input-r1/r2; the corrected assertion compares all KEY_EVENT fields.
Formal x86 incremental ntvdm.exe build passes; log is input-r3/formal-build.log.

Read-only SoftPC comparison at 4a6aca27c0fd252cbfefa7b35ab74ca715e46a13:

- src/common/machine/machine.c::common_machine_drain_input consumes one
  queued event then requests another safe executor callback if input remains.
  It does not impose a fixed per-record Sleep(10) in that function.
- src/app-softpc/machine/driver.c sends relative input through
  softpc_machine_mouse_input to original base/keymouse/mouse.c::mouse_send.
  This is the InPort register/IRQ device route, not NTVDM's nt_mouse/INT33
  integration. Do not replace the NTVDM path merely to copy this architecture.
- src/lib/kvm-window/win32/component.c batches pending motion, flushing before
  button transitions. It is byte-identical to this project's imported file:
  SHA256 84B761EF9DDEE7C172A1716632BB96271062BCD2035B878D0B1ACF1DA0B7C1CE.
- Original OpenNT nt_event.c::nt_process_mouse merges same-button motion when
  its 32-entry buffer exceeds half capacity. The current relative FIFO does
  not; however the old working backup's mouse_delivery.c also used a FIFO.
  This is a further pressure point to measure, not proof of a newly introduced
  regression or permission to silently discard relative motion/buttons.

No SoftPC files, shared library or guest bytes were modified or imported.

The candidate worker was tested in build/M0-T423/S11/p (U:), not published
to O:/winnt. verify-window-mouse.ps1 passed all five real guest cases:
count (CURSOR-COUNT-PASS), video (CURSOR-VIDEO-PASS), text
(CURSOR-TEXT-PASS), move and retire (WINDOW-MOUSE-PASS). Reports are
O:/winnt/Logs2/t423-s11-mouse-r1-{count,video,text,move,retire}.txt
and their captured console output. These prove exercised cursor/input
behaviour, not measured EDIT burst latency or complete S11 acceptance.

### Burst Measurement And Remaining Gate

The input-r4 controlled-queue fixture additionally passes 1000 actual private
CONSOLE_INPUT_RELATIVE_MOUSE records in 200 reads, preserving signed deltas,
all button states and ENTER/LEAVE ordering byte-for-byte. Runtime log:
O:/winnt/Logs2/t423-s11-input-batch-r4.log.

The retained console_startup_observer.c now supports the optional test-only
MVDM_OBSERVER_MOUSE_BURST=200. On its private desktop it posts 200 alternating
relative moves through S7MOUSE.dll, followed by the existing move/down/up
sequence into the unchanged WMS7.COM authored probe. Production input, CCPU,
original IRQ and INT33 callback paths are used. It does not test physical Raw
Input/capture. The report's mouse-burst-to-target-exit-ms includes the guest's
fixed settle/exit delay; it is not a per-event latency measurement.

Procedure: input-r4/observer.exe <root>/run16.exe <root>/ <report>
--observation-timeout-ms 60000 O:/winnt/tests/WMS7.COM, with
MVDM_OBSERVER_PRIVATE_DESKTOP=1, MVDM_OBSERVER_MOUSE_HOOK=
O:/winnt/tests/S7MOUSE.dll and MVDM_OBSERVER_MOUSE_BURST=200.
All four runs exit zero and capture WINDOW-MOUSE-PASS:

| Package | Logs2 report prefix | Burst to exit |
| --- | --- | --- |
| Published S9 baseline R: | t423-s11-burst-baseline-r1 | 5703 ms |
| S11 candidate U: | t423-s11-burst-candidate-r1 | 3531 ms |
| S10 control S: | t423-s11-burst-control-r2 | 5750 ms |
| S11 candidate U: | t423-s11-burst-candidate-r2 | 3688 ms |

The S10/S11 pair differs only in ntvdm.exe; all other six artifact hashes
match. Worker hashes are respectively
7FCD35F6F4E08B21584B50E6F33817E154E409DF217D1DF0328754208D90FA6A and
6AAA34023CBF201980BB022F39033F4572E844974520DEB9DD0A47FC3C876EE1.
The single-worker-variable comparison reduces this bounded burst completion
time by 2062 ms. This supports the input-throttling diagnosis, not a claim
that every latency source or sustained overload is solved.

Window native-zero still fails strict contiguous text overlap under the
unchanged frontend: t423-s11-window-before-r1-native-zero.txt. Its target
exits zero, but the 80-column snapshot has the resumed DOS prompt whereas
the subsequent 120-column snapshot loses it. Missing was not reached in this
fail-fast run. This gate remains failed; no snapshot assertion is relaxed.

At the preceding checkpoint, actual membership remained unimplemented under the no-observer constraint.
GetConsoleProcessList queries the calling process's own Console; it accepts
no HPCON. ReleasePseudoConsole changes keepalive/admission, and early release
was owner-rejected for the permanently reused backend. A short-lived query process would require an explicit
exception; the pending question is superseded by the approved branch design below. No such process,
Job, private Console protocol or lifetime-policy substitution was added.

- [ ] Measure burst queue latency and preserve key/mouse/control event order.
- [ ] Restore bounded batching through both input layers; no fake clicks or
  missing button releases; test scan-less keys and native handoff as well.
- [ ] Establish an authorized actual-use retirement mechanism, not HPCON
  liveness, direct-target lifetime or process-tree inference.
- [ ] COMMAND -> CMD -> exit -> new DOS input -> exit retires the frontend.
- [ ] A still-attached descendant retains its terminal; its final departure
  retires it. Detached surviving processes do not count as attached clients.
- [ ] Pending/new admissions race safely with retirement; no lost request.
- [ ] Resolve S10 Window native-zero/missing text-continuity failures without
  weakening snapshot assertions (15/17 is not a full pass).
- [ ] Full affected x86 build, existing regression and headless WOW frontiers,
  coherent validated publication, documentation checks, commit and push.

Concurrent queue/WOW proposal planning remains separate uncommitted work at
its author's explicit working-tree-only instruction. No blanket clean-tree
claim is made. This ledger records research and candidate verification,
not completed S11 acceptance or a published repair claim.

### Owner-Added Mouse Scheduling Repair

Owner authorizes including the concurrent nt_event.c changes in S11 review,
build, tests and eventual commit/push. They remove the old per-batch Sleep(10)
packing delay and swap the wait-array priority plus both corresponding status
branches. hConsoleSuspend remains the original auto-reset event; the original
nt_process_suspend_event / hConsoleWaitStall / hConsoleWait handshake, keyboard
hardware backpressure, alertable idle wait, mouse IRQ/EOI and callbacks remain.
Source review therefore finds no new suspension state machine. Continuous-input
suspension and downstream FIFO pressure still require runtime evidence.

Build root mouse-owner-r1 contains the successful incremental x86 formal link
and input-batch fixture. Logs2 t423-s11-mouse-owner-batch-r1.log passes ordinary
and private relative mouse ordering/button/bounds, key expansion and shortcuts.
The combined candidate lives only under build/M0-T423/S11/mouse-owner-r1/p,
mapped to Q:. Its worker SHA256 is
1EF894DB4A713BB9A215FF3B093F0E4BE886DD2A6628A1B3481F31BB9BF6EEC5;
ntkvm SHA256 is
41E6B87652DE04D06FEFF8DAC9A79CEDD82360819C4463BEC2D5EDF4E90FC553.
Earlier X:/Y: mapping attempts failed before a guest ran; they are not passes
and existing mappings were not removed.

verify-window-mouse.ps1, input-r4 observer, PackageRoot Q:, Logs2 prefix
t423-s11-mouse-owner-r3 passes all five real guest cases (count/video/text/
move/retire). The same combined candidate passes all Console17 cases through
Verify-CommandExitStatus.ps1 with OrdinaryFrontend and GuestFixturePath
build/M0-T423/S11/mouse-owner-r1/G7.COM; Logs2 prefix
t423-s11-mouse-owner-console-r1. These results supersede reuse of earlier mouse
candidate passes for these specific checks, not the still-failed Window chain.

A 200-record private-desktop Window burst, input-r4 observer and WMS7.COM,
captures WINDOW-MOUSE-PASS and exit zero in Logs2
t423-s11-mouse-owner-burst-r1.txt. Its 3453 ms includes guest settling/exit
delay and changes both worker and frontend from the earlier timing pair; it
cannot isolate the sleep removal's latency improvement. The observer now also
allows 1000 records to test pressure beyond the 256-sample relative FIFO.
The FIFO currently rejects overflow and the caller invokes DisplayErrorTerm;
do not certify unrestricted burst handling merely from the 200-record pass.

The new 1000-record stress test fails on both the combined candidate Q: and
the preserved batch-only candidate U: (which still has Sleep(10)). Reports:
Logs2 t423-s11-mouse-owner-stress-r1 and
t423-s11-mouse-batched-control-stress-r1. Both report timeout, no guest PASS;
the hook reports all 1000 records posted. The combined worker's captured
stack resolves, after image-base relocation, to mvdm_softpc_mouse_receive,
DisplayErrorTerm and nt_event_loop. This is a real error-path failure, not
merely an observer latency metric. The 256-entry queue overflow is a concrete
source candidate; the actual error argument still needs capture before claiming
that exact cause. The control proves the failure is not exclusive to the new
sleep removal; it does not isolate timing impact because its frontend differs.
Keep this strict stress regression and resolve it before mouse acceptance.
No unverified candidate has been published or committed as a completed P.

### Approved Per-Branch Replacement And First Runtime Results

The owner approves separate PTYs for independent native branches, retaining
outer PTYs while DOS or inner native branches execute. Ordinary native children
inherit their actual Console. One ntkvm still owns all presentation and selects
one interactive endpoint. This replaces permanent session-wide reuse; it does
not admit a helper, observer, Job or execution scheduler.

The public [ReleasePseudoConsole contract](https://learn.microsoft.com/en-us/windows/console/releasepseudoconsole)
relinquishes keepalive so Windows closes the session after all attached clients
depart. The minimum OS is Windows 11 24H2 / Server 2025. The candidate checks API
availability and rejects post-release independent attachment. The existing
output reader drains to EOF before resource disposal. Client completion and
frontend admission remain independent. The original OpenNT shared Console
does not supply this modern PTY resource policy; its process/task results and
worker handoff are retained, with the new resource binding solely in ntkvm.

Production candidate changes are native_conpty release admission,
native_console_backend private cancellation, and native_console_frontend's
branch-owned backend/view list. Closing one branch no longer signals the
frontend-wide cancellation event. Retained outer backends still drain replies.
The design does not claim arbitrary sibling foreground selection is proved.

Reproducible resource entrypoint: tests/observation/verify-conpty-launch.ps1,
MSVC x86 /MT environment, BuildRoot build/M0-T423/S11/branch-resource-r3,
LogPath O:/winnt/Logs2/t423-s11-branch-resource-r3.log. All stream-mask,
negative-attachment, lifecycle/input, release/admission and new independent
branch cases pass. conpty_branch_test.c proves A survives B, B's inherited child
outlives its direct parent and produces final output before EOF, isolated A/B
input/output, and a live detached process does not count as Console occupancy.
These are real Windows resource tests, not guest or full product acceptance.

The incremental x86 ntkvm build passes at branch-r1/formal-build.log and
branch-r2/formal-build.log. Candidates V: and W: map only to their respective
build/M0-T423/S11/branch-r{1,2}/p directories. O:/winnt remains untouched.
Verify-CommandExitStatus.ps1 with private desktop, Window input, OrdinaryFrontend
and Cases native-zero,missing,mem passes for branch-r1; Logs2 prefix
t423-s11-branch-window-r1. This improves those focused checks, not the whole
Window matrix.

The strict frontend-chain-a run fails contiguous snapshot overlap for both
branch-r1 and branch-r2 (Logs2 t423-s11-branch-nesting-r{1,2}); fail-fast means
frontend-chain-b was not run. In r2 the inner CMD marker and MEM output appear,
but returning outer CMD overwrites earlier rows. Independent ConPTYs retain
their own native screen/cursor; updating the local parser alone does not update
the outer Windows Console's cursor. No snapshot assertion is relaxed. The
owner has been asked to choose independent-screen restoration versus retained
single-screen continuity; neither is silently declared equivalent.

The actual concurrent native-output/DOS-screen-transaction fixture passes:
input-r4/observer.exe observes the formal native-console-frontend-test.exe
with --concurrent on a private desktop. Logs2 t423-s11-branch-component-r2
captures NATIVE-BASE, DOS-INTERVAL and NATIVE-LATE, exit zero and the explicit
PASS witness. The candidate retains an ended selected branch until DOS has
consumed its final frame. This local test does not resolve the nested screen
contract above. S11 remains open; no production P or publication is claimed.

Further verification: branch-r2 passes all 17 Console guest routes through
Verify-CommandExitStatus.ps1, OrdinaryFrontend, private desktop, PackageRoot W:,
GuestFixturePath build/M0-T423/S11/branch-r2/G7.COM, Logs2 prefix
t423-s11-branch-console-r3. The candidate ntkvm SHA256 is
AD76C66B4BDF7A826B6C56F9BE9CF18C1B02090A71346A970C146B1BE4CC111D.
The initial attempted invocation without GuestFixturePath was rejected before
starting a guest and is not a test failure or pass.

The subsequent backend error-path correction returns startup handshake errors
instead of signaling stop and falsely returning success, initializes target
on unsupported API failure, and captures DuplicateHandle failure immediately.
branch-r3/formal-build.log records the successful incremental x86 build;
ntkvm SHA256 41E6B87652DE04D06FEFF8DAC9A79CEDD82360819C4463BEC2D5EDF4E90FC553.
Logs2 t423-s11-branch-component-r3 observes the current formal frontend fixture
on a private desktop: exit zero, ordinary CAF/X, native mouse button/focus,
typeahead ordering, direct results 37/0/259, cancellation and bounded cleanup
all pass. Console17 belongs to r2, not a silently relabelled r3 full regression.
Documentation governance and git diff --check pass; line-ending notices are
not whitespace errors. Side-chat WIP remains untouched and uncommitted.

### Continuous-Screen Decision And Live Cursor Probe

The owner has now selected continuous single-screen output and rejected
independent-screen restoration. The earlier question is resolved. Nested
return must preserve intervening output and correctly continue positioned
output/scrolling; independent resource lifetime does not authorize page switching.

Question: can the existing public ConPTY channel synchronize a retained outer
Console's real cursor after intervening DOS/inner-PTY output, without a helper,
launcher Console calls, private interfaces or modified Windows components?

Inputs: current production native_conpty.c/native_console_launch.c; host
conhost.exe file/product version 10.0.26100.1; new test-only
tests/observation/conpty_cursor_handoff_test.c. No Microsoft code was imported.
Microsoft's [CreatePseudoConsole documentation](https://learn.microsoft.com/en-us/windows/console/createpseudoconsole)
describes startup cursor inheritance; its
[ResizePseudoConsole documentation](https://learn.microsoft.com/en-us/windows/console/resizepseudoconsole)
does not define a screen/cursor replacement operation. Current upstream
[InputStateMachineEngine](https://github.com/microsoft/terminal/blob/main/src/terminal/parser/InputStateMachineEngine.cpp)
captures a cursor reply only when a request is pending; otherwise it is input.
The upstream [screen-info synchronization path](https://github.com/microsoft/terminal/blob/main/src/host/screenInfo.cpp)
includes newer deferred resynchronization machinery. That moving upstream
source is comparison evidence, not proof that this installed host has it.

Procedure: build x86 /MT with the normal VS2022 environment, then run
tests/observation/verify-conpty-launch.ps1 with BuildRoot
build/M0-T423/S11/cursor-resource-r2 and LogPath
O:/winnt/Logs2/t423-s11-cursor-resource-r2.log. The test launches a disposable
real Console client inside each PTY, establishes cursor (2,3), and attempts
to obtain (10,12). Actual coordinates come from the client's own
GetConsoleScreenBufferInfo, not our terminal model. The controller answers
genuine DSR requests even during the final client query. Cases use independent
PTYs, no physical desktop manipulation and no production helper. All existing
resource/launch/lifecycle/branch cases also pass in this run.

| Case | Actual client cursor (zero-based) | Query/reply count | Input pending | Finding |
| --- | --- | --- | --- | --- |
| Baseline | 2,3 | 0/0 | No | Negative control correct |
| Unsolicited CPR | 2,3 | 0/0 | Yes | Not a cursor setter; pollutes input |
| Resize to same dimensions | 2,3 | 0/0 | No | No resynchronization on this host |
| Resize to different height | 2,3 | 0/0 | Yes | No resynchronization; pending input may be resize notification |
| Initial cursor inheritance | 10,12 | 1/1 | No | Positive control works |

The first cursor-handoff-r1 run did not service queries during the final client
screen-info call; r2 corrects this observation gap and reproduces the results.
Probe success means controls/observation completed, not that runtime handoff
synchronization passed. Neither candidate nor O:/winnt was replaced.

Conclusion: do not inject unsolicited CPR, fake resize the user's terminal,
or label local parser imports as remote synchronization. A presentation-only
coordinate offset could hide one old-cursor write, but does not update the
native program's screen reads, absolute positioning or full-screen redraws;
it is not established as a general shared-screen repair. The original shared
Console did not require copies between independent native Console buffers.

The smallest alternative to investigate is an authenticated, bounded return
handoff performed by the already-attached run16 that launched DOS from an
outer native Console: ntkvm supplies the canonical screen/cursor; that client
applies them before returning to its waiting native parent. This adds no
helper, input pump or renderer, but it DOES change the current explicit ban
on launcher Console I/O. It is a proposal only, not approved or implemented.
Keep the continuous-output gate open rather than silently making that change.

### Owner-Admitted Frontend Composition Prototype

The owner instead admits a bounded composition prototype: keep independent
ConPTY parsers, let ntkvm own one visible canvas and map new native updates
onto the canvas after DOS handoff. No helper or launcher Console I/O is
authorized. Do not identify programs/prompts or guess application intent to
make tests pass. This supersedes the pending launcher-exception suggestion,
not the requirement for continuous output or the open production gate.

Recovery disposition: existing native_terminal/libvterm parsing and the
original DOS I/O boundary are reused as the source contracts. OpenNT's shared
Console has no independent-ConPTY-to-shared-canvas compositor to import; its
real shared buffer avoided this split. The owner specifically admits a small
test-only last-resort compositor to evaluate this modern presentation boundary.
No mirror, launcher, guest, shared library or Windows component was changed.
The test uses the pinned, unmodified libvterm parser/screen callbacks, rather
than implementing another VT parser or rewriting application output by name.

Files: tests/observation/native_projection_test.c (test-only source-damage,
cursor and full-width-scroll mapping); conpty_screen_contract_test.c (real
Windows native API controls); verify-native-projection.ps1 (reproducible
x86 /MT build/run). None is a production provider or formal graph input.

The bounded canvas has 24 columns, 8 viewport rows and 128 history rows solely
for deterministic test vectors; these are not product dimensions or new lib
requirements. Native states remain independent. At a handoff, the prototype
anchors subsequent native damage at the common cursor without restoring old
cells. Full-width native scrolling advances the common history origin.

First build projection-r1 hit the known Windows `small` macro conflict in the
public vterm header; a local push/undef/pop fixes the fixture declaration only.
projection-r2 exposed a real missing common-cursor adjustment when a native
scroll keeps its cursor at the same last row. projection-r3 adds that update
and retains the failed logs; no source-library mutation or assertion relaxation.

Results in Logs2 t423-s11-projection-r3.log: 115 checks, zero fixture-control
failures, four explicit limitations, product-acceptance=no. Passing model
cases include outer native -> DOS-shaped output -> inner native -> DOS ->
outer native return, twenty lines of scrolling with preserved preceding text,
backspace editing, erase-line/history-style redraw and long-line wrapping.
These are terminal-model tests, not real COMMAND/EDIT/MEM acceptance.
Limitations remain fixed common-screen positioning, the mapped clear region,
negative origin after DOS clears/homes, and native screen reads missing DOS
content. Controls confirming a limitation never mean the capability passed.

The decisive real-host counterexample is retained in
Logs2 t423-s11-screen-contract-r3.log. Four independent native clients use the
same executable/command line and production ConPTY resource implementation.
Each initially writes NATIVE and has real cursor (0,1). Two operation choices:
read the current cursor and write AFTER there, or explicitly address (0,1)
and write AFTER. For the two shared-buffer controls only, the fixture first
writes DOS-ONE/DOS-TWO through the actual Windows Console, advancing its
cursor to (0,3); this is a controlled second-writer simulation, not a claim
of having executed a DOS guest. All four clients report their actual final
cursor through their process result; the observer drains native output to EOF.

| Buffer case | Operation | Final real cursor | Output bytes |
| --- | --- | --- | --- |
| Isolated ConPTY | Continue at queried cursor | 5,1 | 146 |
| Isolated ConPTY | Fixed row 1 | 5,1 | 146 |
| Shared buffer after second writer | Continue at queried cursor | 5,3 | 164 |
| Shared buffer after second writer | Fixed row 1 | 5,1 | 175 |

The two isolated outputs are byte-for-byte identical, not merely equal final
screens. They require different placement to reproduce their respective
shared-buffer controls. An output-only compositor cannot recover the native
API intent that was lost before the VT stream was emitted. This disproves
universal shared-Console equivalence from output alone; it does not disprove
limited stream-continuity composition or define every full-screen program as
unsupported. Native buffer reads were already an admitted known boundary;
this witness additionally demonstrates their effect on subsequent output.

The agreed no-application-heuristics stop criterion is reached for a general
replacement. Do not promote the prototype or expand coordinate special cases.
Real nested guest/NTMON/mouse integration has not been run for this rejected
prototype and is not a pass. Preserve all S11 mouse/branch work; the T and S
stay open, and O:/winnt stays the verified S9 package. A changed compatibility
contract or an explicit real-buffer synchronization boundary needs an owner
decision, not a fabricated frontend-only solution.

Reproduction: verify-native-projection.ps1 with BuildRoot
build/M0-T423/S11/projection-r4 and LogPath
O:/winnt/Logs2/t423-s11-projection-r4.log rebuilt the pinned library and both
fixtures under MSVC x86 /MT and reproduced all 115 controls, four limitations
and the byte-identical native counterexample. Exit zero certifies the research
controls, not the rejected capability. No product binary was rebuilt/published
by this prototype run; no production P or S closure is claimed.
