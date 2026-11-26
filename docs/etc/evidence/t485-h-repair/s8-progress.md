# S8 ordered restoration progress

## P1 source review

Baseline 4ffc6a065; fixed XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89.
Read Git object bytes, not the external checkout's line-ending conversion.
Every original object hash is checked against files.csv before restoration.

| Ordered file (under src/mvdm-host) | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| dbg/daytona/makefile | 0/0 -> 0/0 | Exact original bytes, unchanged |
| dbg/daytona/sources | 0/0 -> 0/0 | Exact original bytes, unchanged |
| dbg/dbg.c | 0/0 -> 0/0 | Only newline bytes restored to fixed original |
| dbg/dirs | 0/0 -> 0/0 | Exact original bytes, unchanged |
| dbg/sources.inc | 0/0 -> 0/0 | Exact original bytes, unchanged |
| dbgdll/data.c | 1/3 -> 0/0 | Restore original i386-guarded px86 definition |
| dbgdll/dbgdll.h | 1/3 -> 0/0 | Restore matching original extern declaration |

HXP-R-00003 and HXP-R-00008 already carried real compile/link restoration
evidence. Current ntvdmd-dll/target.cmake selects i386, so final compile/link
is required. Neither declaration needs a shim or algorithm change. Both files
now match their fixed original objects entirely. No exclusion or diagnostic
mapping is added. Existing other-file exceptions remain outside this pair.

Seven of 851 files reviewed, 844 remain; four exact unchanged, one newline-only
restoration, two declaration restorations. Local reduction 2 points/6 lines;
no P1 delivery claimed yet. Cumulative S6/S7 counts are not folded into S8.

Background build/t485-s8/p1/deliver.ps1 runs cached full build then dual
publication. Final targeted results, commit and push remain pending. Evidence
starts at build/t485-s8/first-five.json and p1/. Next file is dbgdll/makefile.
## P1 final verification

Cached full tools/build.cmd passed. Runtime1 thirteen-image package was
published to assets/release and O:/WINNT/SYSTEM32; each hash matches its staged
manifest. The existing verify-ntvdm-export-live.ps1 passes on that package:
NTVDMD load, real breakpoint/SAS identity, event and register writeback,
allocation/reallocation/failure preservation, matching task-termination query,
and MEM exit zero. No original guest was changed or rebuilt.

All seven files match fixed XP object bytes after this batch. Native i386
compilation/link accepts the restored definition and declaration; px86 has no
consumer in dbgdll beyond these two original lines. No new runtime branch or
ABI adapter was introduced. Both HXP-R-00003 and HXP-R-00008 are restored in
the repair register and neither remains a pending decision.

Local and final-verified reduction: 2 non-error points / 6 diff lines; one
additional newline restoration. Seven of 851 verified, 844 remaining.
Commit/push of P1 supplies delivery identity. No Full run is claimed or needed
for this bounded declaration batch. Next file remains dbgdll/makefile.
P1 delivery commit bd86c7f40 was pushed to origin/main.

## P2 metadata accumulation

Next ordered files: dbgdll/makefile and dbgdll/sources were byte-exact;
dbgdll/ntvdmd.def and dbgdll/precomp.h were newline-only and now match the
fixed Git object bytes. All four remain 0/0 non-error differences.

src/mvdm-host/dirs: 5 points/17 lines -> 0/0, exact original restored.
The current worker_original_sources function consumes explicit SOURCES variables
from caller-selected sources.inc files, not the top-level DIRS list. The target
graph selects host objects separately. No tools/src cmd, ps1 or CMake consumer
of mvdm-host/dirs was found. Original guest directory names in this unused
historical descriptor do not add guest to the build. This resolves frozen
HXP-R-00031..00035 without a replacement exclusion or adapter.

12/851 reviewed, 839 remain; 7 delivered in P1, five locally reconciled in P2.
Cumulative non-error reduction 7/23; delivered 2/6, local pending 5/17.
Newline-only restorations total three files. No new build is triggered for
this metadata-only accumulation. Next file: dos/command/cmd.c.
### P2 COMMAND and first DEM files

| File under src/mvdm-host | Points/lines before -> after | Conclusion |
| --- | --- | --- |
| dos/command/cmd.c | 0/0 -> 0/0 | Original newline bytes restored |
| dos/command/cmddata.c | 0/0 -> 0/0 | Original newline bytes restored |
| dos/command/cmddisp.c | 0/0 -> 0/0 | Existing DBG exclusion retained; XP line anchors 67/68/74 corrected |
| dos/command/cmdexit.c | 0/0 -> 0/0 | Original newline bytes restored |
| dos/command/cmdkeyb.h | 0/0 -> 0/0 | Exact unchanged |
| dos/command/cmdpif.c | 0/0 -> 0/0 | Original newline bytes restored |
| dos/command/cmdredir.c | 0/0 -> 0/0 | Exact unchanged |
| dos/command/daytona/makefile | 0/0 -> 0/0 | Exact unchanged |
| dos/command/daytona/sources | 0/0 -> 0/0 | Exact unchanged |
| dos/command/dirs | 2/7 -> 0/0 | Restore unconsumed original language directory metadata |
| dos/command/sources.inc | 0/0 -> 0/0 | Exact unchanged |
| dos/dem/daytona/makefile | 0/0 -> 0/0 | Exact unchanged |
| dos/dem/daytona/sources | 0/0 -> 0/0 | Exact unchanged |
| dos/dem/demdasd.c | 2/8 -> 0/0 | Restore original indentation; preserve four DBG errors, correct XP diagnostic lines |

Whole-file original comparison of demdasd.c leaves only the four pre-existing
DBG exclusions and line directives. Its selected statements are original.
No new exclusions, condition frames or semantic adaptations were introduced.
The original COMMAND directory list is not read by the explicit current graph.
Newline-only count is now seven files. Diagnostic mappings are reported
separately from non-error diff reductions, not added as semantic reductions.

26/851 reviewed, 825 remain. Delivered 7 (P1); local P2 reconciliation 19.
Non-error reduction total 11 points/38 lines; delivered 2/6, local 9/32.
No build/test/publication is claimed for P2 yet: these are no-behavior metadata,
format and line corrections accumulated under owner direction. Next demdasd.h.
### P2 remaining early DEM reconciliation

| File under dos/dem | Points/lines before -> after | Result |
| --- | --- | --- |
| demdasd.h | 0/0 -> 0/0 | Exact unchanged |
| demdata.c | 0/0 -> 0/0 | Remove redundant line anchors; exact original |
| demdir.c | 0/0 -> 0/0 | Exact unchanged |
| demdisp.c | 1/4 -> 0/0 | Restore indentation; two existing DBG exclusions, XP lines |
| demerror.c | 0/0 -> 0/0 | Four existing exclusions; XP lines |
| demfcb.c | 0/0 -> 0/0 | Exact unchanged |
| demfile.c | 0/0 -> 0/0 | Six existing exclusions; remove stale redundant anchors |
| demioctl.c | 0/0 -> 0/0 | One existing exclusion; minimal XP anchor |
| demlabel.c | 0/0 -> 0/0 | Original newline bytes |
| demlock.c | 0/0 -> 0/0 | Original newline bytes |
| demmisc.c | 0/0 -> 0/0 | One existing exclusion; original lines restored |
| demmsg.c | 0/0 -> 0/0 | Whole DBG body remains excluded; minimal XP anchor |

