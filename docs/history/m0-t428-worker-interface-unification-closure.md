# M0 T428 worker interface unification closure

Owner directs closure on 2026-10-04: “先收口t任务吧 准入下一个”. This
supplies the final owner acceptance previously pending after S6 delivery.
Production P 68e860553 and status P b1b77e129 are pushed to main/origin.
The next queue-head package is admitted separately as T429 S1 in CURRENT.

## Delivered scope and provenance

| Stage | Delivered result and evidence |
| --- | --- |
| S1 | Owner-approved [source/ownership audit](../etc/evidence/m0-t428-s1-worker-control-lifecycle-audit.md); source-only, included with S2. |
| S2, 6a8934f6b | [Shared GUI residency](../etc/evidence/m0-t428-s2-shared-gui-worker-residency.md); removed added native GUI-only idle retirement. Its initial reuse gap was resolved and retested in S5, not retrospectively marked passed. |
| S3, ae28ef7ce | [Authenticated parent resume](../etc/evidence/m0-t428-s3-authenticated-parent-resume.md); run16 no longer selects a parent from Console membership; existing service origin and exact completion authorize restoration. |
| S4, 886f13758 | [Common worker shutdown](../etc/evidence/m0-t428-s4-common-worker-shutdown.md); actual shared close mechanism, pre-text native control registration and local closure proof. |
| S5, e81e9fc9f/db855a66c | [Association and paired cancellation](../etc/evidence/m0-t428-s5-worker-association.md); existing watch authority, same-worker GUI reuse, phase-based pending/delivered/closed lease rundown and real close-race repair. |
| S6, 68e860553; status b1b77e129 | [Ownership names and whole-T audit](../etc/evidence/m0-t428-s6-ownership-naming.md); displaced aliases/callers/build selections removed, unchanged shared close body separately linked, exact lifecycle fixtures corrected and integrated delivery passed. |

NTSRV's existing WORKER_WATCH owns logical frontend-worker association;
no second registry, scheduler, helper or descendant observer is added.
run16 remains a thin submitter/result waiter. NTCON is presentation-only;
common carries neutral protocol/client/transport and worker-base carries the
matching worker-only mechanisms. Cancellation and acknowledgment distinguish
I/O lifetime from command completion. NTVDM's original execution, records,
blocking/resume and cleanup remain in src/mvdm/src/opennt-host, unchanged by T428.
Native real target/Console operations remain worker-local rather than becoming
synthetic DOS records. Source-owned differences are explicitly audited, not
hidden by neutral wrappers or mass renaming alone.

Owner confirms no new exclusive GUI launch option. Default native GUI carriers
reside/reuse like shared WOW. Exclusive text retains the existing CloseOnExit
boundary; borrowed/nested requests cannot retire the root. Native30ms polling,
CLI, kind0/1/2 and APP0.0.427/RPC38/I/O25 remain unchanged.

## Verification and publication

The final tested set is build/M0-T428/S6/r002/runtime/system32, published at
O:/winnt/system32: six EXEs plus WOW32.DLL and VDMREDIR.DLL. MSVC14.43,
SDK22621, x86 /MT CCPU40. S6/r007 Product passes Console17 (65940ms),
Window17 (75912ms) and the three retained WOW frontiers (67276ms), total214653ms.
S6/r008 passes29 production-linked service cases; shutdown passes37 assertions
with stable handles; sequential physical lease fixture covers34 joins/closures.
S6/r009 passes six actual GUI cases including reuse and survival after carrier
management close. S6/r010 passes paired frontend loss/receipt1067, actual close,
independent input/exit23, worker-loss target survival and explicit frontend close.

S6/r011 preserves a coherent S5 recovery and publishes all eight tested hashes.
S6/r012 published Console/Window empty/native-zero/MEM/EDIT smoke and eight
hashes pass. Final source/lock/resource/diff, ownership negative controls,
governance and relative links pass. Failed attempts remain in the S evidence.
Guest media, NTVDM.REG and configuration are untouched.

This closure/admission is documentation-only: no rebuild, deployment or process
termination is needed. Existing performance planning is preserved and admitted
through Queue/CURRENT; unrelated planning edits are not discarded.

## Retained limits

Physical RDP/focus is owner-waived/unobserved, not passed. WOW retains its
established frontier nonregression contract rather than full application
usability or gameplay. Host scrollback and descendant observation are not
claimed. NTVDM hot-loop/input/presentation performance is assigned to the
following admitted task, not asserted fixed by worker-interface unification.
The [implementation plan](../etc/operations/t428-worker-unification-plan.md)
is retained historical supporting evidence; CURRENT alone admits live work.
