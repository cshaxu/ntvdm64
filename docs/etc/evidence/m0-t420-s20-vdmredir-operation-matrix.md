# M0 T420 S20 — VDMREDIR operation-family matrix

## Scope and rule

This is the active-package acceptance ledger, not a claim of closure.  The
authoritative dispatch is the 50-entry `VrDispatchTable` in
`src/mvdm/vdmredir/vrdisp.c`; the guest callers are the original redirector
TSR under `src/mvdm/dos/v86/redir/`.  A row can close only with a fresh
matching-package DOS-guest witness, a source-backed absence of any selected
caller, or an owner-approved exclusion.  DLL linkage, host fixtures and an
old-package log do not close a row.

`VDMREDIR.DLL`'s remote RAP conversion path is a pre-approved exclusion.  On
2026-09-18 the owner additionally approved DLC as a modern-host capability
exclusion after the real guest/provider audit below.  Neither exclusion
extends to local NetAPI, named-pipe, mailslot or NetBIOS paths; modern SMB is
available through the host UNC provider and is covered by the pipe witness.

## Present evidence

- The reproducible DOS fixture builder is
  `tools/build/Build-T420S20VdmRedirGuestTests.ps1`.  Version 4 builds
  `VDMPRDR`, `VDMPTX`, `VDMPCAL`, `VDMMAIL`, `VDMNETAP`, `VDMNEI`,
  `VDMPASY`, `VDMPASW`, `VDMUSE` and `VDMWKSTA` below the requested
  `build/` root.
- `VDMNETA.COM` first discovers a real host LANA with `NCBENUM`, then submits
  `NCBADDNAME | ASYNCH` with a guest far post routine.  Its bounded follow-up
  `NCBCANCEL` distinguishes a pending request from a stale LANA assumption.
  It proves the original NetBIOS completion queue, simulated interrupt, INT
  5Ch route, guest `IRET` callback and post-return COMMAND progress—not merely
  a synchronous host `Netbios` result.
- Historical `O:\winnt` logs prove an earlier runtime package reached the
  named-pipe, transact, call, mailslot and `GetUserName` cases.  Its hashes do
  not equal the current formal candidate, so every such witness is marked
  **historical only** below.
- The bounded ConPTY observer is an admissible real-DOS guest route for the
  matching-candidate rows recorded below.  Its process cleanup and broker
  readiness remain part of each run's witness, not a substitute for it.

The current matching package contains `VDMREDIR.dll` SHA-256
`7e4d189e5372a45e0b334bc003bb2c075b8ef74ae327640552fd56ac1b5d6a69`
from `build/M0-T420/S20/formal-x86-013`.
The 2026-09-18 NetBIOS witness below was rerun through that package after a
test-owned worker/broker cleanup and followed by `MEM` plus COMMAND shutdown.

The same deployment also reran `VDMPRDR.COM`: DOS opened and read from
`\\NEKOGP4\PIPE\NTPTEST`, while its host test server listened at
`\\.\pipe\NTPTEST`.  It recorded `READ-RETURNED`, `VDMREDIR-TYPE-OK`, `MEM`
and COMMAND exit in `O:\winnt\logs\t420-s20-pipe-unc-final-r1.raw`.

## 2026-09-18 NetBIOS asynchronous completion witness

`VDMNETA.COM` ran against the matching formal-x86-013 DLL in
`O:\winnt`.  It obtained a host-provided LANA through real `NCBENUM`, then
submitted `NCBADDNAME | ASYNCH`; the host accepted it (`AL=00`).  The guest
recorded `NETBIOS-ASYNC-POST-OK IF=1` and
`NETBIOS-ASYNC-STATUS AL=00 RET=00 CPLT=00 CANCEL=26`, followed by successful
`MEM` and COMMAND `exit` in
`O:\winnt\logs\t420-s20-netbios-async-cancel-r3.raw`.

