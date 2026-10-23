# Project Status

## Current Work

**Active: M0 T448 S12**, Ordinary Mode, single implementer/reviewer. T448 remains open.

## Active Packet

| Field | S12 reproducible host TODO remediation packet |
| --- | --- |
| Identifier Mode | M0 T448 S12, Ordinary Mode; one implementer and independent self-review. |
| Admission And Approval | Owner expands active S12 to fix all reproducible host-side TODO defects; S13 investigates the remaining not-necessarily-reproducible TODO cases. |
| Objective | Audit the complete TODO by actual owner and reproducibility; repair evidenced reproducible host defects, with original-source exception records and real regression evidence. Preserve original guest limitations. |
| Non-goals | No guest modification, rebuild or hot patch; no speculative fixes or unrelated lifecycle and WOW expansion. |
| Reference Baseline | Published S9 package; S10 commits e097bccb0 and 25a507a7d, with full-size SOL gameplay still unverified. |
| Files And ABI Surface | Original host providers and project-owned adapters as proven by each case; preserve guest binaries and existing ABI unless separately justified. |
| Applicable Rules | Reading set, execution, source policy, guest immutability, original-owner evidence, and documentation governance. |
| Verification | Revalidate each original failing witness and owner; prove repairs with positive, negative, lifecycle and actual guest paths, run affected regressions, and distinguish unavailable from nonreproducible. |
| Expected Markers | INT33 checksum, VGA cursor-disable, 8042 C0, redirector zero capacity and DEM deleted-name cases dispositioned with source, tests and mirror-diff accounting. |
| Asset Needs | Published package, immutable guest inputs, isolated desktop and independently authored disposable probes. |
| Reporting Requirements | Per TODO report classification, observed behavior, owner, fix or explicit remaining boundary and evidence. Disposable build/t448-s12 budget 2 GiB and two hours per run; inspect after ten minutes without progress. |
| Stop Conditions | Unapproved semantic change, guest modification, or evidence requiring materially broader implementation. |
| Exit Criteria | All confirmed reproducible host defects in the frozen inventory repaired and tested, or an explicit authority/prerequisite blocker reported; full appropriate build, release, review, commit and push; await owner acceptance. |
| Original Owner Request | Expand S12 to all reproducible host-side TODO repairs; use S13 for all not-necessarily-reproducible TODO investigation. |
| Similar-Issue Sweep | Distinguish original guest defects, original host defects, adapter defects and invalid test witnesses. |

[S11 investigation](../etc/evidence/m0-t448-s11-window-startup.md) reproduces
the already documented original COMMAND large-environment lifetime defect on
the current package. The specific post-prompt CAF report remains unverified;
no product fix or publication is claimed. Prior DOS/mouse S12 observations
are retained in that ledger, not treated as completed triage.

S12 candidate DIV-357–361 repairs the INT33 checksum, VGA cursor-disable,
8042 response, buffered directory-entry loss and redirector zero capacity.
Focused provider/guest/frame tests pass. The four-fix candidate passes DOS17
in both modes, WINMINE, SOL gameplay, the previous WRITE startup frontier and
three CAF self-delete/echo routes; full-package tests for DIV-361 are pending.
WRITE editing/exit remain unverified. The 13-image five-fix checkpoint is
published and hash-verified; S12 remains open. DEM's deleted-name
fallback remains unresolved: the original indexed kernel IRP lacks a documented
user-mode equivalent. See the evidence ledger for alternatives and limits.

