# Project Status

## Current Work

## Active Packet

**Active: M0 T424 S3** — completed delivery/closure record, Ordinary Mode.
Implementation is finished; await owner runtime verification. T remains open;
no further implementation S is active and S4 is not admitted.

| Field | Admitted record |
| --- | --- |
| Identifier Mode | M0 T424 S3, Ordinary Mode; investigate and implement broker-owned component retirement. |
| Candidate Proposal | [Admitted naming package with owner-added investigation](../proposals/proposal-native-worker-frontend-renaming-001.md). |
| Admission And Approval | Owner reports basic S2 acceptance with a lifecycle defect after native commands/possible abnormal exit, and requests an additional investigation S. Insert this as next S3; former frontend rename and final audit become S4/S5. |
| Objective | Repair orphan frontend retirement and repeated-launch failure; NTSRV controls frontend/worker shutdown independently of I/O lease state, with highest-priority shutdown handling. |
| Non-goals | No frontend rename, guest/media change, new helper, scheduler, Job observation, process-tree kill or arbitrary delay workaround. |
| Reference Baseline | S2 `27e85d0a7`, published eight-file package, protocol/RPC 28; S2 acceptance does not waive this reported runtime defect. |
| Files And ABI Surface | NTSRV lifecycle and authenticated worker control; NTKVM shutdown/lease paths; NTVDM/NTW32 control consumers and worker-base; interface declarations and coherent protocol revision if extended; tests, architecture and indexed evidence. |
| Applicable Rules | README reading set, EXECUTION, architecture/coding/document rules and source policy; preserve other-session edits. |
| Verification | x86 incremental affected closure, focused control/admission/abnormal-exit/reuse/independent-root tests, Console17/Window17 and retained WOW frontiers, coherent eight-file publication and hash checks, governance/link/diff review. |
| Expected Markers | Exact command/input, PID and artifact identity, task-completion/lease/ready/retire sequence, wait location and normal-versus-failure contrast; unproved causes labelled hypotheses. |
| Asset Needs | Existing S2 runtime recovery and lifecycle/observer fixtures; new staging/logs/recovery under build/M0-T424/S3, deliberately reuse S2/r001's incremental object cache as recorded in S3 evidence. No new external assets. |
| Reporting Requirements | Report reproduced versus owner-only symptoms, root cause confidence, affected owner and smallest repair/test handoff. Keep live scene until evidence is captured. |
| Stop Conditions | New helper/scheduler, guest changes, loss of previous capabilities or unresolved startup/control safety blocks publication; preserve unrelated edits. |
| Exit Criteria | Broker-owned retirement with highest-priority control, bounded startup exemption, passing regressions and verified publication, reviewed committed/pushed repair P. Keep T open for owner acceptance. |
| Original Owner Request | Basic verification passed, but after run16 cmd and possible abnormal exit, back in CMD a subsequent run16 fails/hangs although the Terminal window remains open; append an S to investigate lifecycle gaps. |
| Similar-Issue Sweep | Normal/abnormal direct completion, root lease return, worker death, broker loss, no-worker grace, failed startup rollback, second launch and independent-session isolation for both workers. |

The completed S3 packet above is retained as its admission/closure record.
Evidence is recorded in [S3 lifecycle repair](../etc/evidence/m0-t424-s3-abnormal-exit-relaunch.md).

## S3 Closure Record

NTSRV now owns orderly frontend/worker retirement. Authenticated shutdown has
priority over pending/lease/I/O and native GetNext, including the initial
pre-presentation wait. Workerless roots retire without an idle prerequisite;
only a live startup owner grants bounded ten-second admission. Existing-worker
reuse does not renew that exemption. Original worker close handling remains
local; there is no guest/shared-library change or new process/scheduler.

Affected x86/MIDL/WOW links pass with protocol/RPC 29. Final Console17 and
Window17 each pass 17/17. Four actual DOS/native worker/frontend failure cases,
same-outer-CMD relaunch, modern EDIT return, and independent native sessions
pass. WINMINE retains its guest main window; SOL/WRITE retain their existing
out-of-memory frontiers, not usability passes. Optional legacy native-worker
fixture and earlier test-harness failures are explicitly retained as non-passes
in evidence; physical desktop/RDP observations remain unclaimed.

All eight O:/winnt hashes match the final tested runtime and published manifest;
previous package recovery is under build/M0-T424/S3/r001/published-recovery.
Guest media, SYSTEM.INI and NTVDM.REG are not overwritten. Z: is removed after
testing. T424 remains open; S4 frontend rename has not started. The owner's
subsequent discussion of moving CreateProcess into NTSRV is not an implemented
change or an admission to extend this completed repair.