The bounded reconciliation compared fixed-object hashes and the complete
conditional-directive sequence. Each existing error-only body was matched to
the same original conditional; retained lines matched original after removing
only indentation/blank-line differences and diagnostic directives. Other
mismatches abort rather than restoring blindly. No new body was excluded.
Original retained content was reconstructed with those same exclusions and
#line after each closing boundary. Evidence: build/t485-s8/dem-reconcile.json.
HXP-R-00081 is restored; original error-only differences remain excluded from
semantic reduction accounting. No runtime algorithm was edited.

38/851 reviewed, 813 remain; seven delivered, 31 local reconciliations.
Total reduction 12 points/42 lines: delivered 2/6, local 10/36. Newline-only
restorations now nine; demdata's redundant anchors are separate. Next file
follows demmsg.c in the ordered S8 list. P2 still accumulates without a new
production build or runtime acceptance claim.
### P2 directory tail, DPMI and first shared headers

| File under src/mvdm-host | Points/lines before -> after | Result |
| --- | --- | --- |
| dos/dem/demmsg.h | 0/0 -> 0/0 | Original newline bytes |
| dos/dem/dirs | 2/8 -> 0/0 | Unconsumed original directory metadata restored |
| dos/dem/dosdef.h | 0/0 -> 0/0 | Exact unchanged |
| dos/dem/sources.inc | 0/0 -> 0/0 | Exact unchanged |
| dos/dirs | 1/3 -> 0/0 | Unconsumed original directory metadata restored |
| dpmi32/buffer.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/daytona/makefile | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/daytona/sources | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/debug.c | 0/0 -> 0/0 | Existing DBG exclusion and lines correct; unchanged |
| dpmi32/dirs | 2/4 -> 0/0 | Unconsumed original directory metadata restored |
| dpmi32/dosmem.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/int21map.h | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/int2f.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/int31.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/register.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/savestat.c | 0/0 -> 0/0 | Existing literal-zero exclusion retained; XP line fixed |
| dpmi32/sources.inc | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/vxd.c | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/xlathlp.h | 0/0 -> 0/0 | Exact unchanged |
| dpmi32/xmem.c | 0/0 -> 0/0 | Five existing DBG exclusions; XP lines fixed |
| inc/apistruc.h | 0/0 -> 0/0 | Original newline bytes |
| inc/bop.h | 0/0 -> 0/0 | Original newline bytes |
| inc/cmdsvc.h | 0/0 -> 0/0 | Exact unchanged |
| inc/curdir.h | 0/0 -> 0/0 | Exact unchanged |
| inc/dbginfo.h | 0/0 -> 0/0 | Original newline bytes |
| inc/dbgsvc.h | 0/0 -> 0/0 | Original newline bytes |
| inc/dirs | 0/0 -> 0/0 | Exact unchanged |
| inc/dossvc.h | 0/0 -> 0/0 | Exact unchanged |
| inc/dpmi.h | 0/0 -> 0/0 | Exact unchanged |
| inc/exterr.h | 0/0 -> 0/0 | Original newline bytes |

The DPMI reconciliation used the same conditional-sequence and retained-line
comparison as DEM (build/t485-s8/dpmi-reconcile.json). No directory restoration
is a guest build admission. Original-error-only files are not labelled exact.
The five frozen metadata points HXP-R-00107..00109 and 00111..00112 are restored.

P2 is now a bounded 61-file reconciliation, after seven P1 deliveries. Total
68/851 reviewed, 783 remain. Cumulative reduction 17 points/57 lines: P1 2/6
and local P2 15/51. Fifteen newline-only files are counted separately. Starting
one cached full build/dual publication for the accumulated P2 batch; final
verification and commit/push remain pending. Next ordered file follows exterr.h.
## P2 final verification

Cached full build and dual runtime2 thirteen-image publication passed; hashes
match at assets/release and O:/WINNT/SYSTEM32. The final source-check.json
independently retrieves the pinned Git objects and confirms all 61 files have
exact original retained lines, unchanged conditional sequences and only their
existing error bodies plus line mappings. No original algorithm or guest was
modified. The published package passes verify-ntvdm-export-live.ps1: MEM exits
zero, NTVDMD loads, actual breakpoint/SAS pairing and register writeback pass,
allocation failure preserves resources and task-termination query succeeds.
Evidence is build/t485-s8/p2/{source-check.json,build-result.txt,
publish-result.txt,test-result.txt,debug-live/trace.txt}. No Full claim.

68/851 final-verified, 783 remain. P2 reduces 15 non-error points/51 lines;
S8 cumulative reduction 17/57 including delivered P1. Fifteen newline-only
restorations remain separate. P2 commit/push is the final delivery step.

P2 delivered and pushed as f7501056b.

## P3 shared headers

- src/mvdm-host/inc/intapi.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/monregs.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/netb.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/oemuni.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/pif.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/rdrexp.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/rdrsvc.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/sharewow.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/sources.inc: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/tdb16.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/usa/makefile: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/usa/makefile.inc: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/usa/sources: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/v86def.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vint.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrdefld.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrdlctab.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrinit.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrmisc.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrmslot.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.
- src/mvdm-host/inc/vrremote.h: 0/0 -> 0/0; fixed XP bytes verified; exact or newline-only restoration.

89/851 reviewed, 762 remain. 68 delivered; 21 local P3 exact reconciliations. Non-error reduction remains 17 points/57 lines, all delivered. Eleven P3 newline restorations bring the total to 26. Next inc/wow.h requires branch-aware line reconciliation; it is not counted reviewed. No build triggered for these newline-only changes.

### P3 WOW header completion

| File under inc | Points/lines before -> after | Conclusion |
| --- | --- | --- |
| wow.h | 0/0 -> 0/0 | Twelve old line anchors corrected by +67 to exact XP positions; branches unchanged |
| wowclip.h | 0/0 -> 0/0 | Original newline bytes |
| wowcmpat.h | 0/0 -> 0/0 | Exact unchanged |
| wowcomm.h | 0/0 -> 0/0 | Existing UNALIGNED error only; original retained text |
| wowfax.h | 0/0 -> 0/0 | Three existing exclusion bodies compacted; original outer guards and XP lines |
| wowgdi.h | 0/0 -> 0/0 | Exact unchanged |
| wowinfo.h | 0/0 -> 0/0 | Existing UNALIGNED exclusion; remove empty/obsolete interior anchors |
| wowmmcb.h | 0/0 -> 0/0 | Existing UNALIGNED exclusion; original retained text |
| wowmmed.h | 0/0 -> 0/0 | Nine existing exclusions; original retained declarations and XP lines |
| wowshell.h | 0/0 -> 0/0 | Exact unchanged |
| wowth.h | 0/0 -> 0/0 | Original newline bytes |

