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
| 1 | src/mvdm-host/inc/wowcmdlg.h | 1 | Pending |
| 2 | src/mvdm-host/inc/wowkbd.h | 1 | Pending |
| 3 | src/mvdm-host/inc/wowkrn.h | 1 | Pending |
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
