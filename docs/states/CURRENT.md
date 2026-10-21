# Project Status

## Current Work

**Active: M0 T448 S3**

T447 closed by [explicit scope transfer](../history/m0-t447-transfer-closure.md)
at pushed commit 4d4ef4d47, with a clean worktree verified before this admission.
Owner authorizes T448: NTSRV lifecycle authority unification. All remaining
WOW32 work is now the first [queued successor](QUEUE.md). Neither admission nor
the prior checkpoint claims that the CAF defect is universally fixed.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T448 S3; Ordinary Mode, single person implementer/reviewer; no subagents. |
| Candidate Proposal | [NTSRV lifecycle authority](../proposals/proposal-ntsrv-lifecycle-authority-001.md). |
| Admission And Approval | Owner approves final design and continuous sequential execution through T pre-closure acceptance; S1 design concluded and pushed at c838c332c. |
| Objective | Implement the approved unified management-close transaction, concurrency and role-specific service-loss cleanup, preserving the S2 normal-chain candidate. |
| Non-goals | No guest modification/rebuild; no native GUI target lifecycle ownership; no unrelated WOW32 capability work; no T closure without owner acceptance. |
| Reference Baseline | S3 P2 communication-cleanup checkpoint: protocol 50, thirteen images published and hash-verified at both destinations; full/incremental build, 38 service cases, four actual deadline scenarios, wire negatives and same-package runtime gates pass within the recorded frontier. S3 exit-permission work remains open; no functional acceptance or waiver. |
| Files And ABI Surface | Run16, NTSRV service/client, common protocol, worker-base, NTCON, NTVDM and NTVWM; production-linked lifecycle tests; coherent protocol updates if required. |
| Applicable Rules | docs/README reading set; execution, architecture, coding, document and source-policy authorities; original-owner recovery and exception registration. |
| Verification | Source-first implementation and diff review; background x86/x64 full-component/incremental builds; affected production-linked service/frontend/launcher fixtures and normal DOS/native/WOW startup/return regression; complete package deployment and hashes; governance. |
| Expected Markers | One service-owned close transaction independent of requester survival; one ten-second deadline; no competing close/restore owner; role-owned loss cleanup; exact late acknowledgements and original exit semantics. |
| Asset Needs | Existing source and read-only original references; retained T447 fault evidence and rollback package; no new guest assets. |
| Reporting Requirements | Distinguish verified source facts, design decisions and runtime proof; report exact remaining gaps without functional-closure claims. |
| Stop Conditions | New observable semantic retreat, missing original-source authority or material scope expansion requires owner decision. |
| Exit Criteria | Complete L05-L08/L10 close/fault chains and applicable L04/L09 cleanup; production-linked and actual close regressions, coherent thirteen-image publication and reviewed push. P1 delivers progress only; unfinished exit-permission and initialization/fault coverage still block S3 closure. |
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

## Prior S1 conclusion and S2 progress

The following records the pre-P1 progression; the S2 Closure Record below
states the superseding delivery outcome, without erasing earlier failures.

Owner approved the four-stage plan and execution. The
[working design](../etc/operations/t448-lifecycle-plan.md) and
[source audit](../etc/evidence/m0-t448-s1-lifecycle-audit.md) record the actual
chains and finite L01-L10 remediation groups. S1 changed no runtime. Owner rejected
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
verification/delivery is recorded at c838c332c and in the linked source audit.
S2 is implementing the approved chain; its
[implementation ledger](../etc/evidence/m0-t448-s2-lifecycle-implementation.md)
records candidate full builds, focused/service-matrix passes, actual shared and
separate WOW startup return, real service-loss return and live-service ten-second
finalization timeout. Final-candidate native CUI/GUI, shared/separate WOW and SOL
regressions pass. DOS fails before its normal return chain with a visible illegal-
instruction dialog; the complete published baseline reproduces the same failure.
WRITE also retains its illegal-instruction failure. S2 has reached a bounded
implementation/review conclusion, not functional acceptance: its P publication,
commit and push remain deferred by those regression gates. Under the execution
rule for sequential S reopening, the approved S3 close/fault group is now active;
the complete S2 diff and immutable candidate are retained, not discarded or
represented as delivered. No T closure or compatibility waiver is implied.

Subsequent S3 environment isolation supersedes that DOS blocker for the fixed
lifecycle matrix: all seventeen DOS/EDIT cases pass on package-r4 with a short,
declared package+host-System32 PATH. Short TEMP alone does not fix the inherited
2457-character PATH failure. WRITE still fails (original out-of-memory dialog
with the short environment). The [S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md)
records the full/incremental build, 33 service cases, actual CUI/WOW loss and CAF
passes, and the still-unproved startup descendant race and healthy-service exit
permission group. No new publication, implementation commit or push yet.

