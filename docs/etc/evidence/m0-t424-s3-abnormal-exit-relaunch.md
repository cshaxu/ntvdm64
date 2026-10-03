# T424 S3 abnormal-exit and re-launch investigation

T424 S11 naming normalization: frontend labels/current source links now use NTCON.
This does not claim the new basename existed in the recorded historical package.
Exact earlier source, commands and product names remain in Git and sealed build
evidence; recorded hashes, dates, results and limitations are unchanged.

## Question and baseline

Owner basically accepts S2 but reports native commands and an apparent abnormal
exit back to outer CMD. Subsequent run16 cmd and run16 command both hang before
any prompt, although Terminal remains open. NTMON shows Ready and no worker.
Baseline: `27e85d0a7`, published S2 eight-file package, protocol/RPC 28.
The initial read-only investigation below is not a runtime pass. The later
owner-approved repair and its separate verification are recorded afterwards.

## Initial read-only procedure and observations

On 2026-10-02, from the main worktree:

```powershell
Get-Process run16,ntsrv,ntcon,ntvdm,ntw32 -ErrorAction SilentlyContinue |
  Select-Object Id,ProcessName,StartTime,Path
Get-Process ntcon,ntsrv -ErrorAction SilentlyContinue | ForEach-Object {
  $_ | Select-Object Id,ProcessName,StartTime,CPU,HandleCount
  $_.Threads | Select-Object Id,ThreadState,WaitReason
}
```

Only NTCON PID 18804 and NTSRV PID 10616 were visible, from O:/winnt. At the
second observation they had survived about eight minutes. No run16, NTVDM or
NTW32 appeared. NTCON had 183 handles/four waiting threads; NTSRV had 117
handles/three waiting threads. No process was killed or input/buffer changed.
Process-name enumeration is not an authenticated broker-state snapshot;
WaitReason does not reveal the actual wait stack or prove deadlock.

## Source observations and confidence

- run16 frontend_scope.c restore waits on restored/root indefinitely.
- NTCON session_service.c parks a borrowed root, resets retire and calls
  FrontendLeaseReady after usage drains, retaining resident worker channels.
- NTSRV service_root_workerless requires borrowed idle root, no join caller,
  no closing state, no live associated worker, pending or task before grace.

Thus a visible no-worker root alone does not prove its timer should have fired.
A stranded lease/join/pending state, blocked frontend pump or missed retirement
notification are hypotheses. None is yet a root-cause conclusion. NTMON Ready
proves its broker query succeeds, not that the root/channel is responsive.

## Follow-up and limits

Capture authenticated root/admission state and low-disturbance wait stacks;
trace normal versus abnormal completion -> final I/O -> receipt -> restore ->
lease-ready -> next admission. Privately reproduce target/launcher/worker
failure and repeated launches for both workers; check independent-session
isolation and broker's workerless grace. Deliver the smallest owner-local
repair recommendation and exact regression cases before implementation.
NTMON is worker/task presentation, not a frontend census; absence of NTCON
there is expected and does not settle its state.

At this investigation checkpoint no new build, publication, runtime pass or
repair was claimed. The implementation record below supersedes that checkpoint.

## Bounded thread probe

Reuse the existing read-only observer, not a product dependency:

```powershell
& build/M0-T423/S9/backend-r2/worker-thread-snapshot.exe 18804
& build/M0-T423/S9/backend-r2/worker-thread-snapshot.exe 10616
```

Sandbox-only capture returned WCT error 1444 and no stack/module evidence.
Elevated capture succeeded; the observer briefly suspends each inspected
thread to read/resume its context. It neither kills processes nor changes UI.
NTCON module base 00CF0000; its pump thread 20676 has frame 00CF1D17, mapping
to frontend_pump (preferred 00401C00) in the S2 ntcon.exe.map. Main thread
29856 waits on thread 20676; WCT reports no cycle. Other frontend threads are
waiting. NTSRV base 00DE0000, main wait frame 00DE37F0; RPC threads are idle in
this snapshot. This supports a live but waiting frontend pump rather than a
proved cyclic deadlock. The event identities and broker idle/pending fields
are not captured, so the lease hypothesis remains unproved.

## Owner-approved implementation

