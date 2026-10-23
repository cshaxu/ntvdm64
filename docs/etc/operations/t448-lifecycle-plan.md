# T448 lifecycle unification working design

## Authority and status

Owner admits S8 after S7's bounded audit conclusion. The [shared-library audit](../evidence/m0-t448-s7-shared-library-import-audit.md#s7-conclusion-and-s8-handoff)
records the pinned import and consumer migration scope. S8 integrates selected
byte-exact public-library sources into NTCON, verifies and publishes the full
package. No guest, RPC, worker protocol or WOW32 feature expansion. T closure
follows S8 acceptance and final audit, not this admission.

### Owner-added S6 cleanup

After S5, remove proved uncalled NTCON synchronization APIs and unreachable
CCPU yield fallback. NTMON samples at 1000 ms but repaints only changed
snapshot, error or UI state (including resize); elapsed time alone is not a
repaint reason. Keep immediate keyboard handling and modal task updates.
The [S6 ledger](../evidence/m0-t448-s6-unused-monitor-cleanup.md) bounds this
cleanup and its full-package acceptance; no new transport or lifecycle policy.

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

## Owner-directed TODO triage

The owner directs the following finite follow-up within T448, after S10's
acceptance and sequential admission. This is planning, not a new active S or
an assertion that any selected defect was reproduced or repaired. S10 removes
the owner-selected MySMB16 performance, wide-text mouse and native-menu
animation TODO rows. Available records do not establish a MySMB16 performance
fix; removal is the owner's backlog disposition, not proof of resolution.
Retain their existing historical evidence.

### S11: DOS CAF and WOW window startup regression

Owner inserts this urgent S before the DOS/mouse triage: reproduce and trace
`run16 command` followed by CAF failing to display its window, and `run16 sol`
failing to show WOWEXEC or Solitaire. Pin the installed package and preserve
any active user session; distinguish desktop visibility, service, worker and
guest startup stages. Correct proven project-owned causes and verify both
actual launch paths and relevant controls before complete-package publication.
No guest modification or unapproved semantic change.

### S12: reproducible host TODO remediation

The owner expands S12 from DOS/mouse triage to all reproducible host-side
TODO defects, and S13 to not-necessarily-reproducible TODO investigation.
This admission does not authorize guest changes or new semantic retreats.
Freeze the TODO inventory at admission and reconcile every item to an owner,
reproducer, repair, evidence and disposition. Do not equate an untested case
with a pass or silently omit an item because it crosses an existing T plan.

Confirmed host-side failure candidates, to revalidate and remedy as one
bounded S12 group, are: INT33 saved-state checksum; VGA cursor disable bit;
8042 C0 response; redirector username zero-capacity underflow; and the DEM indexed directory restart when the remembered
name is deleted. Recover original logic and smallest dependency closure first;
existing prior TODO wording requiring a matching upstream fix is superseded by
the current source-proven original-host correction authority, not by a license
to bypass exception registration or failure testing. Retain existing tests and
add real repro, negatives, lifecycle, relevant build/profile coverage, and
mirror-diff/provenance records for every correction.

Audit the reproduced INT31 invalid-buffer witness as a mixed host/guest case:
the owner previously accepted this specific out-of-contract guest limitation.
Do not manufacture a new error semantic or patch guest; identify any separable
host memory-safety repair and its compatibility evidence before acting. Other
known style coverage limits and the NTVWM output sampler are not automatically
bugs merely because they are observable: examine each actual TODO contract and
fix any demonstrated reproducible host defect without silently dropping
previously accepted boundaries. A test or device unavailable for verification
is a named blocker, not a pass. The external COM row requires a suitable real
peer or separately admitted virtual peer; its missing hardware is not itself a
reproduced host defect.

Original guest issues remain grouped in TODO. MEM's zero-value underflow is
strongly supported but its private variable and separate original-DOS control
remain unobserved; never treat that uncertainty as a proven host defect or a
license to patch guest. Nested BAT redirection is owner-closed after paced and
continuous valid witnesses passed, with historical failure retained.

The earlier S12 investigation and criteria remain supporting evidence:

