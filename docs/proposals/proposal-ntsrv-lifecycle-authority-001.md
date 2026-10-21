# NTSRV lifecycle authority unification

## Owner direction and scope

Owner correction on 2026-10-21 supersedes the blanket control/close deadline
policy in the earlier design and implementation records. Keep the explicit
Run16-to-new-NTSRV startup wait, NTSRV empty-service grace and Console-close
handler grace, each five seconds under the subsequent owner clarification.
Original internal device/task waits retain their source semantics.
Ordinary RPC, report, query, disconnect and lifecycle operations follow normal
completion or actual failure/service loss, not an added arbitrary deadline.
Individually necessary waits must be explained and approved, not inferred from
the earlier instruction to standardize existing bounded waits. S3 includes the
systematic removal audit; the pending r32 timeout candidate is not for delivery.

Owner requests this package ahead of the WOW32 continuation when closing
T447 by explicit transfer. Current admits this package as T448 S1 after
the clean, pushed T447 closure checkpoint 4d4ef4d47.
It inherits the CAF cooked-read/active-buffer evidence and current checkpoint;
neither a passing fixture nor the checkpoint is complete lifecycle acceptance.

NTSRV alone decides managed-component admission, frontend creation/reuse,
association, I/O grants, handoff, parking, closure and final launcher results.
Run16 classifies executable family/machine and IS_CONSOLE_APP; NTSRV validates
and owns those facts, including actual suspended-native image verification.
After admission Run16 communicates only with NTSRV, not worker/NTCON liveness
or restoration channels. Worker-base and NTCON likewise receive control only
from NTSRV. Their direct channel carries user I/O/text/graphics and necessary
data-format acknowledgements, never independent lifecycle negotiation.

DOS and native CUI use an NTCON frontend. WOW and native GUI never initialize
character I/O or attach to NTCON. Worker lifecycle reception is independent
of character-I/O initialization. NTVWM GUI launchers are one-shot: report
delivery and receive service-authorized release; the actual native GUI target
remains read-only Direct/Observed, not service-owned or killed on service loss.
NTMON and W16Style remain ordinary tools. DLLs follow their host lifetimes.

Before admission Run16 may start the absent service under one real five-second
deadline. After admission there is no reconnect/replay. A confirmed lost
authenticated service session invokes role-specific local cleanup and exit;
a single operation timeout is not proof of disconnection. With the session
alive, reportable failures go to NTSRV for disposition. Actual crashes remain
failures observed by the service, not orderly exits. Normal task completion
is a report, not permission for its worker or launcher to retire.

Owner clarification: a later NTSRV process cannot revive an admitted session.
No participant restarts the service, reconnects or replays that session after
service death. A broken worker/frontend I/O channel only authorizes stopping
unsafe I/O and reporting the failure to the still-live service, not local
process retirement. NTSRV decides both sides' disposition; confirmed service
death permits role-owned local cleanup and exit without a service acknowledgement.
The same rule applies symmetrically to frontend and worker failures. Suspension
and resumption of the same live test service is not service resurrection.

## Finite inherited gaps and acceptance obligations

The nine rows below preserve the admission inventory, not assertions that all
listed defects still exist. S2-S4 implemented the approved repairs; the
[integrated reconciliation](../etc/evidence/m0-t448-s5-integrated-acceptance.md)
maps their current source, actual acceptance and remaining limits. CURRENT is
the sole delivery authority. Historical failed observations below cannot be
treated as either current failures or passes without that reconciliation.

1. Run16 startup must share one absolute five-second budget across its phases;
   its existing readiness polling is separately audited in S4.
2. Run16 still observes worker/frontend process handles during admission.
3. DOS parent-resume trigger and separate finalization exit remain
   launcher-owned; task completion must join required handoff at NTSRV.
4. Shared broker watcher covers process death, not uniform confirmed session
   loss; replace forced watcher exit with role-owned cleanup. Healthy-service
   handoff is service-ordered; disconnected Run16 returns the original-shaped
   error independently of NTCON. No final-disposition timeout or peer recovery
   synchronization is permitted.
