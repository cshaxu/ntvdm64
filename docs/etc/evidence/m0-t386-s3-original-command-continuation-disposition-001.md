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

## Superseded early observation

The fresh formal `original-softpc-process.exe` was staged in the fixed
`O:\ntvdm64` package container and invoked through the console-owning,
non-debug observer with the sole declared command:

```text
echo M0T386 > O:\ntvdm64\m0-t386-command-marker.txt
```

The marker name was absent before and after that early run. This result is
**superseded**: the observer forwarded tokenized argv by raw concatenation, so
the multiword `--command` value was split before the product received it. The
durable report is retained as historical evidence only:
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

## Corrected fixed observation and actual result

The observer now re-quotes every forwarded argv item using the Windows
`CreateProcess` command-line rule. The fixed, console-owning non-debug
container used the same staged CPU40/x86 product and immutable guest media:

```text
--command "echo M0T386 > M0386.TXT"
```

This is an original `COMMAND.COM` built-in command. The selected immutable
binary remains SHA-256
`908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
The run exited zero and created `O:\\ntvdm64\\M0387.TXT`; its exact bytes are
`M0T386 <space>\\r\\n`, which is the original COMMAND `echo` spelling used by
this command line. The formally linked candidate was published as
`build/output/ntvdm64-0237.exe`, SHA-256
`1CC9DF57CFEF1296FB3512F9C391945582DEDBAA5577E4106A6AE2E21406A95D`.

The final fixed-result report is
`artifacts/research/m0-t386-s6-command-marker-83.txt` with its BOP sidecars;
it records original `50:03` create, then `50:43` followed by `50:1E`, then
`50:02` close. The separate default-off contract report
`artifacts/research/m0-t386-s3-create-write.{create,write}.txt` records the
normalized create pathname and successful 7-byte then 2-byte writes. `50:43`
is the NT4 kernel `NTFastDOSIO` fast-write interception point. The current
user-mode table cannot own it; returning carry selects the unchanged NTDOS
`$WRITE` slow fallback at `50:1E`. This explains the observed original
`demWrite` result without inventing a user-mode fast-write worker.

The earlier long filename test created `M0-T386-.TXT`, not the requested long
name, because this guest DOS path applies its original 8.3 normalization. The
short `M0386.TXT` control avoids treating that normal guest behavior as a host
file-service failure.

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

`BaseCreateVDMEnvironment` remains the source-shaped prelaunch recovery. The
corrected observer and the `50:43 -> 50:1E` source-owned fallback now provide
the missing observable proof: immutable COMMAND executes its built-in, the
original DOS path creates and writes the redirection target, and the command
then returns through the documented one-shot continuation.

This closes only T386's pure-DOS COMMAND built-in scope. It is not evidence of
external `.COM`/MZ execution, host-native child execution, Redirector, or
Win16 behavior.
