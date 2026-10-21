# NTSRV lifecycle authority unification

## Owner direction and scope

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

Before admission Run16 may start the absent service under one real ten-second
deadline. After admission there is no reconnect/replay. A confirmed lost
authenticated service session invokes role-specific local cleanup and exit;
a single operation timeout is not proof of disconnection. With the session
alive, reportable failures go to NTSRV for disposition. Actual crashes remain
failures observed by the service, not orderly exits. Normal task completion
is a report, not permission for its worker or launcher to retire.

## Finite inherited gaps

1. Run16 startup uses 100 sleeps of 50ms, not an absolute ten-second deadline.
2. Run16 still observes worker/frontend process handles during admission.
3. DOS parent-resume trigger and separate finalization/timeout exit remain
   launcher-owned; task completion must join required handoff at NTSRV.
4. Shared broker watcher covers process death, not uniform confirmed session
   loss; forced exit can race NTCON restoration. Design and prove cleanup
   order without a new direct component-control channel.
5. NTCON still chooses parking from creator death and usage; move the decision
   to service commands and preserve final-buffer/input restoration evidence.
6. WOW skips the character setup containing the worker-close receiver. Trace
   and connect lifecycle-only reception without introducing NTCON to WOW.
7. NTVWM local input/presentation/completion failures and one-shot GUI release
   require explicit service disposition instead of autonomous healthy-service exit.
8. Reconcile bounded ten-second policy, including NTVDM's remaining 5000ms
   Console-close grace; retain original close semantics and register any diff.
9. Complete actual internal/external CAF management-close, normal task return,
   nested CMD/COMMAND and first-character-before-Enter proof. Current installed
   CMD confirmation and COMMAND initial-prompt failures remain non-passes.

## Implementation groups and acceptance

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

Source evidence: [T447 working plan](../etc/operations/t447-wow-execution-plan.md),
[CAF ledger](../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md), and
[closure/transfer](../history/m0-t447-transfer-closure.md).

Exit requires source-backed ownership inventory, no control bypass, role-specific
service-loss cleanup proof, passing affected positive/negative/lifecycle tests,
real pre-Enter echo, full background build/incremental verification, coherent
publication/hashes, mirror exception accounting, reviewed commit/push and owner
acceptance. Existing failures are not passing results or hidden exclusions.
