# Project Status

## Current Work

**Active: M0 T483 S12** (Ordinary Mode). The owner accepted S11 and admitted C-layer direct dependency cleanup.

## Active Packet

| Field | Record |
| --- | --- |
| Original Owner Request | 先解决 C 层可以原地解除的直接依赖，随后处理可安全迁到 F 的产品职责，最后逐案判断 F 回迁 C。 |
| Objective | Remove direct outward C dependencies that can be eliminated in place. Classify all baseline evidence by direct owner and mechanism. |
| Files And ABI Surface | Production build graphs, mirror consumers and their include, link and runtime contracts. Keep the selected ABIs. |
| Reference Baseline | 81f4cef3d and the [C dependency tracker](../../artifacts/mvdm-audit/t483-c-layer-dependencies/README.md). |
| Applicable Rules | Architecture, source policy, coding and execution authorities. Attribute direct dependencies to their actual source. |
| Verification | Check production commands, compiler includes, resolved link providers and applicable behavior. Compare mirror points and lines with the fixed original. |
| Asset Needs | Existing release, fixed OpenNT baseline and cached production graphs. |
| Candidate Proposal | [T483 parent proposal](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Identifier Mode | M0 T483 S12, Ordinary Mode. |
| Admission And Approval | Owner closed S11 and authorized the three sequential C-layer scopes. |
| Owner amendment | Process the named ntvdm.c worker entry migration during S12. Preserve the independent CPU registry binding. Review every transitive witness and associate it with the direct edge. |
| Non-goals | No guest changes. Other ownership migrations belong to the two successor scopes. |
| Expected Markers | Eliminated direct edges have compiler or linker witnesses; indirect-only evidence stays traceable separately. |
| Similar-Issue Sweep | Review both C owners and all supported production targets. |
| Reporting Requirements | Record stable IDs, direct source, provider, disposition, non-exclusion mirror block and line deltas and verification. |
| Stop Conditions | Obtain prior approval before increasing mirror differences. |
| Exit Criteria | Reconcile in-place candidates and open ownership decisions with the tracker; verify and deliver the completed changes. |

## Current Technical Baseline

S12 P1 eliminated C0042, C0139 and C0143 in place. The worker build selects its historical terminate symbol name directly. The two unused private search directories have been removed from product and fixture generators. All eight production graphs covering eleven images passed the command check. The 27 async publication and 14 OEM assertions, original COM lifecycle, 98-object nonworker compiler closure and its injected negative control passed. Both cached architectures built. Both release manifests match package 7fe6d388-ad54-4952-a212-1abeefe9cb75. All executable sections match P7. Mirror block and line deltas are zero. Of 599 stable IDs, 544 remain open, 51 are supporting evidence, three eliminated and one excluded diagnostic. S12 remains active.

P2 removed C0056, C0138, C0140 and C0144 without changing either mirror. The WOW32 graph also dropped its inherited worker command and SoftPC search roots. P2 package 93bb7b13-4fcd-42ce-bbfe-c318a53669ba was built with both cached toolchains and hash-verified at both destinations. Every byte of all eleven P2 images equals P1 after masking only the package GUID and COFF/debug-directory timestamps. The WOW32 formal graph, worker session lifecycle and A/B production boundary checks passed. The expanded compile commands and a VDMREDIR link map corrected the baseline coverage. See the C tracker for the updated counts.

P3 restored the original SoftPC entry and placed process startup and exit in the F worker owner. The [P3 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p3-entry.json) covers both published packages, 539 compiler contexts and the mirror reduction of two blocks and ten lines. The tracker retains all original indirect observations.

P4 removed 60 private include-directory occurrences from AMD64 mirror object commands. Six root IDs remain open across the supported graphs. Both cached architectures built, both eleven-image releases were verified and four targeted native fixtures passed. See the [P4 receipt](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p4-native-roots.json).

P5 scoped three private header roots on x86. The worker graph drops 1171 C-layer search-root occurrences. Eleven published images match P4 after normalizing only the package GUID and PE timestamps. Two redirector and two WOW fixture runs passed. The [P5 receipt](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p5-worker-roots.json) records the counts and fixture corrections.

P6 audited coalesced storage in the eleven product link maps, cross-checking C relocations with tentative definitions in selected COFF objects. The [storage evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p6-common-storage.json) adds eleven direct bindings and distinguishes ten mixed declaration groups. The tracker has 617 IDs; 555 remain open. P6 is evidence-only and retains the P5 package.

P7 traced all ten mixed storage groups: seven redirector slots and the USER wheel remainder have C owners. Two BASE server globals require a startup-binding review in S14. The original server initializes both; the current lifecycle fixture supplies its own heap. Product failure has not been demonstrated. See the [per-symbol review](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p6-common-storage.json).

P8 indexed all 462 open link symbols against 170 fixed original caller sources. The [caller anchors](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p8-caller-anchors.json) distinguish name occurrences from product relocations. C0456 requires a calling-convention and guest-buffer ownership review before restoring the original flush provider. Package and tracker totals remain at P7.

P9 removed the redundant forced service header from srvvdm.c. The normal include remains. Both release destinations have the eleven verified images. The [per-file inventory](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p9-file-dependencies.json) covers 539 product compile contexts and records 116 distinct C-layer files with a direct compile dependency and 170 with a direct link binding (190 in their union). The stable tracker still has 555 open items.

P11 moved shared package directory mechanics and guest directory queries into B compat. cmdconf.c now includes the B header. Both cached product builds, directory/PIF/media/boot tests and the eight-graph library boundary check passed. Eleven images were published and hash-verified at both destinations. See [directory receipt](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p11-directory.json). The sequential compile-dependency review is 1/116; 115 mirror files remain directly dependent. Mirror delta is zero blocks and lines.

P12 moves the existing registry provider and shared declarations from F/D to B. Eight mirror callers only change include paths; mirror delta is zero points/lines and #error/#line remain unchanged. Both cached architectures, final corrected x86 rebuild, eight product A/B graphs, native enum/persistence and 31 embedding checks passed. Eleven images were published and hash-verified at both destinations. See [registry evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p12-registry-candidate.json). File 002 retains its native-child lifecycle dependency for S13. Two files have been reviewed; only file 001 is fully cleared. S12 remains active.

P13 clears the remaining directory-query header dependencies in cmdkeyb.c and dem.c. Both cached architectures built. All eleven images equal P12 after masking only package GUID and PE timestamps; repeated runtime tests were omitted under owner authorization. Both releases were hash-verified. [P13 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p13-directory-callers.json) records 5/116 files reviewed, three fully cleared and two deferred lifecycle moves. Mirror delta is zero; cumulative S12 reduction remains two points and ten lines.

P14 moves the native NT I/O facade and directory restart helper into B. File 008 demsrch.c changes only its include path. Both cached architectures, eight product A/B graphs, x86/x64 native interface checks and 115 original DEM directory assertions passed. The fixture runner now refreshes its selected archives before linking. Eleven images are published and hash-verified. [P14 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p14-native-io.json) records 535 open stable IDs, 8/116 files reviewed, four cleared direct headers and four lifecycle files reserved for S13. Mirror delta is zero; S12 cumulative reduction stays two points and ten lines.

P15 moves the unchanged native invalid-VM failure helper from F to B. Both cached architectures, eight A/B graphs and ten NIDDB rollback cases passed; eleven images are published and hash-verified. [P15 evidence](../../artifacts/mvdm-audit/t483-c-layer-dependencies/s12-p15-native-failure.json) records 11/116 files classified: four cleared, seven lifecycle files reserved for S13, 105 unreviewed. C0241 is eliminated; 534 of 617 IDs remain open. Mirror delta is zero; cumulative reduction remains two points and ten lines.

## Successor scopes

S13 will assess and implement safe C-to-F moves of product-owned behavior, with before/after mirror block and line counts. S14 will review each proposed F-to-C restoration against original ownership, existing mirror differences and production use. Only S12 is active.

## S11 Closure Record

The owner accepted [A and B cleanup](../etc/evidence/m0-t483-s11-layer-boundaries.md) after P7 at 81f4cef3d. The [delivery record](../../artifacts/mvdm-audit/t483-s11-p7-compat-boundary.json) lists the two approved WOW32 exceptions, eight production graphs, both published eleven-image packages and twenty passing Full gates. The C dependency tracker uses direct ownership; transitive observations remain supporting evidence.

## S10 Closure Record

Owner accepted and closed T483 S10. P1 shipped at 14969ed80 with the targeted verification recorded in the [S10 delivery and closure record](../etc/evidence/m0-t483-s10-components.md).

## S9 Closure Record

[40-file compat audit and verification](../../artifacts/t483-s9-non-mirror-audit/sequential.md), delivered at 55643b6c5. [Successor packet delivery](../etc/evidence/m0-t483-s10-components.md) records this revision as its baseline.

## S1 Closure Record

[T483 S1 closure](../etc/evidence/m0-t483-s1-closure.md).

## S2 Closure Record

[T483 S2 closure](../etc/evidence/m0-t483-s2-closure.md).

## S3 Closure Record

[T483 S3 closure](../etc/evidence/m0-t483-s3-closure.md).

## S4 Closure Record

[T483 S4 closure](../etc/evidence/m0-t483-s4-closure.md).

## S5 Closure Record

[T483 S5 closure](../etc/evidence/m0-t483-s5-closure.md).

## S6 Closure Record

[T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md).

## S7 Closure Record

[T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md).

## S8 Closure Record

[T483 S8 closure](../etc/evidence/m0-t483-s8-closure.md).

## Recent M0 Closures

- [T483 S7 closure](../etc/evidence/m0-t483-s7-closure.md)
- [T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md)

## Recent Governance

Each sequential-file entry and exit reports both file and whole-audit totals.
The unrelated project [TODO](TODO.md) remains separate.