Pin the current release and immutable guest hashes, inspect each original
report and execute its actual relevant route with independent success and
failure evidence: (1) nested DOS BAT and MEM file redirection; (2) MEM's
reported largest executable block, including direct original DOS comparison
and units/unsigned-result attribution; (3) INT33 SaveState/RestoreState using
the original provider and a valid real guest state round trip. For each,
record expected and actual output, exact scope, root cause if reproduced,
and whether it is guest-original, original-host or project-owned. The third
already has source and prior failing evidence; a snapshot prototype failure
is not by itself proof of a current ordinary-path regression. Do not modify
or rebuild the guest, or suppress an actual failure.

### S13: not-necessarily-reproducible TODO investigation

The owner accepted S12 and admitted S13. The historical transferred rows below
retain their original wording, not current repair status: S12 delivered
DIV-357 through DIV-362 for mouse checksum, VGA cursor, 8042 response, directory
buffer/restart and redirector zero capacity. See the S12 closure evidence;
do not reclassify those fixed witnesses as pending S13 investigations.

Audit every remaining uncertainty-bearing TODO, not just the four historical
lifecycle/input witnesses: include reported SOL layout and intermittent direct
WINMINE/SOL termination, the common-dialog witness, WRITE lifecycle, potential
8253 timing issue and VDMREDIR zero-length edge. For every row retain original
evidence and choose meaningful witness, controls and finite test budget;
distinguish unknown cause, unavailable environment, confirmed defect and
non-reproduction. Device-dependent COM, conditional suballocator rollback,
NTW16S bounded coverage, NTVWM sampler, T271 and documentation migration must
be inventoried with their real prerequisite/acceptance or engineering nature,
not falsely declared reproduced or dropped. Original guest retained problems
remain tracked without guest mutation. An S13 reproduction is not automatically
repaired as S12 work after S12 closes: record its owner and appropriate follow-on
unless governance explicitly admits the resulting repair.

Use the existing exact witnesses and current published package to run bounded
targeted controls for (1) native rapid interactive nonzero return, (2) two
session management-close RPC failure, (3) outer-CMD missing initial input,
and (4) the two pre-prompt startup timeouts. Record actual target return,
RPC code, participant and console state, input records and failure-stage
snapshots as applicable; never treat a timeout, missing prerequisite, or
unrun case as a successful negative reproduction. A later passing run does
not prove a historical failure was repaired or determine its cause.

### S14: S13-reproduced startup failure