The wow.h anchors map to XP 1279/1280/1282, 1311/1313/1320,
1347/1349/1359 and 1390/1392/1401; no macro or conditional changed.
For wowfax.h and wowinfo.h the original nested bodies remain wholly excluded,
with their original outer conditions retained. All other retained declarations
come from fixed original bytes; no new exclusions were added. Evidence:
build/t485-s8/wow-headers-reconcile.json and wow-nested-reconcile.json.

100/851 reviewed, 751 remain; 68 delivered, 32 P3 local. Non-error reduction
remains 17/57, all previously delivered. Newline-only total 28. No new build
or runtime pass is claimed for this accumulated format/line batch.
### P3 remaining shared headers and BIOS entry

| File under src/mvdm-host | Points/lines before -> after | Conclusion |
| --- | --- | --- |
| inc/wowwife.h | 0/0 -> 0/0 | Exact unchanged |
| inc/wowwsock.h | 0/0 -> 0/0 | Original newline bytes |
| inc/xmsexp.h | 0/0 -> 0/0 | Original newline bytes |
| inc/xmssvc.h | 0/0 -> 0/0 | Original newline bytes |
| oemuni/daytona/makefile | 0/0 -> 0/0 | Exact unchanged |
| oemuni/daytona/sources | 0/0 -> 0/0 | Exact unchanged |
| oemuni/dirs | 2/16 -> 0/0 | Restore original comments and unconsumed directory list |
| oemuni/oem.h | 0/0 -> 0/0 | Original newline bytes |
| oemuni/sources.inc | 0/0 -> 0/0 | Exact unchanged |
| softpc.new/base/bios/bios.c | 1/4 -> 0/0 | Original retained formatting and XP anchors; same 61 excluded arms |
| softpc.new/base/bios/build_id.c | 0/0 -> 0/0 | Original newline bytes |
| softpc.new/base/bios/cmos_bis.c | 0/0 -> 0/0 | Original retained text and same two exclusions |

bios.c was matched sequentially against fixed XP: each error-only arm skips
exactly its original body to the corresponding depth-matched else/endif;
every other nonblank line matches original apart from indentation. The same
comparison passes after reconstruction. No BOP entry, selected statement or
condition was changed; no excluded body was restored. Evidence:
build/t485-s8/bios-exclusions.json and bios-tail-reconcile.json. HXP-R-00273,
00274 and 00359 are restored locally, not final-package certified yet.

112/851 reviewed, 739 remain; 68 delivered, 44 local P3 reconciliations.
Non-error reduction total 20 points/77 lines: delivered 17/57, local 3/20.
Newline-only restorations total 33. P3 continues to accumulate formatting,
metadata and diagnostic-only changes without a new build claim.
### P3 BIOS control and EMS formatting

| File under softpc.new/base/bios | Points/lines before -> after | Conclusion |
| --- | --- | --- |
| cntlbop.c | 0/0 -> 0/0 | Eight existing exclusions; original retained formatting and XP lines |
| daytona/makefile | 0/0 -> 0/0 | Exact unchanged |
| daytona/sources | 0/0 -> 0/0 | Exact unchanged |
| dirs | 2/7 -> 0/0 | Restore unconsumed original language directory metadata |
| emm_fncs.c | 46/92 -> 0/0 | Restore original indentation/blank lines; preserve 27 excluded arms |

Sequential depth-aware original comparison passes before and after both C
reconstructions. Every nonblank retained line equals XP apart from indentation;
only original excluded bodies are skipped to matching conditional boundaries.
The 46 EMS entries are formatting, not memory-algorithm modifications.
Evidence: build/t485-s8/{cntlbop,emm}-exclusions.json and matching check scripts.
No new runtime behavior, exclusion or build profile was introduced.

117/851 reviewed, 734 remain; 68 delivered, 49 local P3 reconciliations.
Cumulative reduction 68 points/176 lines: delivered 17/57, local 51/119.
Newline-only total remains 33. Next: softpc.new/base/bios/equipmnt.c.
P3 remains an accumulated format/metadata/line batch; no new test pass claimed.
### P3 BIOS directory completion and delivery launch

| File under softpc.new/base/bios | Points/lines before -> after | Conclusion |
| --- | --- | --- |
| equipmnt.c | 0/0 -> 0/0 | Same one excluded arm; original retained lines |
| ill_bop.c | 0/0 -> 0/0 | Same five excluded arms; original retained lines |
| mem_size.c | 0/0 -> 0/0 | Same two excluded arms; original retained lines |
| rom_basc.c | 0/0 -> 0/0 | Same one excluded arm; original retained lines |
| rtc_bios.c | 0/0 -> 0/0 | Same one excluded arm; original retained lines |
| sources.inc | 0/0 -> 0/0 | Fixed original byte identity unchanged |

Before/after depth-aware comparisons pass for all five C files. Only existing
error bodies and diagnostic anchors differ from XP; no additional exclusion or
selected algorithm changed. The P3 batch now contains 55 complete reconciliations
following the 68 delivered P1/P2 files. Cached full build/dual publication runs
through build/t485-s8/p3/deliver.ps1; tests and commit/push remain pending.

123/851 reviewed, 728 remain. Reduction remains 68 points/176 lines total:
17/57 delivered, 51/119 local P3. Newline-only total 33. Next file is
softpc.new/base/ccpu386/aaa.c after P3 delivery.### P3 final verification

All 55 files in ordered positions 69-123 are final-verified. Source comparison
against pinned XP objects confirms original retained nonblank code and the same
existing error directives; no excluded body is admitted. Format and diagnostic
anchors change only as recorded above. No product algorithm or ABI changed.
`build/t485-s8/p3/source-check.json` records final SHA256 identities.

Cached full build and dual publication passed. All 13 manifest images match at
assets/release and O:/WINNT/SYSTEM32 (26 comparisons recorded in published-hashes.json).
Runtime3 passed the real MEM/NTVDMD export, debugger-event/register writeback,
allocation rollback and termination witness, and the real DOS4GW/NESticle menu
hide/restore pixel oracle. Evidence: p3/debug-live/{trace,result}.txt and
p3/dos4gw/nesticle.txt.graphics-result.json. This is targeted acceptance, not Full.

Batch reduction: 51 non-error points and 119 added-plus-deleted mirror lines.
S8 cumulative: 68 points/176 lines; newline-only restorations: 33 files separately.
123/851 source-reviewed and final-verified, 728 remain. Commit/push completes P3;
next is softpc.new/base/ccpu386/aaa.c. No new retain decision was introduced.
### P4 ASCII arithmetic files

P3 is committed and pushed as 1784272b5; all 123 reviewed files are delivered.

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| aaa.c | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aaa.h | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aad.c | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aad.h | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aam.c | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aam.h | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aas.c | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |
| aas.h | 0/0 -> 0/0 | Newline-only; restored exact pinned XP bytes |