5. NTCON still chooses parking from creator death and usage; move the decision
   to service commands and preserve final-buffer/input restoration evidence.
6. WOW skips the character setup containing the worker-close receiver. Trace
   and connect lifecycle-only reception without introducing NTCON to WOW.
7. NTVWM local input/presentation/completion failures and one-shot GUI release
   require explicit service disposition instead of autonomous healthy-service exit.
8. Remove extra deadlines; retain only the three approved five-second project
   policies and original internal waits. Register every imported-source diff.
9. Complete actual internal/external CAF management-close, normal task return,
   nested CMD/COMMAND and first-character-before-Enter proof. The admission-time
   CMD confirmation and COMMAND initial-prompt failures remain recorded non-passes;
   later successful observations have their own exact package/run identity.

## Implementation groups and acceptance

Owner adds a dedicated S4 after current S3 for autonomous-code polling audit and
cleanup; original integrated acceptance is S5. Inventory Sleep loops, periodic
scans, repeated state queries and probes by call chain. Prefer existing events,
RPC notifications, I/O completion and process-exit notifications. Record each
retained poll's need, frequency and exit condition. Do not add a third transport,
generic notification framework or hidden helper window. Classify original
scheduling, presentation pacing and test-harness safety separately, not as
automatic deletion candidates. Verify lost-wakeup, concurrent teardown, idle CPU
and actual source/test delta before coherent build/test/publication/commit/push.
S4 follows the recorded S3 closure; this separate polling audit does not expand
into a repository-wide rewrite of original waits. CURRENT owns admission and
delivery status; this proposal does not independently activate a packet.

Design the entire finite transition table before coding: authenticated owner,
request identity, task result versus final completion, command/ack phases,
failure rollback, duplicate/stale messages, nested execution and service loss.
Reuse existing records/receipts and original BaseSrv/MVDM owners; no second
scheduler, helper window, polling authority or guest modification/recompile.
Do not relay frame traffic through NTSRV.

Suggested sequential groups: source/contract audit; service-to-launcher final
completion; worker/frontend control separation; integrated fault/concurrency
and real Console acceptance. Protocol changes deliver coherent thirteen-image
packages, never incompatible half-updates. Preserve normal park-with-live-NTCON,
GUI startup-only/--wait, task versus worker versus Console-close distinctions,
same request path for internal/external NTMON, and native GUI non-ownership.

Owner's final design correction: submissions never retry on failure or ambiguous
reply; NTSRV owns abandoned accepted-request cleanup. Win16 default Run16 return
is startup success, not Win16 task exit. The implementation plan separates task
cleanup from service-authorized whole-worker teardown. Basic loss/timeout exits
are part of the normal-path delivery, not deferred until fault-matrix testing.

Source evidence: [T447 working plan](../etc/operations/t447-wow-execution-plan.md),
[CAF ledger](../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md), and
[closure/transfer](../history/m0-t447-transfer-closure.md).

Exit requires source-backed ownership inventory, no control bypass, role-specific
service-loss cleanup proof, passing affected positive/negative/lifecycle tests,
real pre-Enter echo, full background build/incremental verification, coherent
publication/hashes, mirror exception accounting, reviewed commit/push and owner
acceptance. Existing failures are not passing results or hidden exclusions.

Owner-added S6 removes proved unused NTCON synchronization/default-notifier
interfaces and the unreachable CCPU yield fallback. NTMON samples existing RPC
state at one second and draws only semantic/UI changes, not elapsed-time ticks.
See the [bounded S6 ledger](../etc/evidence/m0-t448-s6-unused-monitor-cleanup.md).
This adds no transport, lifecycle policy, guest change or new original wait rule.

Owner adds S7 shared-library review after S6 acceptance. The
[compatibility audit](../etc/evidence/m0-t448-s7-shared-library-import-audit.md)
must resolve raw-import incompatibilities before implementation, retaining text
capabilities and NTSRV authority without restoring extra deadlines.