Owner's subsequent implementation approval supersedes research-only scope:
NTKVM exits on NTSRV instruction or user Console closure (fault/broker-loss
remain failure exits); workers obey NTSRV shutdown, not frontend-death policy
of their own. An orphan root retires immediately after legitimate startup
admission ends, independently of idle/lease state. NTSRV's ten-second empty
grace begins only after all frontends/workers and legal admissions are gone.

## S2 Closure Record

Owner directs the project Win32 text worker to be named NTW32 and its native
record family Win32Record, across code/configuration/scripts/documentation,
while preserving genuine Console names. The
[S2 evidence](../etc/evidence/m0-t424-s2-native-worker-identity.md) records the
fourteen-file worker move, seventeen test/script moves, NTSRV record spelling,
all consumers and full-tree classification, exact name-only source review,
fresh x86/MIDL/WOW builds, focused identity/lifecycle tests and publication.
Original mirror/shared-library code and guest media are unchanged.

Console17 and Window17 each pass 17/17. Actual COMMAND -> CMD -> modern EDIT ->
CMD -> DOS MEM -> completion passes both staging and the actual O:/winnt
publication. Independent-session management and frontend-loss tests pass;
previous application identity and wrong protocol are rejected by the real
service. WINMINE's guest main window is retained; SOL/WRITE retain their known
out-of-memory frontiers, not usability passes. The supplemental eight-layer
chain ran normally and captured same-worker reuse, but its legacy validator
fails an obsolete EPOCH requirement; its whole-script result is not a pass.
Physical desktop/RDP observation and synthetic Window Ctrl+Q remain unclaimed.

All eight O:/winnt hashes match the verified staging package. The old worker
executable was backed up under build/M0-T424/S2/r001/published-recovery and
removed after verification. SYSTEM.INI, NTVDM.REG and guest media were not
overwritten. Temporary Z: mapping is removed. Frontend is still NTKVM.

## S1 Closure Record

Read-only naming audit delivered as `3c8e6b7d1`, pushed to main. The
[S1 evidence](../etc/evidence/m0-t424-s1-name-referent-audit.md) retains the
complete-tree inventory, original Console exclusions, ABI decision and exact
pre-migration baseline in its sealed inventory and S1 Git revision.

## Admitted T plan

The [T424 working plan](../etc/operations/t424-worker-frontend-renaming-plan.md)
and [proposal](../proposals/proposal-native-worker-frontend-renaming-001.md)
retain five ordered stages. S1 audits names; S2 delivers the native worker
as NTW32 and Win32Record while retaining NTKVM frontend. Added S3 delivers the
approved broker-owned abnormal-exit/re-launch repair before further renaming. S4
renames NTKVM to the reserved NTCON frontend identity after disposition of S3.
S5 owns final referent and semantic audit. T closure
remains owner-controlled. No later queued candidate is admitted here.

## Current Technical Baseline

Published S3 package at O:/winnt: run16.exe, ntsrv.exe, ntvdm.exe, ntw32.exe,
ntkvm.exe, ntmon.exe, WOW32.DLL, VDMREDIR.DLL. APP_VERSION is 0.0.424;
protocol/RPC is 29 with unchanged UUID and a typed authenticated worker shutdown
event RPC. MSVC Win32/x86 /MT CCPU40 is unchanged. Exact hashes and commands are in S3
evidence; predecessor accepted T423 S40 `f64559086` is recoverable.

## Previous T closure

[T423 closure](../history/m0-t423-console-window-runtime-closure.md) records
the owner's 2026-10-02 acceptance and preserves prior status/evidence and
explicit limitations. T423 is closed. T422 remains owner-closed.

## Recent M0 Closures

| Task | Outcome and retained evidence |
| --- | --- |
| T423 | Owner-accepted on 2026-10-02, S40 baseline `f64559086`; [closure and prior status](../history/m0-t423-console-window-runtime-closure.md). |
| T422 | Initial WOW32 and planning milestone; [closure](../history/m0-t422-initial-wow32-closure.md). |

## Recent Governance

The migration's zero gate is zero old project-worker and record referents,
not corruption of original Console APIs/source citations or reserved future
frontend identity. Full-tree audits retain all raw hits and classification.
Approved other-session launch-hook/trace proposals and Queue planning changes
are included without changing this pure naming scope. They remain proposals,
not implemented capability. Final governance, relative-link, diff and
eight-file publication review passed for the S2 P delivery.
