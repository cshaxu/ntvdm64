# S11 Ordered WOW Repair Progress

Admitted after [S10 closure](s10-progress.md#s10-closure), under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Scope:48 files and452
frozen points; no whole-repository or unconverted XP audit. Review complete
files in relative-path order, trace actual providers and preserve original
guest binaries. Residual differences require concrete necessity evidence.

Initial progress:0/48 files,0/452 points reviewed;452 unreviewed. No changes,
restorations or deliveries claimed. Measure each file at entry; do not use
frozen point counts as current byte-sensitive diff counts.

| Order | File | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | src/mvdm-host/inc/wowcmdlg.h | 1 | P1 final verified; commit/push pending |
| 2 | src/mvdm-host/inc/wowkbd.h | 1 | P1 final verified; commit/push pending |
| 3 | src/mvdm-host/inc/wowkrn.h | 1 | P1 final verified; commit/push pending |
| 4 | src/mvdm-host/inc/wowsnd.h | 2 | Pending |
| 5 | src/mvdm-host/wow32/isvwow.h | 1 | Pending |
| 6 | src/mvdm-host/wow32/precomp.h | 3 | Pending |
| 7 | src/mvdm-host/wow32/walias.c | 4 | Pending |
| 8 | src/mvdm-host/wow32/walias.h | 6 | Pending |
| 9 | src/mvdm-host/wow32/wcall16.c | 4 | Pending |
| 10 | src/mvdm-host/wow32/wcntl32.c | 33 | Pending |
| 11 | src/mvdm-host/wow32/wcommdlg.c | 11 | Pending |
| 12 | src/mvdm-host/wow32/wdde.c | 25 | Pending |
| 13 | src/mvdm-host/wow32/wdde.h | 4 | Pending |
| 14 | src/mvdm-host/wow32/wdib.c | 1 | Pending |
| 15 | src/mvdm-host/wow32/wdos.c | 6 | Pending |
| 16 | src/mvdm-host/wow32/wgdi31.c | 1 | Pending |
| 17 | src/mvdm-host/wow32/winsockp.h | 3 | Pending |
| 18 | src/mvdm-host/wow32/wkernel.c | 10 | Pending |
| 19 | src/mvdm-host/wow32/wkfileio.c | 6 | Pending |
| 20 | src/mvdm-host/wow32/wkman.c | 22 | Pending |
| 21 | src/mvdm-host/wow32/wkman.h | 1 | Pending |
| 22 | src/mvdm-host/wow32/wmdisp32.c | 79 | Pending |
| 23 | src/mvdm-host/wow32/wmdisp32.h | 2 | Pending |
| 24 | src/mvdm-host/wow32/wmsg16.c | 37 | Pending |
| 25 | src/mvdm-host/wow32/wmsgcb.c | 5 | Pending |
| 26 | src/mvdm-host/wow32/wmsgem.c | 3 | Pending |
| 27 | src/mvdm-host/wow32/wmsglb.c | 3 | Pending |
| 28 | src/mvdm-host/wow32/wole2.c | 5 | Pending |
| 29 | src/mvdm-host/wow32/wow32.h | 1 | Pending |
| 30 | src/mvdm-host/wow32/wow32fax.c | 7 | Pending |
| 31 | src/mvdm-host/wow32/wowhooks.c | 39 | Pending |
| 32 | src/mvdm-host/wow32/wparam.c | 4 | Pending |
| 33 | src/mvdm-host/wow32/wsdata.c | 19 | Pending |
| 34 | src/mvdm-host/wow32/wsext.c | 4 | Pending |
| 35 | src/mvdm-host/wow32/wshell.c | 1 | Pending |
| 36 | src/mvdm-host/wow32/wshimdb.c | 5 | Pending |
| 37 | src/mvdm-host/wow32/wspool.c | 2 | Pending |
| 38 | src/mvdm-host/wow32/wstruc.c | 3 | Pending |
| 39 | src/mvdm-host/wow32/wuclass.c | 7 | Pending |
| 40 | src/mvdm-host/wow32/wuclip.c | 16 | Pending |
| 41 | src/mvdm-host/wow32/wudlg.c | 5 | Pending |
| 42 | src/mvdm-host/wow32/wuman.c | 5 | Pending |
| 43 | src/mvdm-host/wow32/wumsg.c | 32 | Pending |
| 44 | src/mvdm-host/wow32/wumsg.h | 1 | Pending |
| 45 | src/mvdm-host/wow32/wuser.c | 5 | Pending |
| 46 | src/mvdm-host/wow32/wuser31.c | 2 | Pending |
| 47 | src/mvdm-host/wow32/wutmr.c | 1 | Pending |
| 48 | src/mvdm-host/wow32/wuwind.c | 13 | Pending |

## P1 complete-file source review

Three files reviewed,3/452 frozen points locally restored;0 delivered,449
unreviewed. Original XP Git a2f6d7c93aa4f11efc51147566d5961c59a8ef89 and
files.csv hashes verified. No guest binary/source changed.

| File | Before non-error points/lines | After | Disposition |
| --- | ---: | ---: | --- |
| inc/wowcmdlg.h | 1/397 | 0/0 | Restore LF and final DOS EOF; byte-exact XP. |
| inc/wowkbd.h | 4/4 | 1/1 | Restore EOF; retain one line62 anchor and original NULLSTRUCT exclusion. |
| inc/wowkrn.h | 1/3333 | 13/13 | Remove148-line local frame block; restore LF and13 anchors following the original endif. |

Total6/3734 ->14/14: +8 points,-3720 lines. Standing format approval applies
to block growth exposed by LF restoration. Frozen review points are not the
current diff-block count. Fourteen remaining lines are diagnostic anchors,
not new behavior or retained local algorithms.

Production wkgthunk.c consumes PICALLPROC32W16; wkernel.c and wkman.c use
WOWFREERESOURCE16/WOWOUTPUTDEBUGSTRING16, also indexed by wktbl2.h. No added
legacy frame name has a C/C++ production consumer. The sole host test use
of ICALLPROC32 changes to the existing identical-field ICALLPROC32W16;
its inputs and test logic are unchanged. Legacy guest assembly names cannot
justify this XLATOFF block: it does not generate guest declarations, and
original guest is immutable. Removing the complete block avoids partial
fragmentation. All13 NULLSTRUCT exclusions remain. The final anchors execute after the
excluded arms, preserving selected original source line mapping. Cached full build/publication runner20652 started;
final tests,commit,push pending. This is source review,not delivery.

P1 final runtime1: cached fullbuild PASS; dual publication26/26 hashes PASS.
Existing real WOW generic-call test reports three guest phases,zero errors,
nested callbacks and three releases; actual WINMINE visible window PASS.
No original guest rebuilt; existing independent probe target was cache-clean.
Independent source-check.json verifies all original matched logical lines
(199/113/1586 respectively),unchanged error directives and pinned XP hashes.
All3 files/3 frozen points final verified; commit/push pending. No Full claim.


P1 pre-commit correction: the first local line-anchor candidate placed anchors
inside NULLSTRUCT; the compiler skips them in the supported selection. That
candidate is superseded,not final acceptance. Put anchors back after endif
(wowkbd line62;13 original anchors in wowkrn),keeping the same14/14 census.
No functional adaptation added. Source-check.json now checks placement after
excluded arms and selected original line mappings; discarded excluded endif
line numbers are not claimed as selected-code verification. Final runtime2 is
being rebuilt and republished;runtime1 test successes are candidate evidence
only. Final-code tests/commit/push remain pending.


P1 final acceptance: runtime2 cached fullbuild and dual publication PASS,
26/26 published hashes verified. Final generic-call probe again passed all
three guest phases,zero errors,nested callback counts and three releases;
actual WINMINE visible window PASS. Evidence:build/t485-s11/p1/final.
Runner16956 completed. These results supersede runtime1 for delivery.
3/48 files and3/452 frozen points final-verified;449 points remain unreviewed.
Current reviewed-file census14/14 vs6/3734: +8 blocks,-3720 lines. No functional
residual from these three frozen points;14 necessary line anchors remain.
Commit/push completes this P;Full remains S13.
