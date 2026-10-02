# T424 S3 abnormal-exit and re-launch investigation

## Question and baseline

Owner basically accepts S2 but reports native commands and an apparent abnormal
exit back to outer CMD. Subsequent run16 cmd and run16 command both hang before
any prompt, although Terminal remains open. NTMON shows Ready and no worker.
Baseline: `27e85d0a7`, published S2 eight-file package, protocol/RPC 28.
This is investigation evidence, not a repair or runtime pass.

## Initial read-only procedure and observations

On 2026-10-02, from the main worktree:

```powershell
Get-Process run16,ntsrv,ntkvm,ntvdm,ntw32 -ErrorAction SilentlyContinue |
  Select-Object Id,ProcessName,StartTime,Path
Get-Process ntkvm,ntsrv -ErrorAction SilentlyContinue | ForEach-Object {
  $_ | Select-Object Id,ProcessName,StartTime,CPU,HandleCount
  $_.Threads | Select-Object Id,ThreadState,WaitReason
}
```

Only NTKVM PID 18804 and NTSRV PID 10616 were visible, from O:/winnt. At the
second observation they had survived about eight minutes. No run16, NTVDM or
NTW32 appeared. NTKVM had 183 handles/four waiting threads; NTSRV had 117
handles/three waiting threads. No process was killed or input/buffer changed.
Process-name enumeration is not an authenticated broker-state snapshot;
WaitReason does not reveal the actual wait stack or prove deadlock.

## Source observations and confidence

- run16 frontend_scope.c restore waits on restored/root indefinitely.
- NTKVM session_service.c parks a borrowed root, resets retire and calls
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
NTMON is worker/task presentation, not a frontend census; absence of NTKVM
there is expected and does not settle its state.

Investigation remains open. No new build, publication, runtime pass or repair
is claimed. The verified S2 package is unchanged.

## Bounded thread probe

Reuse the existing read-only observer, not a product dependency:

```powershell
& build/M0-T423/S9/backend-r2/worker-thread-snapshot.exe 18804
& build/M0-T423/S9/backend-r2/worker-thread-snapshot.exe 10616
```

Sandbox-only capture returned WCT error 1444 and no stack/module evidence.
Elevated capture succeeded; the observer briefly suspends each inspected
thread to read/resume its context. It neither kills processes nor changes UI.
NTKVM module base 00CF0000; its pump thread 20676 has frame 00CF1D17, mapping
to frontend_pump (preferred 00401C00) in the S2 ntkvm.exe.map. Main thread
29856 waits on thread 20676; WCT reports no cycle. Other frontend threads are
waiting. NTSRV base 00DE0000, main wait frame 00DE37F0; RPC threads are idle in
this snapshot. This supports a live but waiting frontend pump rather than a
proved cyclic deadlock. The event identities and broker idle/pending fields
are not captured, so the lease hypothesis remains unproved.
