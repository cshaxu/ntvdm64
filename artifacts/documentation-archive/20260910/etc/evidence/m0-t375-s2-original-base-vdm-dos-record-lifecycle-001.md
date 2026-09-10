# M0 T375 S2 — Original Base VDM DOS-record lifecycle recovery

## Question

Can the reached one-session `GetNextVDMCommand`/`ExitVDM` path retain the
observable OpenNT BaseClient/BaseSrv DOS-record ordering without recreating
CSR, CSRSS or an external VDM broker?

## Source-first disposition

1. **Original source.** The retained mirrors
   `opennt-host/base/win32/client/vdm.c` and
   `opennt-host/base/win32/server/srvvdm.c` are the authority.  Their complete
   translation units cannot be linked into this product: `vdm.c` calls the
   private CSR capture/client transport, and `srvvdm.c` owns CSRSS console
   records, duplicated kernel handles and multi-process DOS lists.
2. **Smallest same-shaped seam.** `adapter-mvdm-host-out/basesrv` retains the
   public `GetNextVDMCommand`, `ExitVDM`, current-directory and re-entry
   shapes.  Its private record is only the selected one-session equivalent of
   the reached DOS record and wait pair; it does not carry a guest pointer or
   caller-owned `VDMINFO` across a wait.
3. **No external-code intrusion or new command protocol.** The correction
   changes only state ordering in the existing adapter.  It neither publishes
   a synthetic command nor changes original COMMAND, guest media, SoftPC or
   BOP code.

## Recovered ordering

- Original `srvvdm.c` lines 229–263 distinguish a busy-record return from
  normal first/second/no-parent requests.  Lines 269–296 return a wait object
  for the first no-command request; lines 271–276 return the no-command
  terminal only for `RETURN_ON_NO_COMMAND | ASKING_FOR_SECOND_TIME`.
- Original `vdm.c` lines 595–605 waits the returned object, adds
  `ASKING_FOR_SECOND_TIME`, clears `ExitCode`, then retries the request.
- The adapter now preserves that sequence for
  `NO_PARENT_TO_WAKE | RETURN_ON_NO_COMMAND`: first request waits through the
  private event, retry does not rewrite the busy record, and its empty second
  request returns the original no-command terminal.  The client now clears
  `ErrorCode` before its retry.
- The one-session direct terminal for a normal returned DOS child remains an
  explicit bounded divergence: there is no external parent DOS record or
  duplicated CSRSS handle to wake.  It returns the original empty failure
  result rather than fabricating either a parent or a command.

## Verification

The following x86 checks ran outside the sandbox using MSVC and Ninja:

```text
build/M0-T302/S2/x86: ninja -j 8 test
PASS: local Base VDM broker contract

build/M0-T375/S2/base-vdm-native-child-x86:
  command_native_child_fixture.exe
exit: 0

build/M0-T371/S2/formal-x86:
  run-ninja-parallel.cmd original-softpc-process.exe
exit: 0
```

The focused fixtures prove: first record delivery, mandatory/optional capture
buffers, wait then retry, no-command terminal, DOS/WOW queue separation,
current-directory ownership, re-entry completion and session-only `ExitVDM`.
The rebuilt formal product SHA-256 is
`350d4a84db26972132ba728703d2af1275c1ed1d609f40b6eaec900b05ef11fb`.

## Result and next step

S2 is source/link complete for the reached one-session BaseVDM record
lifecycle.  It does **not** prove guest `COMMAND.COM` built-in completion.
S3 must stage the rebuilt product and make one fixed, non-debug observation
with the correct declared built-in shape (`EXIT`, which app serializes as
`/C EXIT`), then attribute the first later owner if it still does not finish.
