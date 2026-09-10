# M63 DEM Drive and Current-Directory ABI Ledger

Status: source-derived namespace state record, 2026-08-08.

## Sources

- `src/opennt/base/mvdm/dos/dem/demgset.c`
- `src/opennt/base/mvdm/dos/dem/demdir.c`
- `src/opennt/base/mvdm/dos/dem/demerror.c`

## SVC `0Fh`: demGetDrives

`demGetDrives` has no register input. It populates the global
`PhysicalDriveTypes[26]`, updates `IsAPresent`/`IsBPresent`, and calculates
`nDrives` by probing A through Z. A and B use `GetDriveTypeOem` first to avoid
filesystem access to floppy drives; fixed drives are then filtered through
`GetPhysicalDriveType` to distinguish SUBST. Later drives use
`GetPhysicalDriveType` directly.

Its counting behavior is not a bitmap or an arbitrary mount list: it begins at
two and increments only for a contiguous prefix whose physical type is
removable, fixed, CD-ROM, or RAM disk. On the first other type it stops
counting. It writes `nDrives` with `setAX(nDrives)` and clears CF. The source
comment says AL, but the implementation writes AX; that discrepancy must be
preserved in a compatibility test rather than silently resolved by a design
assumption.

This is legacy PC/AT/ambient-Windows drive policy. It is not an acceptable
enumeration contract for a contained wrapper: the wrapper must instead derive
its configured DOS drives from its authorized profile, while retaining any
guest-visible count behavior required by the locked profile.

## SVC `13h`: demQueryCurrentDir

| Aspect | Source-established behavior |
| --- | --- |
| inputs | `DS:SI` points to a guest `CDS` structure; AL is the physical drive number (`A=0`). |
| media validation | Checks host `X:\\` with `GetFileAttributesOem`. Missing or non-directory root calls `demClientError(INVALID_HANDLE_VALUE, driveLetter)`. |
| guest state | If `CDS.CurDir_Text` is missing or not a host directory, overwrites it with `X:\\` and sets `CDS.CurDir_End=2`. |
| host state | On that reset, writes the host process special environment variable `=X:` to `X:\\`. |
| success | Clears CF; no ordinary AX result is set. |

The CDS pointer and its mutable string are raw guest mappings in the original
V86 environment. A software backend must validate the full structure and
string capacity before any reset-to-root write; the operation is a multi-field
guest publication, not merely a path lookup.

## SVC `18h`: demSetCurrentDir

| Aspect | Source-established behavior |
| --- | --- |
| input | `DS:DX` points to a path string. Its first byte must case-fold to `A` through `Z`; otherwise CF is set without an AX code. |
| host operation | Calls `SetCurrentDirectoryOem` directly on that ambient host path. Failure routes through `demClientError`. |
| mirrored state | On success it sets the host process special environment variable `=X:` to the same path. If that environment update fails, only CF is set. |
| success | CF clears. The caller owns the corresponding DOS CDS update. |

## Containment Implications

The historical implementation couples four layers that must be separated in a
non-invasive runner:

1. configured DOS drive profile and media identity;
2. guest CDS/DPB state;
3. per-session current-directory table; and
4. host process working directory plus `=X:` environment variables.

Only the first three belong in a future contained profile adapter. The fourth
is a historical product integration detail and must not mutate the ambient
host process. The adapter needs an internal per-session `drive -> root,
currentDirectory` map, path containment checks before any file capability
operation, and an atomic guest CDS update mechanism. The source does not yet
prove exact CDS size/string bounds, so that part remains a required follow-up
to the guest structure definitions and caller code.

No implementation was added. This record narrows the future namespace contract
and excludes inheriting host drive discovery or host current-directory mutation
as compatibility requirements.
