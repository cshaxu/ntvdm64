# Proposal — NTSRV-owned Direct/Observed worker task traces

## Status, dependency and outcome

Owner-requested unnumbered T candidate, fourth in the remaining queue after
the [native launch-hook candidate](proposal-native-launch-hook-001.md).
The active component-renaming package is absent from Queue and is not
expanded by this planning revision. T423 is owner-closed; this transfers its
former unimplemented S39 observation plan, not an active or completed S.
Implementation depends on both the delivered naming package and the launch
hook package. Here `NTW32` is the Win32 text worker and `NTCON` is the renamed
visible Console/Window frontend.

Provide an NTSRV-authoritative, read-only view of the execution chain for a
selected NTVDM or NTW32 worker. NTMON reads that view and opens a detail modal
showing `DIRECT` and `OBSERVED` nodes, their proved relationships, provenance,
revision and uncertainty. The trace explains what happened; it never becomes
a second task scheduler, a worker-lifetime authority or a source of exit codes.
The transferred source/research basis is the
[T423 S39 plan](proposal-kvm-window-graphics-presentation-001.md#s39-task-prompt-ntsrv-owned-directobserved-task-traces),
including its original DOS `EXEC`/PDB/TSR source anchors. Its historical Job
observer selection is superseded by the owner-approved hook reporting and
process-handle wait plan below. Revalidate those anchors against the renamed
and then-current production graph at admission rather than treating the old
plan as proof of implementation.

## Ownership and unchanged authority

| Datum | Collector | Holder and consumer | Authority |
| --- | --- | --- | --- |
| Direct DOS/Win16/native admission | NTSRV's existing BaseSrv/native admission and completion path | NTSRV; projected to NTMON | Existing original DOS/WOW records and direct native Win32Records alone control receipt/completion. |
| Observed DOS/Win16 execution transition | NTVDM at proved original worker-local service boundaries | NTSRV trace sidecar; NTMON reads snapshot | Diagnostic only. |
| Observed Win32 text descendant creation | nthook32.dll/nthook64.dll in authenticated controlled native parents; NTW32 supplies direct-root identity | NTSRV trace sidecar; NTMON reads snapshot | Covered API launch edges only; diagnostic, not native execution authority. |
| Observed native exit | NTSRV event wait on a validated retained process reference from creation registration | NTSRV trace sidecar; NTMON reads snapshot | Actual registered process exit; never a Direct receipt or worker-lifetime decision. |

Original `DOSRECORD`/`WOWRECORD` and renamed native Direct Win32Records retain
their definitions, list ownership, scheduling, parent waits, completion,
READY/BUSY/EMPTY and cleanup. Do not insert Observed elements into a Win32Record
chain, duplicate a mutable execution stack, or infer a task from Console
membership. NTSRV holds a separate private trace sidecar tied to a
generation-safe service-record identity. It projects Direct nodes from its
authoritative records and receives revisioned Observed deltas from workers
and authenticated hook participants. It authenticates the reporting capability
and real process instances, orders revisions, marks stale/dead or uncovered
sources and serves a copied snapshot. A bare PID, caller-provided parent PID,
pointer or exposed sequence is not an authority.

`src/interface` owns versioned copied DTO/RPC declarations. `worker-base`
contains only mechanisms common to the two workers: validated copied-delta
encoding and an authenticated publication client. The injected hook uses its
finite report client, not worker lifecycle or frontend internals. NTSRV owns
native observation registration/waits; worker-base stores no task registry
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

For NTW32, reuse the delivered launch hook's creation reports. No Job or
completion-port descendant observer is selected. NTW32 reports/authenticates
the direct root; injected controlled parents report actual successful child
creation through the same finite versioned observation contract. Retain a
process-instance reference at creation so an immediately exited child remains
identifiable; a later OpenProcess of a bare PID is not sufficient proof.
NTSRV validates the reporter, worker/session generation, actual parent/child
instances and minimally privileged recipient-owned process attachment before
accepting a relation. Unknown ancestry remains unknown.

NTSRV uses registered/event-driven process waits, not periodic PID/Console
sampling or DLL detach notifications, to mark Observed exit. Handle waits
cover forced exit of a registered process. Creation-before-exit ordering,
concurrent callbacks, cancellation, callback rundown, PID reuse and process
reference release require explicit locks/ownership and tests. These waits are
diagnostic only: NTW32 still waits its actual direct target, performs I/O cleanup
and reports the existing receipt result. An observer must not publish that
receipt, decide BUSY/EMPTY/READY, close a Console or terminate any process.

Uninjected targets, unsupported launch APIs/security/bitness, report failure
and external attachment are explicit coverage gaps; successful native launch
must not become execution failure merely because monitoring is unavailable.
Do not manufacture missing nodes via process-tree scans, Console members,
polling or a fallback Job. The launch package owns installation/propagation;
this package only consumes its source and owns the read-only view. See the
[process wait contract](https://learn.microsoft.com/en-us/windows/win32/procthread/waiting-for-processes).

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
| S1 | Re-audit the renamed graph, delivered hook coverage/creation references, NTSRV process waits, original DOS entry/termination/TSR anchors and existing direct records; freeze DTO, identity/revision, source limits and negative-contract ledger. No implementation claim from research alone. |
| S2 | NTSRV Direct trace projection and authenticated read-only snapshot, using existing DOS/WOW/native records without a second mutable stack. Include malformed, stale, PID-reuse and worker-death tests; NTMON can query a minimal direct-only view. |
| S3 | Consume authenticated hook creation reports and wire NTSRV observation-only process waits to the trace sidecar; short-lived, force-exited and surviving descendants, both bitnesses, gap/parentage uncertainty and callback cleanup. No Job, observer helper or polling; prove direct completion and READY remain unchanged. |
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
direct completion; detached child; nested native CMD; short-lived child exit
before registration processing; forced exit; missing hook/report and unsupported
API/bitness coverage; wait-callback cancellation/rundown; uncertain TSR; DOSONLY on/off; EXEC failure; load-only,
overlay and builtin exclusions; resident reuse; no-Console roots; and NTMON
read-only behavior. Controlled chains show observed behavior but cannot prove
universal no-loss ancestry. No observation may affect Direct admission,
completion, worker lifecycle or management kill. Run MIDL regeneration when
the interface changes, full x86 build plus the launch package's isolated x64
hook verification, focused service/worker/NTMON fixtures, Console and Window
product regressions, and retained DOS/WOW frontiers. Report actual passed,
failed and unsupported rows separately.
