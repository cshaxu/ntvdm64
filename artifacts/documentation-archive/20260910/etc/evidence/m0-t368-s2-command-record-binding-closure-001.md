# M0 T368 S2 — COMMAND record-binding closure

## Decision

S1 found no missing COMMAND host implementation between a successful DOS
`54:01` response and the original guest EXEC transaction.  This S therefore
closes the already-present source-shaped cohort without a code change.  A
new parser, DOS loader, no-next synthetic result, or BOP leaf would violate
the owner and failure boundaries established by the source ledger.

## Binding closure

- `src/mvdm-host/dos/command/cmddisp.c` and `cmdmisc.c` are inputs of
  `original-mvdm-command.lib` in the formal x86 graph.
- `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c` is an input of
  `basesrv-bindings.lib` in the same graph.
- `original-softpc-process.exe` links both libraries.  The product link line
  retains the original COMMAND library and BaseSrv/session binding library as
  explicit inputs; no app command interpreter or host DOS loader is linked.
- `src/app/launch_declaration.c` publishes exactly one DOS-owned record:
  `BASE_VDM_COMMAND_DOS`, an application carrier rooted at
  `system32\\COMMAND.COM`, and an original-shaped `/C <declared command>\r\n`
  command line.  The Base VDM record carries `AppName`, `CmdLine`,
  environment, current drive and directory independently, as required by
  original `cmdGetNextCmd`.
- `base_vdm_local.c` uses the same DOS/WOW ownership discriminant before
  copying `VDMINFO`.  It does not deliver a WOW record to COMMAND, and it
  treats an empty DOS queue as pending except for the source-defined
  `RETURN_ON_NO_COMMAND` terminal cases.

## ABI disposition

| Contract | Current disposition |
| --- | --- |
| `CMDINFO` guest structure | Original host layout and original guest offsets; bounded session leases replace only durable process aliases. |
| `VDMINFO` response | Original field/capacity/copy ordering retained by `cmdGetNextCmd` and the source-derived BaseSrv record seam. |
| Command tail | `/C <declared command>\r\n` plus transport NUL; host `cmdGetNextCmd` retains its CR/LF assertion. |
| External DOS program handoff | Original guest `EXECPATH_SIZE` / `EXECEXT_TYPE` one-shot route into `tmisc1.asm:EXECUTE` and the resident INT 21h EXEC transaction. |
| No-next command | Original-shaped pending/terminal distinction only; no synthetic record. |

## Closure and next boundary

This closes the earliest complete COMMAND cohort as a **no-code source/ABI
closure**.  It does not establish that the selected product reaches the guest
transient parser or the EXEC trap at runtime.  S3 must formally rebuild and
verify the selected final inputs; S4 may make one fixed observation and must
classify any earliest owner reached without introducing a trace-selected
repair.
