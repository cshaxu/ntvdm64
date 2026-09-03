# M0 T386 S3 — original COMMAND post-environment continuation disposition

## Question

Does the selected CPU40/x86 product prove that immutable `COMMAND.COM`
executes its declared `/C` built-in command after the original BaseClient
environment projection removed the initial environment-size retry?

## Source contract

The answer is not inferred from a BOP selector alone.

- Original `base/win32/client/vdm.c::BaseCheckVDM` converts the caller's
  command tail to OEM, appends `CR`, `LF`, and `NUL`, and gives that byte
  sequence and the separate application path to BaseSrv.  The current
  app-owned declaration retains exactly that `CmdLine` shape; it does not
  parse the DOS command.
- Original `base/win32/server/srvvdm.c::BaseSrvGetNextVDMCommand` copies the
  returned `VDMINFO` into a DOS record and marks that record `VDM_BUSY`.
  The selected one-session binding retains this DOS-only record transition.
- Original `dos/command/cmdmisc.c::cmdGetNextCmd` returns to immutable
  `COMMAND.COM` only after it has copied the returned command record into
  `CMDINFO`/`UCOMBUF` and cleared carry.  A later `54:01` is therefore a
  continuation after the first record, but by itself does not prove that the
  guest command had the intended observable effect.

## Fixed observation

The fresh formal `original-softpc-process.exe` was staged in the fixed
`O:\ntvdm64` package container and invoked through the console-owning,
non-debug observer with the sole declared command:

```text
echo M0T386 > O:\ntvdm64\m0-t386-command-marker.txt
```

The marker name was absent before the run and remains absent after it.  The
durable report is
`artifacts/research/m0-t386-s3-command-marker.txt` and its continuation
sidecar is `artifacts/research/m0-t386-s3-command-marker.txt.command.txt`.

The sidecar records:

```text
54:01 stage=0 first=1 dos-state=TO_TAKE_A_COMMAND
54:01 stage=1 first=0 dos-state=BUSY CF=0
54:01 stage=0 first=0 dos-state=BUSY
```

## Follow-up scalar result observation

The same fixed container was then observed with the default-off scalar-only
observer at the existing original `cmdGetNextCmd -> GetNextVDMCommand`
boundary.  It neither reads command text nor retains a guest/host pointer and
does not alter the guest request or result.  The durable sidecar is
`artifacts/research/m0-t386-s3-command-marker-vdminfo.txt.command.txt` and
records:

```text
MVDM-CMD-CONT svc=01 stage=0 ... first=1 ... DOS TO_TAKE_A_COMMAND
MVDM-CMD-GUEST-RETURN code=0000 first=1 repeat=0
MVDM-CMD-VDMINFO stage=0 error=0000 state=0005 success=0 first=1 repeat=0
MVDM-CMD-VDMINFO stage=1 error=0000 state=0000 success=1 first=1 repeat=0
MVDM-CMD-CONT svc=01 stage=1 ... first=0 ... DOS BUSY
MVDM-CMD-CONT svc=01 stage=0 ... first=0 ... DOS BUSY
MVDM-CMD-GUEST-RETURN code=0000 first=0 repeat=0
```

The second `CMDINFO.ReturnCode == 0` is a source-owned result of the nested
immutable `COMMAND.COM /C` lifecycle: the copied first DOS record was consumed
and the guest returned a zero DOS code before re-entering the original
one-session continuation path.  It is stronger than mere `54:01` reachability,
but it is not proof that the requested redirection reached the host file.

## Relative-path control

The same product, stage, container, timeout and observer were used for a
second command that differs only in its target spelling:

```text
echo M0T386 > m0-t386-relative-marker.txt
```

`O:\ntvdm64\m0-t386-relative-marker.txt` was absent before and after the run.
Its sidecars are
`artifacts/research/m0-t386-s3-command-relative-marker.txt` and
`artifacts/research/m0-t386-s3-command-relative-marker.txt.command.txt`.
They retain the same successful first `54:01` result and zero second guest
return, but the BOP dispatch list still ends before any of the original DOS
file-service selectors:

```text
50:03  SVC_DEMCREATE
50:1E  SVC_DEMWRITE
50:02  SVC_DEMCLOSE
```

The original guest source makes those ownership boundaries explicit:
`doskrnl/dos/file.asm` dispatches create through `SVC_DEMCREATE`, while
`doskrnl/dos/handle.asm` dispatches the slow write through `SVC_DEMWRITE` and
the handle close through `SVC_DEMCLOSE`.  Neither selector is reached by either
the absolute or relative command control.  The remaining failure is therefore
earlier than DEM file I/O: it lies in the original post-`54:01` COMMAND
execution/standard-handle path.  This evidence does not authorize a new DEM
provider or a drive-mapping workaround.

The product then exits zero through the already declared one-shot command
exhaustion boundary.  The empty terminal result is deliberately not treated
as a successful `echo` result.

## Disposition

`BaseCreateVDMEnvironment` is a real improvement: the previous released
transient environment retry is gone, the first `54:01` succeeds, and the
returned nested immutable COMMAND lifecycle reports its original zero DOS
return code.  It does not close T386.  There is still no observable proof that
the immutable COMMAND built-in completed its intended DOS file/redirection
operation.

The next investigation must stay on the original pre-file portion of the
post-`cmdGetNextCmd` chain: `COMMAND.COM run_cmd/DOCOM -> standard-handle /
execution dispatch`.  Only after source and bounded evidence prove passage
through that boundary may it investigate `DOS INT 21h -> original DEM file
owner`.  It may not replace the guest parser, add a host command executor, or
alter `COMMAND.COM`.
