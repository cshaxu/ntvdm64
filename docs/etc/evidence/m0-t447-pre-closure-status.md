# Preserved status before T447 transfer closure

Historical snapshot only. The [closure](../../history/m0-t447-transfer-closure.md) supersedes its active wording.

# Project Status

**Active: M0 T447 S4 P4 — NTCON full-screen close handoff delivery review.**

P4 reopens S4 for delayed echo after CAF frontend deletion. Owner-approved
repair: NTSRV owns request completion and the Console lease; NTCON restores
Console; Run16 uniformly waits for service completion on normal/failure exits,
without manipulating Console or selecting a root-only wait. Preparation before
Console-close is distinct from final restoration after worker rundown. Internal
and external NTMON use the same transaction. Normal task completion parks and
returns the Console without requiring NTCON death. No guest or OpenNT mirror
changes; one registered nxvm timeout constant follows the requested ten-second
budget. S5 WOW32 remains deferred. Implementation is staged, not delivered.

The indexed P4 evidence retains the seven-point transaction audit, earlier
negative probes and failed hidden-Console input gate. None are delivery proof.
Regression must cover repeated preparation acknowledgements, worker failure,
setup failure, timeout progression, frozen presentation and direct/nested
self-hosted NTMON. Coherent13 build, deployment and actual pre-Enter echo
verification remain required before delivery.

Bounded production waits in this frontend shutdown path use ten seconds;
test polling remains observation-only. Seven focused lifecycle cases and the
service33 suite pass. Full-build r4 built thirteen images and verified unchanged
1376 intermediates on its second pass. Source review additionally fixed the
dead-frontend drain path, stale prepared-pump wakes and broker-death wait.
Actual conhost CMD self-delete passes pre-Enter active-buffer echo; remaining
runtime gates and coherent publication are still pending.

