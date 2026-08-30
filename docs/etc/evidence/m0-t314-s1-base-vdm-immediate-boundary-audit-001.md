# M0 T314 S1 — Base VDM immediate-boundary audit

## Scope and inputs

This audit covers only the original Base VDM client/server protocol that is
immediately called by selected MVDM host roots.  Its authoritative original
sources are:

- `src/opennt-host/base/win32/client/vdm.c`
- `src/opennt-host/base/win32/server/srvvdm.c`
- `src/opennt-host/base/win32/{inc/basevdm.h,server/srvvdm.h}`
- `src/opennt-abi/source/public/internal/base/inc/vdmapi.h`

The selected MVDM callers are `softpc.new/host/src/{nt_msscs.c,nt_term.c,
nt_event.c,config.c}`, `dos/command/{cmdmisc.c,cmdexec.c}`, and the later
WOW owner `wow32/wkman.c`.  They use the public `VDMINFO` ABI; they do not
call a project command-list ABI.

## Original protocol and boundary

`GetNextVDMCommand` is split across two original product owners:

1. BaseClient snapshots the caller's `VDMINFO`, allocates a CSR capture
   buffer, sends `BasepGetNextVDMCommand`, copies requested-size results on
   `STATUS_INVALID_PARAMETER`, waits on `WaitObjectForVDM`, sets
   `ASKING_FOR_SECOND_TIME`, and finally copies the returned command record.
2. BaseSrv owns console/DOS/WOW records, parent wait pairs, command state,
   first-VDM state and command completion.  In the DOS branch it returns an
   existing command, returns required field sizes without consuming it,
   publishes a wait object when no command exists, and accepts the second
   request after that object is signalled.

The CSR capture buffer, `CsrClientCallServer`, server `PCSR_API_MSG`, console
record lookup, duplicated remote handles, process locks and WOWEXEC window
notification are private NT4 product interfaces.  They are a hard boundary;
they cannot be linked or recreated as a modern public-API dependency.

## Per-operation disposition

| Original public operation | Required local observable contract | Current state | T314 disposition |
| --- | --- | --- | --- |
| `GetNextVDMCommand(NULL)` / `BaseSrvIsFirstVDM` | Bound-session query-and-clear first-VDM state. | Present in `base_vdm_local`. | Retain; local record is source-shaped. |
| `GetNextVDMCommand` reentry count | Increment/decrement without consuming a command; reject underflow. | Present. | Retain and cover with the broker matrix. |
| `GetNextVDMCommand` DOS record/capacity/environment copy | Caller-owned buffers, required-size result before consumption, environment-only copy without consumption. | Present for DOS command/app/environment/current directory. | Retain; compare all fields against the original record during S2. |
| No-command wait/wake/second request | Pending local request, producer wake, second request with `ASKING_FOR_SECOND_TIME`, terminal `RETURN_ON_NO_COMMAND` result. | Missing: current adapter returns an immediate unavailable/no-memory surrogate and has no pending state or wake capability. | Recover in S2 as a single session-owned DOS record and event/notification edge. |
| Parent completion/error route | Original server marks the returned DOS record, signals the parent wait pair and clears its duplicate handle. | Missing. | Recover only the local completion/reentry record in S2; child process/Redirector handling remains the later DEM/COMMAND package. |
| `ExitVDM(FALSE, ...)` | Complete only the bound DOS session, never exit the host process. | Present through session completion. | Retain; connect it to the S2 pending-record teardown. |
| `Set/GetVDMCurrentDirectories` | Copy a per-console MULTI_SZ, return required size on undersized caller buffer, then free after successful get. | Present for one bound session. | Retain; regression under pending/teardown in S2. |
| `CmdBatNotification` | Console-scoped BAT lifecycle flag used for returned command metadata. | No selected MVDM caller or local implementation. | Defer to DEM/COMMAND vertical slice; do not invent a global flag. |
| `VDMOperationStarted`, `BaseCheckVDM`, `BaseSrvCheckVDM` | Process creation/registration protocol and original VDM command publication. | Not selected; depends on BaseSrv console/process records. | Later broker/process and DEM/COMMAND owner packages. |
| PIF, startup-info, title/desktop/reserved, standard-handle duplication | Full BaseSrv command record and console/process resource semantics. | Deliberately absent. | Child lifecycle, Redirector and VDD/WOW owner packages; no placeholder resources. |
| `RegisterWowExec`, WOW command queue and `ExitVDM(TRUE, ...)` | Shared-WOW nonblocking message notification and task lifecycle. | Deliberately unavailable. | WOW32/Win16 and broker packages. |
| CSR transport, cross-process console discovery and duplicated wait handles | NT4 BaseClient/BaseSrv product shell. | Not composable. | Explicit hard boundary; later broker may use public IPC but never CSR recreation. |

## Existing local implementation

`adapter-mvdm-host-out/basesrv/{source/base_vdm_client.c,
source/base_vdm_local.c}` is not a competing MVDM provider: it preserves the
original public symbols and operates on a caller-local `VDMINFO` snapshot.
However it is only a partial source-shaped BaseSrv record today.  The
single-slot copied command and one-shot current-directory ownership are
valid starting points.  Its missing pending/wake/completion state means it
cannot yet claim the original normal `GetNextVDMCommand` control protocol.

The existing focused Win32/x86 fixture was rebuilt after restoring its missing
`opennt-host/public/sdk/inc` include root in
`tools/build/New-T307BaseVdmBrokerNinja.ps1`:

```text
build/M0-T307/S1/x86
8/8 edges; PASS: local Base VDM broker contract
```

This test proves only the current first-VDM, buffer sizing, environment,
reentry, directory and session-terminal subset.  It does not prove wait/wake,
parent completion, child lifecycle, WOW or CSR transport.

## S2 recovery boundary

S2 may extend the existing one-session copied record with one typed pending
request and one public local notification/event mechanism.  It must retain
the original public `VDMINFO` and `GetNextVDMCommand` spellings, caller-owned
copy semantics, required-size-before-consumption behavior and second-request
ordering.  It may not retain caller pointers, native handles or guest pointers
after a call, and may not create a CSRSS, a generic command queue, a WOW
broker or a second session/machine executor.

The S2 completion record is separate from child completion: it may preserve
the BaseSrv DOS record's local state transition and parent-notification
contract only where the original immediate callers require it.  Redirector
handles, child-process execution and PSP return stay owned by later packages.
