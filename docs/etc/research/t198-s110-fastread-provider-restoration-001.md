# M0 T198 S110: Fast-Read Provider Restoration

## Change

S110 removes the S106 no-op family from the active `bx-vdm` composition,
manifest, build probe and focused test membership.  It restores the already
admitted `bx_ntvdm_dem_fastio_provider_v1_dispatch` call at the namespace
plane's `50:42` branch.  That provider remains profile-gated and uses the
existing declared read-only namespace plus checked bulk guest-RAM transaction.

`50:43` has no branch in the plane and therefore remains pass-through; no
write capability was introduced.

## Verification

`tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` source-built, linked and
ran the complete C-only current composition under MSVC x64 `/MT` at
`artifacts/build/t198-s110-fastread-restore-x64-r2`.

The composition fixture proves:

- `50:42`, with a declared namespace handle and ZF clear, produces its checked
  three-byte `REM` write and AX=`3`;
- `50:43` produces no mechanical action and a pass-through result;
- the restored wrapper object participates in the exact build closure.

The first r1 attempt exposed the omitted provider declaration in the compact
plane source; S110 adds its existing header and r2 is the accepted closure.
No guest instruction loop ran.

## Boundary

No Bochs, mantle, ROM, VGA, device, host-filesystem, CLI capability or ABI
change occurred.  S111 alone may make one bounded native observation.