The reopened [self-hosted investigation](../../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md#reopened-investigation-self-hosted-request-2026-10-20)
now exercises actual internal NTMON Delete/Y and captures ConPTY output.
It also corrects the old observer's accidental execution of PATH's q.cmd.
The initial corrected probes alone did not prove the owner's root cause.
The later [preserved owner scene](../../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md#preserved-owner-conhost-scene-distinct-echo-buffer)
provides stronger evidence: the actual pending CMD cooked read contains cddd
and references a different screen buffer from conhost's active-screen slot.
Input mode already includes echo. Published error-completion/close-sweep
paths can release the launcher before buffer restoration; the precise event
chronology remains unrecorded. The current repair and its evidence limits are
in the [service-owned completion ledger](../../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md#service-owned-completion-repair-2026-10-21).

## Historical baseline before the reopened P4 repair

Owner accepts and closes [Td build hygiene](../../etc/operations/td-build-hygiene.md)
at aadde21e8; S4 resumes with stable caches, immutable guest and frozen
performance. P22 retains P21 and delivers349/101 conversion ownership plus
102's bounded native-query acquisition. Source167/native3/transaction21,
native21/original30/S3 twelve/message18/external9/cross6/pending6,
actual faults14/Shell6, application/DOS/logon and installed floors pass.
Both destinations match13. Earlier WRITE,0102 and Shell/error13 failures
remain unlocated, not repaired by later passes. The
[audit](../../etc/evidence/m0-t447-s4-message-source-audit.md#p22-bounded-conversion-ownership-delivery)
retains failed controls, exact identities and evidence limits. No guest change.

Full M1--M6/S4, protocol/output and S5/S6 remain open.
Native output28 passes at860785e88. P22 adds no protocol ownership policy,
stale local fallback or weaker validation. The post-P22
[DDE source ledger](../../etc/evidence/m0-t447-s4-message-source-audit.md#post-p22-dde-ownership-source-ledger)
pins nine entries/three lifetimes.  DIV-350--354 are published in the
coherent 3eae6c637 baseline; DIV-103 remains staged pending its separate
object gate.
Native prerequisite, alias/ACK82 and filter/queue59 controls pass, not guest/
product proof. DIV-354's nested WOW32 build is clean; its alias control proves
only original/service ownership, not a guest DDE conversation.103 reconnects ANSI filter scope; object gate awaits full build.
Queue18 proves exact conversion intervals, not full queue/protocol recovery.
Styled raw-fault recovery, full M1--M6 and M3 idle remain open.

The earlier completed DDE proof scratch was removed. Current P4 scratch,
fault dumps and rollback package are retained until handoff acceptance;
stable caches remain untouched. No prune or guest edit.

## Current Work

The complete earlier S4 chronology is retained in the
[pre-P4 status record](../../etc/evidence/m0-t447-s4-pre-p4-status.md).
It preserves source investigations, failed controls and prior delivery results;
it does not admit concurrent work. The active packet below governs only the
reopened Console lifecycle repair. Deferred WOW32 scope remains in S5.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T447 S4 P4; single-person implementer/reviewer, no subagents. |
| Admission And Approval | Owner approves implementation, full build/test/publication/commit/push and self-review after the preserved cooked-read buffer mismatch is proven. NTSRV coordinates completion, NTCON restores, Run16 is a uniform client. No guest change. |
| Objective | Prevent parent cooked-read admission before final Console restoration after direct or nested CAF self-deletion; retain identical internal/external management policy and correct error results. |
| Non-goals | Guest modification/rebuild, private native SMS/layout decoding, parallel queue/transport/scheduler, helper windows, whole USER server import, new individual-task termination UI or broad unrelated WOW families. |
| Reference Baseline | `a9bc337fd` plus retained staged audit changes; actual owner conhost dump proves a pending cooked read echoes into an inactive buffer despite cooked/echo flags. Earlier automated controls did not reproduce it. |
| Files And ABI Surface | NTSRV frontend registry/lifecycle, NTCON presentation/channel/service, Run16 request completion, existing scalar RPC and host tests/observers. No guest ABI or mirror source change. |
| Applicable Rules | Source-first four-rung audit, original/host/adapter responsibility rule, registered minimal mirror diffs, immutable original guest, grouped background build/tests, coherent13 release and reviewed main/origin commit/push. |
| Verification | Automated isolated CUI parent for both command forms; inject Ctrl+Alt+F; use the same management Console-close RPC as NTMON; record input mode/echo before and after close, first character and Enter; compare ordinary exit and worker-close controls. |
| Expected Markers | Preparation precedes Console-close dispatch; final teardown precedes lease completion; repeated preparation never releases the launcher; failed task status remains failed; no post-return projection; actual first-character echo before Enter. |
| Asset Needs | Existing pinned read-only OpenNT and canonical guest ZIP; host-native fixture/observation probes only under disposable build paths. No new original guest artifact. |
| Reporting Requirements | Exact test identities and failures, root-cause evidence versus inference, coherent package hashes, quality review, commit/push and remaining physical acceptance limitations. |
| Stop Conditions | New observable semantic concession or scope expansion needs approval; no approximate private state, helper HWND, speculative replacement or transferred foundation defect. Existing granted implementation/registration/publication authority remains. |
| Exit Criteria | A bounded evidence record establishes the cause or a finite blocker.  If repair is admitted, it must add an automated reproduction/regression and pass the normal lifecycle release gate before P delivery. |
| Original Owner Request | 单人双角色复通wow32；可接入原版立即接入并删除被取代自主实现；源码追溯优先；整组接通后验收；最小镜像diff，尽可能保持原版功能语义。 |
| Similar-Issue Sweep | Both `cmd`/`command`, normal versus Ctrl+Alt+F, Console-close versus worker-close, NTVWM-CUI and NTVDM-DOS carriers, success/failure/draining races and first-character input mode. |

### Prior P4 delivery record (superseded by reopened audit)

The failure was not an inability to restore cooked input mode: the failing
candidate already reported `0x1f7`.  It was an ordering error.  Management
close first stopped the DOS worker, breaking NTCON's live Window channel; the
frontend could then exit without first selecting the borrowed Console.  P4
adds the existing scalar workerless-frontend operation value
`RETURN_CONSOLE`, used only while a management close is draining.  NTCON
synchronously selects Console and acknowledges; NTSRV then sends the existing
worker-close request.  The final close remains the existing path.  Worker
registry's competing immediate root-retirement writer was also removed, so
the existing lifecycle deadline is the one authority for ordinary workerless
frontend retirement.

No guest, guest media, mirror, protocol-layout or public ABI diff was added.
The final full-component gate builds all 13 product images plus the changed
observer and verifies 1,344 intermediates unchanged on the second pass.
`basesrv-service-reservation-test` passes.  The visible CAF observer cannot
discover `LibKvmWindow` in this noninteractive automation desktop even after
its own quoting/visibility defects were corrected; it therefore remains an
explicit unexecuted runtime acceptance boundary, not a passing claim.  The
same manually reproducible CAF→NTMON route is the owner acceptance test.
Both release destinations hash-match the final 13-image manifest.  Exact
source, test and release evidence is recorded in
[P4's evidence record](../../etc/evidence/m0-t447-s4-p4-ntcon-caf-close.md).

## Current Technical Baseline

The historical S4 P22 baseline retains P21 and adds349/101/102's bounded conversion/acquisition group.
The current coherent13 lifecycle candidate retains WOW32
EC648FB73B11DA61276CABC8ECA95C7E6C0D9AC1A9EA6D2D9E92D72814273DA2 and publishes
NTVDM `FF35E68DBAF35C427012813D2B3D41D97416F9C255B7CDD7292B27268AD5FDF6` and
NTSRV `E0118AD156BDBEC6F6CA86FFA837788285F7B93A72078FCA0B0E99174A1ABD8C`.
The full coherent13 manifest is hash-equal in `assets/release` and
`O:/winnt/system32`; full x86/x64 build-r7 passed with 1,326 unchanged
intermediates on its second build. This is a delivery baseline, not S4
closure: the original M1--M6 inventory remains active.
Original guest, user configuration and assets/winnt.zip are unchanged.

The S4 classic-DDE cohort registers only DIV-350--354 in the existing original
WOW32 conversion/alias owners. Its exact-body alias/free fixture passes82
candidate controls while immutable7835ba25e exposes the three expected
pre-cohort defects; the isolated actual `WM_DDE_EXECUTE`/ACK/deferred-free
lifecycle also passes. The current coherent13 baseline carries that bounded
cohort, but it is not S4 closure: DDEML, the remaining DDE families, and the
broader M1--M6 gates remain outside that result.

Closed S4 contains one bounded host-lifecycle repair for an owner-reported
borrowed Console race: explicit NTMON frontend deletion drains the associated
DOS worker through its existing shutdown event before it allows NTCON to
restore the parent CMD Console mode. This is a project-owned NTSRV/NTCON
ordering boundary only; original DOS `ResetConsoleState` remains the worker's
last mode writer, no guest image changes, and ordinary `run16 cmd` remains a
control path.

| Architecture | Images |
| --- | --- |
| AMD64 | run16.exe, ntsrv.exe, ntcon.exe, ntvwm.exe, ntmon.exe, nthook64.dll, ntw16s64.dll, W16Style.exe |
| I386 | ntvdm.exe, WOW32.DLL, VDMREDIR.DLL, nthook32.dll, ntw16s32.dll |

S3 current evidence: fresh native20/20, original30, MENU/resource/both-width
style guards,12 real Win16 groups; P23 strengthens controls in all3 policies.
WINMINE/SOL gameplay and normal exit, WRITE four startup samples per policy,
Console17/Window17 and installed ordinary-desktop WINMINE pass. These do not
prove full S4 mixed-message/reply behavior, WRITE editing/save/full exit or
S5 per-task termination. Physical control input and every drawing pixel are
not inferred from message-driven probes.

Selected278 mirror files retain177 exact/101 registered content differences,
zero format-only/register gaps. This selected total is not an S3-added count
or proof of dormant branches. Original CCPU40 /MT x86 remains the worker
profile. NTW16S retains the integrated20px policy and approved custom geometry.
Owner-approved thread-level idle observers retain original WOW registration;
NT4 cross-process global-hook priority is explicitly not promised.

The complete-token logon environment is tested, not the owner's existing
shell snapshot. The large developer-environment failure is source-proven
immutable COMMAND EndInit freed-INIT/EnvSiz reuse, recorded in
[TODO](../../states/TODO.md), not filtered or patched. The old unexplained common-dialog
witness and older GDI16/19 risks remain; no current pass claims them repaired.

## T447 Sequence and Delivery

S1 source audit and S2 foundations are closed; S3 dependency closure is
recorded above. S4 is reopened for P4 following its bounded lifecycle repair
and P3 protocol hardening. S5 is deferred and owns the remaining WOW32
message/identity/reply/wait/reentry inventory, plus the existing strict
`run16 command` ConPTY first-prompt gap evidenced identically before and after
P3. S6 individual task termination/NT Monitor and S7 whole-T reconciliation
remain planned, not concurrent work.
The [working plan](../../etc/operations/t447-wow-execution-plan.md) retains the
cross-T ownership boundaries; [Queue](../../states/QUEUE.md) is unchanged. T447 and the
persistent WOW32 objective remain unfinished.

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T446 | Console visibility. [Closure](../../history/m0-t446-carrier-visibility-closure.md) |
| T445 | Integrated Win3.x style. [Closure](../../history/m0-t445-window-style-closure.md) |
| T444 | Indexed16 text. [Closure](../../history/m0-t444-indexed16-text-closure.md) |
| T441 | Release integrity. [Closure](../../history/m0-t441-variable-worker-io-release-integrity-closure.md) |
| T440 | Bounded performance work. [Closure](../../history/m0-t440-text-response-performance-closure.md) |
| T438 | Win3.1 path recovery. [Closure](../../history/m0-t438-win31-path-recovery-closure.md) |
| T436 | Limited setup delivery. [Closure](../../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Portable launch. [Closure](../../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

S3 closeout preserves the complete pre-closure snapshot and indexed evidence;
only CURRENT admits S4. The owner-approved unlocated common-dialog risk and
original-guest limitations remain explicit non-pass outcomes. Queue order and
the full persistent goal are unchanged. Reviewed task commits/pushes to main
and the existing origin remain permanently authorized; no new semantic
concession or runtime mutation is authorized by this documentation closeout.
