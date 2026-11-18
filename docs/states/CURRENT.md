# Project Status

## Current Work

**Active: M0 T483 S7**

The initial production-order pass is complete. Follow the pending-decision
register in order, one file at a time. P48 ends at `cmdmisc.c`.

The owner closed S6 at P32 and admitted continuous S7 review. See
[S6 closure](../etc/evidence/m0-t483-s6-closure.md). The initial class.c,
winmgrc.c, profile.c and illegalp.c work belongs to S7.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T483 S7, Ordinary Mode with initial and counter-review passes. |
| Admission And Approval | Owner directed S6 closure, S7 admission, continuous file-by-file review, and a progress report for each file. |
| Candidate Proposal | [Mirror diff cleanup](../proposals/proposal-unconnected-mirror-diff-cleanup-001.md) |
| Objective | Review remaining non-error mirror differences in ascending verified production integration ratio. Restore original code where feasible and document necessary residuals. |
| Non-goals | No guest edits, reclassification of test-only code as production, or changes to the exclusion policy. Keep previously decided mirror ownership unless the owner revisits it. |
| Reference Baseline | P32, commit 861d7be0fabf2428f83ee24b5b58e62f90614f0d, package 520c9909-bee6-40e6-9345-dc6d85e6cf5f. OpenNT revision 5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. |
| Files And ABI Surface | Remaining classified host mirror files, actual caller/provider boundaries, and active review ledgers. |
| Applicable Rules | Original-first, whole-unit comparison, bare #error branch policy and original logical-line preservation. |
| Verification | Verify file hashes and original line anchors. Accumulate format-only changes. After a substantive file is fully reviewed, run cached x86/x64 builds, publish all 11 images to both destinations, verify, commit and push. Full passed against the published S7 P1 package and again against the current published package at P10 on owner request. Subsequent batches default to applicable targeted verification. |
| Expected Markers | Before/after non-error blocks and changed lines per file, source and ABI evidence, local versus delivered totals. |
| Asset Needs | Existing pinned originals, caches, and project fixtures. |
| Reporting Requirements | Report non-error blocks and changed lines before and after each file, S7 cumulative reductions, current global totals, and remaining file count. Identify measurement corrections separately from source restorations. Record necessary unapproved residuals in the pending backlog. |
| Stop Conditions | Net mirror increase requires owner approval. Missing evidence stays in investigation while independent files advance. |
| Exit Criteria | Every admitted file has a disposition, validated restorations are delivered, and residuals are recorded with evidence. |
| Original Owner Request | Close S6 and admit S7 for continuous audit of all remaining files with non-error differences, using production integration order. |
| Similar-Issue Sweep | Inspect complete original units and their real production consumers. Preserve harmless uncalled original code and existing #error exclusions. |

## Current Technical Baseline

S7 starts with 213 remaining file reviews. P1 package
`dee7d711-ad95-4e21-a76e-31e4daafeeab` contains version `0.0.483` and
protocol 67. The 11 images were published and hash verified at both
destinations. Cached x86/x64, the Redirector OEM capacity and server
enumeration fixtures, and all 20 Full gates passed against this package.
The first Full run exposed the stale `0.0.480` version. After correcting the
version and republishing, the version negatives and the DOS4GW/NESticle
graphics and input check passed in the complete run.
[Delivery evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p1-checkpoint.json).

| S7 file | Non-error blocks before → after | Changed lines before → after | Disposition |
| --- | ---: | ---: | --- |
| kernel/class.c | 12 → 12 | 1,390 → 1,390 | Existing native class ownership and failure handling retained. |
| client/winmgrc.c | 6 → 6 | 500 → 500 | Original active function bodies already retained. The shared fixture compilation boundary remains. |
| kernel/profile.c | 7 → 7 | 866 → 866 | Original read bodies retained. NT4 private profile mapping, enumeration and write providers remain outside the worker read boundary. |
| system/illegalp.c | 1 → 0 | 1 → 0 | Removed one added blank line. Error arms and logical lines checked. |
| kernel/getset.c | 10 → 10 | 678 → 678 | Native HWND result storage and opaque CallProc binding verified. |
| client/clmsg.c | 27 → 27 | 979 → 979 | P11 rechecked without counting it again. Keep the post-WM_MOVE retirement guard. |
| netlib/packstr.c | 5 → 3 | 478 → 261 | Restore two whole original packing functions. Isolated product link retains only the existing production function. |
| kernel/server.c | 20 → 20 | 1,535 → 1,535 | Original message initialization remains selected. Other globals and private kernel declarations conflict with the selected provider or require kernel types. |
| dpmi32/data.c | 1 → 1 | 47 → 47 | Keep separate guest descriptor domains and the relocated IDT query. Original CCPU descriptor fetch has different paging and fault semantics. |

