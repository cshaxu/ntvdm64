# S10 shard 2: DEM divergence-registration reconciliation

Audit of current source and Git history, 2026-10-27. The metadata corrections
below have been applied to `src/mvdm/README.md`; no implementation behavior
or product code was removed. Duplicate active identifiers remain separately
traceable and are not renumbered by this correction.

| Cohort | Current source and historical evidence | Precise proposed registration action |
| --- | --- | --- |
| DIV-231 create/write memory boundary | Current `demfile.c:647` copies DS:SI NUL-terminated pathname with the bounded session guest-location API; failure is mapped to `ERROR_INVALID_ADDRESS` through `demClientError`. Current `demhndl.c:309` takes a read lease for nonzero CX input; CX=0 follows original SetEndOfFile route, and failure/success/disk-full branches release the lease. Both were added by `62a3720e7` and retained now. README has no DIV-231 row. | Add an active DIV-231 row naming exactly both mirror files and `mvdm_guest_location` provider; document original create/write ordering, MAX_PATH bound, zero-length exception, error and release behavior. Link T420 S34 as historical scenario evidence, not blanket current-package proof. |
| DIV-177 demOpen diagnostic | README:1277 describes `mvdm_softpc_record_dem_open` in `demfile.c`; commit `a2ec4d531` removed its entry, failure and success calls. No current source call/helper under that name was found. T420 S13 explicitly describes removing default-off observations. | Mark this specific DEM diagnostic row retired at T420 S13, with the commit and retained historical evidence; remove it from the active mirror-diff inventory only if the document format preserves an explicit historical reference. Do not change active demOpen DIV-195. |
| DIV-177 COMMAND diagnostic | README:1423 independently reuses 177 for table-call attribution; canonical snapshot `622b71cda` contained `mvdm_softpc_record_command_call` in `cmddisp.c` and `cmdmisc.c`, commit `52d701d6b` retired these COMMAND observers in T416 S4, and current files lack those calls. | Mark this specific COMMAND diagnostic row retired at T416 S4 with commit and historical report; leave original COMMAND dispatch and active DIV-111/194/324/331 untouched. |
| DIV-177 SIM32 selector conversion | README:1292 and current `softpc.new/host/src/sim32.c:247` identify the still-present CPU40 protected-mode numeric-address boundary. | Keep this active registration and provider paths. It is not the retired DEM/COMMAND diagnostic despite the duplicate identifier; independently resolve uniqueness as a reviewed metadata change rather than deleting all 177 entries. |
| DIV-205 DEM create/read/write diagnostics | README:1443 describes default-off create/write records. `a2ec4d531` removed `mvdm_softpc_record_dem_create`, `...dem_write` and the related read/seek calls from demfile.c/demhndl.c; current search finds no `mvdm_softpc_record_dem_*` calls. | Mark only this diagnostic row retired at T420 S13 and retain historical provenance. Do not remove or relabel the later active DIV-231 create/write memory boundary. |
| DIV-217 demChMod diagnostic | README:1447 describes entry and result traces. `a2ec4d531` removed `mvdm_softpc_record_dem_chmod` from entry, success and failure; no current call remains. | Mark this specific diagnostic row retired at T420 S13, preserving historical evidence and original demChMod behavior. Do not imply chmod capability was removed. |

Evidence limitations: Git history and current source searches establish whether
these named hooks are present, not that every diagnostic semantic or DEM
runtime behavior passed. Existing `MVDM-HOST-DIV-177` is a known duplicated
namespace; the active SIM32 boundary must remain independently traceable.
The requested new registration should separately validate current
artifact-matched normal/failure and lease retirement before claiming complete
DEM restoration. No guest was modified and no tests were run.
