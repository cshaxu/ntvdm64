# T221 S3 Overlay namespace mutation backend

## Question

Can the four original DEM namespace-mutation owners share one source-derived,
selector-blind Overlay transaction rather than acquire independent BOP-local
write behavior?

## Inputs

- Original owners:
  - `src/opennt/base/mvdm/dos/dem/demdir.c`: `demCreateDir`,
    `demDeleteDir`.
  - `src/opennt/base/mvdm/dos/dem/demfile.c`: `demDelete`, `demRename`.
- Overlay view and COW foundations:
  `t221-s3-overlay-visible-node-foundation-001.md` and
  `t221-s3-overlay-merged-child-foundation-001.md`.
- Implementation:
  `src/bx-vdm/bx_ntvdm_dem_overlay_mutation_backend_v1.[ch]`.

## Implementation

The backend provides one operation per original owner contract:

| Operation | Private state action | Host effect |
| --- | --- | --- |
| create directory | parent/view admission then COW directory entry | none |
| delete file | visible-file admission then file tombstone | none |
| remove directory | merged-empty check then directory tombstone | none |
| rename file | read-only materialize source, private copy, source tombstone | none |
| rename directory | lazy destination-to-source relocation and old-source mask | none |

It preserves the original essential order: cross-drive rename is rejected;
equal names are rejected; source/destination/parent visibility is resolved
before state change; directory removal uses the merged visible child query.
Host materialization uses only the existing read-only root resolver. New
store `copy_file` state is private and never moves a host object.

## Verification

Whole-provider MSVC x64 `/MT` fixture passed:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\056
```

The Overlay family cases cover create, file tombstone, non-empty directory
failure, empty-directory removal, COW file rename, lazy directory rename and
old-source masking. The fixture retains its host-base preservation assertion.

## Limits

This is not a BOP closure. The next binding must retain `demCreateDir`'s EA
refusal and every original `demClientError`-shaped CPU result, map the backend
errors through the existing failure owner, and test Direct/Readonly/Overlay/
Virtual dispositions together. Open-handle delete/rename sharing and advanced
directory relocation conflict handling remain explicit cases for that binding
and its full-family regression; they must not fall through to Direct.
