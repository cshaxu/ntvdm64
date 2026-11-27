# S15 P31 N022 bitmap scaling migration

Full24/24 passed; commit/push follow governance review. Cached full build and runtime32 dual publication
passed; 13 images / 26 destination hashes verified. Native bitmap fixtures
passed; the stale registration source check was updated and passed separately.

Second N022 file: src/non-mvdm/windows/core/ntuser/client/cldib.c.
Pinned XP commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
blob f28888fd055d42c70c4252e10ff93f501e78b1ac. OpenNT before object
80b732e103984fdf02cf3d038f0af59e514d0f32. Original bytes read from Git.

The only residual is the precomp.h replacement by the existing B bitmap
binding header. All other bytes equal XP, including LF. No #error or #line
in this file; one-for-one include replacement preserves original line numbers.
Original precomp.h requires the private USER closure; the bounded public
GDI/heap/assertion binding supplies the selected algorithm's actual consumers.

XP ScaleDIB catches scaling faults and returns FALSE. The selected retail
W32ExceptionHandler(FALSE, flags) in windows/core/w32inc/w32err.h returns
EXCEPTION_EXECUTE_HANDLER without changing LastError. The B binding exposes
that exact FALSE contract and rejects a future TRUE caller at compilation.
No independent filter/state machine; C keeps all original scaling algorithms.
XP palette rounding, WORD narrowing and parameter annotation remain original.

Existing bitmap fixture links the production cldib object. New assertions
check 4-to-8-bit conversion rounding (26/26/26 -> palette index43), inaccessible
pixel memory returning FALSE and preserving LastError. Existing parallel,
nested, named-resource and failure-release cases also passed.

The attached static registration gate initially failed on its old21-slot
expectation. N020 P19 had already migrated to25 input slots and20 output slots.
Updated that stale count and explicitly verified zero/unconsumed
pfnWOWModuleUnload, whose four-argument cleanup boundary is documented in
n020-wowuser.md. No production callback changed. Native fixtures are retained;
only the failed static gate was retried. Full initially did not start because
its prerequisite driver failed; it subsequently passed24/24 against runtime32.

Per-file nonerror diff:1point/1292lines ->1point/2lines (-1290lines).
Global:1325files;777byte-identical;788nonerror same/537different;
7798points/130239lines. Delta:0files/0exact/0same/0different/0points/-1290lines.
1264XP/61OpenNT; one baseline transition. LF normalization is not substituted
for byte-sensitive statistics. No guest or winnt.zip change.

This is the tenth completed file since P26 Full. Full24/24 passed. N0222/39 delivered; directories remain57/159, S15
remains9/14. Ten-file counter resets to zero. Next clglobal.c.
[Receipt](n022-dib-result.json), [census](n022-dib-census.json),
[Full](n022-dib-full.json).

Win95 retains the owner-approved exit1 boundary: desktop/mode passed,
exitAccepted=true and knownExitOne=true, but normalExit=false. This is not
a zero-return normal-exit claim. Win3.1 standard/enhanced normalExit=true.
