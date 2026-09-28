# Project Status

## Current Work

## Active Packet

**Active: M0 T423 S9**

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T423 S9, Ordinary Mode. |
| Candidate Proposal | [Console/Window frontend](../proposals/proposal-kvm-window-graphics-presentation-001.md). |
| Admission And Approval | Owner approved the proposal's ConPTY stage and automatic sequential admission. S8 P2 d253e55af and P3 6ef96410d are pushed; bounded closure review admits S9. |
| Objective | Replace the project hidden-Console/helper backend with frontend-owned ConPTY; retain Console/Window interaction, authenticated nesting, mouse and lifecycle semantics with one shared bitmap text presentation. |
| Non-goals | No guest/shared-library mutation, DOS/WOW scheduler, path-search repair, extra helper executable, permanent legacy backend fallback or automatic T closure. |
| Reference Baseline | S8 P2 d253e55af, P3 6ef96410d and the verified seven-file O:/winnt package; S8 ledger includes retained S7 capabilities and new GUI launch/wait evidence. |
| Files And ABI Surface | ntkvm-exe native backend, input/output view and bitmap presentation; existing authenticated native launch binding and graph/tests. Audit transport-dependent membership/input-return contracts before replacement. |
| Applicable Rules | EXECUTION, source policy, original mirror/ABI, output hygiene and the owner-approved independent frontend boundary. |
| Verification | Audit reusable terminal sources/license/x86 closure first; test split UTF-8/VT, state/replies, Unicode, raw/cooked keyboard/mouse, control events, geometry/scrolling, backend faults/drain and authenticated nested return. Retain DOS17 in both displays, both twelve-target chains, fault matrices, GUI wait and separate WOW frontiers. |
| Expected Markers | A stable ConPTY survives display switching; current complete screen is immediately available; one query-reply owner; original DOS remains outside ConPTY; real target input/output and results match the baseline without helper fallback. |
| Asset Needs | Pinned reusable terminal component source and license audit, Windows ConPTY API evidence, existing immutable guests and checked-in fixtures. New directories only under build/M0-T423/S9; no physical owner-desktop manipulation. |
| Reporting Requirements | Exact tested source/package/configuration, retained failure evidence and source-first ownership disposition; no compile-only or research-only runtime claims. |
| Stop Conditions | Guest/library mutation, unauthenticated ownership, unapproved scheduling, unproved input-record equivalence, or product regression. Do not publish a migration candidate before the complete production gate. |
| Exit Criteria | All S9 proposal rows evidenced, superseded helper/duplicate renderer removed, retained runtime contracts pass, code/import footprint reported, seven-file coherent publication and clean pushed delivery. |
| Original Owner Request | Automatically admit S stages; replace the hidden Console/helper with ConPTY, preserve both displays and use the same text bitmap scheme for DOS/native output. |
| Similar-Issue Sweep | Backend creation/EOF/cancellation/backpressure, final drain, session membership, target descendants, redirected/aliased handles, unconsumed input at DOS/native handoff, mouse/control events, terminal replies, resize and wide characters. |

## S7 Closure Record

Owner addition dated 2026-09-27: rename frontend.exe/frontend-exe to
ntkvm.exe/ntkvm-exe and basesrv.exe/basesrv-exe to ntsrv.exe/ntsrv-exe within
S7. run16.exe and ntvdm.exe keep their names. Preserve historical OpenNT
BaseSrv symbols and semantics, authenticated endpoint contracts, imported
library bytes and guest. The old-name seven-file mouse checkpoint has been
published and its formal-path tests retained; it is not renamed-package
acceptance. Build/startup/process-identity/test/package references have been
updated and the verified new-name package is now coherently published with a
recoverable old-name backup. S7 P1 99276d68d is committed and pushed to main;
closure review confirmed clean synchronization and all seven live hashes.

The [S7 ledger](../etc/evidence/m0-t423-s7-window-mouse.md) records real guest
mouse count, text/graphics painting, relative input/callback and retirement
passes; native Window mouse and retained Console mouse pass. Console/Window
DOS17, both twelve-target chains, both five-case fault matrices, graphics
return and separate inherited WOW frontiers pass on the candidate. The exact
new-name seven-file set is published at O:/winnt with recoverable old-name
backup and unchanged configuration. It passed component17, Console/Window
DOS17, both twelve-target chains, both five-case fault matrices, real DOS
mouse positive/negative cases, graphics return and inherited WOW frontiers.
Published Console17, Window MEM/nested MEM/EDIT and separate WOW frontiers
also passed. S7 is closed at its admitted mouse/naming scope. Shared libraries
and guest media are unchanged. Physical focus/capture remains owner-waived,
and SOL/WRITE retain their known OOM frontier, not usability acceptance.

## S8 Closure Record

Owner addition: rename the product executable monitor.exe to ntmon.exe in
this S8 delivery. Keep src/monitor-exe and the NTVDM Task Monitor UI title;
only the executable name and its live build/test/package references change.
The verified seven-file publication must contain ntmon.exe and retire the old
monitor.exe with a recoverable backup, never leave two product names active.

