# M0 T291 S4 P2 — GetNextVDMCommand typed-provider separation

## Question

Can the reached public `GetNextVDMCommand(VDMINFO)` entry retain its OpenNT
shape without treating BaseSrv command brokerage and `NtVdmControl` as one
generic session operation mechanism?

## Inputs

- `mvdm-platform-abi/source/public/internal/base/inc/vdmapi.h`, where the
  original contract describes BaseSrv notification of shared WOWEXEC;
- `mvdm-host/dos/command/cmdexec.c` and `cmdmisc.c`, including original
  re-entry-count and capacity retry calls;
- `mvdm-host/wow32/wkman.c`, whose no-command success and WOWEXEC retry rules
  prove a distinct future multi-caller requirement; and
- current `app/command_source.c` and monitor/session source.

## Change

`adapter-mvdm-host-out/monitor` now owns a typed, session-bound command
provider with the original public entry and `VDMINFO` argument unchanged. The
temporary operation-number table was removed from `session`; it could not
express BaseSrv ownership, capture, wake-up or caller arbitration. The app
provider remains one bounded immediate producer and never retains `PVDMINFO`.
Absent provider returns the source-shaped unavailable result.

`NtVdmControl(VdmQueryDir)` stays on its independent typed monitor-control
binding. Neither contract invokes the other, and synchronous WOW `CallBack16`
continues to bypass both.

## Verification

The formal Ninja command transport fixture was rebuilt and executed on both
MSVC `/MT` targets:

- x86: `PASS: local VDM command transport`;
- x64: `PASS: local VDM command transport`.

The fixture additionally proves that an active bound session with the command
provider removed gets `ERROR_CALL_NOT_IMPLEMENTED`; no residual generic route
can answer the call.

## Result and follow-up

This is a separation closure, not a BaseSrv/CSR replacement. S5 is the named
successor for source-shaped wait/wake, producer arbitration and any WOWEXEC
consumer admission. S4 P3/P4 still own the host-simulate postcondition and
WOW TD/TEB lifecycle ledgers.
