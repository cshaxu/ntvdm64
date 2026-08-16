# T222 S2 P1 — DEM drive-view state-owner migration

## Change

`bx_ntvdm_dem_package_session_v1` no longer owns parallel `gset`, mutation
profile, CWD and boot-drive fields.  They now have one package-local owner:
`bx_ntvdm_dem_drive_view_provider_v1`.

The provider holds copied profile, admitted drive/volume snapshots, CWD
context and selected boot drive.  Existing `gset` algorithms remain internal
to that owner for later route migration.  No guest pointer, host handle, Bochs
object, selector or BOP service ID enters the provider state.

## Source and boundary

This is the structural implementation of T222 S1 P2, based on OpenNT
`demgset.c`/`demdir.c` shared one-thread drive/CWD state.  The original source
cannot compose because it stores raw `GetVDMAddr` pointers and uses CCPU
register macros.  The adapter preserves only copied, checked replacements.

`boot_namespace_composition` and the retained native boundary fixture were
updated as consumers of the new owner; no BOP semantics changed. Overlay and
Virtual behavior is not added or modified.

## Verification

The initial source closure correctly exposed stale external consumers of the
removed session fields; they were migrated to `dem.drive_view`.  The repaired
normal package-session regression was built and run with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T222-S2\003-provider-state-session `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The MSVC x64 `/MT` executable returned exit `0`.  This regression exercises
the normal ingress/package session and confirms the T221 29-service provider
remains intact after the state ownership migration.

## Follow-up

P2 migrates the `0D,0E,0F,10,41` observation/identity group through this
provider.  That migration must replace the remaining detached deferred leaves,
not add a second dispatcher.