`26` is the original `NRC_CANCEL` result: by the time the bounded cancellation
request ran, `NCBADDNAME` had already completed and queued its post.  Thus the
test proves both a real accepted asynchronous request and the original
completion/acknowledgement path without relying on an invented timer or a
synthetic completion.  Microsoft documents the public `Netbios` API as
unsupported after Windows Vista; this host still supplies the observed
compatibility behavior, so this is a runtime witness rather than a portability
claim.

## 2026-09-18 DLC provider boundary audit

The original `VrDlc5cHandler` is present and reached by `VDMDLC.COM`, but its
first non-immediate action is the unchanged original `LoadDlcDll`: it requires
all three `DLCAPI.DLL` exports—`AcsLan`, `DlcCallDriver`, and `NtAcsLan`—which
in turn are the user-mode front end for the original DLC kernel driver.  This
Windows installation has none of `DLCAPI.DLL`, `DLC.SYS`, or `NETNBF.SYS`, and
neither `dlc` nor `netnbf` service exists.  The actual guest result is the
original `07` failure selected by that load gate.

The external OpenNT archives contain an identical historical `DLCAPI.DLL`
(`1dcbd4fac804c46ca9200bfde8fcc1a7568818530dd62a78d15dbf051be80408`),
but it is a Native-subsystem DLL whose debug provenance names the absent
`private/net/dlc/dlcapi/acslan.c`; it imports `NtCreateFile` and
`NtDeviceIoControlFile` for the old driver.  The two archive `DLC.SYS` files
are different historical driver binaries.  The source tree has only callers
and headers, not implementations of `AcsLan`, `DlcCallDriver`, or `NtAcsLan`.
Under source-policy these opaque binaries are neither runtime inputs nor a
source-recovery substitute.  A functional replacement would be a new host
network-provider protocol, outside S20's admitted VDMREDIR boundary; it needs
an explicit later admission rather than a fake `DLCAPI` shim.

## 2026-09-18 formal-x86-013 reachable-family sweep

The current fixture manifest
`build/M0-T420/S20/netbios-async-final-r1/manifest.json` (schema v4) was run
against the deployed formal-x86-013 package.  The bounded DOS observer passed
UNC pipe open/read, wait/state/peek, Transact, Call, no-server timeout,
asynchronous read and write, mailslot lifecycle, one-task termination cleanup,
synchronous and asynchronous NetBIOS, local user/enumeration/workstation/
message/service APIs, assignment macro lifecycle, and `NetUse` enum/info/
add-delete lifecycle.  Each positive operation was followed by `MEM` and
COMMAND shutdown; no `run16`, `ntvdm` or `basesrv` process remained after the
sweep.

`VDMDLC.COM` was deliberately included as a negative provider witness rather
than counted as a pass: it reached original INT 5Ch DLC dispatch, returned
`DLC-REAL-RESULT=07`, then completed `MEM` and COMMAND shutdown.  The
observer's zero exit for that probe means the diagnostic itself ran and
recorded the expected boundary, not that a DLC provider exists.

The established bounded `COMMAND -> MEM -> EDIT -> MEM` regression also
passed against the same deployment:
`O:\winnt\logs\t420-s20-established-command-edit-mem-r1.raw` records the
DOS banner, EDIT entry, repeated memory reports and COMMAND exit with no
remaining product process.

## 2026-09-17 matching-candidate guest run

The formal x86 candidate rebuilt at 21:26 (SHA-256 `ntvdm.exe`
`9f25a6bdb4fe951f572929a4294c40f5562b15147d0d75e5462b61305a1658a1`,
`VDMREDIR.dll`
`0c7e4a229c410c570601ff1ecf10f0bf0eb084cae3042723bf2e04ed25671e5f`)
was deployed together with freshly assembled DOS fixtures.  Bounded ConPTY
sessions reached COMMAND and passed named-pipe wait/state/info/peek/read,
Transact, Call, delayed async read, async write, mailslot make/write/info/
read/delete, `NetGetUserName`, and `NetGetEnumInfo`.

