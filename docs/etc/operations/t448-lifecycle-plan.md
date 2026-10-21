# T448 lifecycle unification working design

## Authority and status

### S3 initialization capability boundary

Protocol 63 is delivered in P19. The protocol-64 cleanup removes the obsolete
BrokerProcess, WorkerShutdownEvent and WorkerConsoleCloseEvent wire operations
and both redundant worker-event service getters. Connect remains their sole
authenticated export point; existing owned-handle client APIs only duplicate
the connection's local capabilities. Raw-wire tests obtain the service handle
from Connect and verify its restricted rights there. Provider lifecycle tests
use ConnectionControls without retaining parallel provider policy.

Connect must not report success before supplying the service lifetime and
role-appropriate close capabilities. NTSRV derives role from its authenticated
reservation/watch, never a client-selected flag. Non-workers receive no worker
events; WOW receives worker-close only; native launcher controls remain no-op
in that consumer. All exports have wait-only rights (service process also has
query-limited rights). Partial export failure closes exported duplicates and
runs connection rundown before returning failure. There is no reconnect.

The shared RPC client owns these handles until disconnect. Worker lifecycle
owners borrow them without allocation or a second RPC; existing owned-handle
APIs remain local duplicates for I/O consumers. This removes the admission-to-
instruction acquisition window. Original-host callbacks still must be deferred
until original initialization is complete; that remaining change and startup
failure cleanup are part of the same S3 group, not a separate new capability.

Required proof: authenticated role selection, denied mutation rights, partial
duplicate rollback, latched instruction delivery, raw-wire/version mismatch,
real Connect and owner startup failure/service-loss behavior. No guest changes.

The owner's 2026-10-21 correction in the linked proposal supersedes this
document's earlier blanket ten-second control/finalization policy. r32 is
withdrawn from publication. Audit existing timeout owners, their actual purpose
and introduction before removing arbitrary client deadlines, timeout latches,
service-side expiry policies and tests that exist only to enforce those policies.
Retain real error/service-loss cleanup and normal ownership ordering. Existing
RPC async completion may remain where needed for service-loss cancellation;
it is not itself a timeout policy. Test-harness safety limits, nonblocking
liveness probes and presentation pacing are not product operation deadlines.

Owner approved the original four-stage division and subsequently inserted a
polling-cleanup S4; original integrated acceptance becomes S5. Current admits
S3 after the bounded S2 implementation/review conclusion. S3 P1 at d0814cd4c
delivers the retained S2 work and close-arbitration checkpoint; remaining S3
exit permission and initialization/fault work is not closed. S1 design was delivered
at c838c332c. This plan elaborates the approved architecture; it does not claim
runtime implementation or waive the control-only-through-NTSRV boundary.
Baseline: 7917ef25a, following the published T447 checkpoint 4d4ef4d47.
The [source audit](../evidence/m0-t448-s1-lifecycle-audit.md) distinguishes facts,
source facts and approved decisions. The [S2 ledger](../evidence/m0-t448-s2-lifecycle-implementation.md)
tracks candidate implementation and verification. No guest changes or rebuilds.

## Delivery groups

| Stage | Complete chain | Exit condition |
| --- | --- | --- |
| S1 | All role ownership, transitions, failure ordering and original-owner audit | Finite remediation/test matrix; service-loss boundary explicitly settled; reviewed design delivery, not runtime acceptance |
| S2 | Admission, execution result, parent/frontend handoff and launcher final result | No launcher peer-liveness decision or autonomous frontend parking; lifecycle-only WOW binding; coherent package and positive/negative normal-path evidence |
| S3 | Console/worker/WOW-task close, concurrent completion and service loss | Uniform internal/external management transaction; fault cleanup; real pre-Enter echo and failure-injection evidence; coherent delivery |
| S4 | Autonomous-code polling inventory and cleanup | Map every polling loop to owner/purpose/notification alternative; remove unnecessary polling without new transport/framework; prove no lost wakeup, idle busy loop or shutdown hang; record justified retained polling and source/test delta; build/test/full publication/commit/push |
| S5 | Entire matrix, source cleanup and closure (formerly S4) | No remaining control bypass, resource/return regression or unaccounted mirror diff; full build/incremental/publication, reviewed push and owner acceptance |

S2 must include enough lifecycle dispatch separation for normal WOW/native GUI
operation; S3 adds close/fault behavior, not a second competing dispatcher.
Do not publish half of a wire change. Common protocol/IDL stays declaration-only;
NTSRV-private lifecycle policy remains in its existing service units. Workers
share dispatch/cancellation mechanisms, never kind-specific lifecycle policy.