Fixed Git-object SHA256 verified before copying, then final file hashes recorded
in files.csv. No algorithm, exclusion or diagnostic mapping changed. This pure
newline batch accumulates without immediate build. 131/851 reviewed, 720 remain;
123 delivered, eight local P4 files. Cumulative non-error reduction stays
68 points/176 lines; newline-only restorations total 41. Next: adc.c.

### P4 ordered CPU reconciliation through c_stack.c

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| adc.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| adc.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| add.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| add.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| and.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| and.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| arpl.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| arpl.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bound.c | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; diagnostic anchors checked |
| bound.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bsf.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bsf.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bsr.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bsr.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bswap.c | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; diagnostic anchors checked |
| bswap.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bt.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bt.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| btc.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| btc.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| btr.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| btr.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bts.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| bts.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_addr.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_addr.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_div64.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_div64.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_getset.c | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; diagnostic anchors checked |
| c_intr.c | 0/0 -> 0/0 | Original retained code; 11 existing excluded arms; diagnostic anchors checked |
| c_intr.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_main.h | 0/0 -> 0/0 | Original retained code; 3 existing excluded arms; diagnostic anchors checked |
| c_mem.h | 0/0 -> 0/0 | Original retained code; 5 existing excluded arms; diagnostic anchors checked |
| c_mul64.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_mul64.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_neg64.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_neg64.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_page.c | 0/0 -> 0/0 | Original retained code; 2 existing excluded arms; diagnostic anchors checked |
| c_page.h | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; diagnostic anchors checked |
| c_prot.c | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_prot.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_reg.c | 0/0 -> 0/0 | Original retained code; 4 existing excluded arms; diagnostic anchors checked |
| c_reg.h | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; diagnostic anchors checked |
| c_seg.h | 0/0 -> 0/0 | Exact XP bytes; newline-only restoration |
| c_stack.c | 0/0 -> 0/0 | Original retained code; 6 existing excluded arms; diagnostic anchors checked |

Final source comparison passes for all 53 accumulated P4 files: original retained
nonblank code and unchanged error directives. Existing excluded bodies remain
excluded; CPU algorithms and ABI are unchanged. Evidence: build/t485-s8/p4-source-check.json.
176/851 reviewed, 675 remain; 123 delivered, 53 local P4 reconciliations.
Cumulative non-error reduction remains 68 points/176 lines; newline-only total 75.
No runtime or delivery pass claimed for P4. Next: ccpu386/c_stack.h.

### P4 ordered CPU reconciliation through div.c

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| c_stack.h | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| c_tlb.c | 0/0 -> 0/0 | Original retained code; 11 unchanged excluded arms; XP diagnostic anchors |
| c_tlb.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| c_tsksw.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| c_xcptn.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| c_xfer.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| c_xfer.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| c_xtrn.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| c_xtrn.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| call.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cbw.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cbw.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ccpudefs.inc | 0/0 -> 0/0 | Already exact XP bytes |
| ccpupig.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| ccpupig.h | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| ccpusas4.h | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| cdq.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cdq.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| clc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| clc.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cld.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cld.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cli.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cli.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| clts.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| clts.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cmc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cmc.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cmp.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cmp.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cmpxchg.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| cmpxchg.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cpu4gen.h | 0/0 -> 0/0 | Original retained code; 215 unchanged excluded arms; XP diagnostic anchors |
| cpuint_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cwd.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cwd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cwde.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cwde.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| daa.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| daa.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| das.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| das.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| dec.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| dec.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| div.c | 0/0 -> 0/0 | Restored exact XP newline bytes |

Source comparison passes for all 98 accumulated P4 files against pinned XP:
retained nonblank code matches and existing error directives are unchanged.
No CPU algorithms or ABI changed. 221/851 reviewed, 630 remain; 123 delivered,
98 local P4 files. Non-error reduction remains 68 points/176 lines; newline-only
restorations total 110. P4 now enters cached build/publication, then targeted
verification; next file is ccpu386/div.h after delivery.
### P4 publication checkpoint

Runtime4 cached full build and dual publication passed. All 26 destination/image
hashes match the 13-image manifest. Final source and diagnostic-line checks pass
for all 98 P4 files; original retained code, existing error directives and original
logical lines are preserved. Documentation governance passed. Actual MEM/NTVDMD
export, debugger event/register writeback, allocation rollback and termination
witness passed on runtime4. DOS4GW/NESticle is running; commit/push remains pending.
Evidence: build/t485-s8/p4/{line-check,published-hashes}.json and debug-live/trace.txt.
### P4 final verification and register reconciliation

DOS4GW/NESticle menu appearance, Escape hide and restoration passed on runtime4
(p4/dos4gw/nesticle.txt.graphics-result.json). All P4 verification is complete;
commit/push completes delivery. 221/851 final-verified; 630 remain. Non-error
reduction remains 68 points/176 lines, with 110 newline-only files separately.

Owner-reported register mismatch was confirmed for S6 c_seg.c: files.csv still
contained its snapshot hash and not-repaired-by-audit despite delivery f0df10cdb.
Only latest_sha256 and delivery_state are corrected; snapshot_sha256 is immutable.
Current hash: 5878ad3d3acf0111536339d81f8f26bb7c39dda4425000e0011abf5ca70e9405.
CURRENT and this detailed record now both state 221/851, not the historical 12/26
checkpoints. Source history confirms no subsequent c_seg.c product changes.

### P5 ordered reconciliation through into.c

P4 is committed and pushed as 4378cb179; 221 files are delivered.
The 228 final-verified ledger entries were checked against disk: no stale hashes.

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| div.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| enter.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| enter.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| evidgen.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| fault.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| fpu_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| gdpvar.h | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; XP diagnostic anchors |
| idiv.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| idiv.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| imul.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| imul.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| in.c | 0/0 -> 0/0 | Original retained code; 1 existing excluded arms; XP diagnostic anchors |
| in.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| inc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| inc.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| into.c | 0/0 -> 0/0 | Restored exact XP newline bytes |

Pinned-object source comparison passes for all 16 P5 files. No instruction algorithm
or selected branch changed. 237/851 reviewed, 614 remain; 221 delivered, 16 local.
Non-error reduction stays 68 points/176 lines; newline-only restorations total 124.
No P5 build/runtime pass claimed. Next: ccpu386/into.h.

### P5 ordered reconciliation through popa.c

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| into.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| intx.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| intx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| invd.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| invd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| invlpg.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| invlpg.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| iret.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| jcxz.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| jcxz.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| jmp.c | 0/0 -> 0/0 | Original retained code; 3 unchanged excluded arms; XP diagnostic anchors |
| jmp.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| jxx.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| jxx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lahf.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lahf.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lar.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lar.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lea.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lea.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| leave.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| leave.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lgdt.c | 0/0 -> 0/0 | Original retained code; 4 unchanged excluded arms; XP diagnostic anchors |
| lgdt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lidt.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| lidt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lldt.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| lldt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lmsw.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lmsw.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| loopxx.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| loopxx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lsl.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lsl.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ltr.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ltr.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| lxs.c | 0/0 -> 0/0 | Original retained code; 5 unchanged excluded arms; XP diagnostic anchors |
| lxs.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| makefile | 0/0 -> 0/0 | Restored exact XP newline bytes |
| mov.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| mov.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| movsx.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| movsx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| mul.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| mul.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| neg.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| neg.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| newnpx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| nop.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| nop.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| not.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| not.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| or.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| or.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| out.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| out.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pigger_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pop.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pop.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| popa.c | 0/0 -> 0/0 | Restored exact XP newline bytes |

