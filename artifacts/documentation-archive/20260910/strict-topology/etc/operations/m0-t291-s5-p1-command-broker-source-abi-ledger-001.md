# M0 T291 S5 P1 — Command broker source/ABI ledger

## Original API evidence

`mvdm-platform-abi/source/public/internal/base/inc/vdmapi.h` preserves the
public `GetNextVDMCommand(PVDMINFO)` spelling, `VDMINFO` layout and the
request-state bitfield. The same header states that BaseSrv notifies shared
WOWEXEC with `WM_WOWEXECSTARTAPP`, specifically to avoid a permanently blocked
WOW thread. The selected MVDM mirror contains no BaseSrv server body. This was
initially used as a source-supply boundary, but P2 corrected that scope: the
complete approved OpenNT source contains the original server and client
package. This ledger remains the caller-form input; it no longer authorizes a
replacement queue.

## Reached consumer forms

| ID | Original callers | `VDMState` / result contract | Current state | S5 disposition |
| --- | --- | --- | --- | --- |
| CB-01 | `v86/scaffold/softpc.c`, `softpc.new/host/src/nt_msscs.c` | `NULL` first-command/startup query, paired with `VDMOperationStarted`. | Unavailable. | Keep unavailable: historical V86 startup shell is not host runtime; S5 does not invent first-VDM discovery. |
| CB-02 | `dos/command/cmdmisc.c` | `ASKING_FOR_DOS_BINARY`, optionally `ASKING_FOR_ENVIRONMENT`/`ASKING_FOR_SECOND_TIME`; insufficient buffers return required sizes for retry. | Partial immediate app provider. | Recover copied payload/capacity exchange first. |
| CB-03 | `dos/command/cmdexec.c` | `INCREMENT_REENTER_COUNT` / `DECREMENT_REENTER_COUNT` around 32-bit child lifecycle. | Present in app provider. | Retain exact balance and add queued producer proof. |
| CB-04 | `dos/command/cmdexec.c`, `cmdReturnExitCode` | `NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND` and `RETURN_ON_NO_COMMAND`; source chooses re-enter vs carry-clear exit by returned `CmdSize`. | Partial: no-command response is not source-shaped for all forms. | Recover immediate no-command success and queued wake decision before a child/WOW provider is enabled. |
| CB-05 | `softpc.new/host/src/config.c` | `ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY`, fixed request buffers and source PIF fallback. | Explicit unavailable. | Preserve unavailable until original PIF package is admitted; no generic payload reinterpretation. |
| CB-06 | `wow32/wkman.c` | `ASKING_FOR_WOW_BINARY` or `ASKING_FOR_SEPWOW_BINARY`; no command is successful `CmdSize == 0`; capacity retry includes the WOW environment doubling rule; shared WOW receives BaseSrv notification. | Explicit unavailable. | Recover only after a typed WOW consumer owns its copied 16-bit frame/capacity bridge. S5 may provide broker notification record, not WOW execution. |
| CB-07 | `wow32/wkman.c` | A shared WOW skips a `wowexec.exe` payload through `ExitVDM` and asks again. | Unavailable. | Defer to the source-shaped WOW lifecycle group; S5 must not implement `ExitVDM` as a generic command queue operation. |

## Superseded required boundary

The former boundary that assigned a copied queue to `broker` is withdrawn.
P2 first imports and audits the original BaseSrv/client package; only an
identified unavailable BaseSrv/CSR operation may then receive a same-shaped
adapter or bounded broker seam.

## Superseded P1 decision

The former proposal to begin CB-02/CB-04 with a copied bounded queue is
withdrawn. It made an incorrect source-supply inference from the selected
MVDM mirror. CB-02 through CB-07 remain source-owned; their runtime admission
is not implied until the original BaseSrv/client package has been imported and
its private dependencies have been mapped.
