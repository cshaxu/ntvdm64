# T199 S37 DEM Working Service Ledger 001

## Purpose

This is the working control ledger for the active DEM package.  It is derived
from the 73-entry `apfnSVC[]` in
`src/opennt/base/mvdm/dos/dem/demdisp.c`, where `50:49` is the non-callable
sentinel.  Each row has one current disposition.  “Bounded” means a
source-built provider and focused family evidence exist; it does **not** mean
that a historical NT host facility has been recreated.  “Terminal” means an
explicit source-derived CLI result, not a generic CPU pass-through.

## Per-service ledger

| BOP | Original owner | Current disposition | Remaining package work |
| --- | --- | --- | --- |
| `50:00` | `demChgFilePtr` | bounded readonly namespace seek | handle-token negative sweep |
| `50:01` | `demChMod` | terminal AX=5/CF | readonly namespace family review |
| `50:02` | `demClose` | bounded readonly namespace close | handle lifetime sweep |
| `50:03` | `demCreate` | terminal AX=5/CF | readonly namespace family review |
| `50:04` | `demCreateDir` | terminal AX=5/CF | readonly namespace family review |
| `50:05` | `demDelete` | terminal AX=5/CF | readonly namespace family review |
| `50:06` | `demDeleteDir` | terminal AX=5/CF | readonly namespace family review |
| `50:07` | `demDeleteFCB` | terminal AX=5/CF | FCB handle-family review |
| `50:08` | `demFileTimes` | immutable metadata get; set AX=5/CF | readonly namespace family review |
| `50:09` | `demFindFirst` | bounded profile pathname search | DTA/layout negative sweep |
| `50:0A` | `demFindFirstFCB` | bounded profile FCB search | SRCHBUF/layout closure |
| `50:0B` | `demFindNext` | bounded profile pathname continuation | DTA/layout negative sweep |
| `50:0C` | `demFindNextFCB` | bounded profile FCB continuation | SRCHBUF/layout closure |
| `50:0D` | `demGetBootDrive` | bounded profile drive result | GSET cross-provider sweep |
| `50:0E` | `demGetDriveFreeSpace` | immutable-volume snapshot | GSET invalid-drive sweep |
| `50:0F` | `demGetDrives` | immutable drive snapshot | GSET gap/drive sweep |
| `50:10` | `demGSetMediaID` | immutable-volume get; set terminal CF | GSET invalid-range sweep |
| `50:11` | `demLoadDos` | bounded NTDOS load transaction | startup composition integration |
| `50:12` | `demOpen` | bounded readonly namespace open | handle-token negative sweep |
| `50:13` | `demQueryCurrentDir` | bounded immutable-root CDS repair | CDS invalid-range sweep |
| `50:14` | `demQueryDate` | CLI clock provider | clock policy review |
| `50:15` | `demQueryTime` | CLI clock provider | clock policy review |
| `50:16` | `demRead` | bounded readonly namespace read | DTA/handle negative sweep |
| `50:17` | `demRename` | terminal AX=5/CF | readonly namespace family review |
| `50:18` | `demSetCurrentDir` | bounded admitted-root success; other path AX=5/CF | CDS/root negative sweep |
| `50:19` | `demSetDate` | source-derived AL=FF | clock policy review |
| `50:1A` | `demSetDefaultDrive` | checked guard; admitted-root success | CDS/root negative sweep |
| `50:1B` | `demSetDTALocation` | copied DTA registration | invalid-layout sweep |
| `50:1C` | `demSetTime` | source-derived AL=FF | clock policy review |
| `50:1D` | `demSetV86KernelAddr` | source-derived normal return | MISC-family regression retained |
| `50:1E` | `demWrite` | terminal AX=5/CF | readonly namespace family review |
| `50:1F` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:20` | `demRenameFCB` | terminal AX=5/CF | FCB handle-family review |
| `50:21` | `demIOCTL` | metadata bounded; other terminal AX=1/CF | DASD subfunction sweep |
| `50:22` | `demCreateNew` | terminal AX=5/CF | readonly namespace family review |
| `50:23` | `demDiskReset` | source-derived no-device return | no-device boundary review |
| `50:24` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:25` | `demGetDPB` | immutable-volume fake-DPB branch | GSET invalid-range sweep |
| `50:26` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:27` | `demCommit` | source-derived CF-clear completion | readonly namespace family review |
| `50:28` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:29` | `demAbsRead` | no-BDS terminal AX=21/CF | DASD subfunction sweep |
| `50:2A` | `demAbsWrite` | no-BDS terminal AX=21/CF | DASD subfunction sweep |
| `50:2B` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:2C` | `demCreateFCB` | terminal AX=5/CF | FCB handle-family review |
| `50:2D` | `demOpenFCB` | terminal AX=5/CF | FCB handle-family review |
| `50:2E` | `demCloseFCB` | null success; otherwise AX=5/CF | FCB handle-family review |
| `50:2F` | `demFCBIO` | terminal AX=5/CF | FCB handle-family review |
| `50:30` | `demDate16` | CLI DOS-format clock | clock policy review |
| `50:31` | `demGetFileInfo` | terminal AX=5/CF | FCB handle-family review |
| `50:32` | `demSetHardErrorInfo` | copied registration | error registration negative sweep |
| `50:33` | `demRetry` | source-derived AX=FF/CF | error lifetime sweep |
| `50:34` | `demLoadDosAppSym` | no-debug normal return | MISC-family regression retained |
| `50:35` | `demFreeDosAppSym` | no-debug normal return | MISC-family regression retained |
| `50:36` | `demEntryDosApp` | contained no-VDD return | MISC-family regression retained |
| `50:37` | `demDOSDispCall` | no-debug normal return | MISC-family regression retained |
| `50:38` | `demDOSDispRet` | no-debug normal return | MISC-family regression retained |
| `50:39` | `demOutputString` | no-debug normal return | MISC-family regression retained |
| `50:3A` | `demInputString` | no-debug normal return | MISC-family regression retained |
| `50:3B` | `demIsDebug` | bounded no-debug result | MISC-family regression retained |
| `50:3C` | `demTerminatePDB` | no-debug/no-VDD return | MISC-family regression retained |
| `50:3D` | `demExitVDM` | typed controlled stop | terminal-result integration |
| `50:3E` | `demWOWFiles` | no-WOW normal return | MISC-family regression retained |
| `50:3F` | `demLockOper` | terminal AX=5/CF | error/lock review |
| `50:40` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:41` | `demGetComputerName` | no-identity source failure branch | configured-identity capability decision |
| `50:42` | `demNotYetImplemented` | separately admitted FastRead compatibility | compatibility regression |
| `50:43` | `demNotYetImplemented` | original no-op | original-no-op regression |
| `50:44` | `demCheckPath` | bounded `\\DEV\\` success; other path AX=5/CF | readonly namespace family review |
| `50:45` | `demSystemSymbolOp` | bounded no-debug result | MISC-family regression retained |
| `50:46` | `demGetDPBList` | packed Drive/Unit/Next list | GSET invalid-range/two-drive sweep |
| `50:47` | `demPipeFileDataEOF` | source-derived no-projected-pipe AX=6/CF | pipe capability decision |
| `50:48` | `demPipeFileEOF` | source-derived no-projected-pipe AX=6/CF | pipe capability decision |

