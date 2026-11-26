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