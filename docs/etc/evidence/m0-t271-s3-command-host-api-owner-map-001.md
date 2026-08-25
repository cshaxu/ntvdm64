# M0 T271 S3 — COMMAND historical-host API owner map

## Purpose

This map records the remaining non-COMMAND definitions reached by imported
OpenNT COMMAND bodies.  It prevents their project-authored replacements from
remaining in `opennt-bop` merely because they are called by `cmd*.c`.

## Source and destination decisions

- `GetNextVDMCommand(PVDMINFO)` is declared by OpenNT
  `public/internal/base/inc/vdmapi.h:104` and implemented by
  `base/win32/client/vdm.c:338`.  Its original body packages `VDMINFO`, calls
  `BasepGetNextVDMCommand` through CSR/BaseSrv, handles the returned wait
  object, and copies the result back.  Modern Windows exposes neither the
  CSR client nor BaseSrv.  The retained spelling, structure contract,
  re-entry increment/decrement result and bounded copies therefore belong to
  an `adapter-win32` same-shaped facade.  Its queued command declaration and
  lifecycle state must be obtained through neutral `session` records; the
  facade may not depend on `opennt-bop`.
- `SetVDMCurrentDirectories(ULONG, LPSTR)` is implemented by the same
  `vdm.c:802` and similarly submits a captured multisz to
  `BasepSetVDMCurDirs`.  It has the same `adapter-win32` owner.  Its current
  copied publication state is only a temporary source-derived replacement in
  COMMAND composition and must migrate together with the previous facade.
- `nt_std_handle_notification(BOOL)` is defined in
  `base/mvdm/softpc.new/host/src/nt_msscs.c:1067`.  The always-reached source
  behavior stores `stdoutRedirected`; its `X86GFX` mouse-buffer branch is a
  separate display/machine feature.  A true subset belongs under
  `opennt-host/softpc.new/host/src/`, using neutral session notification
  state.  It must retain the original function name and not alter the CLI
  process-wide standard handles.
- `nt_block_event_thread`, `nt_resume_event_thread`, and
  `cmdPushExitInConsoleBuffer` are defined by
  `softpc.new/host/src/nt_event.c:1364`, `:1501`, and `:1879`.
  Their VDD hooks, keyboard/BIOS manipulation, timer, display and console
  product shell are not locally composable.  The original call shapes belong
  to an `opennt-host` true subset only after its session-visible block/resume
  state is separated from the unadmitted VDD/display/device operations.
  `cmdPushExitInConsoleBuffer` additionally requires an explicit console
  input/top-level owner disposition; its existing notification marker is not
  a completed console injection.
- `host_lpt_flush_initialize` is defined by
  `softpc.new/host/src/nt_lpt.c:688`.  It clears only the three
  `dos_opened` bits.  It has moved in this P from COMMAND composition to the
  selector-blind `adapter-softpc` facade, where the existing three-port
  mechanical state preserves that exact reachable operation without creating
  an LPT endpoint.
- `GetWowKernelCmdLine` remains a WOW16 owner terminal: original COMMAND
  calls it only through its `VDMForWOW` branch.  No generic COMMAND or
  adapter facade may fabricate a WOW launch.
- The worker/`CreateProcess` path remains source-owned in `cmdexec.c`; its
  modern public-Win32 child lifecycle is an `opennt-host` capability with
  `adapter-win32` API use, never a BOP dispatcher or Bochs concern.  Ordinary
  DOS EXEC/PSP parent return remains guest-owned and is not folded into this
  package.

## Required migration order

1. Create the neutral session records required by the two historical VDM
   APIs, without putting `VDMINFO`, BOP or COMMAND vocabulary in `session`.
2. Move the same-shaped VDM API facades to `adapter-win32`, preserving the
   original failure/size/re-entry results and retaining public modern Win32
   calls only where applicable.
3. Import the reached `nt_msscs`/`nt_event` true subsets under `opennt-host`,
   with each omitted product-shell branch registered as a mirror divergence.
4. Delete the corresponding project-authored functions from
   `opennt-bop/command/opennt_command_composition.c`, then run the complete
   COMMAND local/failure matrix and a formal Ninja build.

This map is an admission boundary, not evidence that the listed historical
product-shell features are already recovered.

## P3 completed extraction and verification

`host_lpt_flush_initialize` has been removed from
`opennt-bop/command/opennt_command_composition.c`.  The original imported
`cmdmisc.c` calls the unchanged historical name, now exported by
`adapter-softpc/softpc_printer_openclose_shim.c`.  The facade resets exactly
the already-admitted three `dos_opened` bits and has no endpoint, DOS, BOP or
selector knowledge.

On 2026-08-25, outside the sandbox, formal Ninja root
`build/M0-T271-S3/r001` rebuilt both affected libraries and passed:

- `t245-s7-printer-openclose-fixture.exe`, including the direct historical
  flush-name regression;
- `t231-s2-command-misc-direct-import-fixture.exe`;
- `t231-s3-command-misc-registration-fixture.exe`;
- `t231-s4-command-console-keyboard-direct-fixture.exe`;
- `t231-s10-command-native-session-fixture.exe`.

The other rows above remain active migration work.  This evidence does not
claim that BaseSrv/CSR, console injection, VDD/display state, WOW, or DOS
parent return have been recovered.
