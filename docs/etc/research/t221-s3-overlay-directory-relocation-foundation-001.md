# T221 S3 Overlay directory-relocation foundation

## Question

Can the DEM Overlay provider represent the lazy source-tree identity needed
for a later source-derived directory rename without copying or changing the
admitted host directory?

## Inputs

- State-model admission: `t221-s3-overlay-namespace-mutation-state-model-001.md`.
- Original source: `src/opennt/base/mvdm/dos/dem/demfile.c` (`demRename`).
- Implementation: `src/bx-vdm/bx_ntvdm_dem_overlay_store_v1.[ch]`.
- Focused fixture: `tests/bx-vdm/bx_ntvdm_dem_overlay_file_v1_test.c`.

## Implementation

The existing session-private Overlay store now owns bounded relocation records:

```text
visible destination prefix -> effective source prefix
```

Both sides are admitted DOS-relative paths on one drive. The store resolves
the longest destination prefix through a bounded chain, rejects self and
destination-inside-source moves, and tears the records down with the session.
It exports no host path, handle, BOP selector, or guest value.

This is deliberately not a `50:17` route. It only supplies the state primitive
needed by the future selector-blind mutation backend. In particular it does
not yet apply the old-source directory tombstone, merge COW additions with a
relocated base enumeration, or change search/check-path behavior.

## Verification

Focused source-built MSVC x64 `/MT` fixture:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T221S3DemOverlayFileProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\046
```

Result: passed. It proves `WORK\\MOVED\\CHILD` resolves lazily to
`WORK\\DIR\\CHILD`, proves a chained relocation reaches the original effective
source, and rejects a destination nested beneath its source.

The complete current whole-provider fixture also passed under MSVC x64 `/MT`:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\047
```

Its recorded result is `runExitCode: 0`, `passed: true`; it is a composition
build/regression check, not a native guest trace.

## Interpretation and follow-up

This establishes the private lazy-tree foundation only. The next unit must
add one merged visible-node/child-query backend and bind the original
create/delete/rmdir/rename ordering and errors to it as one family. No claim
is made that directory rename, the namespace-mutation subfamily, or the DEM
package is closed.
