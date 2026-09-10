# M0 T317 S3 P3 — COMMAND exit and redirection disposition

## Reached original paths

The selected `mvdm-host/dos/command/cmdexec.c` keeps the original
`cmdReturnExitCode` sequence:

1. copy the DOS exit code and current drive into `VDMINFO`;
2. block the original host event thread;
3. update current directories;
4. resolve the optional `REDIRCOMPLETE_INFO` register-pair identity;
5. run original `cmdCheckCopyForRedirection`;
6. call `GetNextVDMCommand(RETURN_ON_NO_COMMAND)`; and
7. set CF for a returned command, or restore AL/event/standard-handle state
   for the local no-command outcome.

The current command-redirection binding resolves the optional record only
through the session host-resource mapping manager. It rejects an unknown
register-pair identity with `ERROR_INVALID_HANDLE`; no guest-visible field
ever carries a native record pointer or `HANDLE`.

`cmdexit.c` is unchanged original source: `cmdExitVDM` calls
`TerminateVDM`. The selected original SoftPC termination path preserves its
cleanup order and hands the VDM outcome to the bound session rather than
terminating the CLI process.

## Explicit transfers

- A non-empty next Base VDM command is an original broker/guest re-entry
  outcome, not a COMMAND-private queue. The local Base VDM protocol has the
  required copied request, wake and retry form, but actual DOS `EXEC`/PSP
  parent restoration belongs to the queued minimal DOS guest-execution
  package.
- `cmdpif.c` remains formally composed original source. Its PIF UI, ForceDos
  and console-product branches require the named PIF/console owner and are not
  represented as a local success path.
- Remote/named-pipe Redirection and WOW ownership remain their respective
  later packages.

## Verification

- `New-T290S3CmdRedirNinja.ps1 -Architecture x86`, followed by its generated
  Ninja build and `t290-s3-cmdredir-fixture.exe`, completed with
  `PASS: original COMMAND redirection mapping contract`.
- The fresh `build/M0-T317/S3/command-child-r2` formal x86 CCPU40 graph
  compiled all four original S3 units—`cmdexec.c`, `cmdexit.c`, `cmdredir.c`,
  and `cmdpif.c`—into `original-mvdm-command.lib`.
- P1 and P2 provide the complementary child-only standard-stream and detached
  worker/Base VDM re-entry checks.

## Result

T317/S3 is closed as a local source-shaped COMMAND child/stream cohort. It
does not assert guest-call execution, DOS parent return, PIF product behavior,
CSRSS/BaseSrv transport, or remote Redirector behavior.