S10 test-only implementation: classifier cases and MSVC probe compilation
pass; private-desktop r5/r6 verify readiness and normal exit with explicit
partial/unverified gameplay at 320-pixel screen width. Earlier r2/r3 close
failures remain recorded, not explained or erased. Full-size gameplay was not
rerun here. [Evidence](../etc/evidence/m0-t448-s9-console-title-handoff.md#sol-integration-geometry-follow-up)
also distinguishes the owner's separately unmeasured layout report.

Owner additionally directs TODO triage inside T448: remove the MySMB16
performance, wide-text mouse and historical native-menu entries without
claiming repairs; preserve original-capability limitations and T271/document
debt; and plan bounded current-package reproduction of seven selected cases.
The [T448 follow-up plan](../etc/operations/t448-lifecycle-plan.md#owner-directed-todo-triage)
records S12/S13, evidence-based disposition, and unavailable-versus-nonrepro
boundaries. S10 delivery remains pending owner acceptance; approval to begin
S11 does not imply acceptance of S10's unverified full-size gameplay.

S9 implements Console-title handoff for NTVWM; the [S9 evidence](../etc/evidence/m0-t448-s9-console-title-handoff.md)
records full13/incremental, focused channel, real CAF, DOS/PIF title and
runtime results. The full thirteen-image package is published and hash-checked
at assets/release and O:/winnt/system32; the delivery commit records its
identity. The owner approved S9 closure; this is not T closure.

S8 P1 is delivered at b3ee596e4ac85e1a223af24a5ccd410369d6bce3, pushed to
origin/main with a clean checkout and all13 installed/released hashes rechecked.
Owner reports functional acceptance and separately reports that CAF displays
the NTVWM carrier title for `run16 cmd`; title investigation is recorded in the
S8 ledger. T closure is not claimed. The
[S8 ledger](../etc/evidence/m0-t448-s8-shared-library-import.md) records52
byte-exact upstream files,18 translation units, direct exchange-writer decoding,
lazy actual-size payloads and removal of redundant staging. No guest or original
mirror changes. Owner-approved title truncation keeps a complete UTF-8 prefix
(127 bytes plus NUL); producer titles/protocol remain unchanged.

Final build-r20 passes all13 components and1454 unchanged intermediates.
Both-width fixtures, channel/text, all eight CAF cases, DOS17 and Minesweeper
pass against package-r21. WRITE/SOL retain explicit startup-only limits.
An initial r6 cmd SelfDelete readiness failure remains unassigned; later fixed
repeats and two full matrices passed without claiming its cause repaired.
All13 images are published/hash-verified at assets/release and O:/winnt/system32;
publication-r20 retains a rollback backup. Source review and governance pass.
Scratch/evidence remains in build/t448-s8 pending acceptance and disposition;
stable Ninja/ccache caches are retained. T448 remains open.
Owner admits S8 on 2026-10-22 to import the audited public-library closure
unchanged and adapt NTCON. S7 reaches its bounded audit-only conclusion;
no import or downstream runtime qualification was performed in S7. T448
remains open. S8 pins SoftPC 0d9f1803ee04bd89bd6150306912a3e8674d0a83;
no moving HEAD or dirty upstream inputs. See the [S7 audit and S8 handoff](../etc/evidence/m0-t448-s7-shared-library-import-audit.md#s7-conclusion-and-s8-handoff)
and [accepted S6 and prior T audit](../etc/evidence/m0-t448-s6-unused-monitor-cleanup.md#owner-acceptance-and-t-closure-audit).
The pre-S8 delivered baseline was d5d0dc243, protocol64. The S8 release manifest
now records the tested and published complete thirteen-image package.
The proposed WOW read-time clock optimization was not implemented; the owner
accepts retaining the existing publisher after its source feasibility review.

The following delivery checkpoint predates the acceptance above:
Owner admits S6 to remove proved unused wait fallbacks/interfaces and make
NTMON sample once per second but redraw only on actual snapshot or UI changes.
Idle elapsed-time progression alone must not redraw. No new service protocol.
S6 implementation and verification are complete: full13/incremental r5,
monitor/CCPU, frontend library/channel/text, eight CAF r6 cases and Minesweeper
pass. The full13 candidate is published/hash-verified at assets/release and
O:/winnt/system32, with rollback backup. Final reviewed commit/push and clean
verification precede the owner acceptance handoff; T448 is not closed.
See the [S6 ledger](../etc/evidence/m0-t448-s6-unused-monitor-cleanup.md).
S5 delivered at6a956824c, pushed with a clean checkout; T448 remains open.
The [S5 ledger](../etc/evidence/m0-t448-s5-integrated-acceptance.md)
maps all nine inherited obligations to current owners, actual assertions and
explicit coverage limits. Fresh evidence includes eight CAF return cases,
service40, two real-RPC concurrency/fault rounds, actual service loss and both
native GUI widths' startup-only/wait/independent-target/text-return cases.

S4 P2 completes the fixed thirteen-category polling audit against protocol64.
Q05 Console destruction uses notifications; Q07 mouse backpressure uses the
existing readiness event; Q06 removes the artificial10ms sleep while preserving
bounded geometry-consistent acquisition and errors. The [S4 closure ledger](../etc/evidence/m0-t448-s4-polling-audit.md)
maps implementation, retained policies, tests, failures and limitations.

r16 full13/incremental, service/owner/RPC and runtime gates pass. Its stale
presentation-fixture final expectations failed and remain recorded; corrected
r17 passes521 presentation checks,142 native-command checks and four actual
close-wait CPU observations. All13 candidate images are published/hash-verified
at assets/release and O:/winnt/system32; publication-r16 retains P1 recovery.
P2's delivering commit records its identity. S5 implementation begins only
after P2 commit/push/clean-state verification succeeds.

Standing owner approval admits S5 integrated acceptance and final ownership/
evidence reconciliation, not new WOW32 functionality or additional lifecycle
architecture. T closure still awaits owner acceptance. SOL startup is
owner-confirmed with layout TODO; WRITE evidence remains startup-frontier only,
not editing/exit acceptance. Neither limitation is silently upgraded.

S5's [integrated acceptance ledger](../etc/evidence/m0-t448-s5-integrated-acceptance.md)
records eight fresh conhost handoff passes against the unchanged full13 S4
package: natural cmd/command return with frontend survival, external close and
actual internal NTMON self-delete for cmd/command/ntmon, all with before-Enter
echo assertions. Whole-matrix reconciliation and the six-file mirror review
are recorded below; this is not owner T closure or an assertion that every
host/concurrency permutation is covered.

P1 is57bd2b0a6. S3 P20 is2b754a31e with test-only closure P21 at6ad0a4ba6;
the [S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md) preserves
historical failures, removed deadline experiments and L04-L10 reconciliation.
Only the three approved five-second policies remain: bootstrap, empty-service
retirement and original Console-close grace. Normal execution has no deadline;
service loss permits local cleanup, never reconnection or task replay.

## S11 Closure Record

The owner reports current testing is normal and directs the return to S12.
The [S11 investigation](../etc/evidence/m0-t448-s11-window-startup.md)
retains the reproduced original COMMAND environment limitation and the
unverified exact CAF report. No corrective product change, full publication,
or causal repair is claimed; the known limitation remains in TODO.

## S10 Closure Record

S10's bounded test-only implementation and TODO planning were delivered at
e097bccb0 and 25a507a7d. The [evidence](../etc/evidence/m0-t448-s9-console-title-handoff.md)
retains limited-desktop results and prior close failures. Full-size gameplay
was not rerun; owner acceptance remains separate from S11 admission.

## S9 Closure Record

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T448 S9; Ordinary Mode, single person implementer/reviewer; no subagents. Closed by owner approval. |
| Candidate Proposal | [NTSRV lifecycle authority](../proposals/proposal-ntsrv-lifecycle-authority-001.md). |
| Admission And Approval | Owner accepts S8 functionality, reports CAF title showing ntvwm.exe, and explicitly requests a fix. |
| Objective | Restore correct Console title initialization and title handoff for native CUI without exposing the carrier name or overriding application title changes. |
| Non-goals | No guest modification/rebuild, GUI or WOW32 feature changes, new transport, timers/retries/reconnect framework, or T closure without owner acceptance. |
| Reference Baseline | Delivered S8 P1 b3ee596e4; [S8 ledger](../etc/evidence/m0-t448-s8-shared-library-import.md) records current title observations and retained limits. |
| Files And ABI Surface | NTCON/NTVWM title and handoff owners, existing Console I/O protocol where needed, tests and evidence; retain coherent protocol/package. |
| Applicable Rules | docs/README reading set; execution, architecture, coding, document and source-policy authorities; original-owner recovery and exception registration. |
| Verification | Verify native initial, explicit and nested titles, handoff and restoration including DOS/PIF; focused negative/lifecycle tests, full13 incremental, relevant real CAF and DOS regressions, governance and review. All builds/tests hidden background. |
| Expected Markers | Carrier name never substitutes for inherited visible title; app title updates and nested restoration work; DOS and PIF behavior retained; no original mirror diff. |
| Asset Needs | Existing source, isolated package and immutable guest assets; prior S8 evidence and rollback package. |
| Reporting Requirements | Actual title authority, handoff and failure semantics, tests and limits, full package hashes and code/mirror delta. Scratch build/t448-s8 reused for this S: 2GiB, two hours per run, investigate after ten minutes without progress; retain stable caches. |
| Stop Conditions | New observable semantic retreat, missing original-source authority or material scope expansion requires owner decision. |
| Exit Criteria | Required title/lifecycle tests pass; all13 images published/hash-verified at assets/release and configured system32; delivery commit d8a8a9d4a pushed and clean worktree verified; owner approved S closure. |
| Original Owner Request | Fix the CAF title shown as ntvwm.exe; verify whether NTVDM has the same problem. |
| Similar-Issue Sweep | DOS shared Console, PIF title, native nested and app-set titles, channel ownership and restore. |

## S8 Closure Record

The owner accepted S8 functionality; its delivered import, tests, publication,
and retained limitations are recorded in the [S8 evidence](../etc/evidence/m0-t448-s8-shared-library-import.md).
The subsequently reported native Console caption defect is admitted separately
as S9 and does not imply T acceptance.

## S7 Closure Record

S7 reaches its audit-only conclusion under the owner's S8 admission. The
[closure and handoff](../etc/evidence/m0-t448-s7-shared-library-import-audit.md)
records upstream remediation, selected pin and all remaining integration gates.
No downstream import, runtime qualification or product publication is claimed;
those obligations belong to S8. S6 acceptance and the product baseline remain.

## Planned S sequence

S1-S3 implementation and historical P9-P21 attempts are recorded in the
[S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md). Historical
ten-second deadline experiments there are superseded by the approved three
five-second policies; they are not current instructions or open S3 work.

1. S1: whole-system ownership audit and complete transition design.
2. S2: complete normal admission/result/handoff chain, including frontend policy and lifecycle-only WOW binding.
3. S3: unified management-close transactions, concurrency and role-specific service-loss cleanup.
4. S4: owner-added autonomous-code polling audit and cleanup; reuse existing events and completion notifications, justify retained polling, verify wakeup/exit/idle behavior and code delta.
5. S5: integrated fault/concurrency and real Console handoff acceptance, full release and closure audit (formerly S4).
6. S6: owner-added unused fallback/interface cleanup and NTMON change-only rendering with one-second sampling.
7. S7: SoftPC shared-library compatibility and remediation audit; bounded audit conclusion, no runtime import.
8. S8: pinned byte-exact public-library import and NTCON integration, full verification and publication.
9. S9: native Console title initialization and handoff, preserving app-set and restored titles.
10. S10: SOL environment-aware integration evidence and owner-directed TODO triage planning.
11. S11: investigate and repair reported command CAF and SOL window startup failures.
12. S12: audit all TODO and remediate reproduced host-side defects; preserve original guest boundaries.
13. S13: investigate and test not-necessarily-reproducible TODO cases with explicit dispositions and evidence.

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
fault/concurrency verification remain open; S5 now owns final integrated acceptance.

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
retains registered DIV-355 in nt_event.c; current service protocol revision is 62. The
protocol-51 through protocol-54 paragraphs below retain prior checkpoints.

The published protocol-51 continuation removes unused retirement RPC methods and
bounds the actual NTCON pump's control calls through the existing async RPC
mechanism. Real suspended-service NTCON retirement now passes at about ten
seconds with original input mode restored. Expanded bootstrap verification
exposed NTVWM's command-loop failure bypassing the service exit owner before
the service process becomes signalled; r9g routes the managed loop through
the existing fault/permission path. Its established-worker loss, retirement,
eight real deadline cases and full runtime gate pass. The thirteen-image set
is published and hash-verified at both destinations as the S3 P3 checkpoint;
the [S3 ledger](../etc/evidence/m0-t448-s3-close-implementation.md) retains the
failed attempts and remaining startup/worker/exit-permission inventory.

S3 P4 removes the unused client-owned acquisition/cancellation RPCs; NTSRV's
StartFrontend keeps the original service reservation and failure rollback.
Candidate r10b uses RPC major/protocol 52 and reduces production plus test
source by 93 lines, without mirror or guest changes. Full/incremental build,
service38, eight real deadline cases, wire identity/loss/retirement, startup
rejection/timeout, scope lifetime and the same-package runtime gate pass.
All thirteen images were published and hash-verified on 2026-10-21 08:23 UTC.
This is another bounded cleanup checkpoint, not S3 closure; the remaining
finite call families and original exit-permission work remain in the ledger.

S3 P5 bounds frontend root/lease registration, Console membership reporting
and I/O attachment using the existing RPC cancellation/completion owner.
After a terminal failure, startup-result reporting also returns the latched
error locally. Four new paused-service cases prove ten-second return and no
second cleanup RPC. Protocol 53 package-r11 passed the full P regression and
was published/hash-verified at both destinations on 2026-10-21 08:41 UTC.
StartFrontend, state acquisition, normal startup-result, Disconnect/UNKNOWN_IF,
remaining worker controls and original-host exit permission remain open.

S3 P6 bounds StartFrontend, state-event retrieval and startup-result reporting
through existing RPC completion/cancellation. Failed final normalization closes
returned attachments, and latched failure prevents subsequent control calls.
Protocol 54 package-r12 passes fifteen actual deadline cases and the full P
runtime gate; thirteen images published/hash-verified at both destinations on
2026-10-21 09:02 UTC. The source audit retains ordinary Disconnect/UNKNOWN_IF,
remaining worker/inherited-console controls, initialization and original-host
exit permission as open obligations. S3 and T acceptance are not claimed.

[Preserved pre-closure status](../etc/evidence/m0-t447-pre-closure-status.md)
retains the complete prior packet and technical history. Its active wording
is historical, not concurrent admission. The prior
[working plan](../etc/operations/t447-wow-execution-plan.md) is likewise a
transfer inventory, not current task allocation.

## S3 Closure Record

S3 closes against P20's verified/published protocol64 package, commit 2b754a31e,
and the P21 test-only delivery. The [fixed-scope reconciliation and evidence](../etc/evidence/m0-t448-s3-close-implementation.md)
maps L04-L10 to implementation, source review, r57 runtime gates and r58b/r59
restoration/stop/join tests. No foundation defect is transferred to S4 or S5.
S4 is the separately approved polling cleanup; S5 retains integrated acceptance.
SOL is owner-confirmed startup with layout TODO; WRITE retains its explicitly
limited frontier. This is S closure, not T acceptance.

## S4 Closure Record

The [S4 ledger](../etc/evidence/m0-t448-s4-polling-audit.md)
reconciles all13 categories, P1/P2 implementation, r16 runtime and corrected
r17 capture/CPU evidence. P2's complete protocol64 package is published at both
destinations with P1 recovery. No guest or mirror delta. S5 inherits final
whole-T acceptance, not an unresolved S4 implementation item. Closure delivery
requires the P2 commit/push/clean verification recorded by its controller.

## S5 Closure Record

The [final reconciliation](../etc/evidence/m0-t448-s5-integrated-acceptance.md)
records r1/r2/r3 runtime passes, final-review-r2 (controller18296) source,
mirror-registration and all13 package-hash checks. No S5 production or guest
change requires rebuilding/republishing; the full S4 r16/r17 delivered package
remains the latest installed baseline. S5 P1 is3918764fa; final documentation
delivery requires governance, commit/push and clean-state verification.
No known unfulfilled in-scope implementation obligation is transferred out.
SOL layout, WRITE's startup-only frontier, source-reviewed rather than induced
arbitrary fatal guest states, and non-exhaustive host/thread interleavings stay
explicit limitations. T448 remains open for owner acceptance.

## S6 Closure Record

Owner accepted S6 after delivery d5d0dc243; acceptance and the prior whole-T
audit are in the [S6 ledger](../etc/evidence/m0-t448-s6-unused-monitor-cleanup.md).

Removed uncalled NTCON task/event/wait/sleep/yield closure and default mailbox
Event; retained mutexes and actual native window notification. CCPU requires its
startup-created event, without an uninitialized yield fallback. NTMON samples
at1000ms, compares semantic snapshots and UI state, and avoids idle repaint.
Production C/H delta is +84/-376 (net-292); tests +66/-4; build rules +3/-2.
Import-subset provenance/hashes are registered; OpenNT mirror and guest delta0.
Full13 r5 passed with1426 unchanged second-build intermediates. Focused monitor
and CCPU, window library, full private channel, text49, CAF8 and Minesweeper
passed. Historical fixture invocation/link and coordinate-sampling failures
remain in the ledger. Live-view bitmap fixture is compile-checked; live modal
capture was not rerun, and existing SOL/WRITE limits remain unchanged.
Review and complete two-destination publication passed with recovery at
build/t448-s6/publication-r5-backup. Delivery controller records the commit,
remote identity and clean worktree; owner acceptance is now recorded above.

## Temporary evidence

S6 owns build/t448-s6:8GiB ceiling, four-hour per-run budget and ten-minute
no-progress investigation threshold. Retain the installed S4 package and S5
acceptance evidence for comparison; use only existing stable Ninja/ccache trees.

S5 owns build/t448-s5 for final reconciliation/acceptance:8GiB ceiling,
four-hour per-run and ten-minute no-progress investigation budgets. S4 sealed
candidate, recovery and test evidence remain retained for S5 identity comparison.

S4 owns build/t448-s4 for audit/test evidence: 8 GiB ceiling, four-hour per-run
budget and ten-minute no-progress investigation threshold. Reuse only stable
build/.ninja and build/.ccache for compilation. S3 exact package/rollback and
evidence remain retained for S5 comparison; audit before deleting any material.

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
