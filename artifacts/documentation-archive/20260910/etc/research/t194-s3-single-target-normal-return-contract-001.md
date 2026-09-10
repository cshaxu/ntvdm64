# T194 S3 single-target normal-return lifecycle contract 001

Date: 2026-08-12  
Packet: M0 T194 S3  
Disposition: source/ABI contract complete; no implementation yet.

## Original OpenNT contract

`cmddisp.c` maps `54:01` to `cmdGetNextCmd` and `54:11` to
`cmdReturnExitCode`.  The original first handler copies the `CMDINFO` address
from `DS:DX`, obtains one VDM command, writes command/environment/executable
fields, and clears CF.  It uses CF set only for the documented environment
resize retry.

`cmdReturnExitCode` receives the guest DOS status in `DX`, current drive in
`AL`, and redirector information in `BX:CX`.  It asks the historical command
source for a next command.  If one is available it sets CF for re-entry; if no
command is available it clears CF and resumes the COMMAND continuation.  Its
event-thread, directory, redirection and Win32 process work is historical
host composition and is intentionally not callable by the CLI adapter.

## Contained single-target mapping

The CLI-profile equivalent may represent precisely one immutable declared
target and its source-shaped no-next outcome:

| Lifecycle state | Accepted BOP | Result |
| --- | --- | --- |
| `undelivered` | `54:01` with validated copied `CMDINFO` | Write only the existing fixed CMDINFO response for `TARGET`; clear CF; become `target-delivered`. |
| `target-delivered` | `54:11` | Copy the low 16 bits of DX only into process-local state; clear CF; resume by four BOP bytes; become `returned`. |
| `returned` | any `54:01`/`54:11` | Decline/pass through; no queue, no replay and no terminal action. |

The existing fixed-width gather/multi-write transaction remains the only
guest-memory transport.  The adapter does not read arbitrary guest pointers,
launch a host process, provide an ambient command queue, import historical
thread/directory/redirection policy, or turn the copied DX field into a CLI
exit code.  A profile is explicit: the old two-slot `TARGET` then `QUIT.COM`
mode remains unchanged unless a separately encoded single-target mode is
selected.

## Required implementation boundary

One coherent S4 change may add a versioned profile launch-mode discriminator
and update the COMMAND launch state/provider as one unit.  It must prove:

1. legacy two-slot profiles retain exact `TARGET -> QUIT` behavior;
2. a single-target profile admits exactly one `54:01`, then accepts only the
   source-shaped `54:11` no-next path;
3. an early `54:11`, second `54:01`, or repeated `54:11` is not accepted;
4. no process/CLI result or machine/Bochs behavior changes.

No engine build or guest observation is admitted until that focused source and
negative-test closure exists.
