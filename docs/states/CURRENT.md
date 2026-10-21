# Project Status

## Current Work

**Active: M0 T448 S1**

T447 closed by [explicit scope transfer](../history/m0-t447-transfer-closure.md)
at pushed commit 4d4ef4d47, with a clean worktree verified before this admission.
Owner authorizes T448: NTSRV lifecycle authority unification. All remaining
WOW32 work is now the first [queued successor](QUEUE.md). Neither admission nor
the prior checkpoint claims that the CAF defect is universally fixed.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T448 S1; Ordinary Mode, single person implementer/reviewer; no subagents. |
| Candidate Proposal | [NTSRV lifecycle authority](../proposals/proposal-ntsrv-lifecycle-authority-001.md). |
| Admission And Approval | Owner explicitly requests clean T447 closure, then admission of the lifecycle-unification task; closure pushed before this admission. |
| Objective | Complete the source-backed ownership inventory and finite transition/implementation design for every managed role, including Console return. |
| Non-goals | No guest modification/rebuild; no native GUI target lifecycle ownership; no unrelated WOW32 capability work; no new runtime change in this admission. |
| Reference Baseline | 4d4ef4d47 and its coherent published thirteen-image manifest, closure evidence and inherited non-passes. |
| Files And ABI Surface | Current, proposal and architecture/evidence documents; audit Run16, NTSRV, worker-base, NTCON, NTVDM and NTVWM control interfaces before changing ABI. |
| Applicable Rules | docs/README reading set; execution, architecture, coding, document and source-policy authorities; original-owner recovery and exception registration. |
| Verification | Source call-chain audit, role/transition matrix, ownership/rollback/concurrency review and documentation governance; implementation groups later require product/runtime gates. |
| Expected Markers | Every inherited gap has one owner, finite repair scope, failure behavior and concrete acceptance case; no competing lifecycle authority. |
| Asset Needs | Existing source and read-only original references; retained T447 fault evidence and rollback package; no new guest assets. |
| Reporting Requirements | Distinguish verified source facts, design decisions and runtime proof; report exact remaining gaps without functional-closure claims. |
| Stop Conditions | New observable semantic retreat, missing original-source authority or material scope expansion requires owner decision. |
| Exit Criteria | Complete bounded design and review, governance pass, reviewed docs commit/push; S1 design closure does not certify runtime repair. |
| Original Owner Request | Close T447 with all unfinished work assigned; prioritize a new T centralizing lifecycle authority in NTSRV; admit only after clean delivery. |
| Similar-Issue Sweep | All normal/failure/timeout/service-loss exits; GUI/CUI separation; internal/external management; nested launchers; worker/frontend direct-control bypasses. |

## Planned S sequence

1. S1: whole-system ownership audit and complete transition design.
2. S2: complete normal admission/result/handoff chain, including frontend policy and lifecycle-only WOW binding.
3. S3: unified management-close transactions, concurrency and role-specific service-loss cleanup.
4. S4: integrated fault/concurrency and real Console handoff acceptance, full release and closure audit.

The [proposal](../proposals/proposal-ntsrv-lifecycle-authority-001.md) owns the
finite inherited gap list. Cross-group changes must preserve a coherent working
protocol, not publish incompatible partial upgrades.

## S1 Progress

Owner approved the four-stage plan and execution. The
[working design](../etc/operations/t448-lifecycle-plan.md) and
[source audit](../etc/evidence/m0-t448-s1-lifecycle-audit.md) record the actual
chains and finite L01-L10 remediation groups. Runtime is unchanged. Owner rejected
loss-only recovery synchronization: Run16 returns the applicable original
service-disconnection error without any NTCON/worker wait. The source audit
records disconnected-port status conversion to ERROR_INVALID_HANDLE (6), distinct
from other failures and from the current project watcher exit 1722. Selected
original natural/fatal exit sites now have a
bounded three-host-file/seven-call design budget, not an implemented mirror diff.
Service-created endpoint-signalled fences are prohibited, not classified as
service-only communication. Loss return does not certify Console restoration;
healthy-service handoff ordering remains mandatory. Owner approves the final
design corrections and continuous sequential execution through T pre-closure
acceptance: finalization timeout returns locally, submissions never retry,
NTSRV selects scope-specific phases, task cleanup precedes no unauthorized
worker teardown, and Win16 default launcher return is startup-only. S1 design
verification/delivery precedes S2 admission; no runtime repair is claimed here.

## Current Technical Baseline

The coherent thirteen-image checkpoint is recorded in
[release manifest](../../assets/release/release-manifest.json) and the
[CAF ledger](../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md).
Both assets/release and O:/winnt/system32 were hash-verified. All-component
build/incremental and focused lifecycle tests pass; actual remaining failed
and unexecuted gates remain explicitly listed in the closure, not converted to
passes. Guest, configuration and assets/winnt.zip are unchanged. One registered
nxvm timeout constant changed; no OpenNT mirror change.

[Preserved pre-closure status](../etc/evidence/m0-t447-pre-closure-status.md)
retains the complete prior packet and technical history. Its active wording
is historical, not concurrent admission. The prior
[working plan](../etc/operations/t447-wow-execution-plan.md) is likewise a
transfer inventory, not current task allocation.

## Temporary evidence

S1 owns build/t448-s1 for documentation-gate output only: 100 MiB ceiling,
one-hour run budget and ten-minute no-progress limit; inspect and dispose at
the S1 boundary after retaining the concise result. No new compiler tree.

build/t447-s4-p4 is retained for the immediate lifecycle successor: fault dumps,
exact package/rollback identities and final test logs. No live background test
may be inherited without checking its PID. The new task must review retention
at its first phase boundary; no use as a cross-task compiler cache. Stable
build/.ninja and build/.ccache remain untouched.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T447 | [Owner-directed scope-transfer closure](../history/m0-t447-transfer-closure.md), not full functional acceptance. |

## Recent Governance

Prior governance evidence remains indexed in the preserved pre-closure status.
