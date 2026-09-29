# Project Status

## Current Work

**No active M/T/S packet.**

M0 T423 S12 implementation and verification are complete. Its P2 delivery
contains the independent NTCON worker, common worker mechanisms, production
integration, obsolete-backend removal and the accumulated reviewed tests.
The coherent eight-file package is published at O:/winnt and post-publication
checks pass. Await owner side-test acceptance; do not close T423 automatically.

T423 remains open. S13 product-experience/component-lifetime work is planned
but not admitted by this closure. It is not the cancelled RDP packet. Do not
start it while waiting for the owner's next instruction.

Authorities: [T423 proposal](../proposals/proposal-kvm-window-graphics-presentation-001.md),
[S12 source/test ledger](../etc/evidence/m0-t423-s12-ntcon-backend.md), and
[Queue](QUEUE.md). The detailed S12 chronology, failed attempts and owner
boundary changes remain in the ledger; they are not current open gates.

## Current Technical Baseline

- MSVC Win32/x86 /MT CCPU40; no guest or shared-library modifications.
- Runtime: run16.exe, ntsrv.exe, ntvdm.exe, ntkvm.exe, ntcon.exe, ntmon.exe,
  WOW32.DLL and VDMREDIR.DLL. Application 0.0.423; service protocol 16,
  copied Console protocol 17, native request protocol 4.
- NTVDM owns original DOS/WOW execution. NTCON owns native text execution
  and its ordinary hidden Console, without ConPTY or a private helper.
  NTSRV handles authenticated registration and management; NTKVM owns visible
  Console/Window and the common frame renderer. run16 waits for direct results.
- worker-base owns matching project-added worker connection/client mechanisms:
  ordered transfer, validation/cancellation, frame chunks, input codec,
  activation/key return and client event lifecycle. Original mirror execution,
  scheduling, task completion, blocking/resume and cleanup remain in place.
- Native actual Console members are independent of worker residency. Returning
  to DOS does not destroy NTCON; direct target completion does not kill its
  surviving descendants. Parent output waits for the final presentation fence.
- Cross-component declarations are under interface. The displaced frontend
  executor, ConPTY parser/carrier and duplicate native renderer are removed.

Formal caches remain under build/M0-T423/S1/restart-formal-x86 and
restart-wow-x86; selected source/build inputs, provenance and run evidence are
recorded by S12. Candidate tests use build/M0-T423/S12/p. Formal publication
backup and exact old/new SHA-256 manifest are under
build/M0-T423/S12/publication-backup-r131. O:/winnt is the usable package,
not the build directory. Existing original guest/configuration hashes were
checked unchanged before publication; NTVDM.REG/user state was not replaced.

## S12 Closure Evidence

| Requirement | Verified evidence |
| --- | --- |
| Native execution, registration/reuse, version/auth and failure cleanup | Actual NTCON RPC/public-launch tests; concurrent creation, forged/stale context, stream/EOF, direct results and failed export/launch cases in S12 ledger. |
| Common mechanisms and owner boundaries | r106-r120 provenance audit, both production worker links, strict frontend leakage negative controls; transport fixture 336/0 and execution lifecycle 333/0 with zero remaining handles. |
| DOS/native I/O, completion barrier, key return | Real production round trips, copied input FIFO/negative tests, real unread Console return, final-ack failure/EOF and resume failure fixtures; original DOS block/resume remains the caller. |
| Nesting and isolation | r130 DDWWDDWW: sixteen input/output checkpoints, same worker identities and restored original DOS depths/tasks. Both twelve-target chains pass separate frontend groups and final retirement. |
| Members, management and failures | Surviving attached client in both display modes; expanded lifecycle faults and two-session management/frontend/worker-loss tests; unrelated session survives and direct worker failure returns 1067. |
| Published DOS regression | r131 Console17 and Window17 each 17/17, guest text and interaction checked. r132 CMD return and surviving-client checks pass in both modes. |
| Mouse | r128 candidate and r132 published burst/retire/latency: 1000/1000/200 records, guest PASS and input-sink acknowledgment. Physical desktop focus/clipping remains owner-waived, not passed. |
| WOW non-regression | r127 candidate and r132 published: WINMINE main window; SOL and WRITE original OOM frontiers. Separate headless observations, not full SOL/WRITE acceptance or interactive play. |
| Publication | All eight published hashes match the tested formal candidate; final process query empty. Old coherent seven-file set retained for recovery. |

Known immutable-guest limitation: sufficiently large inherited environments can
overwrite COMMAND's discarded INIT references. r126 matches the original
S35 binary/source defect, including with DOS=HIGH. It is registered in
[TODO](TODO.md), not fixed or counted as a passing capability. No environment
truncation, guest patch or allocator workaround is introduced. Historical r70
lacks the same memory witness and is not independently attributed by resemblance.

## Recent M0 Closures

S12 is the latest implementation closure; owner side-test acceptance is pending.
S11 965083eec remains the recoverable owner-accepted baseline;
[S11 evidence](../etc/evidence/m0-t423-s11-interaction-retirement.md).
Earlier T423 stage records are linked by the proposal and S12 ledger.
T422 remains owner-closed; T423 itself is not closed.

## Recent Governance

This delivery preserves and includes the owner's authorized side-chat Queue/WOW
proposal changes. It does not admit those candidates or S13. No forced push,
guest/lib change, new scheduler, recursive kill or helper is part of S12.
Commit/push and repository synchronization are verified as the final delivery
step; source/test evidence cannot substitute for that check.
