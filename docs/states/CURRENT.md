# Project Status

## Current Work

**Active: M0 T485 S16** (Ordinary Mode), G001-G033.
[S15 closure](../etc/evidence/t485-s1/s15-closure.md): N013-N026 delivered;
S15 delivered62/159 directories, H001-H036 and N001-N026. S16 P1/G001 brings
the total to63/159; P2/G002 to64/159; P3/G003 to65/159; P4/G004 to66/159; P5/G005 to67/159.
S16 runtime5 is published with archive-derived XP SP3 DEBUG verification;
runtime2 retains the latest Full24/24 checkpoint.
The [working plan](../etc/operations/m0-t485-h-repair-plan.md)
retains S17-S20 and all remaining migration/capability obligations.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T485 S16, Ordinary Mode. |
| Candidate Proposal | [XP baseline migration](../proposals/proposal-windows-xp-ntvdm-baseline-upgrade-001.md). |
| Admission And Approval | Owner continuous-execution approval; S15 P92 closes N026 with runtime108 Full24/24 and admits the next original directory cohort. |
| Objective | Reconcile G001-G033 existing guest components with pinned XP SP1 and required host/media counterparts, preserving original binaries and minimal evidenced mirror adaptations. |
| Reference Baseline | S15 P92/runtime108; XP a2f6d7c93aa4f11efc51147566d5961c59a8ef89; registered OpenNT original per not-yet-migrated file. |
| Files And ABI Surface | G001-G033:33 directories,896 frozen entries (211 ancestor-direct,685 leaf), bin86 and DOS tools/COMMAND/devices/kernel/headers/redirector. Required counterparts are admitted with their consumer. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [documents](../rules/DOCUMENT.md), [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Each P cached full build, dual publication/hash checks, affected targeted tests, commit,push. Every10 completed files and S closure require final-package Full. Retry only failed/unexecuted gates on unchanged inputs. |
| Expected Markers | Original Git identities and three-way comparison; every missing XP path relocated or retired with evidence; selected branches/ABI retained; package/media identities and real guest boundaries verified. |
| Asset Needs | Pinned local XP SP1 source and approved English x86 Professional SP3 media. Preserve supplied original guest binary bytes; no rebuilding or patching. |
| Constraints | DIR order, complete-file review, minimal nonerror diff, correct #error/#line. No Computer Use, unrelated changes, original guest rebuild/patch, or unapproved large mirror increase. |
| Non-goals | Independent XP-only components belong to next T unless required now. No new scheduler, emulator backend or unrelated product redesign. |
| Reporting Requirements | Before each batch: scope and functional changes. After: per-file original-relative points/lines, global non-guest census and commit delta, migration counts, actual verification/publication state. |
| Stop Conditions | Missing required original input, unsupported required ABI, unexplained regression, or material unapproved boundary/mirror growth; resolve within authority or request bounded decision. |
| Exit Criteria | All33 directory entries reconciled with dependencies and minimality evidence; final coherent published package Full passed; clean committed/pushed tree. |
| Original Owner Request | Complete existing-component XP baseline migration through G097; preserve architecture and original guest, avoid excessive repeated validation, retain all remaining capability obligations. |
| Similar-Issue Sweep | XP-absent files and historical products, raw newline/EOF, production input identity, host/guest ABI, dormant original code, changed compile guards and false pass/skip outcomes. |

## S16 Progress

5/33 directories complete through P5. [G001](../etc/evidence/t485-s1/g001.md):
18 exact XP files and1 retired makefile. Four existing archive members advance
to identical XP SP1/SP3 originals, without adding archive members. Cached
build, dual publication (26 host and8 guest hashes), WINHELP document/topic
observation and final-package Full24/24 passed. The initial WINHELP assertion
used the wrong class; evidence retains that correction and exit limitations.
[P1 receipt](../etc/evidence/t485-s1/g001-result.json) records final identities.
[G002](../etc/evidence/t485-s1/g002.md) is complete:5 same-path XP
originals,2 original-path relocations,9 retirements and1 direct original
cleanup dependency. No guest binary is rebuilt and no runtime payload changes.
The complete16-entry cohort passed cached build, dual26 publication hashes,
raw-original identity checks and final-package Full24/24 on runtime2.
[P2 receipt](../etc/evidence/t485-s1/g002-result.json) records final identities.
G003 delivered the XP BACKUP original in the archive and runtime;65 media
identities, actual help and exit0 passed. [P3](../etc/evidence/t485-s1/g003-result.json).
G004 delivered48 raw XP files/relocations and46 retired old paths; archive
COMMAND remains the approved XP original. Cached build,dual26hash, archive
classifier/lifecycle and COMMAND VER exit0 passed. The latter uses the existing
normal-logon environment helper; inherited agent environment failed with an
illegal instruction and is not claimed supported. No product workaround.
[P4 receipt](../etc/evidence/t485-s1/g004-result.json). G005 delivered30 raw XP counterparts and10 retired paths. Source-tree DEBUG
reported Incorrect DOS version; approved SP3 system DEBUG replaces that runtime
candidate. Cached build,dual publication,prompt/arithmetic/quit exit0 passed.
[P5 receipt](../etc/evidence/t485-s1/g005-result.json). Next is G006.
For guest-source-only batches the affected targeted check is original identity,
paths and consumers; replacing runtime guest binaries adds their direct runtime
tests. Tests must consume the XP originals from winnt.zip; old runtime or
fixture directories must not silently supply older guest binaries. Audit and
close source/archive/test identity gaps before completing T485.
S15 final census1340files/801byte-identical/812nonerror-identical/528different/
8191points/45674lines;1340XP/0OpenNT in the non-guest population only.
Full counter reset at S16 P2/runtime2 after16 entries plus1 direct dependency. No guest-directory completion
credit is inferred from paired host dependencies or existing package tests.

## S15 Closure Record

[S15 closure](../etc/evidence/t485-s1/s15-closure.md),
[137-entry inventory](../etc/evidence/t485-s1/s15-closure-inventory.json),
[final receipt](../etc/evidence/t485-s1/n026-result.json).
14/14 directories;98 retained paths,34 relocated,5 retired. runtime108 cached
build, dual26hash, original lifecycle and Full24/24 pass. Win95 normal-exit
remains the approved boundary. Overall62/159; T485 remains active.

## S14 Closure Record

S14 completed N004-N012; final runtime12 Full24/24 and clean delivery admitted
S15. [Closure](../etc/evidence/t485-s1/s14-closure.md) and
[archived detailed status](../etc/evidence/t485-s1/s14-status-history.md)
retain all per-P results, counterexamples and approved boundaries.

## S13 Closure Record

[Closure](../etc/evidence/t485-h-repair/s13-closure.md) and
[receipt](../etc/evidence/t485-h-repair/s13-closure.json) retain the1039-file
hash audit, frozen diff dispositions, corrected historical totals, published
package equivalence and Full evidence. Win95 exit-one remains a boundary.

## S1 Closure Record

[Inventory, media qualification, H001 ABI evidence and rollback](../etc/evidence/t485-s1/README.md).

## S2 Closure Record

[H001-H008 delivery](../etc/evidence/t485-s1/h005-h008.md).

## S3 Closure Record

[H009-H022 delivery](../etc/evidence/t485-s1/h022.md).

## S4 Closure Record

[H023-H036 closure](../etc/evidence/t485-s1/s4-closure.md).

## S5 Closure Record

[S5 P4 repair and closure](../etc/evidence/t485-s1/s5-p4-dpmi-ownership.md).

## S6 Closure Record

[S6 selector ownership repair](../etc/evidence/t485-h-repair/s6-selector-investigation.md).

## S7 Closure Record

[S7 closure](../etc/evidence/t485-h-repair/s7-progress.md).

## S8 Closure Record

[S8 progress](../etc/evidence/t485-h-repair/s8-progress.md).

## S9 Closure Record

[S9 closure](../etc/evidence/t485-h-repair/s9-progress.md).

## S10 Closure Record

[S10 progress](../etc/evidence/t485-h-repair/s10-progress.md).

## S11 Closure Record

[S11 closure](../etc/evidence/t485-h-repair/s11-progress.md).

## S12 Closure Record

[S12 list](../etc/evidence/t485-h-repair/s12-progress.md).

## Recent M0 Closures

- [T484](../history/m0-t484-closure.md).
- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Recent Governance

S14 N004 restores the required governance section omitted by the prior
status compaction; the current delivery rechecks structure and links.

## Current Technical Baseline

Version0.0.485 XP migration retains selected CCPU, immutable guest and thirteen
host images including NTVDMD.DLL, VCDEX.DLL and VJOY.DLL. Source baseline is
XP SP1; approved retail reference is English x86 Professional SP3. The owner
approved local source use and ordinary delivery. Required dependencies may be
imported with the current directory; independent XP additions belong to the
next T. Source/media provenance and original notice obligations remain.
