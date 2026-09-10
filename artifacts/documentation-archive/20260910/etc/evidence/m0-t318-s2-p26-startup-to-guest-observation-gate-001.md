# M0 T318 S2 P26 — Startup-to-guest observation gate

## Question

Which original boundary can distinguish a live host startup from actual guest
progress, without changing the fixed non-debug observation container or
inventing a loader, command dispatcher, or BOP provider?

## Source-shaped result

The selected source order is materially narrower than a repeated whole-startup
experiment:

```text
host_main
  -> InitialiseDosEmulation
     -> scs_init
        -> GetNextVDMCommand(NULL)       [first-VDM query only]
        -> CMDInit / DemInit / XMSInit / DBGInit
     -> original NTIO.SYS file read and CS:IP assignment
  -> host_start_cpu -> cpu_simulate
  -> guest NTDOS/COMMAND
     -> BOP 54:01 -> MS_bop_4 -> CmdDispatch -> cmdGetNextCmd
        -> GetNextVDMCommand(&VDMInfo)  [ordinary-child consumption]
```

Evidence for each transition:

- `nt_msscs.c::InitialiseDosEmulation` calls `scs_init` before its unchanged
  `ntio.sys` lookup/read/SAS/CS:IP sequence. Its direct terminal branches in
  this cohort are limited to original failed initializers and the NTIO file
  path; P10 already proved that the selected profile reaches the latter.
- `nt_msscs.c::scs_init` calls `GetNextVDMCommand(NULL)` only to obtain the
  original first-VDM Boolean. The source does not use that call to copy the
  declared child command. It then calls `CMDInit`, `DemInit`, `XMSInit`, and
  `DBGInit` in that exact order.
- `dos/command/cmd.c::CMDInit` only obtains its home-drive information. It
  does not retrieve a command.
- The original guest invokes `cmdGetNextCmd` through COMMAND selector `54h`,
  service `01h`. `cmdGetNextCmd` constructs its caller-owned `VDMINFO` and
  calls the historical `GetNextVDMCommand(&VDMInfo)`; this is where the
  admitted `--ordinary-child` record is actually consumed.
- The original host BOP chain is retained in `nt_bop.c::MS_bop_0`, which reads
  the selector service byte, invokes `DemDispatch`, then advances IP. The
  parallel COMMAND owner table is `cmddisp.c::CmdDispatch`. Neither route is
  an app-owned replacement lifecycle.

## Disposition

Console/display, event-thread setup, BaseVDM first-VDM admission, and CCPU
entry are no longer valid candidates for another undifferentiated startup
experiment: P25's immutable container survives the entire eight-second bound,
and P10 already proved original NTIO input sensitivity.

The next permitted runtime evidence is a **passive, original-dispatch reach
observation** for the selector/service boundary. It may record the existing
original dispatch input and resulting return only. It must not recognise a new
service, change a register, change IP advancement, provide a BOP body, alter
the fixed container, or alter media. Its first required milestone is reaching
the original COMMAND `54:01` path; only then may the declared child profile be
evaluated for NTDOS `EXEC` and parent return.

