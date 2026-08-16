# T221 S3 P47 — Package-session declared search chain

## Scope

The package-session fixture now exercises the original pathname-search
transport for the two views that own the declared merged snapshot: Direct and
Readonly.

After the common `50:1B` DTA/PDB registration, it installs an original-shaped
128-byte `C:\\*.*` pathname input, a DTA far output pointer, and a copied
CurrentPDB word in checked guest RAM. It then sends `50:09` (PATHFIRST) and
`50:0B` (PATHNEXT) through BOP ingress, registry selection, and the DEM
package session. The first request writes the 43-byte result/continuation to
the DTA target; PATHNEXT consumes that target and the same CurrentPDB.

## Disposition boundary

Direct and Readonly intentionally use the declared merged snapshot. Overlay
and Virtual must enumerate their private namespace views; treating the
declared snapshot as their result set would hide the profile boundary and is
therefore prohibited. Their positive search legs remain separate work.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\144-package-session-declared-search `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The manifest records MSVC x64 `/MT` and source-built compile/link/run exit
code `0`.

## Remaining work

This adds only the Direct/Readonly pathname search pair. FCB first/next,
Overlay/Virtual private enumeration, namespace mutation and the full
29-identity positive/negative matrix remain open.