Pinned-object source comparison passes for all 76 P5 files. Existing exclusions
and selected algorithms are unchanged. 297/851 reviewed, 554 remain; 221 delivered,
76 local P5 files. Cumulative non-error reduction remains 68 points/176 lines;
newline-only restorations total 175. No P5 build/runtime pass claimed.
Next file: ccpu386/popa.h.

### P5 CPU formats and original build manifest

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| popa.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| popf.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| popf.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| push.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| push.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pusha.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pusha.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pushf.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| pushf.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rcl.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rcl.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rcr.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rcr.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ret.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rol.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rol.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ror.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ror.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rsrvd.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| rsrvd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sahf.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sahf.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sar.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sar.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sascdef.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sbb.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sbb.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| setxx.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| setxx.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sgdt.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| sgdt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shl.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shl.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shld.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| shld.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shr.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shr.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| shrd.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| shrd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sidt.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| sidt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sldt.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sldt.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| smsw.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| smsw.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sources | 1/1 -> 0/0 | Exact XP metadata; provider selection explicit in existing CMake graph |

HXP-R-00597 correction: the frozen claim that SOURCES was unconsumed is false.
tools/build/worker/source-groups.cmake reads it through worker_original_sources.
The deleted ntstubs.c entry is restored; the existing source-group composition now
removes it before appending localfm.c. Both original providers define Gdp, Cpu and
Video; the selected localfm.c remains the only linked vector-storage provider.
No algorithm is moved or duplicated. The selected file list must remain unchanged
after cached configure/build. The original snapshot disposition is retained as
history; this final investigation supersedes its inaccurate unconsumed rationale.

343/851 reviewed, 508 remain; 221 delivered, 122 local P5 files. Non-error
reduction totals 69 points/177 lines: 68/176 delivered, 1/1 local. Newline-only
restorations total 216. Whole sources file is complete; start P5 cached build,
dual publication and targeted verification. Next: ccpu386/stc.c after delivery.
### P5 final source review

All 122 files pass pinned-XP retained-code comparison and original logical-line
checks. The source selection before and after the SOURCES restoration is the
same ordered 130-file list, including exactly one localfm.c and no ntstubs.c.
Evidence: build/t485-s8/p5/{line-check,source-selection}.json and
build/t485-s8/p5-source-check.json. This proves selection preservation, not yet
runtime acceptance; cached full build and publication are still running.
### P5 final delivery verification

Runtime5 cached full build and dual 13-image publication passed; all 26 published
hashes match. Real MEM/NTVDMD export, debugger event/register writeback, allocation
rollback and task termination passed. DOS4GW/NESticle menu pixels and Escape
hide/restore passed. Evidence: build/t485-s8/p5/debug-live/trace.txt,
p5/dos4gw/nesticle.txt.graphics-result.json and p5/published-hashes.json.

All 122 P5 files are final-verified; selected 130-file CCPU list is unchanged.
P5 reduces one non-error point/line, cumulative 69/177; newline-only total 216.
343/851 reviewed and final-verified, 508 remain. Commit/push completes delivery.
No Full result is claimed; verification is scoped to this format/build-selection
batch. Next: ccpu386/stc.c. N004 remains gated on S13.

### P6 CCPU directory completion

P5 committed and pushed as 34e59dfaf; 343 files delivered.

| File under softpc.new/base/ccpu386 | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| stc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| stc.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| std.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| std.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sti.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sti.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| str.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| str.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| stubs.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sub.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| sub.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| test.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| test.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| univer_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| verr.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| verr.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| verw.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| verw.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| wait.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| wait.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| wbinvd.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| wbinvd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xadd.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| xadd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xchg.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xchg.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xlat.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xlat.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xor.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| xor.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| zfrsrvd.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| zfrsrvd.h | 0/0 -> 0/0 | Restored exact XP newline bytes |

All 32 files pass pinned-XP retained-source and unchanged-error checks.
375/851 reviewed, 476 remain; 343 delivered and 32 P6 files local. Cumulative
non-error reduction remains 69 points/177 lines; newline-only total 245.
No new P6 build/runtime pass claimed. Next: base/comms/com.c.

### P6 communications reconciliation

| File under softpc.new/base | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| comms/com.c | 23/82 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/daytona/makefile | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/daytona/sources | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/dirs | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/printer.c | 10/28 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/printer_.c | 4/12 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/prt_scrn.c | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/rs232_io.c | 5/10 -> 0/0 | Original retained code/format; existing exclusions preserved |
| comms/sources.inc | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| condincl.inc | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| cpu/src/evid/vglob.c | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |

Whole-file depth-aware comparison verifies original retained nonblank code for
all 43 P6 files, with unchanged error directives. The 42 communications points
(132 added-plus-deleted lines) are original format restorations, not serial or
printer algorithm changes. Source identities: build/t485-s8/p6-source-check.json.

386/851 reviewed, 465 remain; 343 delivered and 43 P6 files local. Cumulative
non-error reduction is 111 points/309 lines (42/132 local); newline-only total 247.
P6 now enters cached build, dual publication and targeted verification.
Next file after delivery: base/cvidc/accessfn.c.
### P6 publication checkpoint

Runtime6 cached full build and dual 13-image publication passed. All 26 published
hash comparisons match the final manifest. All 43 file identities and original
retained logical lines pass (p6/line-check.json); source comparison is recorded
in p6-source-check.json. Governance passed. Final-package MEM/NTVDMD and
DOS4GW/NESticle targeted tests are running; no test or delivery completion is
claimed until their results and commit/push are recorded.
### P6 final verification

Final runtime6 passed real MEM/NTVDMD export/debugger/register/allocation/termination
checks and DOS4GW/NESticle rendered menu plus Escape hide/restore. Evidence:
build/t485-s8/p6/debug-live/trace.txt and dos4gw/nesticle.txt.graphics-result.json.
All 43 source identities and diagnostic mappings are verified, cached full build
and dual 13-image publication passed. Commit/push completes P6, not S8.
386/851 final-verified, 465 remain. This batch removes 42 points/132 lines;
cumulative reduction is 111/309, newline-only count 247. Next: base/cvidc/accessfn.c.

### P7 C-VID headers and accessors

P6 committed and pushed as 6ed2d9a8b; 386 files delivered.

| File under softpc.new/base/cvidc | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| accessfn.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| c2cpusad.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| ccpudefs.inc | 0/0 -> 0/0 | Already exact XP bytes |
| cpu4gen.h | 0/0 -> 0/0 | Original retained code; 209 unchanged excluded arms; XP diagnostic anchors |
| cpu_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cpuint_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| evid_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| evidfunc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| evidfunc.h | 0/0 -> 0/0 | Already exact XP bytes |
| fm_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |

All ten files pass pinned-source retained-code and unchanged-error checks.
396/851 reviewed, 455 remain; 386 delivered and ten P7 files local. Non-error
reduction stays 111 points/309 lines; newline-only restorations total 254.
No P7 build/runtime pass claimed. Next: cvidc/fpu_c.h.

### P7 C-VID and debug reconciliation

| File under softpc.new/base | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| cvidc/fpu_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/gdpvar.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/j_c_lang.c | 0/0 -> 0/0 | Already exact XP bytes |
| cvidc/j_c_lang.h | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| cvidc/makefile | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/pigger_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid002.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid003.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid005.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid006.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid007.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid008.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid009.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid010.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid011.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid012.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid013.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid014.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid015.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid020.c | 0/0 -> 0/0 | Already exact XP bytes |
| cvidc/sevid021.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid022.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid023.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid024.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid025.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid026.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid027.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid028.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid029.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sevid030.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sinit011.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sinit012.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sinit013.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/sources | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/univer_c.h | 0/0 -> 0/0 | Restored exact XP newline bytes |
| cvidc/vglfunc.c | 0/0 -> 0/0 | Restored exact XP newline bytes |
| debug/btrace.c | 0/0 -> 0/0 | Original retained code; 3 unchanged excluded arms; XP diagnostic anchors |
| debug/dasm.c | 0/0 -> 0/0 | Original retained code; 2 unchanged excluded arms; XP diagnostic anchors |
| debug/daytona/makefile | 0/0 -> 0/0 | Already exact XP bytes |
| debug/daytona/sources | 0/0 -> 0/0 | Already exact XP bytes |
| debug/debuggng.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| debug/dirs | 0/0 -> 0/0 | Already exact XP bytes |
| debug/ega_dump.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| debug/pigyoda.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| debug/profile.c | 0/0 -> 0/0 | Original retained code; 1 unchanged excluded arms; XP diagnostic anchors |
| debug/sources.inc | 0/0 -> 0/0 | Already exact XP bytes |
| debug/trace.c | 0/0 -> 0/0 | Original retained code; 7 unchanged excluded arms; XP diagnostic anchors |
| debug/yoda.c | 0/0 -> 0/0 | Original retained code; 4 unchanged excluded arms; XP diagnostic anchors |
| dirs | 0/0 -> 0/0 | Restored exact XP newline bytes |
| disks/daytona/makefile | 0/0 -> 0/0 | Already exact XP bytes |

All 60 P7 files pass pinned-source retained-code and unchanged-error comparisons.
446/851 reviewed, 405 remain; 386 delivered, 60 locally complete. Cumulative
non-error reduction remains 111 points/309 lines; newline-only total 288.
No P7 build/runtime pass claimed.
Next: src/mvdm-host/softpc.new/base/disks/daytona/sources.

### P7 disk formats and fixed ordering

| File under softpc.new/base/disks | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| daytona/sources | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| dirs | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| diskbios.c | 2/6 -> 0/0 | Original retained code/format; existing exclusions preserved |
| fdisk.c | 95/202 -> 0/0 | Original retained code/format; existing exclusions preserved |
| fla.c | 57/126 -> 0/0 | Original retained code/format; existing exclusions preserved |
| floppy_i.c | 1/2 -> 0/0 | Original retained code/format; existing exclusions preserved |
| floppy.c | 36/80 -> 0/0 | Original retained code/format; existing exclusions preserved |
| gfi_ibm.c | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| gfi_mpty.c | 2/6 -> 0/0 | Original retained code/format; existing exclusions preserved |
| gfi_sflp.c | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| gfi_test.c | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| gfi.c | 17/36 -> 0/0 | Original retained code/format; existing exclusions preserved |
| sources.inc | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |

The helper comparison initially used Python lexical order, which differs for
underscore names from the recorded PowerShell path ordering. It stopped on a
missing extracted source before completing its ledger write. The helper now uses
the explicit ordered-paths.json extracted from the canonical checklist. Recheck
confirmed the ledger covers exactly the first 454 entries before the final five.
No extra file was admitted; retained-code comparison passes for all 73 P7 files.

Disk restorations remove 210 points/458 lines; no disk algorithm changes.
459/851 reviewed, 392 remain; 386 delivered and 73 locally complete. Cumulative
non-error reduction is 321 points/767 lines (210/458 local), newline-only total 288.
P7 enters cached build/dual publication then targeted verification. Next is
Next file: src/mvdm-host/softpc.new/base/dos/daytona/makefile.
### P7 publication checkpoint

Runtime7 cached full build and dual 13-image publication passed; all 26 destination
hash comparisons match. All 73 source hashes and original diagnostic mappings
pass; documentation governance passed. Final-package MEM/NTVDMD and DOS4GW tests
are running through p7/verify-all.ps1. No final runtime/delivery result claimed yet.
Evidence: build/t485-s8/p7/{line-check,published-hashes,test-process}.json.
### P7 final verification

Runtime7 passed real MEM/NTVDMD export/debugger/register/allocation/termination
checks and DOS4GW/NESticle rendered menu plus Escape hide/restore. Evidence:
build/t485-s8/p7/debug-live/trace.txt and dos4gw/nesticle.txt.graphics-result.json.
All 73 final source identities and diagnostic mappings passed. Cached full build
and dual 13-image publication passed. Commit/push completes P7, not S8.
459/851 final-verified, 392 remain. Batch reduction 210 points/458 lines;
cumulative 321/767, newline-only count 288. Next: base/dos/daytona/makefile.

### P8 DOS metadata and initial common headers

P7 committed and pushed as 0109ef454; 459 files delivered.

| File under softpc.new/base | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| dos/daytona/makefile | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| dos/daytona/sources | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| dos/dirs | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| dos/sources.inc | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/base_def.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/bios.h | 8/16 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/build_id.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/cga.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/chkmallc.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/ckmalloc.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/cmos.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |
| inc/cmosbios.h | 0/0 -> 0/0 | Original retained code/format; existing exclusions preserved |

All twelve files pass pinned-source retained-code and unchanged-error checks.
bios.h restores eight format points/16 lines, without declaration or ABI changes.
471/851 reviewed, 380 remain; 459 delivered and twelve P8 files local. Cumulative
non-error reduction is 329 points/783 lines (8/16 local); newline-only total 292.
No P8 build/runtime pass claimed.
Next: src/mvdm-host/softpc.new/base/inc/cntlbop.h.

### P8 control and EGA headers

| File under softpc.new/base/inc | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| cntlbop.h | 5/10 -> 0/0 | Original retained content; existing exclusions preserved |
| config.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| cpu_vid.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| cpu4.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| debug.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| dma.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| dsktrace.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| dterm.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egacpu.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egagraph.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egamode.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egaports.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egaread.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |
| egavideo.h | 17/72 -> 0/0 | Original retained content; existing exclusions preserved |
| emm.h | 0/0 -> 0/0 | Original retained content; existing exclusions preserved |

