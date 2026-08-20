# M0 T232 S2 — DEM Unified-Handle Migration Result

## Question

Can the complete direct DEM file-handle seam move from its private 64-entry
array to the T232 manager without changing original DEM service bodies or the
guest-facing AX:BP ABI shape?

## Inputs

- T232 S1 manager foundation (`9ac61269`).
- Original DEM mirrors `demfile.c`, `demfcb.c`, `demhndl.c` and their existing
  direct-context seam.
- `dem_direct_host_session.{h,c}` and `demhndl_shim.c`.

## Procedure

1. Replaced `dem_direct_host_session.handles[64]` with a session-owned
   `bx_ntvdm_host_handle_manager`.
2. Published DEM-created file handles as owned entries; delegated lookup,
   close/release, and session cleanup to the manager.
3. Preserved the original `AX:BP` register pair at the mirror/shim boundary.
   The new shim accepts only a nonzero low-word opaque ID and rejects a
   nonzero high word before provider lookup.
4. Built and ran the manager, DEM direct-session, and original DEM handle
   import fixtures using `build/M0-T232-S1/formal-r1`.

## Observations

- No DEM private `HANDLE[64]` field or 64-handle limit remains.
- `t232-s1-host-handle-manager-fixture.exe` exited zero.
- `t230-s15-direct-host-session-fixture.exe` exited zero.
- `t230-s2-demhndl-direct-import-fixture.exe` exited zero and printed its
  existing original-import success marker. Its new high-word case reaches the
  normal `ERROR_INVALID_HANDLE`/CF result rather than resolving a host value.

## Interpretation And Confidence

The direct DEM product route now uses T232's manager for open/create/FCB
publication and all `demhndl` consuming services, including close. Original
OpenNT service logic is unchanged; the representation divergence is confined
to the adapter seam. Confidence is high for DEM's direct lifecycle. COMMAND
still has its own table and is the next migration scope.

## Follow-up

S3 migrates COMMAND `54:06` and launch/redirection lookup to the same manager
implementation. S4 must decide and prove the single top-level session owner
when DEM and COMMAND are simultaneously composed, then remove every remaining
private host-handle table.