The owner subsequently authorizes broker-owned retirement and requires NTSRV
instructions to take precedence for NTCON, NTVDM and NTW32. The corrected
contract does not depend on identifying which inaccessible old lease field
pinned PID 18804: source inspection proves that the previous borrowed/idle/join/
pending gates could veto orphan retirement, and that the frontend could skip
the broker close check after a pending/tasks result. The live scene did not
prove the exact internal field; do not claim it did.

- NTSRV decides root retirement using its registered worker associations, not
  frontend idle or pending/task leftovers. A live authenticated startup owner
  grants only a ten-second admission deadline; existing worker registration
  consumes that exemption, and reuse of an already registered worker does not
  grant another startup window. There is no post-start orphan grace.
- NTSRV registers a one-shot root process wait whose process-handle copy is
  owned until unregister/join, independently of registry-handle disposal.
  Death wakes the broker's existing lifetime reconciliation; it does not
  directly run a worker cleanup inside the callback.
- NTSRV creates a manual-reset worker close event and exports synchronize-only
  duplicates only to the exact authenticated registered worker generation.
  It signals the event when that worker loses its frontend association.
  Reconciliation also wakes the existing GetNext condition variable. Native
  GetNext checks close before taking another command, including its first
  pre-presentation wait; tests assert cancellation without returned handles.
- NTCON checks the authenticated close decision before joins, channel work,
  pending/tasks and lease restoration. Direct completion parks both borrowed
  and dedicated roots; it no longer makes a dedicated root self-retire.
- NTVDM/NTW32 replace direct frontend-process lifetime waits with the broker
  event in the existing wait-set, before ordinary input/quit work. Original
  DOS close callback/bounded Console-close handling and native real Console
  closure/acknowledgment stay local. Unrecoverable fault, broker loss and user
  closure retain their failure/resource boundaries. Shared WOW without a
  character frontend is not fabricated into a root association.
- NTSRV's existing empty predicate already requires no registry entries,
  worker watches or reservations; frontends and legitimate launch admissions
  therefore prevent its ten-second empty grace. NTMON observations do not pin
  that registry. No second empty census was introduced.

No guest, OpenNT/MVDM mirror or shared KVM library is modified. No helper,
process-tree kill, Job observation, new task registry or scheduler is added.
The established NTW32 30ms capture sampling remains explicitly unchanged.

## Build, artifacts and focused verification

New logs/staging/recovery are under build/M0-T424/S3/r001. Reuse the existing
S2/r001 Ninja object cache intentionally; it is an incremental build cache,
not a claim that the old S2 compilation outputs are immutable recovery media.
The separate S2 runtime and S3 published-recovery eight-file sets retain the
old products. Regenerate New-T310OriginalSoftpcNinja.ps1 with x86 and the S2
cache root, then run run-ninja-parallel.cmd product-programs and selected
fixture targets. Run build-supplement.cmd to relink WOW32 against the rebuilt
parent import library. MIDL regenerates all consumers with protocol/RPC major
29, unchanged UUID and application 0.0.424. New typed shutdown RPC requires
this coherent revision; no mixed 28/29 package is published.

| Check | Actual result / boundary |
| --- | --- |
| basesrv-service-reservation-test --frontend-authority | Pass both borrowed/dedicated roots without LeaseReady, bounded live startup exemption, orphan retirement, generation rejection, non-worker rejection, synchronize-only event, frontend-loss instruction and no active process kill. |
| frontend-scope-lifetime-test | Pass park without self-retirement, broker close overriding stale pending, nested ownership, joined channels and resource lifetime. |
| ntw32-next-command-test | Pass command ownership and failure disposal/completion. |
| ntw32-text-frame-test text-frame-01.txt | 133 checks, zero failures; production receiver, not a real transport test. |
| ntw32-execution-lifetime-test execution-01.txt | 586 checks, zero failures; actual target result, canceled requests, completion failure, target survival and zero residual handles. |
| console-channel-lifetime-test --private-desktop-full channel-01.txt | Pass actual Console/geometry/input/cursor/channel/park/resource invariants; no physical desktop observation. |
| frontend-request-client-test | Pass final/resume presentation barriers and structured errors. |
| worker-identity-version-test new-ntsrv old-S2-run16 | Pass wrong application identity/protocol and real previous RPC client rejection before target launch; RPC major equals protocol. |
| verify-broker-retirement.ps1, prefix t424-s3-complete2-retirement | Pass final-package DOS/native worker-loss and frontend-loss: failed direct receipt, broker-ordered peer retirement and subsequent empty-service exit. Fault injection is test-only. |
| verify-frontend-relaunch.ps1 relaunch-complete2.txt | Pass final-package one outer CMD: DOS MEM/EXIT, native VER, DOS MEM/EXIT, cooked CMD result 19; both MEM outputs checked. |

