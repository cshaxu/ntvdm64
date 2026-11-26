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