S7 P2 completes eight more file reviews. The three restored directive
comments in `userrtl.h` remove 3 non-error points and 6 changed lines.
The corrected exclusion pairing in `nt_fulsc.c` removes another 2 points
and 18 lines from the measurement. This correction changes no source.
[P2 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p2-checkpoint.json).

S7 P3 completes eight more file reviews and restores original comments and
an empty source row in `nt_cga.c`. Existing error exclusions and selected
code are preserved. [P3 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p3-checkpoint.json).

S7 P4 restores 264 original lines in `ntexapi.h` without increasing the
number of difference points. Package `25f82d1b-6746-4022-b5e9-ec3f0f319fdc`
passed cached x86 and x64 builds and was published with matching hashes at
both destinations. Targeted export, WOW startup, and DOS lifecycle checks
passed. [P4 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p4-checkpoint.json).

S7 P5 restores the original `Gdp` common declaration in `localfm.c`
and the original pointer-state extern in `nt_event.h`. The matching
common symbol resolves to the same address. The legacy extern compiles
with an implicit-int warning on the selected MSVC profile. Package
`ffb2a4c8-714f-4e75-b2f8-cab52be55db3` passed cached builds, dual
publication, and the targeted WOW, DOS and DOS4GW/NESticle checks.
[P5 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p5-checkpoint.json).

S7 P6 completes nine more file reviews. It restores original blank source
rows in `dpmi386.c` and `nt_cpu.c` and returns `lmsw.c` to byte identity
with its pinned original. Original diagnostic coordinates are preserved.
The P5 runtime package is retained.
[P6 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p6-checkpoint.json).

S7 P7 completes eight more file reviews. It restores formatting in
`rtl/wow.c` and `wuser.c` and removes the unlinked CCPU alternative to
the excluded monitor carrier in `ios.c`. Package
`619955ae-00b2-4174-b75a-471ac49f7add` passed both cached builds,
dual publication, and targeted CCPU, WOW, DOS and DOS4GW checks.
All 11 images match P5 after normalizing COFF and debug directory timestamps
and the package GUID.
[P7 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p7-checkpoint.json).

S7 P8 completes ten more reviews and removes two added blank lines in
`nt_reset.c`. Selected code and preserved diagnostic anchors are unchanged.
The P7 runtime package is retained.
[P8 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p8-checkpoint.json).

S7 P9 completes ten more file reviews. It restores original blank
placement in `cpu_vid.h` and `nt_timer.c` and removes an added comment
in `ev_glue.c`. The code and original diagnostic anchors are preserved.
The P7 runtime package is retained.
[P9 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p9-checkpoint.json).

S7 P10 completes ten more file reviews and restores formatting in
`nt_emm.c` and `wow32.c`. The selected statements, exclusions and original
diagnostic anchors are preserved. The P7 runtime package is retained.
Postpublication Full passed all 20 gates against the retained P7 package.
The test manifest matches the 11 images at both published destinations.
DOS4GW/NESticle rendered its menu and passed Escape hide/restore.
[P10 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p10-checkpoint.json).

S7 P11 completes six file reviews with production source unchanged.
A virtual.c blank-line experiment was reverted because it split a filtered
difference point. The reviewed residuals remain in the pending decision list.
[P11 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p11-checkpoint.json).

S7 P12 completes eight file reviews with production source unchanged.
The actual x86 translation unit passes, and restoring the two original
VDD registration casts gives C2440 at both original logical lines.
Both evidenced residuals are recorded for owner decision.
[P12 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p12-checkpoint.json).

S7 P13 completes eight file reviews. It restores the original conditional
spelling in `dpmidata.h` and removes one added blank in `nt_mem.c`.
The selected code tokens and executable logical lines are preserved.
[P13 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p13-checkpoint.json).

S7 P14 completes ten file reviews. It removes one redundant blank in
`nt_mouse.c` while preserving selected tokens and original line anchors.
[P14 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p14-checkpoint.json).

