# M60 NT4 BOP/SVC Dispatch Ledger

Status: first source-derived index and transport contract, 2026-08-08.

## Scope

This is an index of the fixed NT4 OpenNT source dispatch surface, not an
implementation contract and not a request to add a BOP/SVC hook to any
runtime. Its purpose is to separate facts established by the original source
from per-handler ABI facts that still require source-level extraction.

Primary source inputs:

- `src/opennt/base/mvdm/inc/bop.h`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`
- `src/opennt/base/mvdm/inc/dossvc.h`
- `src/opennt/base/mvdm/dos/dem/demdisp.c`

## BOP Transport Facts

`bop.h` defines a BOP as the three guest bytes `C4 C4 selector`. The fixed
NT4 selector assignments relevant to the host dispatcher are:

| Selector | Original owner | Meaning / disposition |
| --- | --- | --- |
| `50h` | `MS_bop_0` | DEM DOS-service dispatcher; followed by one SVC byte. |
| `51h` | `MS_bop_1` | WOW dispatcher; out of the fixed DOS-only first cut. |
| `52h` | `MS_bop_2` | XMS dispatcher; followed by one service byte. |
| `53h` | `MS_bop_3` | DPMI dispatcher; protected-mode feature tier. |
| `54h` | `MS_bop_4` | COMMAND dispatcher; real-mode only and followed by one command SVC byte. |
| `56h` | `MS_bop_6` | historical debugger dispatcher; excluded from first cut. |
| `57h` | `MS_bop_7` | VDM redirector dispatcher; optional namespace/network tier. |
| `59h`-`5Fh` | monitor/host handlers | warning, idle, debugger, keyboard, video, notification, and unimplemented-interrupt paths; not DEM SVCs. |
| `FDh` | monitor | switch-to-real-mode historical monitor path; not an adapter permission. |
| `FEh` | monitor | end execution of code in a VDM; backend-controlled stop/return boundary. |

`MS_bop_0` obtains the service byte through
`Sim32GetVDMPointer(SEGOFF(getCS(), getIP()), 1, FALSE)`, calls
`DemDispatch(service)`, then performs `setIP(getIP() + 1)`. The byte is
therefore consumed by the original dispatcher, not by an arbitrary service
callback. A future backend must make this sequence observable and atomic at
the instruction/guest-state boundary; it must not give a profile handler an
unconstrained CPU-mode or instruction-pointer override.

`DemDispatch` rejects a service index at or above `SVC_DEMLASTSVC` (`49h`) by
setting CF and returning `FALSE`. For an in-range entry it clears the pending
hard-error packet's interrupt-24 flag when present, records `CurrentISVC`, and
calls the table entry. The historical `demNotYetImplemented` placeholder
clears CF. This odd distinction is source fact, not a modern error-policy
recommendation.

## DEM Service Index

`demdisp.c` declares exactly `49h` indices (`00h` through `48h`). `delegated`
means an original named DEM owner exists in the table; `placeholder` means the
original table points at `demNotYetImplemented`, not that the capability is
implemented by the research project.

| SVC | Symbol | Original table owner | State family | Dispatch state |
| --- | --- | --- | --- | --- |
| `00h` | `SVC_DEMCHGFILEPTR` | `demChgFilePtr` | file handle | delegated |
| `01h` | `SVC_DEMCHMOD` | `demChMod` | path metadata | delegated |
| `02h` | `SVC_DEMCLOSE` | `demClose` | file handle | delegated |
| `03h` | `SVC_DEMCREATE` | `demCreate` | file/path | delegated |
| `04h` | `SVC_DEMCREATEDIR` | `demCreateDir` | directory | delegated |
| `05h` | `SVC_DEMDELETE` | `demDelete` | file/path | delegated |
| `06h` | `SVC_DEMDELETEDIR` | `demDeleteDir` | directory | delegated |
| `07h` | `SVC_DEMDELETEFCB` | `demDeleteFCB` | FCB file | delegated |
| `08h` | `SVC_DEMFILETIMES` | `demFileTimes` | file metadata | delegated |
| `09h` | `SVC_DEMFINDFIRST` | `demFindFirst` | directory enumeration | delegated |
| `0Ah` | `SVC_DEMFINDFIRSTFCB` | `demFindFirstFCB` | FCB enumeration | delegated |
| `0Bh` | `SVC_DEMFINDNEXT` | `demFindNext` | directory enumeration | delegated |
| `0Ch` | `SVC_DEMFINDNEXTFCB` | `demFindNextFCB` | FCB enumeration | delegated |
| `0Dh` | `SVC_DEMGETBOOTDRIVE` | `demGetBootDrive` | boot/profile | delegated |
| `0Eh` | `SVC_DEMGETDRIVEFREESPACE` | `demGetDriveFreeSpace` | drive/media | delegated |
| `0Fh` | `SVC_DEMGETDRIVES` | `demGetDrives` | drive namespace | delegated |
| `10h` | `SVC_DEMGSETMEDIAID` | `demGSetMediaID` | media | delegated |
| `11h` | `SVC_DEMLOADDOS` | `demLoadDos` | bootstrap image | delegated |
| `12h` | `SVC_DEMOPEN` | `demOpen` | file/path | delegated |
| `13h` | `SVC_DEMQUERYCURRENTDIR` | `demQueryCurrentDir` | directory | delegated |
| `14h` | `SVC_DEMQUERYDATE` | `demQueryDate` | clock | delegated |
| `15h` | `SVC_DEMQUERYTIME` | `demQueryTime` | clock | delegated |
| `16h` | `SVC_DEMREAD` | `demRead` | file/guest buffer | delegated |
| `17h` | `SVC_DEMRENAME` | `demRename` | file/path | delegated |
| `18h` | `SVC_DEMSETCURRENTDIR` | `demSetCurrentDir` | directory | delegated |
| `19h` | `SVC_DEMSETDATE` | `demSetDate` | clock | delegated |
| `1Ah` | `SVC_DEMSETDEFAULTDRIVE` | `demSetDefaultDrive` | drive namespace | delegated |
| `1Bh` | `SVC_DEMSETDTALOCATION` | `demSetDTALocation` | guest memory / DTA | delegated |
| `1Ch` | `SVC_DEMSETTIME` | `demSetTime` | clock | delegated |
| `1Dh` | `SVC_DEMSETV86KERNELADDR` | `demSetV86KernelAddr` | historical monitor | delegated |
| `1Eh` | `SVC_DEMWRITE` | `demWrite` | file/guest buffer | delegated |
| `1Fh` | `SVC_GETDRIVEINFO` | `demNotYetImplemented` | drive | placeholder |
| `20h` | `SVC_DEMRENAMEFCB` | `demRenameFCB` | FCB file | delegated |
| `21h` | `SVC_DEMIOCTL` | `demIOCTL` | device/media | delegated |
| `22h` | `SVC_DEMCREATENEW` | `demCreateNew` | file/path | delegated |
| `23h` | `SVC_DEMDISKRESET` | `demDiskReset` | disk/media | delegated |
| `24h` | `SVC_DEMSETDPB` | `demNotYetImplemented` | DOS disk parameter block | placeholder |
| `25h` | `SVC_DEMGETDPB` | `demGetDPB` | DOS disk parameter block | delegated |
| `26h` | `SVC_DEMSLEAZEFUNC` | `demNotYetImplemented` | compatibility | placeholder |
| `27h` | `SVC_DEMCOMMIT` | `demCommit` | file durability | delegated |
| `28h` | `SVC_DEMEXTHANDLE` | `demNotYetImplemented` | extended handle | placeholder |
| `29h` | `SVC_DEMABSDRD` | `demAbsRead` | absolute disk | delegated |
| `2Ah` | `SVC_DEMABSDWRT` | `demAbsWrite` | absolute disk | delegated |
| `2Bh` | `SVC_DEMGSETCDPG` | `demNotYetImplemented` | code page | placeholder |
| `2Ch` | `SVC_DEMCREATEFCB` | `demCreateFCB` | FCB file | delegated |
| `2Dh` | `SVC_DEMOPENFCB` | `demOpenFCB` | FCB file | delegated |
| `2Eh` | `SVC_DEMCLOSEFCB` | `demCloseFCB` | FCB file | delegated |
| `2Fh` | `SVC_DEMFCBIO` | `demFCBIO` | FCB file/guest buffer | delegated |
| `30h` | `SVC_DEMDATE16` | `demDate16` | clock | delegated |
| `31h` | `SVC_DEMGETFILEINFO` | `demGetFileInfo` | file metadata | delegated |
| `32h` | `SVC_DEMSETHARDERRORINFO` | `demSetHardErrorInfo` | error/INT 24 | delegated |
| `33h` | `SVC_DEMRETRY` | `demRetry` | error/retry | delegated |
| `34h` | `SVC_DEMLOADDOSAPPSYM` | `demLoadDosAppSym` | debug symbols | delegated |
| `35h` | `SVC_DEMFREEDOSAPPSYM` | `demFreeDosAppSym` | debug symbols | delegated |
| `36h` | `SVC_DEMENTRYDOSAPP` | `demEntryDosApp` | process lifecycle | delegated |
| `37h` | `SVC_DEMDOSDISPCALL` | `demDOSDispCall` | display/process | delegated |
| `38h` | `SVC_DEMDOSDISPRET` | `demDOSDispRet` | display/process | delegated |
| `39h` | `SVC_OUTPUT_STRING` | `demOutputString` | stream/output | delegated |
| `3Ah` | `SVC_INPUT_STRING` | `demInputString` | stream/input | delegated |
| `3Bh` | `SVC_ISDEBUG` | `demIsDebug` | debugger | delegated |
| `3Ch` | `SVC_PDBTERMINATE` | `demTerminatePDB` | process lifecycle | delegated |
| `3Dh` | `SVC_DEMEXITVDM` | `demExitVDM` | process/session exit | delegated |
| `3Eh` | `SVC_DEMWOWFILES` | `demWOWFiles` | WOW file policy | delegated |
| `3Fh` | `SVC_DEMLOCKOPER` | `demLockOper` | locking | delegated |
| `40h` | `SVC_DEMDRIVEFROMHANDLE` | `demNotYetImplemented` | drive/handle | placeholder |
| `41h` | `SVC_DEMGETCOMPUTERNAME` | `demGetComputerName` | host identity | delegated |
| `42h` | `SVC_DEMFASTREAD` | `demNotYetImplemented` | file/guest buffer | placeholder |
| `43h` | `SVC_DEMFASTWRITE` | `demNotYetImplemented` | file/guest buffer | placeholder |
| `44h` | `SVC_DEMCHECKPATH` | `demCheckPath` | path containment | delegated |
| `45h` | `SVC_DEMSYSTEMSYMBOLOP` | `demSystemSymbolOp` | debug symbols | delegated |
| `46h` | `SVC_DEMGETDPBLIST` | `demGetDPBList` | DOS disk parameter blocks | delegated |
| `47h` | `SVC_DEMPIPEFILEDATAEOF` | `demPipeFileDataEOF` | pipe | delegated |
| `48h` | `SVC_DEMPIPEFILEEOF` | `demPipeFileEOF` | pipe | delegated |

## Execution Tiers

The index supports a future fixed-profile tiering exercise, but does not itself
authorize any handler implementation:

1. **Bootstrap candidates:** `0Dh`, `0Fh`, `11h`, `14h`, `15h`, and the
   guest-memory aspects of `1Bh`. Their actual reachability must be traced.
2. **First command candidates:** file/path and directory handlers, `39h`,
   `3Ah`, and lifecycle handlers. Their exact span, error, and reentrancy ABI
   remains unextracted.
3. **Optional/explicitly deferred:** FCB, absolute-disk, redirector, WOW,
   debugger, DPMI, XMS, and all placeholder services.

The next ledger increment must inspect the original handler implementations
starting with `demLoadDos`, `demOpen`, `demRead`, `demWrite`,
`demQueryCurrentDir`, `demSetCurrentDir`, and `demGetDrives`. For each it must
record registers, guest-memory spans, output/CF behavior, shared DEM state,
host dependency, and synchronous/asynchronous behavior. It may not replace a
handler or introduce a synthetic dispatcher while gathering that evidence.
