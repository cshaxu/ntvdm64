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

| Stage | Outcome and decisive evidence |
| --- | --- |
| S1 (active) | Source-first contract: both BOP call chains, escaping the unreturned host stack, precise VM identity, VMM wait/resume legality, toolchain and NIDDB decision. Freeze ABI and acceptance matrix. |
| S2 (planned) | Project-owned VxD registration, payload/VM generations and retirement connected to the host registry; verify each supported OS/load mode. |
| S3 (planned) | Asynchronous 54:08/54:0A continuation and child-chain completion; two VMs in both completion orders, prove actual guest caller resume while sibling child remains alive. |
| S4 (planned) | VM-specific input/output, final-output handoff, cancellation, nested chains and real Console behavior; preserve single-task/GUI/redirection paths. |
| S5 (planned) | Integrated failure/lifecycle matrix, coherent cached build, full regression, publication and closure audit. |

Only S1 is admitted. Stage boundaries may be refined from concrete source
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
