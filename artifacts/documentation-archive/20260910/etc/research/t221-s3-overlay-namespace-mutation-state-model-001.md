# T221 S3 Overlay namespace-mutation state model

## Question

What private provider state is necessary to recover the complete Overlay
namespace-mutation subfamily (`50:04`, `50:05`, `50:06`, and `50:17`) without
mutating an admitted host root or accidentally reducing directory rename to a
file-only operation?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demdir.c`: `demCreateDir` and
  `demDeleteDir` call the host directory operations and return their original
  `demClientError` result on failure.
- `src/opennt/base/mvdm/dos/dem/demfile.c`: `demDelete` calls
  `DeleteFileOem`; `demRename` first rejects cross-drive source/destination
  with `AX=11h`, rejects equal paths with `AX=5`, then calls `MoveFileOem`.
  `MoveFileOem` is not a file-only contract: a directory path is eligible.
- `src/bx-vdm/bx_ntvdm_host_namespace.[ch]`: admitted-root operations and
  enumeration keep root handles private, but Direct mutation operations alter
  the host filesystem.
- `src/bx-vdm/bx_ntvdm_dem_overlay_store_v1.[ch]` and
  `bx_ntvdm_dem_overlay_namespace_backend_v1.[ch]`: current private file COW,
  exact file/directory tombstones, and opening subfamily.

## Procedure

1. Compared the four original owners' path, ordering, and failure contracts.
2. Traced each current Overlay state representation against a rename of a
   host-base directory which has child files never previously opened by the
   Overlay session.
3. Compared that case with create/delete/rmdir visibility and the existing
   host-root enumeration boundary.

## Observations

1. An exact COW directory entry and an exact directory tombstone are necessary
   but not sufficient. They cannot say that `OLD\\CHILD.EXE`, which remains
   only in the host base, is now visible as `NEW\\CHILD.EXE` after
   `MoveFileOem("OLD", "NEW")`.
2. Moving only the COW entries would lose unmaterialized base descendants;
   moving only a root marker would leave the old base path visible. Either is
   a semantic regression from the original directory-capable `MoveFileOem`
   route.
3. `RemoveDirectoryOem` requires the *merged* view to be empty. COW children,
   visible base children, and base children masked by a COW tombstone must be
   distinguished. Existing `has_descendant` observes only COW entries, while
   `host_namespace_v1_enumerate` can supply the read-only base entries.
4. The current Overlay resolver copies a base file on first open. It does not
   provide a representation for an unmaterialized base directory subtree, so
   it cannot be reused as a directory-rename substitute.

## Required provider model

The selector-blind mutation backend must own one transactionally updated,
session-private namespace model with all of the following:

| State | Meaning | Required consumers |
| --- | --- | --- |
| COW file / COW directory | Created or materialized Overlay node. | open/create, delete, rmdir, search, metadata |
| file / directory tombstone | Hides the matching base or COW node. A directory tombstone also masks its inherited subtree. | path lookup, enumeration, rmdir, rename |
| directory relocation | Maps a visible destination subtree to an earlier source subtree while keeping the old source path masked. | lookup, open resolver, enumeration, rename |
| merged child query | Combines the effective base directory after relocation with COW additions and tombstones. | rmdir emptiness, search, check-path |

The relocation record is a provider-private mapping of `(drive, destination
prefix) -> (drive, effective source prefix)`, plus an old-source directory
tombstone. It is not a guest path, host handle, BOP selector, or Bochs
concept. Lookup and enumeration must resolve the longest applicable relocation
before reading the admitted root; COW entries are then applied at the visible
destination path. This preserves lazy host-base contents without copying a
whole host directory tree into memory.

## Disposition and failure rules

- The backend is used only after shared profile selection chooses Overlay.
  Direct continues to call the admitted host namespace; Readonly remains
  `AX=5, CF=1`; Virtual remains a separate, explicit view.
- `50:04` retains the original `BX:SI != 0` unsupported-EA refusal before
  namespace mutation.
- `50:17` retains cross-drive `AX=11h`, identical-path `AX=5`, and source
  `demClientError`-shaped failures. It must not silently implement a
  file-only rename.
- All admitted-root reads use the existing bounded relative-path/root-handle
  capability. Overlay performs no create, remove, rename, attribute, or time
  mutation on the host root.
- If the bounded model cannot represent a relocation, merge, capacity, or
  conflict case, it returns an explicit source-shaped failure; it never falls
  back to Direct host mutation.

## Interpretation

The existing exact-entry store is a useful foundation, but routing
`50:04/05/06/17` through it now would be a partial directory implementation
that violates the source-first owner contract. The next implementation unit is
therefore one selector-blind Overlay namespace model/backend, not four BOP
handlers and not a special-case host-directory copy.

## Follow-up

Implement the private relocation and merged-child-query state APIs with
focused Direct-host-preservation tests; then bind all four services together in
`bx_ntvdm_dem_namespace_partition_v1.c` and execute the complete namespace
mutation family regression. Search, metadata, and `50:44` must adopt the same
visible-view query in their later admitted subfamilies; they are not implicitly
closed by this record.