## Next owner-package sequence

1. Close the readonly namespace/FCB review rows as two shared providers:
   handle-token and DTA/SRCHBUF layout/error contracts, not individual BOP
   implementations.
2. Close error/lock and miscellaneous lifecycle/console/WOW rows by their
   original component boundaries.
3. Regress DASD/IOCTL as one no-device component.
4. Replace this working ledger with the final S37 evidence ledger only after
   each row has its source/ABI/failure evidence and the complete family
   regression is stronger than the current routing sweep. One native trace
   then verifies integration and cannot add a new row.

## Evidence-key draft for final ledger

The 73 rows above remain the identity authority.  The following keys bind
their provider families to ABI/failure evidence; a final ledger row may cite a
key only when its listed service set contains that identity.

| Key | Services | ABI and terminal contract | Source-built evidence |
| --- | --- | --- | --- |
| N1 readonly namespace | `00-06,08,12-13,16-18,1E,22,27,44,47-48` | copied path/handle/DTA transactions; `08` reads immutable DOS metadata and rejects set AX=5/CF; immutable mutations return AX=5/CF; `27` retains OpenNT's CF-clear completion; `44` retains `\\DEV\\` with DX=0/CF clear; unprojected pipe endpoints `47-48` return AX=6/CF | r84 readonly-file fixture, r82 all-DEM fixture |
| S1 search/FCB | `07,09-0C,20,2C-31` | DTA/SRCHBUF checked writes; no-match/token failure AX=12/CF; absent FCB handle backend AX=5/CF | r69 all-DEM fixture |
| G1 immutable drive state | `0D-10,14-15,19,1A-1C,25,41,46` | immutable snapshots and checked guest writes; setters use recorded source-shaped terminal forms | GSET regression 002, r73 |
| M1 lifecycle/MISC | `11,1D,23,34-3E,45` | admitted NTDOS copy, controlled stop, or no-debug/no-VDD/no-WOW return; no host debugger/VDD | r71, lifecycle r68 |
| E1 error/lock | `32-33,3F` | copied real-mode locators; retry AX=FF/CF; no handle lock AX=5/CF | r74 focused fixture, r73 |
| D1 DASD/IOCTL | `21,29-2A` | 08/09 immutable metadata; invalid drive AX=15/CF; no-device terminals AX=1 or 21/CF | r73 |
| O1 original no-op/FastRead | `1F,24,26,28,2B,40,42-43` | original CF-clear return, except separately bounded FastRead compatibility | S4/S30 and r73 |
