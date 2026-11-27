# N020 Console private declarations - S15 P15

Starting HEADdc0f8a0ed. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`.
Source/XPSP1/NT/public/internal/windows/inc/winconp.h,
blob19d0402426dee7c5bb3f4e9a202222b0f6789b90.
Raw originals, branch mapping and execution: build/t485-s15/n020-console;
three-way research: build/t485-s15/n020-review.
[Census](n020-console-census.json).

## Original content and selected consumer

Restore complete XP declarations, including unused extended-edit tables,
additional registry names and original FE_SB branches. Keep original pshpack2/
poppack around ExtKey definitions. XP appends hWnd before ConsoleTitle in
CONSOLE_STATE_INFO, and optionally CodePage under FE_SB. Repository source
search finds no consumer of that structure outside the defining header; no
invented adapter or duplicate old layout is needed. The real cmdkeyb.c caller
and compat/conapi.h include use the retained keyboard layout declarations.
OUT annotations do not change their types or call ABI.

Preserve the three previously excluded original arms: both C++ linkage arms
and the UNICODE name alias. Each replaces one original line with one #error,
so all original physical line numbers stay correct and the seven old #line
anchors can be removed. No new exclusion, condition, shim or algorithm.
Do not exclude the new unused FE_SB declarations merely because not selected.
No original guest changes or rebuild.

## Metrics

OpenNT before7nonerror points/7lines; XP after0/0. Raw bytes differ solely in
three existing #error lines; this file is nonerror-identical, not byte-exact.
Global1315files(0),763byte-exact(0),772nonerror same(+1),543different(-1),
7728points(-7),163264lines(-7).1234XP/81OpenNT. One baseline transition,
no path or population change.

## Verification

Cached full build, runtime15 dual13-image26hash publication, existing font
positive/regression-control and Console input-batch contracts, and seven
real-package COMMAND cases passed. [Receipt](n020-console-result.json).
N0203/7 delivered, four remain. Overall55/159 and S15 7/14 unchanged.
Two files since P13 Full24/24; no repeat Full. Next winddi.h.