The completed local-workstation XACTSRV/RAP slice links `VDMREDIR.dll` as
SHA-256 `c52bc971b84291060460aff503040029460d95b25bd075383a32d19f17fc3e463`.
The real DOS `INT 21h/5F44h` fixture returns `NETWKSTA-OK`, then completes
`MEM`.  Fresh isolated runs of all listed pipe, async, mailslot and local
NetAPI fixtures likewise completed `MEM` and COMMAND shutdown; afterwards no
`run16`, `ntvdm`, `basesrv` or observer process remained.

Two negative results are original provider contracts, not standalone
substitutes: on the local byte-pipe, `NetHandleGetInfo` and
`NetHandleSetInfo` return mapped `ERROR_INVALID_PARAMETER` (87), as the
original source documents local endpoints cannot supply those fields;
`VrPeekMailslot` returns its original `ERROR_NOT_SUPPORTED` (50) because
Win32 mailslots have no peek operation.

`INT 21h/5F44h` had previously returned `ERROR_INVALID_FUNCTION` because
the standalone adapter stubbed `XsNetWkstaGetInfo`.  It now enters original
`ds/netapi/xactsrv/apiwksta.c`, with its exact selected XACTSRV/RAP descriptor
helpers restored under `opennt-host`; no local provider remains in that stub.
RAP remote transport remains the sole approved exclusion.

### XACTSRV function-slice admission ledger

The source owner audit is function-scoped, not a directory import:

| Selected provider | Original owner | Direct modern/public provider | Required original helper slice | Transport disposition |
| --- | --- | --- | --- | --- |
| `XsNetWkstaGetInfo`, `XsNetWkstaSetInfo` | `ds/netapi/xactsrv/apiwksta.c` | `NetWkstaGetInfo`, `NetWkstaSetInfo`, `GetSystemDirectory` | `XsCheckBufferSize`, `XsAddVarString`, `XsSetDataCount`, descriptor/word conversions from `xssubs.c` | no RAP transport |
| `XsNetUseAdd/Del/Enum/GetInfo` | `ds/netapi/xactsrv/apiuse.c` | `NetUseAdd`, `NetUseDel`, `NetUseEnum`, `NetUseGetInfo` | bounded RAP-format structure conversion/packing helpers from `xssubs.c`; this is local format adaptation, not remote RAP transport | local provider admissible; remote server input remains excluded |
| `XsNetMessageBufferSend` | `ds/netapi/xactsrv/apimsg.c` | legacy public `NetMessageBufferSend`, availability to be probed | `XsConvertTextParameter`, `XsCopyBufToTBuf`, `NetpMemoryAllocate/Free` from the already accepted XACTSRV/Netlib local closure | function-slice admitted; x86 object compiles, guest BOP `1A` proof pending |
| `XsNetServiceControl` | `ds/netapi/xactsrv/apisvc.c` | public `NetServiceControl` | exact `xlatesvc.c`, `svcinfo.c` and reached `NetpCopyStringToBuffer` slice from `packstr.c`; unrelated RPC enum allocator excluded at compile selection | function-slice admitted; x86 link and guest BOP `2E` proof pass |

`RxpTransactSmb`, `GetLanmanSessionKey`, `XsNetServerEnum2` remote browser
operations, null transaction and remote API calls remain in the RAP/private
transport cohort.  They are the only entries allowed to retain the explicit
unavailable boundary.  The present single `remote_unavailable.c` cannot own
both cohorts and must be split when the local slice is composed.

## 2026-09-17 startup-control finding

The current `O:\winnt` package was rerun after terminating every
`run16.exe`, `basesrv.exe` and `ntvdm.exe` process created by prior test
containers.  A stale `basesrv-service-reservation-test` process was also
found to have remained live since an earlier fixture run.  It could occupy
the BaseSrv endpoint, so it was terminated and is not admissible as a product
broker or acceptance witness.

