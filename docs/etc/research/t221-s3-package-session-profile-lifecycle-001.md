# T221 S3 P45 — Package-session profile lifecycle

## Finding

The first attempted four-profile package-session matrix exposed a real
lifecycle defect, not a Readonly provider limitation.  A boot namespace plane
may borrow a DEM CWD context from a package session.  Its initializer rebuilt
the embedded boot provider but left that borrowed pointer (and other
session-owned transient fields) intact.  After the first session was torn
down, reuse of the plane storage could therefore validate a stale pointer and
reject the next profile before normal BOP ingress.

`bx_ntvdm_boot_namespace_plane_v1_initialize` now clears the complete plane
before initializing its owned provider.  It neither changes BOP selection nor
adds a profile-specific behavior: every new package session starts with no
borrowed DEM context, no DTA, no drive snapshot, and no pending guest action.

## Regression

`tests/bx-vdm/bx_ntvdm_dem_whole_provider_session_v1_test.c` now constructs a
fresh boot plane and DEM package session for each Direct, Readonly, Overlay,
and Virtual profile.  Each session registers the shared DEM mutation classes,
installs the same admitted host `C:` namespace, and sends `50:00`, `50:47`,
and `50:48` through BOP ingress, registry selection, and package dispatch.

The witness proves the common session-installation path exists for all four
profiles.  It also preserves the already-established dispositions: `50:00`
with a zero opaque token returns `AX=6, CF=1`, while `50:47/48` retain their
Redirector-deferred invalid-handle result.  It does **not** claim the complete
29-identity package matrix; its purpose is lifecycle isolation for the matrix
designed in P44.

## Reproducible evidence

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\M0-T221-S3\142-all-profile-session-matrix-fixed `
  -Fixture dem-whole-provider-session -HostArchitecture x64
```

The manifest reports MSVC x64 `/MT`, source-built compile/link/run exit code
`0`, and excludes `bx-core`, `bx-mantle`, Bochs binaries, and an OpenNT runtime
from the focused closure.

## Remaining work

P45 establishes the common four-profile session lifecycle only.  The next
matrix work must use this lifecycle to exercise the namespace, handle, search,
and FCB identities and their positive/negative behavior through normal
package-session routes.  Native trace remains outside S3 until that family
regression closes.
