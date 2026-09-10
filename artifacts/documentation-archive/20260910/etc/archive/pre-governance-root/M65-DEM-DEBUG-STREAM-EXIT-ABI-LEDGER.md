# M65 DEM Debug Stream and Exit ABI Ledger

Status: source-derived debug-channel and termination record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/dem/demmisc.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_reset.c`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_term.c`
- `src/opennt/base/mvdm/inc/dossvc.h`

## SVC `39h` and `3Ah` Are Debug Channels

The names `SVC_OUTPUT_STRING` and `SVC_INPUT_STRING` do not designate the DOS
standard-stream implementation.

| SVC | Handler | Inputs | Actual action |
| --- | --- | --- | --- |
| `39h` | `demOutputString` | DS:SI guest pointer, BX length, PE-state flag | If `IsDebuggee()` is false: no-op. Otherwise obtains a monitor mapping with `Sim32GetVDMPointer` and calls `OutputDebugStringOem`. |
| `3Ah` | `demInputString` | DS:DI guest pointer, BX length, PE-state flag | If `IsDebuggee()` is false: no-op. Otherwise obtains a monitor mapping and calls `DbgPrompt` with a fixed 128-byte input limit. |

Neither handler sets documented success/error registers, owns normal console
I/O, or establishes a terminal capability. Both use `ISPESET` when requesting
their guest mapping, so their exact segment/mode behavior remains a monitor
ABI issue. They belong to a debugger tier and must not be used as evidence
that the first CLI command runner needs a historical debugger or that it may
expose arbitrary guest-memory pointers to host streams.

## SVC `3Dh`: demExitVDM

The comment declares `DS:SI` as a message string. The fixed implementation
does not read any guest register or memory. It instead calls:

```text
RcErrorDialogBox(ED_BADSYSFILE, "config.nt", NULL)
TerminateVDM()
```

Consequently the message-pointer contract is stale documentation for this
source revision. A locked profile must follow observed code, not that comment.

`TerminateVDM` delegates to the SoftPC `terminate()` shutdown path. The
historical host termination owners then perform graphics, printer, serial,
mouse, configuration-file, VDM broker, and process cleanup before calling
`ExitProcess`; WOW additionally uses `ExitVDM` with a different exit value.
This is historical NT product integration, not an admissible dependency of the
research CLI.

## Contained Runner Consequences

1. A normal command's stdin/stdout/stderr behavior must come from file/device
   and command-service evidence, not `39h`/`3Ah`.
2. A bounded runner can provide a debug trace sink behind an explicit
   debug-only capability, but must not treat it as guest console transport.
3. A future `3Dh` adapter must translate the historical terminal request to a
   backend-owned finite stop/result. It must not display an old configuration
   dialog, call `ExitVDM`, mutate configuration files, or terminate the host
   process from inside a service callback.
4. The exit code and normal COMMAND termination path remain unproven here and
   must be obtained from `demTerminatePDB`, COMMAND dispatch, and the command
   initialization/exit sources.

No debug stream or exit handler was implemented. This record prevents
accidentally elevating debugger-only services to the CLI product boundary.
