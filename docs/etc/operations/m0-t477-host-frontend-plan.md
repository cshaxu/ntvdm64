# Host frontend implementation sequence

## Admission and baseline

The owner admits the host foundation in an independent worktree from main
869f001e6719cc77e35b44197a75dbd2d3addc72. The sole active S and its full
execution brief are in [Current](../../states/CURRENT.md). This is a working
sequence, not simultaneous S admission or completed implementation evidence.
The [proposal](../../proposals/proposal-host-common-frontend-foundation-001.md)
retains the approved requirements.

## Ownership and invariants

- common/frontend-base owns reusable per-instance frontend channel and
  connection mechanics extracted from NTCON. Native presentation stays NTCON-owned.
- worker-base retains shared backend I/O mechanisms and does not depend on
  frontend-base. NTVDM directly composes common/frontend-base for its N
  downstream frontend instances alongside its existing upstream backend role.
  NTCON also uses frontend-base directly and never depends on worker-base.
- NTVDM owns payload/VM identity, scoped registry and guest adapter. Initial
  enablement is NTVDM-only; future NTVWM adoption can directly compose the same
  frontend-base without introducing a worker-base dependency on it.
- NTSRV owns authenticated associations, active-worker handoff and lifecycle.
  It does not relay I/O. Every frontend has at most one active worker.
- A multitasking payload remains active on its upstream chain while its VM
  frontends initiate independent downstream chains, including nesting.
  Downstream launch never steals the containing payload's upstream frontend.
- Scope VM keys to worker/payload lifetime and reject stale generations;
  never use a bare process ID or VM handle as global frontend identity.
- Reuse current common console protocol/client and lib-win32 transport,
  existing NTSRV lifecycle and cleanup. Do not duplicate their authorities.

## Sequential stages

| Stage | Bounded outcome | Required proof |
| --- | --- | --- |
| S1 | Extract common/frontend-base and reconnect NTCON. Audit and separate presentation callbacks from connection state, cancellation and cleanup; remove displaced duplication. | NTCON input/output, handoff, rollback and disconnect regressions; dependency and per-instance tests. |
| S2 | Extend NTSRV registration, identity and handoff to multiple frontend instances per authenticated host. Reuse existing lifecycle decisions and client transport. | Two identities on one host, independent chains, wrong/stale identity rejection, completion in both orders and host-loss cleanup. |
| S3 | Connect NTVDM directly to common/frontend-base and bind its scoped payload VM registry to independent frontend instances. Keep worker-base free of frontend-base dependencies. | 1+N isolation, dependency direction, VM-handle reuse, retirement, retained upstream ownership, nested host-fixture chains and cancellation under failure. |
| S4 | Extend NTMON's existing management view to typed VM frontends and complete integrated host acceptance. | Owner/payload/VM and chain display, unchanged NTCON rows and management behavior, full regression and coherent published package. |

S1 is closed in the [S1 evidence](../evidence/m0-t477-s1-frontend-base.md).
S2 is admitted for [audit/design](m0-t477-s2-frontend-instances-design.md); later stages require sequential admission. Each production P includes implementation,
reproducible tests, build, review, coherent publication and delivery evidence
under the execution rules; a preparatory source move alone is not a completed
frontend capability. Exact P splits follow the audited source boundaries.

## Guest follow-up boundary

The separate queued guest driver supplies actual VMM VM identity, asynchronous
cmdExec32 integration and guest I/O semantics. Host fixtures exercise real
production owners but do not prove a functional guest VxD. This task neither
rewrites immutable guest media nor claims real guest multitasking acceptance.

## Admission verification

Admission changes planning/state documents and corrects the documentation gate. Run the documentation
governance gate (including relative links) and git diff --check. No product
build or release refresh is claimed for this documentation-only checkpoint.

The former gate derived the next number solely from closed-task history and
rejected this owner-directed admission because the main baseline still has an
active task. The gate now also reads the committed baseline packet: it permits
its continuation or the next T at S1, without fabricating a predecessor closure.
The independent main task and its uncommitted work remain untouched.

Admission validation: owned background PID 40912 ran
`pwsh -NoProfile -ExecutionPolicy Bypass -File tools/governance/Verify-DocumentationGovernance.ps1 -SelfTest`
from this worktree on 2026-11-05 and exited 0. Both self-test and full
documentation verification (including relative links) passed.
`git diff --check` passed. This is an admission checkpoint, not an S1 product
delivery; no product source, binaries or guest inputs changed.
