# M0 T318 S2 P72 — COMMAND ingress and Base VDM host-path carrier

## Scope

This record tests the already admitted single `--ordinary-child` declaration.
It does not add a guest loader, BOP provider, DOS executor, or COMMAND
semantics.  Its only source change restores the original BaseClient host-path
capacity for `VDMINFO.CurDirectory`.

## Original contract

`cmdGetNextCmd` in the imported original COMMAND mirror declares
`CurDirectoryLen = MAX_PATH + 1` and supplies a host-side current-directory
buffer of that capacity.  This is distinct from a drive-relative DOS path
component.  The original BaseClient/BaseSrv transport carries that host path
to the command consumer before COMMAND writes its guest command information.

## Defect and repair

The session-local BaseSrv record and the app declaration object had retained
`MAXIMUM_VDM_PATH_STRING` storage (81 bytes), despite their comments and the
original COMMAND request identifying the carrier as a host path.  The fixed
runtime container's `dos` root is 83 bytes before its NUL.  Consequently the
app rejected `--ordinary-child` before original SoftPC startup with exit
`0x45` (`APP_STARTUP_COMMAND_REJECTED`).

The carrier now consistently uses `MAX_PATH + 1` in:

- `src/app/launch_declaration.h`;
- `src/adapter-mvdm-host-out/basesrv/include/base_vdm_local.h`; and
- `src/adapter-mvdm-host-out/basesrv/source/base_vdm_local.c`.

This is a binding/capacity correction only: no guest-visible DOS limit is
changed, and the original `cmdGetNextCmd` request shape remains intact.

## Verification

- Fresh selected x86 and x64 CPU40 Ninja links succeed.
- The focused x86 `base_vdm_local_fixture.exe` passes, including its long
  executable-relative package-root declaration case.
- In the fixed console-owning non-debug container, the corrected product no
  longer exits with `0x45`.  That container's DOS media root is nevertheless
  too long for the distinct original `cmdconf.c` 64-byte CONFIG carrier, so
  its watchdog result is not CONFIG-continuity evidence; see the P74
  correction below.
- The neutral existing BOP observation now records original COMMAND ingress
  after it decodes the service byte: `54:05` (`cmdSetInfo`) and `54:0C`
  (`cmdGetConfigSys`) follow the DEM bootstrap sequence.  The observer leaves
  CPU, guest, session, selector, service, return, and failure state unchanged.

The raw reports are:

- `artifacts/research/m0-t318-s2-p72-command-ingress-observation.txt*`;
- `artifacts/research/m0-t318-s2-p72-command-ingress-after-host-path-fix.txt*`;
- `artifacts/research/m0-t318-s2-p72-command-ingress-after-complete-host-path-fix.txt*`.

## Disposition

The declared ordinary child is now proved to enter original COMMAND bootstrap
and consume the original Base VDM path carrier.  This is not yet evidence of
successful CONFIG preprocessing, `54:01`, guest `EXEC`, `50:36`, `50:3C`,
`54:0B`, or parent restoration.  P74 records the separate original CONFIG
root constraint.  The next work remains the source-shaped COMMAND/guest
lifecycle, not a new BOP implementation.
