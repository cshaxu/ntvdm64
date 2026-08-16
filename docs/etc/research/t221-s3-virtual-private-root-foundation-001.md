# T221 S3 P31: Virtual private-root foundation

## Question

Can the Virtual profile establish a DOS-visible private root without admitting
or consulting a real host namespace?

## Change

`bx_ntvdm_dem_virtual_namespace_view_v1` is a selector-blind view over the
existing private store. It recognizes the configured drive root, private file
and directory entries, and enumerates direct children in DOS-name order. It
accepts no host namespace, root handle or host path parameter.

This deliberately reuses bounded private store entries and the established
adapter-local directory-entry layout, but not Overlay's host-base lookup or
copy-on-write resolver.

## Verification

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/probe/Invoke-T221S3DemVirtualNamespaceProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\100
```

The x64 MSVC `/MT` probe compiled and passed. It creates private `BIN` and
`BIN\TEST.COM` entries, verifies root enumeration and file lookup, then
verifies that an absent private file remains absent. The probe declares
`hostIo=false` and uses no host namespace source; it is not guest execution.

## Limitation and follow-up

This is only the shared private-root foundation. It does not yet bind the
virtual view to opaque file tokens, namespace mutations, FCB, search, startup
inputs or a CLI/engine option. Virtual continues to be unavailable from the
current executable composition until that full package closes.