## Three independent facts

1. Task outcome: source-owned execution result, startup failure or worker loss.
2. I/O ownership: authenticated frontend route and its current lease generation.
3. Component lifetime: service-granted admission, residency and release.

Task completion cannot alone prove Console restoration or require worker exit.
Do not clone original DOS/WOW task records into a second task scheduler. Extend
the existing direct-request envelope with finalization state and preserve the
original result producer. Management projections remain read-only projections.

## Protocol and identity

Reuse authenticated connection generations, service instance and request
identity. Add an operation sequence/phase only where the existing identity cannot
distinguish repeated use of a route. Acknowledge an exact operation and phase,
not a PID or an unqualified signalled event. Signal notifications only after
committing state under the existing service lock; readers query authoritative
state. Hold no service lock across OS waits, RPC callbacks or component cleanup.
Pin records/handles across unlocked actions, then revalidate operation identity.

Keep execution result, infrastructure error and final release disposition
separate. A failed handoff does not fabricate successful task completion or
discard a real result. A service-issued final envelope gives the launcher its
actual return status after required handoff. A timeout response is not that
envelope. Retain the terminal result until consumer acknowledgement or verified
caller rundown; repeated queries observe the same result without resubmission.

Submission is single-attempt: failure, including an ambiguous lost reply,
returns submission failure without replay. Missing-service bootstrap is a
separate pre-submission operation with its one absolute five-second budget.
NTSRV must retire abandoned request resources independently of the requester;
an ambiguous reply is not proof that the target never started and does not
authorize killing an already committed GUI target.
Win16 and native GUI default completion is successful startup, not task exit.
The service releases their launcher on startup confirmation; subsequent task
exit does not release that already departed launcher again. Explicit --wait
retains its existing completion contract without GUI management rights.

## Normal operation

Run16 retains search, classification and CLI behavior. Before authenticated
admission it may start NTSRV, with a single absolute five-second budget including
connect operations. Service singleton admission and protocol-version rejection
remain authoritative; do not start replacements for incompatible peers.

After admission, NTSRV obtains/reserves the carrier and, for CUI only, the frontend.
It verifies native target machine/subsystem before execution commitment. Run16
does not retain worker/frontend process handles as liveness or exit criteria.
Remove its parent-resume trigger: the service joins source completion with the
already recorded execution-parent relation and grants the correct route.

NTCON reports physical attachment, quiescence, restoration and faults; it does
not count task usage to choose parking. The service orders park/reuse/retire.
Normal root return may park a live NTCON; nested return resumes the recorded
parent execution rather than returning the outer Console. Native GUI and WOW
never enter character frontend setup. GUI launcher reports delivery, receives
release and exits; the GUI target remains independently running/observed.

## Close transaction

The service owns one close operation per affected scope. NTMON submits and gets
an acceptance/operation identity; its continued existence is not required to
advance the operation. An external NTMON can observe progress using the same
identity. No internal-versus-external caller branch is allowed.

| Phase | Service action and required evidence |
| --- | --- |
| Admitted | Validate key/permissions, freeze new admission and retain affected resources |
| Quiescing | Revoke I/O grants; NTCON stops/cancels input, freezes presentation and rejects stale frames; acknowledge actual quiescence |
| Closing carriers | Dispatch precise Console-close or worker-close; original/local owners clean up and report; verify carrier exit where required |
| Restoring | NTCON restores canonical active buffer and input state after old writers/readers cannot regain access; report failure independently for each operation |
| Final | Service latches results, releases launchers and orders frontend retirement or retention; free records only after references/waits are retired |

Phases advance on their actual acknowledgements, operation failures and process
exit observations, without a transaction deadline or timed-out state. Duplicate
requests join the accepted operation rather than restarting it. Neither elapsed
time nor a late acknowledgement authorizes reopening admission or reporting
successful Console restoration. The live service retains responsibility for
accepted work and unfinished handoff; a confirmed service loss permits the
role's local cleanup without a peer recovery fence.

The only project-owned five-second policies are newly started NTSRV bootstrap,
empty-service retirement and Console-close handler grace. Original internal
waits retain their source semantics. There is no workerless-frontend expiry,
ordinary RPC deadline, final-disposition timeout or automatic retry/force-kill
escalation. The explicitly authorized Console-close forced exit remains at
its named owner, not a generic close-transaction fallback.

