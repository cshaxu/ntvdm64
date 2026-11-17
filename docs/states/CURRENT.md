# Project Status

## Current Work

**Active: M0 T483 S7**

Continue the original-first audit in production integration order.

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

S7 progress is 119 reviewed and 94 remaining. Source restorations reduce
22 points and 551 changed lines in S7. The current census is 2,026 non-error
points and 28,427 changed lines across 237 mirrors with filtered differences.

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
