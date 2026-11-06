# Guest multitasking bridge mainline and side-task policy

## Admission

The owner accepts T477 and admits this guest mainline on 2026-11-05, then
explicitly postpones importing the adjacent worktree's unfinished commits.
Baseline is cd8c8583e. T478 is already allocated in
O:/repos.hobby/ntvdm64/.worktrees/mirror-source-audit: its CURRENT records S2
closure and S1 inventory limitations, but its Git HEAD remains 5ec7351c9 and
its S1/S2 changes are uncommitted at inspection. No commits were imported,
no external working files were changed, and none of those results are claimed
as integrated. Reserve T478 and allocate T479 rather than reuse its number.
[Current](../../states/CURRENT.md) owns the only local active packet.

## Mainline contract

Consume [the guest proposal](../../proposals/proposal-multitask-guest-vdm-frontends-001.md)
and the delivered T477 foundation. A containing Win3.1 enhanced/Win95 payload
keeps running and retains its upstream frontend when a DOS VM invokes either
54:08 or 54:0A. The exact calling VM gets an independently authenticated
frontend and downstream chain. That VM may wait through a proven VMM contract;
other VMs and the containing guest must continue. No identity inference from
focus, CR3, PID or recent scheduling is permitted.

NTSRV owns association and lifecycle, frontend-base owns connection mechanics,
NTVDM owns guest context/identity/adaptation, and the project VxD bridges VMM
services. Original guest Windows components remain immutable. Independently
authored add-on source/build/load provenance must be recorded before code or
media work; this admission is not permission to patch existing installations.

## Sequential bounded stages

Owner naming direction during S4: all independently authored `.386` / VxD
implementation belongs to `src/addon/ntvdm64-386/`. The validated release
artifacts are `assets/release/ntvdm64.386` (Win3.1 enhanced mode) and
`assets/release/ntvdm64.vxd` (Win95). Build/probe outputs remain task-owned;
neither artifact is a host-system32 component. This names the admitted bridge,
not a completed runtime capability or permission to change original guest.

| Stage | Outcome and decisive evidence |
| --- | --- |
| S1 (closed research) | Original call chains and candidate guest bridge, with executable proof explicitly deferred. |
| S2/host-foundation (delivered) | Independent VM registry lifetime without lock-held RPC/joins; cancellable provider dispatch; paired frontend/execution capabilities. Concurrent two-VM and negative lifecycle tests plus product delivery. |
| S3/host-command-preparation (delivered) | Per-request command/environment/CWD/standard handles/results without process-global mutation. Preserve ordinary DOS path; parallel host fixtures and redirection rollback. No guest multitasking entry enabled yet. |
| S4/guest-driver (active) | Project VxD build/load, actual VM identity and inventory, generation/retirement, callback and counted wait/wake through a claimed emulated notification resource. Prove Win3.1 enhanced and Win95 independently. |
| S5/multitask-command | Both 54:08/0A asynchronous execution and original continuation/reentry semantics, including complete client state restoration; two-VM completion in both orders. |
| S6/guest-io | Complete per-VM text/graphics/input/Console contract, final-output serial, cancellation and nested chains; actual guest acceptance. |
| S7/integration | Complete failure/lifecycle and product regression, coherent cached full build, publication and final T acceptance. |

S1 is closed and S2/guest-bridge is admitted. Stage boundaries may be refined from concrete source
findings, without dropping the full proposal requirements. Tests must prove
actual guest recovery rather than screenshots or host-child exit alone.

## Mixed-task discipline

Future owner-designated side commits may enter this T in separately admitted
bounded S/P deliveries. Record source commit ranges, preserve history, audit
conflicts against current mainline, and verify the combined source before
publication. Imported CURRENT/queue cannot replace local active authority;
foreign release binaries cannot silently replace the newer coherent package.
Do not import uncommitted work or count external test evidence as combined
runtime proof. No side task is currently active or required before S1.

## Admission evidence

This checkpoint changes planning/governance only. The published T477 package
is retained; no product rebuild or runtime acceptance is claimed. Run the
documentation governance self-test/verification and diff checks before delivery.

Admission validation: owned background PID 18504 started at
2026-11-05T23:07:34.5415727Z and exited zero. Documentation governance
self-test and verification passed, including the explicit adjacent-task
reservation at S1. git diff --check passed. No runtime source or released
image changed; this is an admission checkpoint, not S1 capability closure.

## Superseding owner direction: committed side work and duplicate S labels

The owner subsequently authorizes importing all adjacent commits and permits
one S number to describe different content within this mixed T. Qualify each
record with its workline/topic (S1/guest-bridge versus S1/generic-thunk-import),
its source T/S/P and commit range. Never combine their acceptance or silently
mark the mainline done. This replaces the earlier deferral above.
The imported range is e1867aaa2 through cd91c8a57, based on cd8c8583e;
see [integration evidence](../evidence/m0-t479-s1-thunk-import.md).

S1/generic-thunk-import is delivered with coherent 0.0.479 publication and
Full 16/16 acceptance. The owner now closes S1/guest-bridge on its current
research results; see the [closure and mandatory handoff](../evidence/m0-t479-s1-guest-research-closure.md).
This supersedes the earlier active-S1 wording and moves the unproved executable
contracts to the named S2--S5 gates, without declaring them passed. T479 remains
open; no next S is admitted by this closure.

## S2 shared-library side stage

The owner now admits S2/shared-library-import, recorded in its
[audit](m0-t479-s2-shared-import.md), including generic shared-test consolidation.
This is the sole active packet. Planned S2/guest-bridge retains its qualified
identity under the mixed-task rule; no guest implementation or proof gate is
silently closed or renumbered by this side work.

## Accepted shared stage and resumed guest mainline

Owner acceptance closes S2/shared-library-import at 967ef3d31, retaining the
published and tested package. This supersedes the sole-active-side-stage
wording above. S2/guest-bridge is now the sole active packet.
The [design boundary audit](m0-t479-s2-guest-design-boundaries.md) resolves
source-confirmed lock, cancellation, launch-context and global-command hazards,
and assigns executable guest proof and complete I/O acceptance to S2--S5.
It does not close S2 registration/runtime acceptance or narrow the proposal.


## Approved continuous execution

The owner approves the host-first S2--S7 sequence above and a persistent goal.
Each stage must finish cached build, applicable tests, coherent publication,
reviewed commit and push before advancing automatically to the next stage.
No repeated stage approval is required within this scope. Original guest bytes
remain immutable. Earlier S2--S5 references describe the superseded grouping;
the complete command, guest and I/O obligations now map to S3--S7 above.
New semantic concessions are not authorized by continuous execution.

Owner's subsequent execution priority is Win3.1 enhanced mode first: implement
and exercise its actual bridge, wait/wake and multitask command path before
resuming Win95 work. Preserve the completed Win95 load/callback evidence and
the remaining obligations; this priority change neither drops Win95 from the
T objective nor turns its untested paths into acceptance.


S2 host foundation is [delivered](../evidence/m0-t479-s2-host-foundation.md)
with the complete tested/published package. Automatic continuation admits S3;
no guest implementation gate has been claimed satisfied by this host delivery.

S3 host preparation is now [delivered](../evidence/m0-t479-s3-command-preparation.md)
with complete cached builds, focused units, Full 16/16 and coherent publication.
Automatic continuation admits S4; source registration is not substituted for
actual guest callback, wait/wake or resource-lifetime acceptance.
