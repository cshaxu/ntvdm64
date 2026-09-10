# M0 T282 S2 — COMMAND declaration-carrier evidence

## Question

Can the original COMMAND package select exact `VDMINFO`/PIF declaration
carriers on both supported host architectures without enabling a BOP provider
or implementing any historical function?

## Inputs and procedure

- Compared the imported platform-ABI files with the pinned OpenNT paths:
  `public/internal/base/inc/vdmapi.h` and
  `public/internal/windows/inc/pif.h`.
- Registered both hashes and original consumers in the platform-ABI import
  manifest.
- Made the existing adapter `vdmapi.h` a declaration-only forwarder; the
  generated COMMAND graph keeps the adapter headers before the sole original
  PIF-header search directory, so it does not select the conflicting original
  SDK `nt.h`.
- Outside the sandbox, generated and ran the formal MSVC `/MT` Ninja probes
  under `build/M0-T282/S1/{x64,x86}` for original `cmdexec.c` and `cmdpif.c`.

## Observations

- Both imported files equal their pinned source hashes:
  `vdmapi.h` `b942656ba8761ddb9325a7eea7997394562d2e57797c923ff5688d7be86426de`;
  `pif.h` `224f6bdf5cfa15a37d95f97e24c30d37d947d5f7f5199d50aeab65b39d150c55`.
- `cmdexec.c` compiled on x86 and x64, parsing original `VDMINFO` records and
  `GetNextVDMCommand` calls. Its retained x64 width warnings are source audit
  inputs, not a declaration-carrier failure or authorization to change the
  mirror body.
- `cmdpif.c` compiled on x86 and x64 using exact original PIF layouts.
- A full selector-disabled package attempt stops first at original
  `cmdkeyb.c:10`, which includes missing historical private `winconp.h`.
  That console-private declaration/API is not PIF or VDMAPI behavior and is
  transferred to the named later console/monitor boundary.

## Interpretation and follow-up

The two S2 carriers are complete as byte-exact declarations and are selected
by both architecture probes. No `GetNextVDMCommand`, PIF parser, COMMAND
dispatcher, provider, executable, Bochs input, guest route or trace was
enabled. Future callable VDM command acquisition belongs to the session and
VDM-monitor binding package; the PIF provider remains in the COMMAND package;
`winconp.h` is a separate historical console-private interface audit.
