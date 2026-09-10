# T172 S1 declared DOS guest-target lifecycle ABI audit 001

## Original guest sequence

In `dos/v86/cmd/command/tcode.asm`, COMMAND prepares `UCOMBUF`, `EXECPATH`,
environment and redirection fields, then calls `CMDSVC SVC_CMDGETNEXTCMD`
(`54:01`) with `DS:DX` pointing at its `CMDINFO`. Carry means that the guest
environment buffer must grow; clear carry enters `run_cmd`.

After a DOS child returns, the normal `retcode32` and `d16_return32` paths put
the DOS return code in `DX`, the current drive in `AL`, redirection completion
information in `BX:CX`, then call `CMDSVC SVC_RETURNEXITCODE` (`54:11`). Carry
set means the historical host command broker supplied another command and
COMMAND re-enters; clear carry means the shell-out session is over. This is
not the same event as `BOP_UNSIMULATE` (`FE`).

## Existing contained launch half

`bx_ntvdm_cmd_get_next_service.c` already reconstructs the safe `54:01`
subset as a copied transaction:

- accepts only real-mode `C4 C4 54 01` with a bounded `DS:DX` CMDINFO range;
- reads the fixed CMDINFO bytes, writes a finite `TARGET.COM`/`TARGET.EXE`
  path and `TARGET [tail]` command into caller-owned guest buffers;
- bounds tail bytes to 118, as imposed by COMMAND's 128-byte UCOMBUF;
- writes only declared profile drive metadata and the registered COMMAND
  fields, then resumes four bytes with CF clear.

The CLI launch declaration is correspondingly fixed-width: target kind plus
ASCII tail, no host path, handle, guest pointer, Bochs object or selector. It
matches the source-level 54:01 start ABI closely enough to be the correct
component boundary; it is not host `cmdExec`.

## Missing return half and disposition

No current provider owns `54:11`. The original handler calls
`GetNextVDMCommand`, redirection copying, directory synchronization and
event-thread controls; that composition cannot be linked. A contained CLI
replacement must instead declare one of two results after a guest DOS child:

| Result | Required guest-visible `54:11` outcome |
| --- | --- |
| no next declared target | CF clear; preserve the bounded DOS exit value under a separately recorded CLI result contract |
| next declared target exists | CF set only after a separately admitted finite next-target envelope; no broker, host process or ambient queue |

The current profile permits exactly one target, so only the first row can be
admitted initially. Its exact register/stack/redirection-memory details still
need a source-derived return ABI record before implementation. `FE` remains a
distinct terminal action and must not be used as normal-return emulation.

## Outcome

The lifecycle component is feasible as one paired `54:01`/`54:11` provider
family. The launch half is already present and source-shaped; T173 must audit
the original `54:11` register/memory result and contained no-next-target error
contract before a paired completion is implemented.