With that contamination removed, both the existing real-Console observer and
the bounded ConPTY observer reproduce the same current prerequisite failure:
`ntvdm.exe` attaches to the Console and sets its normal title, but no DOS
banner or DOS command prompt becomes visible before the observer timeout.
The worker remains live while its broker remains live; when the broker is
ended, the worker exits as designed.  This is an existing COMMAND/worker
startup regression, not a VDMREDIR selector result.  The historical pipe,
transact, call, mailslot and NetAPI logs therefore remain historical only.
S20 must not claim any runtime operation-family pass until the owning
startup regression is separately re-admitted and a clean matching package is
available.

The 2026-09-17 x86 diagnostic control resolves one important ambiguity in
that observation.  It recorded the live worker image base as `00A20000`; its
only active emulation-side thread was at `00A848B6`.  The matching formal map
places relative address `000648B6` in the original
`src/mvdm/softpc.new/host/src/fprt.c` `fgets` implementation, not in
`VDMREDIR`, BaseSrv RPC, or the CCPU execution core.  A separate hidden
`COMMAND.COM /C MEM.EXE` control could not produce a usable redirected stdout
record either.  Consequently these controls demonstrate that the automated
Console injection/capture route is not yet equivalent to the owner-tested
interactive Console; they do **not** prove a product startup regression.  No
VDMREDIR source or protocol policy may be changed on this evidence alone.

## Selector ledger