This table is the Console-close sequence, not a universal teardown recipe.
NTSRV selects phases from the actual scope: WOW task close retains its worker;
WOW worker close has no frontend phases; GUI launcher commands are inapplicable
where specified; ordinary completion need not close a carrier. Worker-base
dispatches commands without choosing any of these policies.

Normal task completion racing management close preserves already committed
task results. Worker-close does not become Console-close. A subsequently
accepted Console-close can encompass an existing worker-close without sending
duplicate destructive actions: wait for its actual teardown, then finish the
remaining frontend work. The service owns the ordering; event-index priority
inside input_watch is not policy. Reject stale generation/phase acknowledgements.

## Role-specific commands

Spontaneous whole-worker exit intent uses the same service close arbitration
as management, authenticated to the reporting worker's connection generation.
Acceptance is not teardown permission. The existing shutdown/Console-close
instruction is the permission: if Console-close was accepted first, reporting
a local failure must not bypass frontend quiescence, restart its transaction or
signal a competing shutdown. No extra worker registry, peer fence or retry
protocol is needed. A rejected/stale report cannot authorize local teardown.
This request is for managed workers; transient native GUI launchers retain
their distinct startup-release contract. Component wiring and original-owner
cleanup synchronization must be verified before declaring this group complete.

DOS and native CUI support Console-close and worker-close, not individual task
termination. WOW supports worker-close and source-shaped individual task close,
not Console-close. Native GUI launcher consumes generic notifications and
reports a non-applicable result without acquiring Console resources. The service
does not offer management actions for native GUI targets. Worker-base dispatches
the service instruction; each executable implements the semantics.

Keep original CntrlHandler/TerminateVDM and WOW task cleanup at their source
owners. Original spontaneous termination/fatal exits must be audited separately
from management callbacks: reporting ExitVDM is not proof that the subsequent
ExitProcess already has service permission. Do not block on RPC while holding
guest/USER/service locks or while teardown has destroyed required client state.

## Service loss: resolved facts and explicit boundary decision

A single RPC timeout is not proof of service death. Distinguish authenticated
process death, definitive context/session loss, request rejection and operation
timeout. No reconnect/replay after admission. Replace the shared unconditional
TerminateProcess watcher with a local loss notification to the role's cleanup
owner; transport code cannot implement frontend restoration or worker policy.

There is a necessary ordering conflict, not an unexplored API choice:
after the service dies, Run16 cannot know whether NTCON has restored the Console
if it is forbidden to receive any NTCON-produced completion information.
Both processes merely observing service death provides no ordering. Waiting a
fixed ten seconds cannot prove restoration: NTCON can be suspended longer.
The visible mode bits alone do not prove cancellation of an old input read or
selection of the final output buffer. A forced NTCON crash cannot certify cleanup.

Owner explicitly rejected the recovery-fence exception. The selected contract
is strict service-only control: after confirmed admitted-session loss, Run16
cleans up its own resources and returns the applicable original error to its
parent without waiting for NTCON or a worker. NTCON and workers independently
perform their own loss cleanup. No peer event, peer-process wait, helper,
replacement service, synthetic Enter or sleep-based ordering is permitted.
Run16 failure return does not certify Console restoration. Ordering guarantees
while NTSRV is alive remain mandatory; do not weaken those normal paths.

Error provenance is explicit: original base/ntdll/csrutil.c CsrClientCallServer
copies a failed NtRequestWaitReplyPort status into ReturnValue and returns it.
Original base/ntos/rtl/generr.c maps STATUS_PORT_DISCONNECTED to
ERROR_INVALID_HANDLE (6). Represent confirmed service-session disconnection at
the NTSTATUS compatibility seam with that disconnected-port status and preserve
its Win32 conversion for the launcher failure return. OpenNT has no product
Run16 CLI: returning that API error as the launcher exit code is this product's
binding, not an original Run16 implementation. Do not map every timeout,
rejection, startup error or task failure to 6. Preserve those distinct results.

S2 must deliver basic loss and service-directed finalization with its normal
waiting protocol; S3 extends the full phase/concurrency/failure matrix.
The pre-refactor shared watcher hard-exited with RPC_S_SERVER_UNAVAILABLE (1722),
and launch-style rpc_failure discarded its input into STATUS_UNSUCCESSFUL;
neither established the required original error propagation. Some original
BaseCheckForVDM failure exits do not set LastError, so stale GetLastError is
not evidence of the failure cause. S2 must carry the explicit transport status
through the failure result, rather than infer it from stale thread state.

