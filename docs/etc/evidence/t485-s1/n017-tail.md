# N017 final six declarations - S15 P10

Starting HEAD0e799d1d0; XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, raw Git bytes.
Three-way originals: build/t485-s15/n017. Final batch and execution:
build/t485-s15/n017-tail. [Source moves](n017-tail-sources.json),
[original-relative census](n017-tail-census.json).

## Whole-file decisions

DIR positions14-19: rxp/rxuser/xsprocs/xstypes move to original XP
public/internal/net/inc; strucinf/timelib move to ds/netapi/inc. Product paths
were already connected in P9, so no new build configuration or forwarding file.
Actual consumers are VDMREDIR vrnetapi/vrremote, RAP convert and Netlib
svcinfo/xlatesvc/time/timezone. Restore all original declarations including
unused _WIN64 pointer macros. Existing NT_INCLUDED and SRVDBG2 excluded arms
replace exactly one line each, so no #line adaptation is needed.

time_now's DWORD declaration is now original XP and matches the existing XP
implementation. The removed old time conversions and XS_API_TABLE_ENTRY have
no current production consumers; no substitute or duplicated interface.
NetpAlertStructureInfo's XP declaration no longer includes StringSize; no
current call uses that API. No guest binaries changed or rebuilt.
Four files are XP byte-exact; timelib and xstypes differ only in their one
existing #error body. All six have zero nonerror diff, including newline/EOF.

## Metrics

| File | Before nonerror points/lines | Final |
| --- | ---: | ---: |
| rxp.h | 1/804 | 0/0 |
| rxuser.h | 1/230 | 0/0 |
| strucinf.h | 1/406 | 0/0 |
| timelib.h | 1/262 | 0/0 |
| xsprocs.h | 1/1810 | 0/0 |
| xstypes.h | 1/344 | 0/0 |

Before uses registered OpenNT, after fixed XP. Global1315files(0),
760raw exact(+4),768nonerror same(+6),547different(-6),7748points(-6),
164027lines(-3856).1229XP/86OpenNT. Six relocations, no added/deleted mirrors.
No functional adaptation remains in this batch.

## Verification status

Whole six-file migration completed before cached full build. Final build,
runtime10 dual13-image26hash publication, RAP11/11, both redirector lifecycle
fixtures and real guest NetAPI passed. [Receipt](n017-tail-result.json).
Six files since P9 Full24/24; Full not repeated. N017 is19/19 complete;
overall53/159 directories and S15 5/14. Next N018. Earlier paired userenvp.h
remains at its existing XP identity and is not counted twice.
