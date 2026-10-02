# Proposal — NTSRV-owned Direct/Observed worker task traces

## Status, dependency and outcome

Owner-requested unnumbered T candidate, second in the queue immediately after
the worker/frontend component-renaming candidate. This transfers the former
T423 S39 plan; S39 is not admitted or completed. T423 remains open for owner
acceptance after S38 delivery. This proposal becomes implementable only after the naming T
has delivered its final coherent package. In this proposal, `NTW32` means the
Win32 text worker; “frontend” means the renamed visible Console/Window owner.

Provide an NTSRV-authoritative, read-only view of the execution chain for a
selected NTVDM or NTW32 worker. NTMON reads that view and opens a detail modal
showing `DIRECT` and `OBSERVED` nodes, their proved relationships, provenance,
revision and uncertainty. The trace explains what happened; it never becomes
a second task scheduler, a worker-lifetime authority or a source of exit codes.
The transferred source/research basis is the
[T423 S39 plan](proposal-kvm-window-graphics-presentation-001.md#s39-task-prompt-ntsrv-owned-directobserved-task-traces),
including its original DOS `EXEC`/PDB/TSR source anchors and bounded Windows
Job-notification limitations. Revalidate those anchors against the renamed
and then-current production graph at admission rather than treating the old
plan as proof of implementation.

## Ownership and unchanged authority

| Datum | Collector | Holder and consumer | Authority |
| --- | --- | --- | --- |
| Direct DOS/Win16/native admission | NTSRV's existing BaseSrv/native admission and completion path | NTSRV; projected to NTMON | Existing original DOS/WOW records and direct native Win32Records alone control receipt/completion. |
| Observed DOS/Win16 execution transition | NTVDM at proved original worker-local service boundaries | NTSRV trace sidecar; NTMON reads snapshot | Diagnostic only. |
| Observed Win32 text descendant | NTW32 at its real native process/Console boundary | NTSRV trace sidecar; NTMON reads snapshot | Best-effort diagnostic only. |

Original `DOSRECORD`/`WOWRECORD` and renamed native Direct Win32Records retain
their definitions, list ownership, scheduling, parent waits, completion,
READY/BUSY/EMPTY and cleanup. Do not insert Observed elements into a Win32Record
chain, duplicate a mutable execution stack, or infer a task from Console
membership. NTSRV holds a separate private trace sidecar tied to a
generation-safe service-record identity. It projects Direct nodes from its
authoritative records and receives revisioned Observed deltas from workers.
It authenticates the worker capability, orders revisions, marks stale/dead
sources and serves a copied snapshot. A bare PID, caller-provided parent PID,
pointer or exposed sequence is not an authority.

`src/interface` owns versioned copied DTO/RPC declarations. `worker-base`
contains only mechanisms common to the two workers: validated copied-delta
encoding and an authenticated publication client. It stores no task registry
and is not an NTSRV/NTMON dependency. The frontend owns neither collection nor worker
life; it only presents the existing frames. NTMON queries NTSRV and never
enumerates processes, creates Jobs or contacts a worker synchronously to
populate its view.

## Trace contract and fidelity limits

Each node carries opaque node identity, a parent-node identity only when
proved, kind (`DOS`, `WIN16`, `WIN32`), relation (`DIRECT`/`OBSERVED`),
active/exited/failed/unknown state, image/path, optional PID, start/end times,
source and snapshot revision. Parent links are valid only within their trace
revision. Unknown is a result, not permission to invent a completion. A
Direct root also seen at an original execution-entry hook is deduplicated.
Only actual Direct completion may release a waiting run16 or change worker
state; Observed deltas cannot affect admission, receipt, worker selection,
root retirement, task kill or process termination.

For NTW32, any Job/completion-port observer belongs in NTW32 at its suspended
direct-target boundary before `ResumeThread`; the Job is observation-only and
has no kill-on-close or scheduling policy. `NEW_PROCESS`/`EXIT_PROCESS` messages
are not a guaranteed lossless event log. Breakaway, missed/late notifications,
PID reuse and unproved parentage remain explicit best-effort gaps. Direct
target completion still follows the real target and current NTSRV receipt.
See the [Windows Job completion-port contract](https://learn.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-jobobject_associate_completion_port).

For NTVDM, first reuse proved original DOS entry and PDB termination services
without modifying guest media or moving original execution logic. `DOSONLY=1`
children do not become invented Run16/NTSRV Direct admissions. Failed EXEC,
load-only/overlay and COMMAND builtins are not running child tasks. Correlate
observed guest execution through worker generation plus PSP plus occurrence,
using a bounded guest-memory lease for copied fields. Original TSR return
does not provide a proved host terminal event; absent a real event, report
`unknown`/`uncertain`, not a guessed exit. The original source anchors and
distinct `DOSONLY` paths are enumerated in the transferred T423 S39 plan.

`WorkerTaskTrace` is an authenticated, versioned, read-only NTSRV query keyed
by a currently registered worker PID. NTSRV resolves the PID under the
registration lock; removed or reused identities cannot disclose another
worker's trace. NTMON Enter opens a modal with hierarchy, source, revision,
staleness and explicit unavailable states; existing selection/Delete behavior
and the ordinary management summary remain unchanged.

## Proposed bounded S sequence after admission

| S | Complete result and gate |
| --- | --- |
| S1 | Re-audit the renamed production graph, original DOS entry/termination/TSR anchors, native Job limits and all existing direct records; freeze the DTO, identity/revision and negative-contract ledger. No implementation claim from research alone. |
| S2 | NTSRV Direct trace projection and authenticated read-only snapshot, using existing DOS/WOW/native records without a second mutable stack. Include malformed, stale, PID-reuse and worker-death tests; NTMON can query a minimal direct-only view. |
| S3 | NTW32 Observed descendant collection/push with a bounded Job observer and explicit loss/parentage uncertainty; prove direct completion and READY remain unchanged, including short-lived and surviving descendants. |
| S4 | NTVDM Observed guest entry/ordinary termination push, DOSONLY on/off, nested EXEC and truthful TSR/unsupported states. Prove original guest scheduler, PDB cleanup and Direct receipt remain unchanged. |
| S5 | NTMON hierarchy/modal and full multiworker acceptance: source/revision/gap display, independent sessions, faults, nested DOS/native chains, retained product gates, coherent package publication and final diff/ownership audit. Stop for owner review; do not silently close the T. |

Each code-bearing S must actually wire its completed slice to production and
deliver its focused positive/negative/lifecycle tests, full existing
production-P regression gate, coherent package, evidence, commit and push.
Do not leave an implemented client disconnected until S5. A new event source
whose fidelity cannot meet this contract is reported unavailable rather than
papered over with polling, process ancestry guesses or weaker assertions.

## Final acceptance matrix

Prove forged/stale worker or trace capability rejection; cross-worker/root
isolation; out-of-order/replayed revision handling; worker disconnect/death;
PID reuse; direct target launch failure; observed child exit before/after
direct completion; detached child; nested native CMD; deliberately missing
Job notification; uncertain TSR; DOSONLY on/off; EXEC failure; load-only,
overlay and builtin exclusions; resident reuse; no-Console roots; and NTMON
read-only behavior. Controlled chains show observed behavior but cannot prove
universal no-loss ancestry. Run MIDL regeneration when the interface changes,
full x86 build, focused service/worker/NTMON fixtures, Console and Window
product regressions, and retained DOS/WOW frontiers. Report actual passed,
failed and unsupported rows separately.