## S2 Closure Record

Owner requested prompt incremental publication without waiting for every S3
exit boundary. S3 P1 carries the retained S2 implementation. On r7, DOS17,
WINMINE/SOL gameplay, WRITE's startup frontier and all three internal CAF
NTMON self-delete cases pass. The thirteen-image package was published and
hash-verified at both destinations on 2026-10-21 05:24 UTC. Earlier failed
attempts above are historical, superseded only within these tested boundaries.
See the [S3 delivery ledger](../etc/evidence/m0-t448-s3-close-implementation.md).
S2 has a bounded implementation conclusion and this delivery, not owner
functional acceptance. WRITE intermittency is not called repaired. S3's
healthy-service exit permission, partial-initialization cleanup and remaining
fault/concurrency verification remain open; S4 owns final integrated acceptance.

S3 continuation after pushed P1 d0814cd4c now implements spontaneous exit
requests through the existing close arbiter. Acceptance remains distinct from
the service's teardown instruction, preserving a prior Console quiescence
barrier. The owner-side operation now has RPC/worker-base wiring and local
permission tests, but no second published package. Real suspended-service
verification exposed that the synchronous local-RPC timeout option is ineffective.
S3 is reusing the existing asynchronous RPC completion/cancellation mechanism
and auditing the remaining startup/finalization callers under the owner's
explicit two-mechanism boundary: RPC control, direct worker/frontend I/O only.
The linked S3 ledger records failed and pending gates; no transport workaround
is admitted.

The communication cleanup now removes retired I/O activation and the ineffective
local-RPC timeout setter, reusing one existing async completion/cancellation
owner. Full thirteen-component and incremental builds pass on r8i; r8j passes
38 service cases and four real suspended-service/frontend deadline scenarios.
r8k passes real management and application/protocol identity RPC negatives.
Same-package DOS17, WINMINE/SOL gameplay, WRITE's startup-only frontier and all
three internal CAF deletion cases passed on r8k. The coherent thirteen-image
set was published and hash-verified at both destinations at 06:30:19 UTC on
2026-10-21. Remaining synchronous control/diagnostic calls and host exit-permission
integration remain explicit S3 gaps; this P2 progress delivery does not close S3.

## S1 Closure Record

Design-only conclusion delivered at c838c332c. The
[source audit and final review](../etc/evidence/m0-t448-s1-lifecycle-audit.md)
records ownership, finite L01-L10 scope, owner corrections, original error
provenance and passed background governance. No production change, build or
release was required; all runtime acceptance remains S2-S4 work.

## Current Technical Baseline

The coherent thirteen-image checkpoint is recorded in
[release manifest](../../assets/release/release-manifest.json) and the
[S3 delivery ledger](../etc/evidence/m0-t448-s3-close-implementation.md).
Both assets/release and O:/winnt/system32 were hash-verified. All-component
build/incremental and focused lifecycle tests pass; actual remaining failed
and unexecuted gates remain explicitly listed in the closure, not converted to
passes. Guest, configuration and assets/winnt.zip are unchanged. This checkpoint
adds no OpenNT mirror diff; service protocol revision is 50.

[Preserved pre-closure status](../etc/evidence/m0-t447-pre-closure-status.md)
retains the complete prior packet and technical history. Its active wording
is historical, not concurrent admission. The prior
[working plan](../etc/operations/t447-wow-execution-plan.md) is likewise a
transfer inventory, not current task allocation.

## Temporary evidence

S3 uses build/t448-s3 for owned runners, logs, staging and recovery, with an
8 GiB ceiling, four-hour per-run budget and ten-minute no-progress investigation
threshold. Join background jobs before changing their mutable inputs. Retain
S2's exact package/results until the deferred delivery and comparison are resolved.

S2 owns build/t448-s2 for runners, logs, staging and recovery: 8 GiB ceiling,
four-hour per-run budget and ten-minute no-progress investigation threshold.
The existing DOS-route harness additionally requires build/M0-T448/S2/fixture
for its tiny independently authored G7.COM input, under the same owner/budget.
Background build/test jobs are sequential per mutable build tree, with PID and
exit records. Stable architecture configurations remain under build/.ninja.
S1 retained build/t448-s1 for documentation-gate output only: 100 MiB ceiling,
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