The first relaunch-01 probe returned outer CMD result 19 but incorrectly
required an MS-DOS VER string in an immediate input snapshot. It is not a
pass. The revised test uses the existing DOS MEM marker and the subsequent
processed snapshots; it strengthens the actual DOS workload evidence rather
than accepting input echo. The optional legacy --native-worker service fixture
also failed at RequestFrontend after replacing its retained root. Its existing
RetainFrontendRoot generation/Console guards were not relaxed; it is not
counted as a passing test or a substitute for the real authenticated runtime.
The first final-package retirement attempt passed both DOS cases but sampled
the native worker immediately after registration, before its target creation.
It is not an all-case pass. The test now waits within the same finite startup
deadline for the actual pinned CMD before fault injection; it does not change
the production ordering, loosen fault assertions or add product polling.

Publication occurred after the x86/focused gate and same-Console relaunch, per
the owner's release-before-full-matrix direction. All eight O:/winnt files
match runtime/ hashes in published-sha256.json. Recovery is in
published-recovery/; original guest files, SYSTEM.INI and NTVDM.REG were not
overwritten. Publication alone is not T acceptance.

## Final sealed-package verification and S closure

The final production inputs include the pre-binding GetNext close check and
no renewed admission grace when reusing an existing registered worker. No
production source changed after the final build/publication. Exact run root is
build/M0-T424/S3/r001; observer/fixtures reuse build/M0-T424/S2/r001's verified
incremental build outputs. PackageRoot=Z:/ and ProcessPackageRoot=run-root/runtime
for the isolated scripts; reports stay in run-root. Commands/results:

| Entrypoint and final run evidence | Result |
| --- | --- |
| Product route matrix, prefixes t424-s3-complete-console17 / t424-s3-complete-window17, respective summary.json | 17/17 each, guest-text and interaction assertions retained. |
| verify-broker-retirement.ps1 -Observer cache/observer.exe -PackageRoot Z:/ -ProcessPackageRoot run-root/runtime -LogRoot run-root -LogPrefix t424-s3-complete2-retirement | Four DOS/native worker/root death cases pass; failed receipt, ordered peer shutdown and empty broker retirement verified. |
| verify-frontend-relaunch.ps1 -Observer cache/observer.exe -PackageRoot Z:/ -ReportPath run-root/relaunch-complete2.txt | Both DOS MEM workloads, native VER, one outer CMD and result 19 pass. |
| verify-command-native-edit-return.ps1 -Observer cache/observer.exe -PackageRoot Z:/ -ReportPath run-root/edit-return-complete2.txt | Actual modern EDIT menu/document, Ctrl+Q, CMD echo, DOS MEM and direct completion pass. |
| verify-ntw32-management.ps1 -Observer cache/observer.exe -MonitorRpc cache/monitor-rpc-test.exe -PackageRoot Z:/ -ProcessPackageRoot run-root/runtime -LogRoot run-root -LogPrefix t424-s3-complete2-isolation -TwoSessions | Acknowledged real Console closure passes; other Console accepts new input and returns 23 without being terminated. |
| observe-wow-frontiers.ps1 -Observer cache/observer.exe -WindowObserver build/M0-T423/S9/publication-window-reader.exe -PackageRoot Z:/ -ProcessPackageRoot run-root/runtime -LogRoot run-root -Prefix t424-s3-complete2-wow -PostExitObservationMs 5000 | WINMINE guest main window remains live; SOL and WRITE retain previous original memory-error dialogs. Not gameplay/usability acceptance. |

Final hash review compares each of the eight staging files with O:/winnt and
published-sha256.json, all equal. Test products are cleaned by exact isolated
package identity; Z: mapping is removed. Documentation governance, relative
links and diff checks pass. Reviewed side-session proposal/queue changes are
included as planning only, not runtime implementation. S3 is delivered; T424
remains open for owner acceptance and S4 is not admitted here. The exact old
owner scene's inaccessible lease field and physical Terminal/RDP interaction
are not retrospectively asserted as independently reproduced/passed.
