# T221 S3 FCB wildcard Overlay enumeration audit

## Question

What single provider seam is required before `demDeleteFCB` (`50:07`) and
`demRenameFCB` (`50:20`) can operate in Overlay without a Direct host fallback?

## Inputs

- `src/opennt/base/mvdm/dos/dem/demfcb.c` (`demDeleteFCB`, `demRenameFCB`).
- Current Direct-only wildcard partition and Overlay store/view/mutation
  backends.

## Source and ABI map

Both historical handlers enumerate the source directory, filter the FCB
pattern and attributes, then perform a mutation for each visible match. Delete
retains partial-success semantics: excluded attributes become `ACCESS_DENIED`,
while a successfully deleted sibling produces success. Rename substitutes the
destination template per visible DOS 8.3 name and rejects a same-name target.
Neither handler has a guest-memory or CPU-mechanics requirement after its
already-copied input paths arrive at the provider.

## Required provider seam

The missing seam is one selector-blind **visible-directory enumeration**:

- Resolve a visible directory through lazy relocation.
- Enumerate the admitted host base read-only.
- Re-query every candidate through the Overlay visible-node model so private
  files, directories, metadata, tombstones and old relocation sources decide
  visibility.
- Add private direct children (including children materialized at the
  effective relocated source), deduplicated by DOS name.
- Return only existing adapter-local `bx_ntvdm_host_namespace_entry_v1`
  records—never host paths or handles.

This seam belongs below the wildcard partition and above the Overlay mutation
backend. It is reusable by the separate pathname/FCB search package, but that
package is not admitted by this P.

## Provider disposition

| View | `50:07/20` disposition |
| --- | --- |
| Direct | Retain original Win32 enumeration/mutation route. |
| Readonly | Original-shaped `ACCESS_DENIED`. |
| Overlay | Bind both services together only after the visible enumeration seam and full-family regression exist. |
| Virtual | Explicit unavailable; no host fallback. |

## Follow-up

Implement and test the selector-blind merged enumeration, then bind both
wildcard services through the existing private mutation backend in one change.
