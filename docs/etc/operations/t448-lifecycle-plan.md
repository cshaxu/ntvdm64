# T448 lifecycle unification working design

## Authority and status

Owner approved the four-stage division and requested execution. Current admits
S1 only. This plan elaborates the approved architecture; it does not claim
runtime implementation or waive the control-only-through-NTSRV boundary.
Baseline: 7917ef25a, following the published T447 checkpoint 4d4ef4d47.
The [source audit](../evidence/m0-t448-s1-lifecycle-audit.md) distinguishes facts,
proposed changes and remaining design approval. No guest changes or rebuilds.

## Delivery groups

| Stage | Complete chain | Exit condition |
| --- | --- | --- |
| S1 | All role ownership, transitions, failure ordering and original-owner audit | Finite remediation/test matrix; service-loss boundary explicitly settled; reviewed design delivery, not runtime acceptance |
| S2 | Admission, execution result, parent/frontend handoff and launcher final result | No launcher peer-liveness decision or autonomous frontend parking; lifecycle-only WOW binding; coherent package and positive/negative normal-path evidence |
| S3 | Console/worker/WOW-task close, concurrent completion and service loss | Uniform internal/external management transaction; fault cleanup; real pre-Enter echo and failure-injection evidence; coherent delivery |
| S4 | Entire matrix, source cleanup and closure | No remaining control bypass, resource/return regression or unaccounted mirror diff; full build/incremental/publication, reviewed push and owner acceptance |

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
separate pre-submission operation with its one absolute ten-second budget.
NTSRV must retire abandoned request resources independently of the requester;
an ambiguous reply is not proof that the target never started and does not
authorize killing an already committed GUI target.
Win16 and native GUI default completion is successful startup, not task exit.
The service releases their launcher on startup confirmation; subsequent task
exit does not release that already departed launcher again. Explicit --wait
retains its existing completion contract without GUI management rights.

## Normal operation

Run16 retains search, classification and CLI behavior. Before authenticated
admission it may start NTSRV, with a single absolute ten-second budget including
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

All phases share one absolute ten-second deadline. Waits use the remaining
budget; duplicate requests and late acknowledgements do not reset it. Natural
task execution is not given a ten-second lifetime. Empty-service/frontend grace
timers remain separate policies, each ten seconds where applicable.

Timeout moves the operation to a recorded timed-out/quiesced condition, not
success and not reopened admission. A matching late acknowledgement or actual
process-exit observation may finish it; there is no automatic retry loop or
silent force-kill escalation. Existing explicitly authorized forced-close
semantics must be identified at their original owner, not added generically.
Owner supersedes indefinite launcher retention: after the final-disposition
wait deadline, Run16 uses the same local cleanup/return mechanism as service
loss, retaining the timeout error rather than fabricating disconnection or
successful Console restoration. This deadline does not limit normal task
execution. The still-live service retains responsibility for accepted work
and the unfinished handoff transaction after launcher departure.

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

DOS and native CUI support Console-close and worker-close, not individual task
termination. WOW supports worker-close and source-shaped individual task close,
not Console-close. Native GUI launcher consumes generic notifications and
reports a non-applicable result without acquiring Console resources. The service
does not offer management actions for native GUI targets. Worker-base dispatches
the reason, operation and deadline; each executable implements the semantics.

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

S2 must deliver basic loss and bounded-finalization exits with its normal
waiting protocol; S3 extends the full phase/concurrency/failure matrix.
The current shared watcher hard-exits with RPC_S_SERVER_UNAVAILABLE (1722),
and launch-style rpc_failure discards its input into STATUS_UNSUCCESSFUL;
neither establishes the required original error propagation. Some original
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
| L05 | NTSRV close operation / S3 | Internal/external deletion, caller dies after acceptance, normal-result/close race, one cleanup, one absolute deadline |
| L06 | NTCON quiesce/restore / S3 | Delayed frame/read/EOF, each restoration error, join failure, no false acknowledgement, first character before Enter |
| L07 | shared RPC notification + role cleanup / S2-S3 | Basic loss/finalization-timeout exits ship with S2; S3 tests every phase; explicit original status/error propagation; Run16 returns even if NTCON is stalled or absent, with no peer wait; timeout/rejection remain distinct; no unconditional watcher kill |
| L08 | original NTVDM/WOW exit boundary / S3 | Natural/fatal/management paths retain original cleanup, no wait under guest locks, source-proven mirror exception if needed |
| L09 | GUI launch and failure / S2-S3 | Startup-only/--wait, suspension/binding failure rollback, launcher departure; GUI target remains alive on service loss |
| L10 | record/attachment rundown / S3-S4 | PID/handle reuse, late command/ack, abandoned reservation, released wait callbacks, no leaked resources |

S4 repeats real conhost and terminal paths with CMD, COMMAND and direct NTMON,
CAF before/after NTMON, internal/external close, normal exit and no-CAF controls.
Use existing tests/observation/verify-console-caf-self-delete.ps1 and strengthen
service fixtures in tests/adapter-basesrv/base_service_reservation_test.c;
frontend_scope_lifetime_test and console_channel_lifetime_test cover local
failure ordering. Add explicit deterministic phase/cancellation assertions to
production-linked fixtures rather than only checking exit status or screenshots.
Preserve T447 COMMAND/WRITE/SOL non-passes; do not count them as restored here.

## Mirror and delivery budget

S1 changes documentation only: zero runtime, mirror, guest or release changes.
S2 aims for zero mirror changes, using existing project-owned adapters. The
selected S3 orderly-exit budget is at most three original host files and seven
ExitProcess call substitutions: nt_reset.c (two), wow32/wkman.c (four), and
wow32/wow32.c (one). Prefer reducing this through proven existing seams; do not
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