## Finite remediation and tests

| ID | Repair owner / stage | Required acceptance (planned, not executed here) |
| --- | --- | --- |
| L01 | Run16 admission / S2 | Concurrent absent-service startup, incompatible version, real total deadline, no post-admission replay |
| L02 | NTSRV completion + launcher / S2 | Normal and failed native/DOS result; nested parent restore; duplicate result query; no peer wait |
| L03 | NTSRV frontend policy + NTCON / S2 | Normal return keeps frontend reusable; creator death cannot independently park; two simultaneous launchers |
| L04 | worker-base dispatch + NTVDM bootstrap / S2-S3 | WOW receives worker-close without character initialization; GUI ignores inapplicable command; no kind policy in worker-base |
| L05 | NTSRV close operation / S3 | Internal/external deletion, caller dies after acceptance, normal-result/close race, one cleanup, event-driven acknowledgement and actual exit |
| L06 | NTCON quiesce/restore / S3 | Delayed frame/read/EOF, each restoration error, join failure, no false acknowledgement, first character before Enter |
| L07 | shared RPC notification + role cleanup / S2-S3 | Remove S2's finalization timeout; test actual loss at each phase and original status/error propagation; disconnected Run16 returns even if NTCON is stalled or absent, with no peer wait; no unconditional watcher kill |
| L08 | original NTVDM/WOW exit boundary / S3 | Natural/fatal/management paths retain original cleanup, no wait under guest locks, source-proven mirror exception if needed |
| L09 | GUI launch and failure / S2-S3 | Startup-only/--wait, suspension/binding failure rollback, launcher departure; GUI target remains alive on service loss |
| L10 | record/attachment rundown / S3 and S5 | PID/handle reuse, late command/ack, abandoned reservation, released wait callbacks, no leaked resources |

S5 repeats real conhost and terminal paths with CMD, COMMAND and direct NTMON,
CAF before/after NTMON, internal/external close, normal exit and no-CAF controls.
Use existing tests/observation/verify-console-caf-self-delete.ps1 and strengthen
service fixtures in tests/adapter-basesrv/base_service_reservation_test.c;
frontend_scope_lifetime_test and console_channel_lifetime_test cover local
failure ordering. Add explicit deterministic phase/cancellation assertions to
production-linked fixtures rather than only checking exit status or screenshots.
Preserve T447 COMMAND/WRITE/SOL non-passes; do not count them as restored here.

## Mirror and delivery budget

### Communication unification within the lifecycle group

Owner requires reuse of the existing RPC control and worker/frontend I/O
mechanisms, with no third communication mechanism. NTSRV remains the only
control authority. Audit existing lifecycle callers as well as new code;
consolidate bounded local RPC on the existing asynchronous completion/cancel
mechanism rather than RPC_C_OPT_CALL_TIMEOUT (ignored by ncalrpc). Cover
startup Connect/BrokerProcess, final disposition, worker exit requests,
frontend acknowledgements and management close. Intentional blocking command
receive and normal task execution are not ten-second operations. Maintain
authenticated identity, cancellation lifetime, no replay, and service-owned
late completion. Direct worker/NTCON pipes remain I/O only. Results and the
finite remaining inventory belong in the S3 implementation ledger; this is
not a claim that every current call has been corrected.

The current frontend retirement group removes unused ReturnFrontendConsole /
RetireFrontend methods rather than preserving two alternate controllers.
Its actual-pump sweep additionally bounds join candidate/decision, lease-ready,
request retrieval and I/O-disconnected acknowledgements on the same RPC
mechanism. Timeout latching prevents a cleanup RPC after the first terminal
failure. This does not yet certify startup registration / state-event
attachment / AttachFrontendRequest, ordinary Disconnect, remaining worker
control calls or UNKNOWN_IF diagnostic querying; those finite call families
remain in the S3 communication inventory. No retries or transport substitute
are permitted to hide a stalled control call.

The obsolete client-owned AcquireFrontendRoot / CancelFrontendRootReservation
wire entrypoints are removed in the next candidate. StartFrontend is the
production request: NTSRV alone invokes acquisition and rollback internally.
Retain those service algorithms and their reservation tests; remove only the
unused remote/client facade and its tests. This narrows the control authority
without replacing the transport or creating a second startup implementation.

