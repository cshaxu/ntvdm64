# M0 T245 S11 — Config-complete source-body closure

OpenNT `nt_bop.c:MS_bop_E` is mirrored as one body: when `AL==0`, it calls
the original-empty `UMBNotify(0)` and imported `demDasdInit()` in that order.

The new bx-vdm scoped-body helper is selector-blind: provider code recognizes
`C4 C4 5E`; the helper only scopes copied CPU state, invokes the body, and
uses an explicit three- or four-byte resume width.  No Bochs code changed.

Formal `M0-T245-S11/formal-r3` built the 265-edge closure.  The focused
fixture bound an empty synthetic drive snapshot, dispatched AL=0 at `0x1234`,
and exited zero after verifying typed resume at `0x1237` with no GPR delta.
This proves local source/ABI closure only; it is not a guest-continuity or
floppy-device claim.

## One Native Integration Observation

The same formal-r3 `ntdos64-native.exe` ran the source-built DOS/WOW16 bundle
with a 1,000,000-tick budget and BOP/UD sequence observation.  Entry 25 was
the expected `5E` at `8e08:08af`, accepted with disposition 1.  Execution
then reached a separate DEM `50:3D` owner at `8e08:0984`, which declined
(disposition 2).  This confirms the `5E` continuation; it neither makes
`50:3D` an automatic implementation task nor claims continuous execution.