The [S8 ledger](../etc/evidence/m0-t423-s8-gui-launch-wait.md) retains detailed
source analysis, every failed attempt, exact hashes and reproducible evidence.
Implemented: default GUI asynchronous startup, explicit --wait, authenticated
WOW startup receipt distinct from task completion, original WOWEXEC no-op
acknowledgement and bounded unfiltered WOWEXEC pending-post binding. Protocol
9 is selected consistently; original DOS/WOW record policy remains unchanged.
The approved side-chat repair retains the ten-second empty-broker grace and
passes seven real RPC cases; it introduces no idle-worker eviction.

Candidate verification passed option19, native GUI16, CMD/batch/input-loop
Win16 callers, original COMMAND /c and persistent interactive GUI callers,
new/reused WOW startup faults, real loader failure/recovery, component17,
Console/Window DOS17, both twelve-target chains and both lifecycle matrices.
Interactive tests require actual MEM output; explicit waits also require no
MEM execution before target release. Five real guest mouse cases, a no-input
negative, Console keymouse and graphics return pass. Separate headless WOW
observations retain WINMINE and the inherited SOL/WRITE OOM frontiers, not
new SOL/WRITE usability. Physical desktop focus remains owner-waived.

Final source review is complete. The coherent tested seven-file S8 set is now
at O:/winnt, including ntmon.exe; guest/configuration and NTVDM.REG were
preserved. The complete previous package and before/after hash manifest are
under build/M0-T423/S8/pre-publication. Both formal graphs have no pending
work, and candidate/published hashes match. Published-path Console DOS17,
Window MEM/nested MEM/EDIT and the three separate WOW frontiers passed.
S8 P2 d253e55af is committed and pushed to main, including ntmon naming and
the approved side-chat ntsrv repair. P3 6ef96410d records delivery; clean
HEAD/origin identity and the requirement-by-requirement ledger support bounded
S8 closure. This does not close T423 or claim SOL/WRITE usability.

## S9 Progress

The [S9 ledger](../etc/evidence/m0-t423-s9-conpty-migration.md) records the
source/boundary audit. Existing ConPTY observer fixtures are reusable
test infrastructure, not a production terminal parser. The current backend
also exposes Console membership and unread INPUT_RECORD reclamation; replacing
only its launch and output paths would lose accepted nesting/lifetime behavior.
These contracts must be proved against ConPTY before a production migration.
The checked-in x86 lifetime probe now passes three real ConPTY cases:
descendant survives its direct parent, explicit close delivers CTRL_CLOSE,
and ReleasePseudoConsole permits natural EOF after the last client exits.
Retained-HPCON is a negative control: it does not naturally reach EOF.
This was verified on kernel32 10.0.26100.9549; it does not establish a new
product OS minimum or solve unread input return. The ledger retains the
initial misdirected-stdout failure and corrected screen-output assertions.
The verified S8 package remains published; no S9 candidate is deployed.

## Current Technical Baseline

S6 P1 f0f671e5e delivered independent frontend Console/Window display;
[S6 ledger](../etc/evidence/m0-t423-s6-window-display.md). S7's accepted
ntkvm/ntsrv seven-file mouse checkpoint is recoverable under
build/M0-T423/S8/pre-publication; the reviewed S8 set is now at O:/winnt,
with published-path verification complete and S8 P2 d253e55af pushed.

Earlier S1/S2 lifecycle and copied-I/O delivery, S3 preservation/replanning,
S4 independent frontend delivery (9c27b5fd2), and S5 hidden-backend acceptance
(3e425046d / 1fb291a8f) remain retained in the indexed evidence. S3 is not
retroactively labelled functional closure. Full previous CURRENT chronology
is preserved in [the status archive](../etc/evidence/m0-t423-restart-prior-status.md#archived-s7-pre-rename-status-chronology-2026-09-27).

## Remaining admitted sequence

S9 ConPTY migration is the only active packet, followed by S10
minimization/full closure audit. Automatic sequential
admission is authorized; final T closure requires owner acceptance.

Physical focus/clipping observation is owner-waived with source/unit evidence;
private-desktop integration remains required. Separate WINMINE, SOL and WRITE
headless frontiers remain mandatory; existing SOL/WRITE OOM is not usability.
Guest and imported library changes remain prohibited without distinct approval.

## S1 Closure Record

Delivered lifecycle baseline; [S1 evidence](../etc/evidence/m0-t423-s1-restart-lifecycle.md).

## S2 Closure Record

Delivered copied I/O boundary; [S2 ledger](../etc/evidence/m0-t423-s2-console-boundary-ledger.md).

## S3 Closure Record

Preserved and replanned, not functional closure; [S3 handoff](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S4 Closure Record

Delivered independent frontend; [S4 evidence](../etc/evidence/m0-t423-s3-hidden-console-ledger.md).

## S5 Closure Record

Delivered hidden backend; [S5 acceptance](../etc/evidence/m0-t423-s5-hidden-backend-acceptance.md).

## S6 Closure Record

Delivered Console/Window display; [S6 ledger](../etc/evidence/m0-t423-s6-window-display.md).

## Recent M0 Closures

T422 remains owner-closed. T423 S8 P2 d253e55af / P3 6ef96410d is the last closed
subtask; S9 ConPTY is admitted. Prior closure records
are preserved in the indexed status archive above.

## Recent Governance

One active S only. Preserve concurrent owner-approved queue/proposal changes
in the reviewed delivery. T422 stays owner-closed. The full objective and T423
remain open until all admitted stages and owner acceptance are complete.
