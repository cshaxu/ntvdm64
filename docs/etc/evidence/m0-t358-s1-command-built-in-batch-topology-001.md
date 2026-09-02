# M0 T358 S1 — COMMAND built-in and batch topology

## Selected built-in workload

The immutable workload is the app declaration `--command EXIT`.  App retains
the original Base-VDM shape:

```text
application = <mvdm-root>\system32\COMMAND.COM
command     = /C EXIT\r\n
environment = COMSPEC/PATH record
```

`src/mvdm-host/dos/command/cmddisp.c` selects original `cmdGetNextCmd` for
`SVC_CMDGETNEXTCMD`.  Its original body in `cmdmisc.c` forms `VDMINFO`, calls
the Base-VDM `GetNextVDMCommand` binding and writes the DOS-binary command
record.  In the immutable guest, `tcode.asm` issues `CMDSVC
SVC_CMDGETNEXTCMD`; clear carry takes `run_cmd`.

The guest's `/C` parser sets `SINGLECOM`.  Its normal single-command terminal
is guest `$EXIT` in `tcmd2b.asm`, which calls `SVC_CMDEXITVDM` and never
returns.  The original host table maps that service to `cmdExitVDM` in
`cmdexit.c`, which calls `TerminateVDM`.  The existing same-shaped SoftPC
termination/session binding turns that into a current-session controlled stop
instead of terminating the host process.

Thus the expected proof is a source-owned `54:01` command delivery followed
by `54:00` and a controlled product return.  It neither needs NTDOS `$Exec`
nor `cmdExec32`.

## Formal owner check

The formal CPU40 manifest selects the complete original COMMAND cohort
(`cmd.c`, `cmddata.c`, `cmddisp.c`, `cmdexec.c`, `cmdexit.c`, `cmdmisc.c`,
`cmdpif.c`, `cmdredir.c`, `cmdconf.c`, `cmdkeyb.c`, `cmdenv.c`) as
`original-mvdm-command.lib`, along with Base-VDM and SoftPC termination
bindings.  No immediate original owner is absent from the formal graph for
the selected `EXIT` route.  Consequently T358 S2 has no recovery work.

## Batch disposition

The staged immutable `autoexec.nt` is the only selected batch source.  It
contains `lh ...mscdexnt.exe`, `lh ...redir` and `lh ...dosx`.  Original
guest `tbatch.asm` opens/reads batch text through DOS and dispatches each
line; those program lines require external executable resolution and NTDOS
`$Exec`.  They are therefore not a COMMAND-only batch workload.

T358 does not run or claim this batch.  Its exact owner handoff is M0 T359,
DOS `.COM`/MZ `.EXE` execution and parent return.  This is a dependency
boundary, not a missing COMMAND implementation.