The next bounded registration/attachment group converts RegisterFrontendRoot,
RegisterFrontendLease, ReportConsoleMembers and AttachFrontendRequest to the
same asynchronous RPC completion/cancellation path. Keep sampled member storage
alive until completion drains; publish the attachment generation only on final
success. A latched registration/attachment timeout prevents a subsequent startup
result or cleanup RPC, preserving the original failure and local teardown.
Service policy/authentication stays unchanged. Actual paused-service cases cover
each operation and immediate failed follow-up calls; normal startup, forged
identity, I/O attachment, rollback and runtime remain mandatory. StartFrontend,
state-event acquisition, ordinary Disconnect and UNKNOWN_IF diagnosis remain
separate explicitly open entries, not implicitly solved by these four methods.

The subsequent frontend group bounds StartFrontend, FrontendStateChanged and
FrontendStartupResult on the same RPC helper. The executable facade applies
terminal-failure latching before issuance and after completion, closing any
returned attachments if a successful reply races confirmed service death.
No task-running wait acquires a ten-second lifetime. NTSRV still owns startup
reservation completion/rollback; client cancellation is not a second owner.
Raw authentication tests reuse the common frontend client for the changed
calls. Success/retirement remain real-service tests; issuance exceptions retain
exact attachment cleanup and zero handle-delta assertions.

The ordinary Disconnect cleanup also uses the existing asynchronous RPC
completion/cancellation helper with a ten-second deadline and service-loss
stop. It retains the void cleanup contract: remote cleanup failure cannot
replace an already determined task result. Context/binding storage remains
live through cancellation completion; repeated local cleanup is harmless.
This does not resolve UNKNOWN_IF diagnosis or remaining worker controls.

The notification-handle group unifies WorkerFrontendCapability,
WorkerShutdownEvent, WorkerConsoleCloseEvent and WorkerIoReleaseEvent through
one existing async-RPC attachment acquisition body. WorkerStateChanged has no
production consumer: remove that obsolete launcher-retry notification and its
service state, retaining the service's internal condition-variable wake.
Its old bootstrap fixture must submit once just like Run16, not wait/retry.
The facade preserves caller-specific timeout latching and closes attachments
on a final service-loss result. Three notification results require a non-null
handle; the frontend capability retains its prior provider-status contract.
This does not change service authentication, permissions or I/O ownership, and
does not add a deadline to intentional command receives or task execution.

The remaining worker-control group comprises AcquireConsoleContext,
BindConsoleContext, RegisterNativeBackend, WorkerIoCheckpoint,
WorkerIoTransition and Take/WaitFrontend. Reuse async RPC and the existing
ten-second constant; preserve the noserialize context annotations needed by
the resident command receiver. WaitFrontend awaits an already granted I/O
attachment, not another task: bound its server CV wait too so cancellation
cannot retain a server invocation indefinitely. Failed acquisition leaves
outputs empty; a timed-out mutation is not replayed or represented as rolled
back. Service rundown owns any committed state. Actual service fixtures and
authenticated runtime calls cover successful state/attachment behavior;
paused-service cases cover each client deadline and immediate latched follow-up.

### Fixed remaining RPC closure after protocol 57

