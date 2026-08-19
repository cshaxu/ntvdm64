# T229 S2 P23 — DEM Disk Reset COMMAND Composition

## Question

Can DEM `50:23` retain the OpenNT disk-reset continuation without inventing a
host raw-media implementation in the CLI composition?

## Original contract

`src/opennt/base/mvdm/dos/dem/demmisc.c:600-615` defines `demDiskReset`.
It invokes `HostFloppyReset`, invokes `HostFdiskReset`, then clears the
`WORD` at `pFDAccess`.  `src/opennt/base/mvdm/dos/command/cmdmisc.c:588-602`
shows that COMMAND `54:05` registers this location from guest `DS:CX`.

The historical reset calls belong to the former SoftPC/raw-media product
composition.  No raw media owner is admitted in this CLI profile, so this
closure must not claim that either host reset happened.

## Implemented composition

`src/bx-vdm/bx_ntvdm_dem_disk_reset_service_v1.c` is selector-blind except
for its typed DEM ingress validation.  It accepts only real-mode vector-6
`50:23`, validates the registered guest-RAM word and emits one checked,
two-byte zero-write mechanical action with an ordinary resume at `RIP + 4`.
`bx_ntvdm_boot_namespace_composition_v1.c` obtains the pointer solely from
its prior COMMAND `54:05` registration and asks bx-mantle to perform that
opaque action.

This preserves the source-visible `*pFDAccess = 0` state transition.  It does
not put COMMAND, BOP, DOS, or media semantics in Bochs/bx-mantle; nor does it
create a raw host-device fallback.

## Profile disposition

The action mutates only guest RAM, not the host namespace or a host device.
Therefore Direct and Readonly intentionally use the same disposition.  The
shared profile ABI remains the seam for a later admitted Overlay policy; this
change supplies no Overlay behavior.

## Reproduction

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T229-S2\formal-disk-reset-20260818r5
ninja -C build/M0-T229-S2/formal-disk-reset-20260818r5 `
  bin/t229-s2-dem-disk-reset-service-fixture.exe
& build/M0-T229-S2/formal-disk-reset-20260818r5/bin/t229-s2-dem-disk-reset-service-fixture.exe
```

The final graph target builds `bx-vdm` with the new service and the dedicated
fixture verifies the accepted action, ordinary continuation, wrong-service
rejection, and one-megabyte guest-RAM bounds.  The fixture exited zero on
2026-08-18.

## Boundary

This closes the source-proven guest-state part of `50:23` only.  A future
raw-media feature requires a separately admitted bx-mantle owner and its own
Direct/Readonly safety contract; it cannot be inferred from this DEM action.