**Superseded by owner decision:** do not pursue the proposed host workaround
or allocate S14 for this case. Retain the existing original-guest P1 in TODO
and its evidence; do not claim the defect fixed. The following is historical
planning, not authorized work. The owner instead allocated S14 to the
byte-exact SoftPC library synchronization and accepted that delivery; see
[S14 evidence](../evidence/m0-t448-s8-shared-library-import.md#s14-synchronization-and-verification).

Owner-directed follow-on, planned rather than an additional active packet:
the current inherited-environment DOS startup fails before its first prompt.
The outer-CMD relaunch and rapid DOS/native sequences are witnessed scenarios,
not independently established defects. The host-only diagnostic directly
observes the original COMMAND discarded-INIT `EnvSiz` overwrite in its own
run; it does not establish that every earlier timeout had the same cause.
Inputs, hashes, controls and limits are in the [S13 evidence](../evidence/m0-t448-s13-uncertain-todo-audit.md#direct-guest-state-attribution-of-the-current-inherited-environment-failure).

Audit whether a source-supported host-side remedy can preserve original
semantics and unchanged guest bytes. Do not modify, rebuild or hot-patch the
guest, filter or truncate the environment, inject `/E:`, force DOS=HIGH, or
add an allocator special case. Do not represent the original guest defect as
a proven host defect. Any new observable semantic retreat requires separate
approval. If no compatible remedy is demonstrated, document the evidence and
retain the single existing original-guest P1 limitation in TODO; do not claim a
repair or manufacture a duplicate TODO. If a valid remedy is found, implement
and verify the real inherited environment and short-environment controls,
including DOS/native relaunch and guest identity, then apply the normal full
delivery gates. S14's eventual packet must define its exact finite acceptance.

Unreproduced historical failures, the reported intermittent game exits, SOL
layout, unavailable external COM validation and later WOW32 work are not
silently reclassified as S14 reproductions; their existing dispositions and
owners remain in the S13 ledger and relevant TODO or future package.

### Transferred TODO inventory

The owner directs removal of entries taken into S12/S13 from TODO to avoid
duplicate tracking. The complete transferred entries follow as the historical
intake, unchanged in substance; individual S12 fixes and S13 dispositions
supersede their original pending language. The S12/S13 categorization and accepted guest, device and semantic boundaries
above govern disposition. MEM is an investigated guest-retained case and is
not permission to modify guest. An unresolved item must be returned to TODO or
assigned to its proper future packet before T448 closes; do not lose debt.

| Priority | Transferred item | Existing admission path and evidence |
| --- | --- | --- |
| P2 | Owner confirms direct SOL startup succeeds, but reports incorrect window-content layout; the actual desktop and application geometry for that report were not captured. The separately investigated 427×320 test-desktop width limit follows original OpenNT maximum-tracking semantics and is not an adapter defect; see [SOL geometry evidence](../evidence/m0-t448-s9-console-title-handoff.md#sol-integration-geometry-follow-up). | Preserve the distinct owner report until its actual geometry is observed; do not presume it is the small-desktop case. Full gameplay and visual correctness are not certified by startup or a geometry-limited test. No guest changes or unapproved sizing overrides. |
| P2 | One common-dialog witness exited after DestroyWindow before its second report. Host return was1; old evidence lacks guest AX and local IsWindow result. Later same-image runs and16 create/destroy rounds pass. Owner permits closure with this unlocated risk; no stable reproduction, introduced revision or causal repair is established. See [retained witness](../evidence/m0-t447-s2-scope-freeze.md#guest-observed-return-boundary). | Preserve the failing record and strict CDL2 return/identity assertions. On recurrence capture both guest results and task/object identity before attributing or repairing; no guest modification or weakened success condition. |
| P2 | Owner reports that direct `run16 WINMINE` and `run16 SOL` can, after ordinary play, exit their WOW worker and application together. It is intermittent: bounded direct-game probes, including real click/reset churn, do not reproduce it. One separate temporary-worker WER record contains `0xC0000420` near WOW32, but its runtime package is not proven to be the owner's occurrence. | On recurrence preserve the exact package, process identity, WER/fail-fast trace and task-retirement trace before cleanup; correlate the failure stage to the direct `run16` worker before selecting a repair. Do not blame a Win3.1 PIF path, suppress the fail-fast, add a retry/delay, or change guest media. |
| P3 | Historical NTW16S retained bounded style coverage: CP1252 bitmap glyphs, native fallback for unsupported custom drawing/large scrollbar positions, and possible text clipping in fixed-layout host About dialogs. S9 did not implement automatic arbitrary-application reflow. The component was removed by T450; see [S9 evidence](../evidence/m0-t445-s9-integrated-acceptance.md). | Admit concrete application/layout or Unicode cases with original geometry and lifecycle evidence; do not silently resize third-party controls or claim universal style coverage. |
| P2 | Original NTVDM host mouse INT33 SaveState writes checksum zero, while RestoreState requires the summed cookie/context checksum. The real-provider guest probe reproduces failure and a test-buffer checksum control restores position; SoftPC currently retains the same defect. See [S12 evidence](../evidence/m0-t448-s11-window-startup.md#s12-resumed-first-bounded-witnesses). | Fix in the original host saver under the current source-proven original-host correction policy, with a registered minimal divergence, unchanged guest and real positive, invalid-cookie/checksum and lifecycle tests. No fix is yet implemented. |
| P2 | One T433 S5 full-run rapid interactive burst returned outer81 after marker11; the old fixture did not preserve the direct nonzero code. Three S4/S5 comparison groups each,144 diagnostic rounds and final unchanged-package Full pass, but no causal repair is claimed. See [retained rapid observation](../evidence/m0-t433-s5-native-launcher-migration.md#retained-rapid-interactive-failure-and-further-evidence). | Preserve the failed report and failure-only code/worker diagnostics. On recurrence capture startup, real target result, final I/O and Console restoration separately before cleanup. Continue under an admitted lifecycle/integrated audit, not retries, longer sleeps, explicit exit0 or weakened assertions. Do not call it harness-only or an original-program exemption without evidence. |
| P2 | Original VDMREDIR VrGetUserName compares len with `(DWORD)CX-1`; CX=0 underflows. The same expression exists in pinned OpenNT. No matching sibling SoftPC repair is established; its current source tree has no vrnetapi.c counterpart. See [origin gate](../evidence/m0-t430-s2-softpc-repair-gate.md). This inherited issue is distinct from project Unicode/OEM bounded-copy adaptation. | Preserve for later source-based research; do not invent an original-host correction now. Adopt only a subsequently verified matching SoftPC fix under the owner provenance gate. No confirmed product runtime failure is claimed. |
| P2 | Unchanged original VGA `vga_prts.c` does not honor a bit-5-only cursor-disable write: the register assignment depends on changed start bits and `do_new_cursor` derives visibility from scan-line geometry. The register-only probe's first attempt preserves this failure; start beyond character height remains supported. See [source and frame evidence](../evidence/m0-t425-s8-worker-frame-deduplication.md#natural-route-closure-checks-r041-r044). | Admit an original VGA cursor-register contract repair separately if required; compare pinned source and test disable-bit, shape and position independently. No frontend/transport dedup or guest patch may hide it. |
| P2 | One private two-session management close returned an RPC failure under the unified text-surface package. The original fixture did not retain its numerical error or live state, so production versus probe attribution is unproved. Four same-production-source repetitions pass; no causal repair is claimed. See [failure and replay record](../evidence/m0-t424-s12-logical-surface.md#final-verification-and-release). | Retain the failed logs and strengthened error/state diagnostics. On recurrence freeze exact binaries and capture authenticated request, worker/target/Console state before cleanup; admit only a proven owner-boundary repair. Do not weaken tests, add delays/retries, claim harness-only failure or classify it as an original guest limitation. |
| P2 | One automated outer-CMD relaunch observation read the second `run16 command` as `un16 command`. Identical reruns and three consecutive published-package probes pass; six untouched-baseline controls also pass. Cause is not established as harness-only or production, and no repair is claimed. See [input observation and controls](../evidence/m0-t424-s8-ntsrv-service-separation.md#verification-progress). | On recurrence, freeze the exact package and input records, trace frontend ownership release/park, input consumption/return and outer CMD readiness before cleanup. Distinguish fixed-rhythm synthetic input from a genuine product handoff defect; admit the proven owner-boundary repair. Do not add Sleep, weaken assertions, declare an original-guest limitation or treat an isolated passing rerun as causal repair. |
| P2 | NTVWM retains timed acquisition of active hidden Console output at 20 ms, as delivered by [T429 S7](../evidence/m0-t429-s7-native-acquisition-interval.md). The earlier [WinEvent probe](../evidence/m0-t423-s24-native-participant-graph.md#verification) found no real buffer-update or caret event for ordinary Console writes; no verified replacement output-change source is established. [T429 S8](../evidence/m0-t429-s8-shared-event-input.md) separates event-driven input and admission from this output capture interval; idle waits indefinitely. | Revisit output acquisition only when a reliable output-change signal or a different verified capture contract is available; preserve native text updates, cursor behavior and DOS/native handoff. This disclosed sampler exception does not apply to input, command acquisition, completion, handoff or other controllable events. |
| P2 | Two historical pre-prompt startup timeouts have no exact failed-artifact manifest or live-worker snapshot. The final identified independent-frontend package passes DOS17 and strict cold starts; this does not prove the historical cause repaired. See [final disposition](../evidence/m0-t423-s3-hidden-console-ledger.md#historical-startup-timeout-disposition). | Preserve failed reports. On recurrence, stop the affected delivery, pin binaries/configuration and collect live worker threads/windows with the enhanced observer before cleanup; admit the demonstrated owner-boundary repair. Do not treat this unattributed observation as an original-guest exemption or invent a retry/timeout workaround. |
| P2 | Owner-accepted original INT31/0500 invalid-buffer limitation: the original host writes its 36-byte memory-information structure without validating the complete ES span, and the original DOSX return path clears CF. A reduced-limit probe reproduces the write while deliberately violating the required valid-buffer precondition. See [DPMI32 evidence](../evidence/m0-t420-s38-dpmi32-capability-review.md). | Preserve the negative reproducer and immutable guest; no invented error policy or guest patch. This specific owner acceptance is not a blanket exemption for original host defects or project host/adapter failures. Normal-input successes do not prove invalid-span protection. |
| P3 | Original suballocator partial callback failure rollback is incomplete: fixed-size FreeChunk restoration omits trailing bitmap bytes, and earlier successful commits/decommits are not undone. The selected original CPU40 SAS callbacks have no such fallible operations after local zeroing removal; this is not an active product failure. See [suballocator evidence](../evidence/m0-t420-s36-suballoc-capability-review.md#final-capability-ledger-and-closure). | Before admitting a fallible backing commit/decommit provider, verify whole-operation rollback of metadata and data, including compensation failure; do not patch only the bitmap or revive local clearing callbacks. |
| P2 | Original selected NTVDM 8042 response branch invalidates its cached byte/output slot without installing `code_to_send`; C0 therefore returns stale output rather than initialized BF. Two sentinel cases reproduce it in the actual production controller. The verified sibling SoftPC C0 assignment alone cannot repair this branch; its port60 IRQ-clear correction is in excluded non-NTVDM code. Actual NTVDM read/EOI queue ownership passes the bounded PIC fixture. See [selected-branch evidence](../evidence/m0-t430-s3-controller-pic-boundary.md). | Preserve this original host response defect and reproducer until an existing matching SoftPC repair for the selected NTVDM contract is verified. No new original fix, bulk branch replacement or IRQ-clear-on-read policy is authorized. Timer, BIOS ISR and concurrency proof remain separate from the controlled host fixture. |
| P1 | Full WRITE lifecycle remains unproved; preserve the malformed-arena/environment evidence and unaccepted provider/transition WIP without promoting it to a product baseline. | Owner review of the original-owner audit, followed by an active recovery packet. |
| P2 | The original 8253 mode-zero elapsed-counter calculation may report subsequent terminal-count periods differently from NTVDMx64's comparative patch. No current workload reproduces a timer fault. | Admit only after a real DOS/Win16 timer symptom; then compare the exact mode-zero first-count and 65536-wrap contract with focused positive and negative tests. |
| P1 | Complete real external COM transmit/receive acceptance for the retained original `rs232_io.c` INT 14h sender/receiver and `com.c` → `nt_com.c` provider path. The current host has only `COM3` (`usbser` USB Billboard), with CTS/DSR both low, so the original CTS gate correctly prevents external transmit; no product diff, overlay, provider or fake success is retained. The test-only direct-source mock covers the original provider's successful open/write/read/close contract but does not substitute for a real peer. | Supply a CTS-capable paired/null-modem or loopback serial endpoint, or explicitly admit a virtual serial-pair provider. Re-run the S30 real DOS guest transmit, host-driven receive, failure and teardown matrix; see [S30 external COM boundary](../evidence/m0-t420-s30-external-com-boundary.md). |
| P2 | DEM indexed directory restart has no production VdmQueryDir binding; the unchanged original FileFindReset slow fallback fails when its remembered name was deleted. Real filesystem cases reproduce this limit; ordinary guest FCB first/next/end passes. See [directory-reset evidence](../evidence/m0-t430-s5-dem-directory-reset.md). | Preserve the original algorithm and missing fast-path/performance limit. A future admitted finite kernel-directory carrier must retain index/name/status and resource ordering, not a dummy callback or another slow scan. Adopt an original-host mutation correction only if a matching SoftPC repair is subsequently established. |
| P2 | MEM.EXE reports a near-4 GiB largest executable size despite 655360 conventional bytes. A DOS EXEC probe observes the real MEM PSP and an output exactly matching zero-minus-PSP unsigned underflow; associated guest source declares the top-of-memory value as a zero dummy. See [S12 investigation](../evidence/m0-t448-s11-window-startup.md#s12-resumed-first-bounded-witnesses). | Preserve immutable guest; the private value and a separate original-DOS comparison remain unobserved. Do not attribute this to the host memory provider or patch the guest without evidence and authority. |

For S12 and S13, choose finite case-specific rounds and documented resource,
desktop and time budgets before each run. If valid, representative witness
and controls complete without recurrence, the owner authorizes removing that
row from the active TODO while retaining the prior failed evidence and stating
that the cause remains unknown. If the necessary environment, executable,
fixture or observation is unavailable, explicitly report not tested and do
not pretend nonreproduction. Reproduced cases stay tracked with their cause
and a source-backed repair boundary; implementing any material new fix needs
the applicable S brief, semantic authority and full product delivery gates.
No guest changes or invented timeout/retry policy are authorized.

The owner retains the separately described original defects/capability
boundaries, the T271 reconciliation and historical documentation migration.
The INT33 row is specifically included in S12 for fresh reproduction and
owner-directed disposition; do not silently drop its existing proof.
