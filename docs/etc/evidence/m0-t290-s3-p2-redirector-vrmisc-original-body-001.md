# M0 T290 S3 P2 — Original Redirector lifecycle-helper body

## Question

Can the selected lifecycle helper remain an unmodified original provider body
while consuming the established same-shaped host register declarations?

## Inputs and procedure

- Exact mirror input: `mvdm-host/vdmredir/vrmisc.c`.
- Exact original declaration carriers: `mvdm-support/inc/{softpc.h,vrmslot.h,vrnmpipe.h}`.
- The fixture supplies only static-link observations for the two downstream
  owner-group calls and the source-shaped register forms; it does not route a
  selector or implement a provider.
- `tools/build/New-T290S3VrMiscNinja.ps1` generates the formal `/MT` graph.

## Observation

- x64 fixture `build/M0-T290/S3/vrmisc-x64/bin/t290-s3-vrmisc-fixture.exe`
  exited `0`.
- x86 fixture `build/M0-T290/S3/vrmisc-x86/bin/t290-s3-vrmisc-fixture.exe`
  exited `0`.
- In both builds the original `VrTerminateDosProcess` reads `AX` once and
  passes the unchanged PDB to mailslot cleanup before named-pipe cleanup.
- The original `VrUnsupportedFunction` preserves the `SET_ERROR` result:
  `AX = ERROR_NOT_SUPPORTED` and carry set.

## Interpretation and boundary

The exact original helper is now locally executable on both targets.  The
downstream mailslot owner remains S4, while named-pipe cleanup is an S3
consumer to compose through the separately audited synchronous pipe group.
No Redirector selector, VDD success, async completion, host handle or guest
pointer is enabled by this proof.

## Follow-up

P3 must compose the original `VrInitialize` failure direction through a
same-shaped VDD binding, before admitting any initialized lifecycle state.