The complete IDL/ACF and production-consumer audit is recorded in the
[S3 ledger](../evidence/m0-t448-s3-close-implementation.md#protocol-57-complete-rpc-inventory).
It supersedes an informal count of only the core/native wrappers: StartVdmWorker
and the original VDM adapter are also on the startup/exit chain. There are 68
wire methods: 34 async, 25 live synchronous finite controls, eight synchronous
obsolete client surfaces, and one intentional long-lived native command receive.
These are a fixed remediation set, not permission to add new product features.

Implement the nine startup/result/context methods as one group; then the
fourteen VDM/stream/WOW methods and two management readers as their owning
groups. Retire the eight unused client wire surfaces only after moving each
still-valid assertion to the current production path or service fixture.
Keep the service-internal providers used by live algorithms. Do not convert
dead methods merely to grow an async facade. The DOS/WOW Get RPC returns an
original wait-event attachment; it is not the long-lived task wait itself.
Only GetNextNativeCommand intentionally waits for future command arrival.

Transport failure is distinct from an authenticated application-status reply:
FinishNativeRequest may legitimately return an I/O error with completed=TRUE
and the real exit code. Use the existing async completion primitive's separate
transport status and reply value; close/drain before freeing storage. Preserve
this outcome on real replies, clear outputs on transport failure, reject invalid
completion flags, and never replay consumption/submission. The same distinction
applies to original NTSTATUS-bearing VDM replies and receipt ownership.

The eight retirements and 25 adaptations complete the fixed wire-method sweep,
not S3 by themselves. The external RpcMgmtInqIfIds diagnosis, the native
observation client's current 250ms bound versus the approved 10s policy,
partial-initialization/fault cleanup and original-host exit permission remain
explicit checks. Resolve them in existing RPC/process-local mechanisms; no
new IPC, private helper process, detached pending call, guest patch or silent
version-mismatch/error-code reclassification is authorized.

Retirement migration exposed two native admission boundaries that must retain
the existing product contract. NTSRV selects or reserves a resident CUI carrier
under its existing service lock. Concurrent first-start requests wait for the
same authenticated reservation to become selectable, or fail within the shared
ten-second control policy; callers do not replay submission. This uses the
existing service condition, not new cross-process communication. GUI launcher
reservations remain exact and one-shot.

The native carrier's first target type is not the type authority for later
nested targets. Each authenticated launcher admission records its requested
type and copies it into the queued direct request. That immutable request
value is compared with the actual suspended target's machine/subsystem at
BindNativeTarget. The persistent worker's CUI/GUI role and exact GUI reservation
are still independently enforced. A Win32-to-Win64 CUI nesting does not create
another hidden Console merely because target bitness changes. Request state
is a controlled immutable projection of admission, not a second independently
maintained carrier classification. Migration tests must retain wrong-type
rejection, exact exit results, same-carrier reuse and concurrent-start coverage.

S1 changes documentation only: zero runtime, mirror, guest or release changes.
S2 aims for zero mirror changes, using existing project-owned adapters. The
selected S3 orderly-exit budget is at most three original host files and seven
ExitProcess call substitutions: nt_reset.c (two), wow32/wkman.c (four), and
wow32/wow32.c (one). Post-P16 source tracing establishes that this is an exit
inventory, not sufficient insertion placement: original teardown and ExitVDM
can precede those sites. The
[L08 ordering audit](../evidence/m0-t448-s3-close-implementation.md#l08-source-order-audit-after-p16)
therefore requires proving pre-cleanup entry seams before implementation; do
not implement a final-exit wait as if it authorized earlier cleanup.
Prefer reducing this through proven existing seams; do not
globally intercept ExitProcess or alter unrelated CRT exits. Keep original
task completion and source-owned task cleanup in place. Classify each exit site
before inserting an executable-owned worker-exit-intent binding: worker-wide
resource teardown follows service permission, not the reverse. Retain the
control connection and handoff resources until that permission or confirmed
loss. A received service close command is already permission and must not
trigger a second permission RPC. Natural/management races share exactly one
teardown. The service
must not wait for that worker's death before replying with exit permission;
it may permit worker exit while retaining the launcher's final-return barrier.
No frontend/guest callbacks are invoked by this exit permission operation.
Keep the authenticated client alive until permission/confirmed loss. An actual
unhandled exception remains catastrophic failure, not an orderly exit waiting
for RPC (nt_timer.c VdmUnhandledExceptionFilter is retained).
The existing session-escape path uses the owned standalone cleanup boundary,
before broker disconnect, rather than a mirror hook. Every actual substitution
requires local divergence/provenance registration and lock/reentry tests;
this budget is design, not an implemented or measured final diff.
No guest diff is permitted. Protocol changes update the real common IDL
and all consumers together, with the complete thirteen-image release gate.

The original fTSRExit/Console-group entry uses an explicit Console scope on
the existing participant-exit RPC, not a new transport or a target NTCON PID.
NTSRV authenticates the participant, derives its retained frontend generation,
and enters the same frontend-close transaction used by management. WOW and
native GUI carriers cannot request this scope; a missing association is an
error, never permission to select another Console. Ordinary participant faults
retain their existing participant scope. Acceptance remains distinct from the
existing close instruction. Protocol 62 stages this finite service/client edge;
original pre-cleanup entry integration and lock-safe permission are still open,
so it cannot be published as completed L08 or mixed with protocol-61 images.

The pre-cleanup sweep additionally identifies nt_event.c's Console-group
intent, absent from the earlier final-ExitProcess inventory. DIV-355 registers
that one owner-local entry/handler change: source-owned cleanup moves from
before the Console request to its service-delivered close handler, preserving
host_applClose, ExitVDM and original zero process-exit ordering. The RPC carries
scope, not an arbitrary frontend identity. This is the explicit correction to
the earlier file/call inventory, not a claim that its remaining sites are done.
