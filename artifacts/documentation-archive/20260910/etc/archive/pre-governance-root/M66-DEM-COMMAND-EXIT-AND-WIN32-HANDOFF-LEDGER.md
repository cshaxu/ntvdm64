# M66 DEM/COMMAND Exit and Win32 Handoff Ledger

Status: source-derived normal termination and host-program handoff record,
2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/dem/demsrch.c`
- `src/opennt/base/mvdm/dos/command/cmddisp.c`
- `src/opennt/base/mvdm/dos/command/cmdexit.c`
- `src/opennt/base/mvdm/dos/command/cmdexec.c`
- `src/opennt/base/mvdm/dos/command/cmdmisc.c`
- `src/opennt/base/mvdm/dos/v86/cmd/command/tcode.asm`

## DOS Program Termination Notification

`SVC_PDBTERMINATE` (`3Ch`) enters `demTerminatePDB` with terminating PSP in
BX. It does not set a result register. Its historical work is:

1. notify the VDD user hook unless this is the first call;
2. call `HostTerminatePDB(PSP)`;
3. find, free, and remove that PSP's cached FIND-FIRST/FIND-NEXT state.

This service establishes that process exit has session-owned search and host
notification cleanup. The first contained runner needs equivalent lifecycle
cleanup for its own opaque per-PSP/search state, but no VDD hook or historical
host process side effect.

## COMMAND SVC Exit Surface

The command dispatcher reserves `SVC_CMDEXITVDM` (`00h`) and
`SVC_RETURNEXITCODE` (`0Bh`). `cmdExitVDM` simply calls `TerminateVDM`; it is
the same historical product-wide terminal path classified in M65, not the
normal bounded-command result.

`cmdReturnExitCode` is the important normal return path:

| Input | Historical role |
| --- | --- |
| DX | DOS program exit code. |
| AL | current DOS drive. |
| BX:CX | redirector completion-info guest address. |

It packages DX into `VDMINFO.ErrorCode`, updates inherited current directories,
performs redirector copy completion, then calls `GetNextVDMCommand`. If that
broker provides another command, CF sets and COMMAND re-enters DOS execution.
Otherwise CF clears, AL receives the low byte of `dwExitCode32`, and the
historical event thread/standard-handle notification are resumed.

The guest `tcode.asm` caller loads DX from its saved `retcode`, obtains current
drive through INT 21h/AH=19h, passes redirector info in BX:CX, emits the
COMMAND SVC, then clears its saved return code. This confirms DX is a real
guest-to-host command return value, not merely a debug trace.

## Historical Win32 Handoff

`cmdExec32` and `cmdCreateProcess` demonstrate the original product's direct
Win32 program path:

1. transform DOS/OEM command and environment data;
2. substitute the process standard handles with 16-bit values;
3. call `CreateProcess` suspended with inherited handles and a chosen current
   directory;
4. resume and wait for the process; store either `GetLastError` or its exit
   code in `dwExitCode32`; then restore standard handles; and
5. coordinate process reentry through `GetNextVDMCommand`, event-thread block
   state, and the historical VDM command broker.

Thus a direct host transfer is historically real, including host 32-bit
process execution. The source does not establish a separate 64-bit protocol:
on a modern x64 host the contained runner's normal `CreateProcess` policy can
delegate compatible 32- or 64-bit host executables, but that is a new host
capability decision, not an NT4 binary behavior claim.

## Contained CLI Boundary

The first research CLI needs a narrow command broker replacement with only:

- a bounded request to launch an authorized host executable;
- explicit inherited stream policy, working directory from the contained DOS
  namespace, and transformed environment ownership;
- wait/cancel/result collection; and
- one controlled re-entry or final exit-result decision.

It must not reuse `GetNextVDMCommand`, BaseSrv/CSR state, the historical event
thread, ambient process standard-handle mutation, or VDD lifecycle hooks. A
future adapter must translate the observed COMMAND CF/AL convention and DOS DX
return code through a validated backend result path. It must not let a guest
service call `ExitProcess` or the old VDM broker.

No command broker or host-process launch path was implemented. This ledger is
the source basis for the future non-invasive CLI handoff design.