egavideo.h contains interior Tab/space alignment changes in comments and macro
definitions. Before restoration these were compared after collapsing horizontal
whitespace only where no string/character literal occurs outside comments. Final
comparison is strict original retained nonblank content, with the unchanged error
directives. No macro value, declaration, ABI or selected algorithm changed.
dterm.h was restored during the first pass before a later comparison stopped;
it is counted once as a newline-only restoration. All 27 P8 files now pass final
retained-source comparison (build/t485-s8/p8-source-check.json).

486/851 reviewed, 365 remain; 459 delivered and 27 local. Cumulative reduction
351 points/865 lines (30/98 local); newline-only total 293. No P8 delivery yet.
Next: base/inc/equip.h.

### P8 remaining headers through keyba.h

| File under softpc.new/base/inc | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| equip.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| error.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| fdisk.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| fla.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| floppy.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| ga_defs.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| ga_mark.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gendrvr.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| get_env.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gfi.h | 25/58 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gfitest.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gfx_upd.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gisp_sas.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gispsvga.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| gmi.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| hfx.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_cfg.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_com.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_gwi.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_lpt.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_nls.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host_sas.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| host.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| hunter.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| idetect.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| ios.h | 71/534 -> 0/0 | Retained source matches pinned original; exclusions preserved |
| keyba.h | 0/0 -> 0/0 | Retained source matches pinned original; exclusions preserved |

All 54 P8 source hashes match the current files. Final retained-source and logical-line evidence is in build/t485-s8/p8-source-check.json and p8/line-check.json. gfi.h restores 25 points/58 lines; ios.h restores 71 points/534 lines of alignment without changing macro values or declarations. Existing exclusions remain unchanged.

513/851 reviewed; 338 remain. 459 are delivered and 54 are locally reviewed, awaiting P8 build/publication/runtime verification. Cumulative non-error reduction: 447 points/1457 lines, including 126/690 local. Newline-only count: 301. Next: base/inc/keyboard.h.

Registration reconciliation: c_seg.c is S6-P1-final-verified and its latest_sha256 matches the current source (5878ad3d3acf0111536339d81f8f26bb7c39dda4425000e0011abf5ca70e9405). Its immutable snapshot hash is preserved. CURRENT is synchronized to the file ledger; review is not delivery.

### P8 final verification

All 54 current source hashes and logical-line mappings passed. Cached full build and dual 13-image publication passed; all 26 destination hashes match runtime8 (build/t485-s8/p8/published-hashes.json). Actual MEM/NTVDMD lifecycle passed (debug-live/trace.txt); DOS4GW/NESticle rendered menu and Escape hide/restore passed (dos4gw/nesticle.txt.graphics-result.json). Documentation governance passed. No Full or ROM gameplay claim. Batch reduction: 126 non-error points/690 lines; cumulative 447/1457. 513/851 final-verified, 338 remain. Commit/push completes P8; S8 remains active.

### P9 headers and keymouse reconciliation

P8 committed and pushed as 25af4b12d; 513 files delivered.

| File under softpc.new/base | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| inc/keyboard.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/lock.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/mouse.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/ppi.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/printer.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/profile.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/quick_ev.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/rom.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/rs232.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/rtc_bios.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/sas.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/sasp.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/sndblst.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/spcfile.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/suplib.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/tape_io.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/timer.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/timestmp.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/tmstrobe.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/trace.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/umb.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/vga_dac.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/vgaports.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/video.h | 1/4 -> 0/0 | Original retained code and exclusions checked |
| inc/virtual.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/xt.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| inc/yoda.h | 0/0 -> 0/0 | Original retained code and exclusions checked |
| keymouse/daytona/makefile | 0/0 -> 0/0 | Original retained code and exclusions checked |
| keymouse/daytona/sources | 0/0 -> 0/0 | Original retained code and exclusions checked |
| keymouse/dirs | 0/0 -> 0/0 | Original retained code and exclusions checked |
| keymouse/mouse.c | 5/14 -> 0/0 | Original retained code and exclusions checked |
| keymouse/ppi.c | 0/0 -> 0/0 | Original retained code and exclusions checked |
| keymouse/sources.inc | 0/0 -> 0/0 | Original retained code and exclusions checked |

All 33 files passed retained-source and unchanged-exclusion checks against the pinned XP object, plus original logical-line mapping (build/t485-s8/p9-source-check.json and p9/line-check.json). video.h restores trailing spaces on two constant definitions (1 point/4 lines). mouse.c restores five format points/14 lines; no selected statement, declaration or literal changes. Eleven newline-only files restore exact upstream bytes. No product runtime pass is claimed for P9 yet.

546/851 reviewed: 513 delivered, 33 locally complete, 305 remain. Cumulative non-error reduction 453 points/1475 lines (P9 local 6/18); newline-only count 312. Next: base/support/daytona/makefile. P9 contains only format/line reconciliation and accumulates before the next delivery.

### P9 support and DMA formatting

| File under softpc.new/base | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| support/daytona/makefile | 0/0 -> 0/0 | Original retained code and exclusions checked |
| support/daytona/sources | 0/0 -> 0/0 | Original retained code and exclusions checked |
| support/dirs | 0/0 -> 0/0 | Original retained code and exclusions checked |
| support/get_env.c | 0/0 -> 0/0 | Original retained code and exclusions checked |
| support/ios.c | 12/32 -> 0/0 | Original retained code and exclusions checked |
| support/sources.inc | 0/0 -> 0/0 | Original retained code and exclusions checked |
| support/time_day.c | 0/0 -> 0/0 | Original retained code and exclusions checked |
| system/at_dma.c | 161/1294 -> 0/0 | Original retained code and exclusions checked |

ios.c retained lines compare directly after trimming boundary whitespace. at_dma.c additionally required per-line token comparison after masking comments while preserving newlines and quoted literals; multi-character operators and macro continuation tokens remained identical. The final restored retained text is checked strictly against the pinned original, not merely token-normalized. Existing error directives are unchanged and all retained logical lines map to original lines.

All 41 P9 files pass build/t485-s8/p9-source-check.json and p9/line-check.json. 554/851 reviewed: 513 delivered, 41 local, 297 remain. P9 reduction 179 points/1344 lines; cumulative 626/2801. Newline-only total remains 312. Next: base/system/cmosnt.c. Build/publication/runtime verification pending.

### P9 final verification

All 41 final source hashes, original retained text, unchanged exclusions and logical-line mappings passed. Cached full build and dual 13-image publication passed; 26 destination hashes match runtime9 (build/t485-s8/p9/published-hashes.json). MEM/NTVDMD lifecycle passed, including debug register writeback, allocation rollback and matching task termination (debug-live/trace.txt). DOS4GW/NESticle rendered menu and Escape hide/restore passed (dos4gw/nesticle.txt.graphics-result.json). No Full or gameplay claim. Batch reduction 179 non-error points/1344 lines; cumulative 626/2801. 554/851 final-verified, 297 remain. Commit/push completes P9, not S8.

### P10 system, video and directory metadata

P9 committed and pushed as 1b93a8a7b; 554 files delivered.

