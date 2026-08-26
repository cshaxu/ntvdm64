# M0 T271 S4 P2 — XMS A20 mapping-manager migration

## Question

Does the reached original XMS A20/HIMEM state path use the one session-owned
guest-memory mapping manager rather than keeping a family-local guest-pointer
scheme?

## Inputs

- OpenNT `base/mvdm/xms.486/xmsa20.c` and `xmsumb.c`.
- `src/adapter-softpc/opennt_xms_softpc_facade.c`.
- The three-instance session mapping-manager contract in
  `src/adapter-softpc/guest_pointer_manager.{c,h}`.

## Change

The XMS source-call context now opens the existing `guest_memory` instance
for the duration of its synchronous invocation.  The HIMEM `AX:BX` state
location is validated with a real-mode one-byte read lease; the retained
state is the guest-linear address, not a native pointer.  A20 source writeback
acquires a fresh one-byte write lease and releases it with writeback.

## Result

- `GetVDMAddr` is not used to publish or retain a host pointer by the reached
  XMS bodies.
- The original `xmsEnableA20Wrapping` / `xmsDisableA20Wrapping` ordering and
  their A20 state-byte result remain unchanged.
- `BOP-DEPENDENCY-037`, `BOP-XMS-52-00`, and `BOP-XMS-52-06` now have the
  `opennt-shaped-facade` / `retain-facade` disposition.
- `SOFTPC-DIV-006` records the unavoidable historical-pointer divergence.
- Formal `r001` rebuilt the changed adapter archive and both
  `t237-s2-xms-source-mirror-fixture.exe` and
  `t237-s3-xms-a20-source-mirror-fixture.exe` returned zero.

## Residual scope

This does not fabricate a physical UMB span or change XMS block copy/allocator
mechanics. Those are separate machine-capability calls already tracked by
their XMS rows.
