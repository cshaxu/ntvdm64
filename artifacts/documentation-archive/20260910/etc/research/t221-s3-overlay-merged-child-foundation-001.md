# T221 S3 Overlay merged-child foundation

## Question

Can the Overlay view determine directory emptiness from the same visible
namespace used by lazy rename, rather than asking the host root directly?

## Inputs

- `t221-s3-overlay-visible-node-foundation-001.md`.
- `src/bx-vdm/bx_ntvdm_dem_overlay_namespace_view_v1.[ch]`.
- Original `demDeleteDir` owner in
  `src/opennt/base/mvdm/dos/dem/demdir.c`.

## Implementation

`bx_ntvdm_dem_overlay_namespace_view_v1_directory_empty` first confirms the
visible node is a directory, resolves its effective relocated source, and
uses bounded admitted-root enumeration only for that base directory. Each
base child is re-queried through the visible view, so tombstones hide base
children. It then checks direct COW children at both visible and effective
paths. The result is therefore a merged view suitable for the original
`RemoveDirectoryOem` empty-directory decision.

No host create/remove/rename operation is used. Enumeration capacity or a
read failure returns an explicit scalar failure rather than treating an
unknown directory as empty.

## Verification

The whole-provider MSVC x64 `/MT` fixture passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\055
```

The Overlay checks prove a private directory is empty after lazy relocation,
then non-empty after one private visible child is added. The existing fixture
also proves the admitted host-base file remains unchanged.

## Follow-up

Implement one selector-blind Overlay namespace mutation transaction using the
visible-node and merged-child owners for `demCreateDir`, `demDelete`,
`demDeleteDir`, and `demRename`; bind the four services only after its family
tests cover source ordering and error results.
