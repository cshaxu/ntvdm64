# Project Status

## Current Work

**Active: M0 T483 S20** (Ordinary Mode). Owner authorized sequential
S18-S20 execution. S19 is closed; S20 is the only active packet.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 清理 compat→non-mvdm、compat→mvdm-host、non-mvdm→mvdm-host；C拆为C1 non-mvdm、C2 mvdm-host，连续执行S18、S19、S20。 |
| Identifier Mode | M0 T483 S20, Ordinary Mode. |
| Admission And Approval | Owner accepted S17 and authorized successor admission, 2026-11-20. |
| Objective | Remove B-to-C1 private coupling through B-owned typed callbacks supplied by C1; preserve original algorithms, state ownership and synchronous lifetime. |
| Reference Baseline | S19 P29 verified packagedefd1dcf-12b6-4637-b606-fa7b8dbb49d5. [S19 closure](../etc/evidence/m0-t483-s19-closure.md). |
| Files And ABI Surface | DIR-ordered compat callers and actual non-mvdm providers; callback signatures, binding and teardown, original layouts and current product graphs. |
| Candidate Proposal | [T483 parent](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Applicable Rules | Ordered A/B/C1/C2/D/E/F architecture; historical B-to-C exceptions are S19/S20 cleanup obligations; reviewed inner-owned callbacks and cross-image ABI rules. |
| Verification | Exercise consolidated checks against current graphs and negative controls. Any production/build changes receive cached full build, dual11 publication, targeted tests, commit and push; evidence-only changes use governance/link checks without redundant product builds. |
| Expected Markers | DIR-ordered affected-file ledger, separate per-layer violation and approved runtime relationship counts, explicit coverage gaps and reproducible gate commands. |
| Asset Needs | Existing built products, compiler header records, linker maps and source evidence; no new guest assets. |
| Non-goals | No fresh mirror-hunk audit, addon work, original guest modification/rebuild, queued T admission or broad product redesign. |
| Similar-Issue Sweep | Direct/transitive/forced headers, private search paths, linked and generated providers, dynamic binding and callback ownership; distinguish named-image dependencies. |
| Reporting Requirements | Per-file findings and cumulative progress; A-F counts separately; non-error mirror point/line changes; no silent exceptions or unsupported zero claims. |
| Stop Conditions | No artificial same-thread queue, duplicate state machine or blanket exception. Mirror increases below5 lines and at most2 points, or exactly1 new point and fewer than100 added diff lines, have standing approval; larger increases require approval. Preserve unrelated work. |
| Exit Criteria | All B-to-C1 candidates have provider/lifetime dispositions; direct private implementation dependencies removed, typed callbacks verified, small-five delivered; S19 B-to-C2 zero baseline retained. |

## Current Technical Baseline

S14 production is delivered and owner accepted. Its [621-ID closure](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-final-dependency-closure.json)
and [P163 callback simplification](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s14-control-callbacks.md)
record the S14 baseline and approved callbacks D9/F3; the S18-S20 boundary audit continues below. Runtime callbacks are not
reported as absent. One necessary Console cleanup handoff remains.

## S1 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S2 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S3 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S4 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S5 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S6 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S7 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S8 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S9 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S10 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S11 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S12 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S13 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S14 Closure Record

Closed; [evidence](../etc/evidence/m0-t483-s14-closure.md).

## S15 Closure Record

Owner-closed on2026-11-20. [Final ownership and delivery evidence](../etc/evidence/m0-t483-s15-closure.md).

## S16 Closure Record

Owner-authorized closure after verification. [S16 closure evidence](../etc/evidence/m0-t483-s16-closure.md).

## S17 Closure Record

Owner accepted; [S17 closure and delivery](../etc/evidence/m0-t483-s17-closure.md).

## S18 Closure Record

Owner-authorized sequential closure; [S18 verification and publication](../etc/evidence/m0-t483-s18-closure.md).

## S19 Closure Record

[S19 closure](../etc/evidence/m0-t483-s19-closure.md):38/38 files complete;
B-to-C2 direct links, compiled headers and search roots are zero. P29 small-five
is the closure delivery. Typed runtime callbacks remain explicit.

## S20 Progress

The [DIR inventory](../../artifacts/mvdm-audit/t483-b-c1-dependencies/file-inventory.json)
is authoritative for individual files and evidence. P80 final compiled-product review leaves 81 of 94
files complete. B-to-C1 has zero direct function links (previously 12 unique/18 raw),
17 compiled-header rows across 9 sources and zero private search roots. B-to-C2 has zero
direct links, compiled-header rows and private search roots under the current
scan. These measures are distinct and must not be summed. Original-layout
and callback-provider relationships remain subject to their recorded scope.

S20 cumulative non-error mirror delta is +9 points/+481 lines; S19 retained
delta is +12 points/+574 lines. Full regression is required at S20 closure;
individual P deliveries use cached full build, dual publication and targeted
tests. The P62 package is 71882068-22ca-4a1a-8a84-20c8d7f7cb5c.
[P62 delivery evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p62-delivery.json). The prior
function-relocation scan did not count a direct data reference removed in P52;
other data-symbol coverage remains to be audited.

The [preserved status chronology](../../artifacts/mvdm-audit/t483-b-c1-dependencies/current-before-p49.txt)
and the inventory-linked P delivery records retain earlier progress and
verification evidence. Earlier closure evidence: [S14](../etc/evidence/m0-t483-s14-closure.md),
[S17](../etc/evidence/m0-t483-s17-closure.md),
[S19](../etc/evidence/m0-t483-s19-closure.md), [T482](../history/m0-t482-closure.md),
and [T481](../history/m0-t481-closure.md).

## Recent M0 Closures

- [T482 closure](../history/m0-t482-closure.md).
- [T481 closure](../history/m0-t481-closure.md).

## Recent Governance

Detailed prior governance records are retained in the preserved status chronology.
P63 published checkpoint e20b6e48-fb35-4fc1-bb5e-6de74d75b652 passed cached x86/x64/reference builds and published-package targeted tests. Native Shell nested exception intermittency remains unresolved; no completion increment. [Evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p63-checkpoint.json).

P64 cached full builds and dual11 publication passed, package
6479f7ba-76c7-4d01-846b-e8b06d52fb9a. Seven native lifecycle/window cases
passed; published USER16 and complete-Userenv COMMAND/EDIT/CMD/MEM passed.
Inherited developer-environment COMMAND fails on both P63 and P64; no product-fix
claim. Final fixture verification passed; P64 records this bounded delivery.
No completed-file credit
or mirror change. [Batch evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p64-direct-provider-progress.json).

P65 removed the redundant NT header chain in 14 B files: 84 header rows removed,
five files complete and 44 remain. Cached full build, dual 11-image publication and
seven targeted fixtures plus published USER16/complete-Userenv COMMAND passed.
The RTL fallback declaration preserves original link-time provider selection.
No mirror or guest change. [P65 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p65-header-closure.json).

P66 completes the three button/resource/DrawState B files through opaque C-owned
geometry and synchronous rendering, preserving scoped B GDI cleanup. Remaining:41
files. Mirror delta0 points/+37 lines, registered in pending decisions. Cached full
build, dual11 publication, button/control and seven lifecycle fixtures, published
USER16 and complete-Userenv COMMAND passed. [P66 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p66-button-resource-closure.json).

P67 completes five B files by moving original message-table storage/reset to C1
and length validation to the existing merge provider. Remaining:36 files; private
header rows77->72. Mirror+1point/+15lines, pending R2756. Cached full build, dual11
publication, failure/bitmap/default/resource and lifecycle fixtures, published
USER16 and COMMAND passed. [P67 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p67-message-table-closure.json).

P68 reconciles three unchanged delivered providers (profile, startup notification,
timer) against current compiled evidence and their original delivery tests. No
product or mirror change; remaining33 files. [P68 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p68-existing-provider-reconciliation.json).

P69 closes Shell dependency ownership only: real guest Shell and outer-unwind
controls pass; direct native catch-and-resume nesting fails in Windows.Storage
without the project adapter. Preserve that failed diagnostic; no product repair
or Full-pass claim. Remaining32 files. [Shell evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p69-shell-provider-review.json).

Class storage candidate requires owner approval for +2 non-error points/+20lines.
Its first build failed on PCALLPROCDATA redefinition; all candidate production
edits were withdrawn. [Pending candidate](../../artifacts/mvdm-audit/t483-b-c1-dependencies/class-owner-pending-candidate-status.json).

P70 completes menu and task-lifecycle source boundaries through the existing
C1 cleanup provider, without changing original type values or lifetime policy.
Header rows72->70; remaining30 files; non-error mirror delta0points/0lines.
Cached full build, dual11 publication, six targeted cases and published USER16
and COMMAND passed. [P70 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p70-object-types-closure.json).

P71 completes default-native/default-window/window-access boundaries through
the existing original-window provider. Remaining27 files; header rows70->64.
Mirror+1point/-1normalized line (actual insertion5lines), pending R2757.
Cached full build, dual11 publication, three boundary/six lifecycle cases and
published USER16/COMMAND passed after fixing a test-only missing declaration.
[P71 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p71-window-identity-closure.json).

P72 completes EDIT conversion/thread boundaries with existing providers and
opaque pointer declarations. Remaining25 files; header rows64->60. No mirror
or guest change. Cached full build, dual11 publication, three boundary/six
lifecycle cases and published USER16/COMMAND passed.
[P72 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p72-edit-boundary-closure.json).

P73 completes EDIT call boundary using the existing control-state provider.
Remaining24 files; header rows60->58. Mirror +2points/0normalized lines
(actual insertion6lines), pending R2758. No guest change. Cached full build, dual11 publication, three boundary/six
lifecycle cases and published USER16/COMMAND passed.
[P73 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p73-edit-boundary-closure.json).

P74 completes EDIT entry boundary through existing entry/registration owners.
Remaining23 files; header rows58->54. Mirror +1point/+5lines, pending R2759.
No guest change. Cached full build, dual11 publication, three boundary/eight
lifecycle cases and published USER16/COMMAND passed.
[P74 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p74-edit-boundary-closure.json).

Control-state ownership proposal is pending owner approval: two original C
control operations, +2 non-error points/+79lines. This candidate is not in P76;
no completion increment or build claim applies to that candidate. [Decision](../../artifacts/mvdm-audit/t483-b-c1-dependencies/control-state-placement-decision.json).

P76 completes the shared capture interface: original layouts are explicit at
C/F consumers, never copied into B. Remaining22 files; compiled-header rows
remain54 across18 sources. Mirror +0points/+4lines, pending R2760/R2761.
Cached full build, dual11 publication, original capture and eight lifecycle/class
cases plus published USER16/COMMAND passed.
[P76 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p76-capture-interface-closure.json).

P77 completes dialog creation/finalization boundaries through existing providers.
Remaining20 files; header rows54->47 across16 sources. Mirror +0points/+39lines
at one original manager insertion, pending R2762. Full cached build, dual11
publication, actual C finalization/creation boundaries, eight lifecycle/class
cases and published USER16/COMMAND passed.
[P77 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p77-dialog-boundary-closure.json).

P78 completes dialog lookup, pointer and pointer-state boundaries. Original
_GetDlgItem and SetDialogPointer bodies now reside in the existing C manager;
B uses the existing provider for validated pointer publication. Remaining17
files; header rows47->32 across13 sources. Mirror +0points/+54lines at one
insertion, pending R2763. Cached full build, dual11 publication, targeted
boundaries/lifecycles and published USER16/COMMAND passed.
[P78 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p78-dialog-boundary-closure.json).

P79 completes dialog memory and the borrow/private-access WW layout group.
Remaining14 files; compiled header rows32->26 across10 sources. Original dialog
callbacks now reside in C; B retains its scope and calls the existing C
window-access provider for WW layout operations. Non-error mirror +1point/+67lines,
pending R2764/R2765. Cached full build, dual11 publication, targeted boundaries,
borrow graph/lifecycles and published USER16/COMMAND passed.
[P79 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p79-dialog-and-words-closure.json).

P80 returns original object classification table to its sole C cleanup reader.
Remaining13 files; compiled header rows26->17 across9 sources. No callback or
second state; actual WOW32 map identifies cleanup.obj as the unique table owner.
Non-error mirror +0points/+22lines, pending R2766. Cached full build, dual11
publication, source identity/cleanup/task/menu tests and published USER16/COMMAND passed.
[P80 evidence](../../artifacts/mvdm-audit/t483-b-c1-dependencies/p80-object-ownership-closure.json).
