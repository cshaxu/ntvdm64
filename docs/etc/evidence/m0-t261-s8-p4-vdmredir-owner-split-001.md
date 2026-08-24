# M0 T261 S8 P4 — VDMREDIR owner split and reached helper recovery

## Inputs

- `refs/opennt/base/mvdm/vdmredir/vrnmpipe.c` (named-pipe interceptors and
  private `OPEN_NAMED_PIPE_INFO` list);
- `vrinit.c` (initialization and asynchronous VDD/ICA completion ownership);
- `vrdisp.c` (the 0x00–0x31 service table);
- `vrmslot.c` (mailslot records and DOS 16-bit handle semantics);
- former `src/opennt-host/redir/redir_session_shim.{c,h}`.

## Disposition

`redir_session_shim` combined three different owners.  Its selector decoding,
checked guest spans, typed resume, async IRQ publication and mailslot BOP
record handling are product composition, and have moved unchanged to
`opennt-bop/ingress/redir_native_session.{c,h}`.  This keeps `opennt-host`
free of BOP recognition and guest CPU-state ownership.

The reached synchronous DEM named-pipe contract is now owned by re-rooted
`opennt-host/vdmredir/vrnmpipe.{c,compat.h}`.  It preserves the historical
`VrInitialize`, `VrUninitialize`, `VrAddOpenNamedPipeInfo`,
`VrRemoveOpenNamedPipeInfo`, `VrIsNamedPipeHandle`, `VrReadNamedPipe`,
`VrWriteNamedPipe`, and `VrConvertLocalNtPipeName` entry spellings.  The
private list is the original VDMREDIR pipe-name metadata list; it is not a
second guest/host handle mapper and never issues guest-visible IDs.

`VrConvertLocalNtPipeName` also retains the original allocation/copy ordering:
it returns an allocated/copy-normalized buffer for remote UNC names, maps only
the local computer name to `\\.`, and canonicalizes forward slashes.  It does
not return the caller's source pointer as an optimization.

`VrRemoveOpenNamedPipeInfo` likewise uses the OpenNT non-DBG behavior: removal
is opportunistic and the intercept returns `TRUE` even for an ordinary handle;
the close path must not gain a new Redirector-specific failure.

## Recovery ladder

1. Direct compile of the complete historical units was examined first.
   `vrnmpipe.c` requires the product VDD cancellation list, SoftPC/ICA and
   private `vrdlctab` declarations; `vrinit.c` requires `VDDInstallUserHook`,
   NetBIOS/DLC initialization and ICA interrupt control; `vrmslot.c` needs
   original DOS guest-address structures; `vrdisp.c` names all of those
   functions together.
2. The directly reached synchronous helper fragment composes through public
   Win32 overlapped I/O and the existing VDMREDIR-shaped header.  It retains
   the original call sequence, return types and `ERROR_MORE_DATA`/no-data
   treatment.  `HOST-DIV-015` records the only required VDD cancellation-list
   divergence.
3. The remaining async/mailslot BOP delivery composition is not relabelled as
   host VDMREDIR source.  It is retained in `opennt-bop` until its source
   units can be brought through the existing `adapter-softpc`/`session`
   mechanical seams without re-creating the NT4 VDD/ICA product shell.

## Verification to date

- no production include of `opennt-host/redir/redir_session_shim.h` remains;
- no production `bx_ntvdm_redir_load/read/write/...` helper remains;
- native-session teardown calls `VrUninitialize` even when the guest did not
  first issue service `57:01`, so the original owner list cannot survive a
  session reset;
- the Redirector focused fixture now opens, adds, queries, reads/writes, and
  removes a named-pipe metadata record in the source order rather than using
  `GetNamedPipeInfo` as an out-of-band recognition shortcut; its MSVC object
  compiles under `/W4 /WX`.
- fresh `r006` MSVC targeted objects passed:
  `vrnmpipe.c`, `opennt_demfile_composition.c`,
  `redir_native_session.c`, and `opennt_dem_ccpu_sas_facade.c`;
- `git diff --check` passes (line-ending advisories only).

The first r006 full-graph invocation was contaminated by an independently
running Ninja writer, yielding Ninja's `premature end of file` warning.  It is
not accepted as formal closure; a fresh uncontended full graph remains required.
