# T229 S2 P6 — DEM demCreateNew Direct/Readonly conformance

OpenNT `demCreateNew` calls `demCreateCommon` with `CREATE_NEW`. On an existing
file, its `demClientError` returns the actual non-hard `GetLastError()` in AX
with CF set. The current Windows host reports `ERROR_ALREADY_EXISTS` (183);
other CreateFile implementations may report `ERROR_FILE_EXISTS` (80). Both are
source-compatible results of the original error path.

The formal whole-provider fixture first creates a fixture-owned file via
`50:03`, then calls `50:22` through ingress, checked path/PDB reads and the
provider. Direct preserves the existing-file error and the file; Readonly
returns `AX=5, CF=1` before host creation and leaves no file. Overlay behavior
is not added.

All Direct/Readonly formal fixture modes exited zero on 2026-08-18. This closes
only collision/refusal behavior; create-new success and attribute variants
remain in the file package matrix.