# M0 T291 S5 P5 — Base VDM vertical-slice audit

## Result

The admissible non-MVDM source is not the whole BaseClient/BaseSrv product.
It is the minimum Base VDM service vertical slice whose client and server
operations form the lifecycle directly consumed by selected MVDM packages.
The selected source remains mirror-only until the wider P5 package/interface
audit completes; this record makes no runtime-binding claim.

## Direct MVDM consumers

The original selected MVDM tree calls the client VDM API as follows:

- `GetNextVDMCommand`: `dos/command/cmdexec.c`, `cmdmisc.c`,
  `softpc.new/host/src/config.c`, `nt_msscs.c`,
  `v86/scaffold/i386/softpc.c`, and `wow32/wkman.c`.
- `ExitVDM`: `softpc.new/host/src/nt_term.c`, `nt_reset.c`, `nt_event.c`,
  `v86/scaffold/i386/{softpc,fakekbd}.c`, and the WOW task lifecycle in
  `wow32/wkman.c`/`wow32.c`.
- `SetVDMCurrentDirectories`: `dos/command/cmdmisc.c`.
- `RegisterWowExec`: `wow32/wkman.c`.

These calls prove that the service is shared by COMMAND, SoftPC host control,
the V86 scaffold and WOW; it cannot remain a COMMAND-local provider.

## Required original client/server operations

The required source-shaped client surface is:

- `GetNextVDMCommand`, including its original first-VDM and re-entry special
  cases, capture sizing, capacity return, wait/retry and copy-out order;
- `ExitVDM`;
- `SetVDMCurrentDirectories`; and
- `RegisterWowExec` when the WOW package is admitted.

The matching server-side service includes `BaseSrvGetNextVDMCommand`,
`BaseSrvExitVDM`, `BaseSrvIsFirstVDM`, `BaseSrvSetReenterCount`,
`BaseSrvSetVDMCurDirs`, their record/list helpers, and
`BaseSrvRegisterWowExec` only for an admitted WOW profile. The server-side
`CheckVDM`/DOS/WOW record creation path is part of the same protocol family,
but its client producer is not currently a direct MVDM call; it is retained as
a separately classified service-producer subgraph rather than assumed enabled.

## Explicit exclusions from this external package selection

- Client `GetBinaryType*`, short-path, VDM environment, binary-check and
  general BaseClient functions in `base/win32/client/vdm.c` are not shown as
  direct selected-MVDM calls. They cannot enter merely because the historical
  source shares a translation unit.
- General BaseSrv services in `srvtask.c`, `srvnls.c`, `srvini.c`, `srvacces.c`
  and unrelated `srvinit.c` dispatch entries are outside the VDM vertical
  slice.
- `base/ntdll/csrutil.c`, `base/subsys/csr/server`, and `base/ntos/vdm` are
  product-shell boundaries, not selected service source.

## Existing project code subject to reversion review

- `src/app/command_source.c` currently supplies a bounded immediate command
  source. It is a potential producer replacement only after the original
  server-side `CheckVDM`/record-creation subgraph has a finite accepted
  boundary.
- `src/adapter-mvdm-host-out/monitor/source/vdm_command.c` supplies the
  current `GetNextVDMCommand` facade. It is a potential binding replacement;
  it must not be deleted until the selected original client/server slice has
  an equivalent source-shaped, tested route.

## Required tracker actions

The current full-file mirrors `opennt-host/base/win32/client/vdm.c` and
`opennt-host/base/win32/server/srvvdm.c` are evidence imports, not final
selection proof. P5 must either register a true original subset containing the
operations above or prove that every retained body belongs to the admitted VDM
vertical slice. Any omitted body receives a source/README exception entry;
none may be silently retained as reference code in a production component.

## Sources

- `O:\repos.external\opennt\base\win32\client\vdm.c`
- `O:\repos.external\opennt\base\win32\server\srvvdm.c`
- `O:\repos.external\opennt\base\win32\server\srvinit.c`
- selected `O:\repos.external\opennt\base\mvdm` consumer paths above.