| File under softpc.new | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| base/system/cmosnt.c | 4/8 -> 0/0 | Original retained text and exclusions verified |
| base/system/daytona/makefile | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/daytona/sources | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/dirs | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/dummy_nt.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/idetect.c | 10/20 -> 0/0 | Original retained text and exclusions verified |
| base/system/illegalp.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/qevnt.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/rom.c | 8/16 -> 0/0 | Original retained text and exclusions verified |
| base/system/sources.inc | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/timestrb.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/system/unexp_nt.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/unix/makefile | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/unix/sources | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/daytona/makefile | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/daytona/sources | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/dirs | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/ega_dmmy.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/ega_mode.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/ega_trcr.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/gvi.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/sources.inc | 0/0 -> 0/0 | Original retained text and exclusions verified |
| base/video/vga_vide.c | 101/1296 -> 0/0 | Original retained text and exclusions verified |
| base/video/video_io.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| dirs | 2/5 -> 0/0 | Original retained text and exclusions verified |

cmosnt.c, idetect.c and rom.c restore boundary whitespace; vga_vide.c also restores Tab/space alignment after per-line code-token, literal and operator comparison. Final retained text and logical lines match the pinned XP source. Existing error directives are unchanged. Evidence: build/t485-s8/p10-source-check.json and p10/line-check.json (25 files).

softpc.new/dirs is restored byte-exact, including the original data directory entry. Actual worker composition uses explicit paths and worker_original_sources calls for SOURCES manifests (tools/build/worker/original-sources.cmake, source-groups.cmake and units.cmake), not DIRS traversal. The data files remain under src/mvdm-guest/softpc.new/data; no guest source or binary was changed and no new build input was introduced.

579/851 reviewed: 554 delivered, 25 local, 272 remain. P10 local reduction 125 points/1345 lines; cumulative 751/4146. Newline-only count remains 312. Final package build/publication/runtime verification is pending. Next: softpc.new/host/dirs.

### P10 initial host headers

| File under softpc.new/host | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| dirs | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/error.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_bop.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_emm.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_gen.h | 1/2 -> 0/0 | Original retained text and exclusions verified |
| inc/host_gfx.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_hfx.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_inc.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_qev.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_rrr.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/host_trc.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/hostsync.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/insignia.h | 2/4 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_cga.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_cgalt.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_com.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_det.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_ega.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_egalt.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_eoi.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_event.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_extra.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_fdisk.h | 0/0 -> 0/0 | Original retained text and exclusions verified |

The 23 files restore fourteen newline-only files and three format points/six lines. insignia.h uses per-line token comparison before restoration; final text comparison is strict. All 48 P10 files pass original retained-source, unchanged-error and logical-line checks (build/t485-s8/p10-source-check.json and p10/line-check.json).

602/851 reviewed: 554 delivered, 48 local, 249 remain. P10 reduction 128 points/1351 lines; cumulative 754/4152. Newline-only count 326. Next: host/inc/nt_fulsc.h. Cached build, dual publication and targeted runtime checks follow; no P10 runtime pass yet.

### P10 final verification

All 48 final source hashes, original retained text, unchanged exclusions and logical-line mappings passed. Cached full build and dual 13-image publication passed; 26 destination hashes match runtime10 (build/t485-s8/p10/published-hashes.json). MEM/NTVDMD lifecycle passed with actual DLL load, debug writeback, allocation rollback and matching termination (debug-live/trace.txt). DOS4GW/NESticle menu pixels and Escape hide/restore passed (dos4gw/nesticle.txt.graphics-result.json). No Full or ROM gameplay claim. P10 reduction 128 non-error points/1351 lines; cumulative 754/4152. 602/851 final-verified, 249 remain. Commit/push completes P10, not S8.

### P11 remaining host headers and initial host sources

P10 committed and pushed as 1e6014476; 602 files delivered.

| File under softpc.new/host | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| inc/nt_fulsc.h | 18/98 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_graph.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_inthk.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_mem.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_mouse.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_pif.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_reset.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_sb.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_thred.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_timer.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_uis.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/nt_vddp.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/ntcheese.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| inc/sim32.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| link/makefile | 0/0 -> 0/0 | Original retained text and exclusions verified |
| link/nt_uis.h | 0/0 -> 0/0 | Original retained text and exclusions verified |
| link/softpc.def | 0/0 -> 0/0 | Original retained text and exclusions verified |
| link/sources | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/cpucstbs.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/daytona/makefile | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/daytona/sources | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/dirs | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/fprt.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/makefile.inc | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/nt_det.c | 9/60 -> 0/0 | Original retained text and exclusions verified |
| src/nt_ega.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| src/nt_fdisk.c | 69/492 -> 0/0 | Original retained text and exclusions verified |

nt_fulsc.h, nt_det.c and nt_fdisk.c restore formatting after per-line token/literal/operator checks. No macro value, structure field order or selected statement changes. Final retained text compares strictly to the pinned XP object, with unchanged error directives and original logical-line mapping. All 27 files passed build/t485-s8/p11-source-check.json and p11/line-check.json. Nine newline-only files restore original bytes.

629/851 reviewed: 602 delivered, 27 local, 222 remain. P11 local reduction 96 points/650 lines; cumulative 850/4802. Newline-only count 335. No P11 build/publication/runtime pass claimed. Next: host/src/nt_input.c.

### P11 input, printer and NLS reconciliation

| File under softpc.new/host/src | Non-error points/lines before -> after | Result |
| --- | --- | --- |
| nt_input.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_inthk.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_lpt.c | 6/34 -> 0/0 | Original retained text and exclusions verified |
| nt_munge.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_nls.c | 5/9 -> 0/0 | Original retained text and exclusions verified |
| nt_ntfun.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_sas.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_sbdsp.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_sbmid.c | 0/0 -> 0/0 | Original retained text and exclusions verified |
| nt_sbmxr.c | 0/0 -> 0/0 | Original retained text and exclusions verified |

All 37 P11 files pass strict retained-source and logical-line checks, with unchanged existing error directives. nt_lpt.c restores six format points/34 lines; nt_nls.c restores one blank line and four alignment points (5/9). No selected statement, macro, literal or declaration changes. nt_munge.c restores original newline bytes.

639/851 reviewed: 602 delivered, 37 local, 212 remain. P11 reduction 107 points/693 lines; cumulative 861/4845. Newline-only count 336. Build/publication/runtime verification pending. Next: host/src/nt_sec.c.

### P11 final verification

All 37 final source identities, retained text, exclusions and logical-line mappings passed. Cached full build and dual 13-image publication passed; 26 destination hashes match runtime11 (build/t485-s8/p11/published-hashes.json). Actual MEM/NTVDMD load, debug register writeback, allocation rollback and matching task termination passed (debug-live/trace.txt). DOS4GW/NESticle rendered menu and Escape hide/restore passed (dos4gw/nesticle.txt.graphics-result.json). No Full or ROM gameplay claim. P11 reduction 107 non-error points/693 lines; cumulative 861/4845. 639/851 final-verified, 212 remain. Commit/push completes P11, not S8.
