# M0 T275 S1 P1 adapter recovery-candidate inventory

## Procedure

`Export-T275S1AdapterCandidateInventory.ps1` recursively inventories only the
three existing non-formal adapter recovery roots under `src.old/`. It records a
component-relative path, SHA-256, broad interface family and file-kind. The
tool resolves its input root before deriving the relative path; the first
review caught and corrected an unnormalized-path defect before this record was
accepted.

## Observations

- 453 reference candidates were found.
- `adapter-bop`: seven candidates — three implementation, three declaration,
  one supporting reference.
- `adapter-softpc`: 397 candidates — 48 implementation, 344 declaration and
  five supporting references.
- `adapter-win32`: 49 candidates — 10 implementation, 38 declaration and one
  supporting reference.
- No candidate enters a production root, formal build input, or active ABI in
  this delivery.

## Disposition

P1 is intentionally an inventory, not a migration. The remaining S1 work
cross-references every implementation candidate and every actual historical
caller against the exact MVDM mirror, then assigns a source-first reuse,
split, replacement or retirement disposition. Header closure will be assessed
only after that caller map exists; a broad header-tree copy is prohibited.