S7 P15 completes ten file reviews. Original conditional spelling in
`nt_msscs.c`, original indentation in `nt_rflop.c`, and original comment
spacing and line mapping in `wcall16.c` were restored.
[P15 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p15-checkpoint.json).

S7 P16 completes ten file reviews. It removes one redundant blank and
one repeated logical-line directive in `nt_graph.c`. Matching the constant
false arm in `copy_fnc.c` corrects the census by one point and two lines.
[P16 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p16-checkpoint.json).

S7 P17 completes ten file reviews. It restores three original closing
comments, the original DPMI gap and the unused original `CallCsrFlag`
declaration. Selected x86 object controls differ only by COFF timestamp.
[P17 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p17-checkpoint.json).

S7 P18 completes ten file reviews and restores original blank-line
layout in `demsrch.c` and `wole2.c`. The existing line anchors, mixed
line endings and excluded branches are preserved.
[P18 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p18-checkpoint.json).

S7 P19 completes ten file reviews. It restores original blank-line layout
in `cmdmisc.c` and minimizes the package-root comment in `cmdconf.c`
while restoring the original logical line for the call.
[P19 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p19-checkpoint.json).

S7 P20 completes ten file reviews. It removes two redundant comments and
five repeated line resets in `dlgmgr2.c`. Existing original code anchors
and the excluded branches are preserved.
[P20 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p20-checkpoint.json).

S7 P21 completes ten file reviews. It restores the original unused
`wgdi.h` declaration and verifies selected x86 provider object sections
and symbol bindings. The header has no remaining non-error difference.
[P21 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p21-checkpoint.json).

S7 P22 completes ten file reviews and restores the original blank-line
layout around the EXEC translation in `int21map.c`. Existing logical
line resets and excluded branches are preserved.
[P22 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p22-checkpoint.json).

S7 P23 completes the last fourteen file reviews and restores one original
blank in `dlgbegin.c` before the existing logical-line reset.
[P23 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p23-checkpoint.json).

The S7 file pass covers all 213 admitted paths. Source restorations reduce
73 points and 1,764 changed lines in S7. The current census is 1,974 non-error
points and 27,210 changed lines across 231 mirrors with filtered differences.
The reconciled worklist has 242 unique paths and all source hashes match.
P24 reconciles nineteen historical mask cases across five previously
reviewed paths. Both ledgers have zero unresolved mask cases.
[P24 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p24-checkpoint.json).

P25 removes nineteen local frames and restores R0158. Any new local
conditional requires build, ABI or behavior evidence in a supported context.
[P25 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p25-checkpoint.json).

P26 restores `xmsmemr.c` byte-for-byte. The mapped XMS fixture passes.
The historical startup fixture fails to link identically before and after.
[P26 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p26-checkpoint.json).

P27 and P28 simplify X86GFX and DPMI guards. Both worker selections match.
[Batch evidence index](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

P29 removes ten local EMS and graphics frames, restores R1127 and passes
the display fixtures.
[P29 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p29-checkpoint.json).

P30 restores original WOW logging and removes two local startup frames.
[P30 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p30-checkpoint.json).

P31 clears the non-error differences in `egacpu.h` and `gmi.h`.
`ica.h` keeps only the omission of two conflicting macros.
[P31 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p31-checkpoint.json).

P32 restores four original EGA writer ranges. Both published packages match
the tested manifest and differ from P10 only in PE timestamps.
[P32 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p32-checkpoint.json).

P33 to P36 simplify local compile guards and preserve original line anchors.
[Batch evidence index](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

P37 removes four DPMI CPU40 guards with 35 matching inputs.
[P37 evidence](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/s7-p37-checkpoint.json).

P38 to P40 simplify guards, restore original spacing, and correct a mask pair.
[Batch evidence index](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

P41 to P43 simplify BaseSrv, DPMI and window-manager selectors.
[Batch evidence index](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

P44 to P48 consolidate selectors, line anchors, VDM configuration generation,
and COMMAND capture and directory handling.
[Batch evidence index](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/README.md).

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

## Recent M0 Closures

- [T483 S6 closure](../etc/evidence/m0-t483-s6-closure.md)
- [T483 S5 closure](../etc/evidence/m0-t483-s5-closure.md)

## Recent Governance

The active review ledger remains [the mirror worklist](../../artifacts/mvdm-audit/t483-s6-diff-fragmentation/file-worklist.csv).
Existing unrelated debt remains in [TODO](TODO.md).
