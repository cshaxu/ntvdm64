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