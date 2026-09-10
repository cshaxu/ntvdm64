# M0 T271 S3 P4 — COMMAND composition owner disposition

## Scope

`opennt-bop/command/opennt_command_composition.c` is a temporary recovery
composition, not an OpenNT mirror. This ledger assigns every remaining block
to one final owner before moving it. It prevents a mechanical BOP ingress,
a historical Win32 facade, and an OpenNT COMMAND provider from being mixed again.

## Retain temporarily in `opennt-bop`

- The short active-call binding and the source-entry invocation at lines
  992–1141 stay only until a selector-blind copied-frame call boundary exists
  in `adapter-bop`. The retained `body` argument is not a selector/service
  switch: it permits the imported `MS_bop_4` body to execute with bounded
  state. A later move must leave selector and service decoding solely in the
  imported OpenNT entry/provider source.
- Source-specific `cmdVDMEnvBlk`, `IsRepeatCall`, `SCSINFO`, `PIF_DATA`,
  `VDMINFO`, `cmd*.c` globals and source failure ordering remain with the
  imported COMMAND package. They cannot be renamed into an adapter merely
  because their original callers are COMMAND source.
- `GetWowKernelCmdLine` remains an explicit terminal for the original
  `VDMForWOW` branch until the separate WOW owner package admits its input,
  publication and loader contract.

## Move to `adapter-softpc`

- Copied CPU register getters/setters and `sas_load` are CCPU/SAS mechanical
  compatibility.  P12 has moved those generic operations into the
  thread-scoped `ccpu_frame_context` boundary.  It carries only a borrowed
  typed CPU/result record and checked callbacks; no COMMAND table, `54:xx`
  number or provider state moved with it.
- `GetVDMAddr` is not part of that generic extraction: its allocation,
  writeback and source-service ordering are COMMAND-specific source glue and
  remain for the remaining provider-body review.  Checked guest spans and
  typed ingress/outcome conversion are already owned by the selector-blind
  `adapter-bop` frame transaction (P9).
- The completed `host_lpt_flush_initialize` extraction is the model: retain
  the original SoftPC spelling and reachable mechanical state, but keep the
  original COMMAND caller unchanged.

## Move to `adapter-win32`

- `GetNextVDMCommand` and `SetVDMCurrentDirectories` are historical Kernel32
  VDM APIs, not COMMAND functions. Their exact `VDMINFO` ABI and size/
  failure/re-entry rules must be exported by a same-shaped modern facade.
  OpenNT `base/win32/client/vdm.c` is the algorithmic source reference;
  unavailable CSR/BaseSrv transport must be replaced only at that transport
  boundary.
- OEM conversion/environment/drive and system-directory facades have already
  moved in P2. No new COMSPEC, COMMAND or guest semantics are permitted in
  that facade layer.

## Move to `opennt-host`

- `nt_std_handle_notification` is the reached subset of OpenNT
  `softpc.new/host/src/nt_msscs.c`. Its `stdoutRedirected` state belongs to
  an imported OpenNT host subset; fullscreen mouse-buffer handling remains
  a separately admitted display/machine feature.
- `nt_block_event_thread`, `nt_resume_event_thread`, and
  `cmdPushExitInConsoleBuffer` source from `nt_event.c`. Their retained
  non-product state and original void call shapes belong in an OpenNT host
  subset. VDD hooks, BIOS keyboard writes, timers, screen state, process
  group termination and window messages require their separate owner packages
  and may not be replaced by silent success.
- Child record allocation, `CreateProcess` setup, job ownership, wait,
  cancellation, exit capture and stream endpoint preparation are a host
  capability. The original `cmdexec.c` call order remains in `opennt-bop`;
  the modern public-Win32 implementation belongs in `opennt-host`, using only
  declared `adapter-win32` calls.

## Move to `session`

- The one active session's owned resource identities, teardown ordering,
  completion/cancellation state and neutral copied payload lifetime must be
  consolidated in `session`. `session` may expose fixed-width neutral data
  records and teardown hooks, but must not expose `VDMINFO`, BOP selector,
  DOS, COMMAND, WOW, SoftPC or Win32 names.
- The temporary `runtime_command_misc_session` currently mixes these neutral
  responsibilities with COMMAND source data. It must be split by fields,
  not copied as a renamed struct: host handles and pending child lifecycle
  first; then inbound/outbound copied payload ownership; only then COMMAND
  source globals can consume those records through typed owner facades.

## Explicit non-moves

- `adapter-bop` may receive only a generic copied-frame/typed-result call
  transaction after `adapter-softpc` exposes it. It never implements
  `GetNextVDMCommand`, lifecycle policy, selector recognition or a COMMAND
  service table.
- `adapter-bochs` and `bochs-core` receive none of these blocks. The
  historical functions are host/session compatibility, not Bochs mechanics.
- `app` retains only construction and final binding of the one session. It
  does not become a hidden COMMAND/CSR provider.

## Implementation order

1. Split a neutral resource/payload record from the current COMMAND session
   and bind its lifecycle to `session`.
2. Recover `GetNextVDMCommand` and `SetVDMCurrentDirectories` as
   `adapter-win32` same-shaped facades over that record, with original
   capacity, re-entry and failure tests.
3. Import the reached `nt_msscs` and `nt_event` host subsets into
   `opennt-host`, registering every omitted product-shell branch.
4. Move child lifecycle into `opennt-host`; preserve `cmdexec.c`'s original
   call ordering and keep DOS EXEC/PSP parent return explicitly guest-owned.
5. P9 extracted the generic copied-frame transaction to `adapter-bop`; P12
   extracted the generic CCPU register/load facade to `adapter-softpc`.
   The residual source-specific body blocks require their individual final
   owner dispositions before the temporary composition can be deleted.
