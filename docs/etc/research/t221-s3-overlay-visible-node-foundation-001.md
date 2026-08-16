# T221 S3 Overlay visible-node foundation

## Question

Can one selector-blind provider component determine whether a visible Overlay
path is a file, directory, or absent after applying COW entries, tombstones,
and lazy directory relocation, without mutating or exposing the host root?

## Inputs

- Original source and state-model records:
  `src/opennt/base/mvdm/dos/dem/demdir.c`,
  `src/opennt/base/mvdm/dos/dem/demfile.c`, and
  `t221-s3-overlay-namespace-mutation-state-model-001.md`.
- Existing admitted-root capability:
  `src/bx-vdm/bx_ntvdm_host_namespace.[ch]`.
- New provider component:
  `src/bx-vdm/bx_ntvdm_dem_overlay_namespace_view_v1.[ch]`.

## Implementation

`bx_ntvdm_dem_overlay_namespace_view_v1_query` owns this order:

1. mask an explicitly tombstoned visible directory subtree or old relocation
   source subtree;
2. return an exact visible COW file/directory or exact tombstone result;
3. resolve the visible name through the private longest-prefix relocation map
   and use any effective-source COW node;
4. only then query the admitted host root read-only for a file or directory.

The host namespace now also offers a read-only `directory_exists_ex` scalar
result so this source-derived decision preserves a usable native failure value
without exporting a directory handle. No BOP, guest pointer, host path, or
host handle enters this component.

## Verification

The whole-provider MSVC x64 `/MT` fixture passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\051
```

The focused Overlay block proves an existing host-base file remains visible
through the COW entry, an Overlay-created directory is visible at its moved
destination, and the original source name becomes absent in the same view.
The fixture's existing host-base preservation check remains part of this run.

## Limits and follow-up

This is a node query, not a merged directory enumerator. It does not yet
provide child enumeration, parent checks, non-empty-directory decisions,
rename transactions, or BOP routes. Those operations must use this same view
owner rather than bypass it with raw `host_namespace` calls.