| Selector | Original entry | Current caller classification | Required witness / disposition | State |
| --- | --- | --- | --- | --- |
| 00 | `VrInitialize` | `REDIR.EXE` load | Load redirector then execute one service | pass: every current guest probe |
| 01 | `VrUninitialize` | `REDIR.EXE` unload | original caller audit | no DOS caller: `segorder.inc` explicitly states that REDIR does not uninstall; the original DLL invokes it only on worker `DLL_PROCESS_DETACH` |
| 02 | `VrGetNamedPipeInfo` | DOS `5F32` | `VDMPRDR` | pass: current guest |
| 03 | `VrGetNamedPipeHandleState` | DOS `5F33` | `VDMPRDR` | pass: current guest |
| 04 | `VrSetNamedPipeHandleState` | DOS `5F34` | `VDMPRDR` | pass: current guest |
| 05 | `VrPeekNamedPipe` | DOS `5F35` | `VDMPRDR` | pass: current guest |
| 06 | `VrTransactNamedPipe` | DOS `5F36` | `VDMPTX` | pass: current guest |
| 07 | `VrCallNamedPipe` | DOS `5F37` | `VDMPCAL`, including timeout-negative case | pass: positive and no-server negative |
| 08 | `VrWaitNamedPipe` | DOS `5F38` | `VDMPRDR`, plus no-server timeout-negative case | pass: positive and no-server negative |
| 09 | `VrDeleteMailslot` | DOS `5F4E` | `VDMMAIL` | pass: current guest |
| 0A | `VrGetMailslotInfo` | DOS `5F4F` | `VDMMAIL` | pass: current guest |
| 0B | `VrMakeMailslot` | DOS `5F4D` | `VDMMAIL` | pass: current guest |
| 0C | `VrPeekMailslot` | DOS `5F51` | `VDMMAIL` | pass: original `ERROR_NOT_SUPPORTED` (50) contract |
| 0D | `VrReadMailslot` | DOS `5F50` | `VDMMAIL` | pass: current guest |
| 0E | `VrWriteMailslot` | DOS `5F52` | `VDMMAIL` | pass: current guest |
| 0F | `VrTerminateDosProcess` | TSR process termination | One DOS child leaves a mailslot open; a second recreates and removes it | pass: `VDMTRM1.COM` exits without `5F4E`; `VDMTRM2.COM` creates the same slot and deletes it; `O:\winnt\logs\t420-s20-terminate.raw` |
| 10 | `VrNetTransactApi` | original `netapis.asm` | RAP transaction | excluded: RAP; original body unconditionally reaches `RxpTransactSmb` |
| 11 | `VrNetRemoteApi` | original `netapis.asm` | RAP remote conversion | excluded: RAP |
| 12 | `VrNetNullTransactApi` | original `netapis.asm` | RAP/null-session transaction | excluded: RAP |
| 13 | `VrNetServerEnum` | original `netapis.asm`; remoted | RAP/server enumeration | excluded: RAP |
| 14 | `VrNetUseAdd` | original `netapis.asm` | local add/delete/enumerate lifecycle | pass: `VDMUSL.COM` current guest lifecycle |
| 15 | `VrNetUseDel` | original `netapis.asm` | local add/delete/enumerate lifecycle | pass: `VDMUSL.COM` current guest lifecycle and no-mapping original error |
| 16 | `VrNetUseEnum` | original `netapis.asm` | local add/delete/enumerate lifecycle | pass: `VDMUSE` current guest |
| 17 | `VrNetUseGetInfo` | original `netapis.asm` | local use-info/error mapping | pass: `VDMUSL.COM` current guest lifecycle and no-mapping original error |
| 18 | `VrNetWkstaGetInfo` | original `netapis.asm` | local workstation data copy | pass: `VDMWKSTA` current guest |
| 19 | `VrNetWkstaSetInfo` | original `netapis.asm` | local set-info/error mapping | pass: `VDMWKSS.COM` invokes `INT 21h/5F45h`, observes the original `ERROR_NOT_SUPPORTED` (`50`) policy and then completes `MEM`; runtime log `O:\winnt\logs\t420-s20-wksta-set.raw` |
| 1A | `VrNetMessageBufferSend` | original `netapis.asm` | local message/error mapping | pass: `VDMMSG.COM` invokes `INT 21h/5F40h`, receives a real mapped modern-OS Messenger failure rather than the former `ERROR_INVALID_FUNCTION`, then completes `MEM`; runtime log `O:\winnt\logs\t420-s20-message.raw` |
| 1B | `VrGetCDNames` | TSR `resident.asm` | three DOS far-pointer result targets | pass: expanded `VDMNEI.COM` selector 0 writes the computer-name target through the bounded worker binding; `O:\winnt\logs\t420-s20-net-enum-cd.raw` |
| 1C | `VrGetComputerName` | TSR `resident.asm` | `VDMNEI` guest-buffer publish | pass: current guest |
| 1D | `VrGetUserName` | TSR `resident.asm` | `VDMNETAP` and `VDMNEI` | pass: current guest |
| 1E | `VrGetDomainName` | TSR `resident.asm` | `VDMNEI` original unsupported error | pass: current guest original error |
| 1F | `VrGetLogonServer` | TSR `resident.asm` | `VDMNEI` original unsupported error | pass: current guest original error |
| 20 | `VrNetHandleGetInfo` | DOS `5F3A` | named-pipe handle fields | pass: original local-pipe error contract |
| 21 | `VrNetHandleSetInfo` | DOS `5F3B` | named-pipe handle fields | pass: original local-pipe error contract |
| 22 | `VrNetGetDCName` | original NetAPI guest path | original caller audit | no selected DOS caller: `SVC_RDRGETDCNAME` has no source use; original body returns `ERROR_NOT_SUPPORTED` |
| 23 | `VrReadWriteAsyncNmPipe` | DOS `1186/1190` | `VDMPASY`, callback plus read data | pass: current guest |
| 24 | `VrReadWriteAsyncNmPipe` | DOS `118F/1191` | `VDMPASW`, callback plus server receive | pass: current guest |
| 25 | `VrNetbios5c` | original NetBIOS callout | real non-mutating NCB enumeration | pass: `VDMNETB.COM` invokes original `INT 5Ch` `NCBENUM` and returns `00`; `O:\winnt\logs\t420-s20-netbios.raw` |
| 26 | `VrHandleAsyncCompletion` | redirector INT 5C route | `VDMNETA.COM` real async NetBIOS callback, plus 23/24 named-pipe callbacks | pass: host-LANA `NCBADDNAME | ASYNCH` accepted with `AL=00`, reaches guest post (`IF=1`) and retains successful follow-on `MEM`/COMMAND exit; cancellation observes original `NRC_CANCEL` only because the real request already completed. |
| 27 | `VrDlc5cHandler` | DLC INT 5C route | real non-mutating DIR request | owner-approved exclusion: `VDMDLC.COM` reaches the unchanged original DLC branch and returns `07` (`LLC_STATUS_COMMAND_CANCELLED_FAILURE`) because Windows 11 has no `DLCAPI.DLL`/DLC provider; `O:\winnt\logs\t420-s20-dlc-formal013-r2.raw` proves dispatch, original failure and subsequent `MEM`/COMMAND cleanup. |
| 28 | `VrVdmWindowInit` | DLC/window initialization | source-backed selected caller disposition | no selected caller: only `SVC_VDM_WINDOW_INIT` use in `redir.asm` is commented-out historical code |
| 29 | `VrReturnAssignMode` | TSR assignment API | original guest call/result contract | source-complete: original host body is intentionally empty; DOS TSR retains its local redirection mode state |
| 2A | `VrSetAssignMode` | TSR assignment API | original guest call/result contract | source-complete: original host body is intentionally empty; DOS TSR retains its local redirection mode state |
| 2B | `VrGetAssignListEntry` | TSR `resident.asm` | original guest call/result contract | pass: `VDMASGN.COM` current guest lifecycle |
| 2C | `VrDefineMacro` | TSR `resident.asm` | define/break lifecycle | pass: `VDMASGN.COM` maps `Z:` to local `\\\\LOCALHOST\\O$`, retains user word `BEEF`, then queries it |
| 2D | `VrBreakMacro` | TSR `resident.asm` | define/break lifecycle | pass: `VDMASGN.COM` removes the mapping; host `net use Z:` confirms no residual connection; `O:\winnt\logs\t420-s20-assign-r4.raw` |
| 2E | `VrNetServiceControl` | original `netapis.asm` | local service-control/error mapping | pass: `VDMSVC.COM` invokes original `INT 21h/5F42h` interrogate contract, receives a real result rather than `ERROR_INVALID_FUNCTION`, then completes `MEM`; runtime log `O:\winnt\logs\t420-s20-service.raw` |
| 2F | `VrDismissInterrupt` | redirector INT 5C acknowledge | source-backed caller disposition | no selected SVC caller; the only in-tree direct call in `vrnmpipe.c` is commented out |
| 30 | `VrEoiAndDismissInterrupt` | original DOS `int5c.asm` and protected-mode `dxnetbio.asm` acknowledge | post-callback interrupt acknowledgement | pass for selected real-mode caller: `int5c.asm` unconditionally issues `SVC_RDRINTACK2` after the real `VDMNETA.COM` callback; `MEM` and COMMAND exit subsequently succeed. The prior no-caller classification was false. |
| 31 | `VrCheckPmNetbiosAnr` | protected-mode NetBIOS path | source-backed caller disposition | no selected SVC caller; it is a protected-mode WOW helper and has no DOS direct caller |

## Cancellation boundary

`VrCancelPipeIo` is not a BOP 7 selector.  Its original reachable call in the
selected source tree is `src/mvdm/wow32/wkman.c`, where WOW task termination
cancels requests owned by that WOW thread.  There is no DOS TSR call.  Until
WOW32 is admitted and its task manager path becomes selected, this is a
source-backed no-current-caller disposition, not a reason to invent a DOS
cancel API.  The independent target must still retain the original cancellation
body unchanged and its compile/link proof.
