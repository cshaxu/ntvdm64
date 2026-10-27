# S10 shard 2: DEM initialization, dispatch and file cohort

## Reconciled source owners and differences

Original/current SHA256 identities were rechecked against the frozen rows;
all three selected modified source files remain partial.

- `dem.c::DemInit`, DIV-154, preserves its original allocated destination and
  MAX_PATH failure check while replacing the initial host system-directory
  query with `NtvdmGetSystemDirectoryA`. It returns FALSE for allocation,
  provider failure or overlength before the WOW fast return. T427 source
  ownership audits identify the package-system-directory boundary; they do
  not establish all current-package allocation, malformed-root and WOW failure
  behavior. No alternate DEM path policy is implied.
- `demdisp.c` retains the `DemDispatch` service table and original invalid
  index carry failure. Fast selectors 42/43 are changed from the NT kernel
  VDM trap owner to a local CF=1 result (DIV-188/204). The original NTDOS
  `handle.asm` explicitly branches on CF to normal SVC_DEMREAD/DEMWRITE.
  T409's source verifier checks these table/guest branches and its x86 build
  passed historically; its own report explicitly says a live fast-I/O hit
  was not demonstrated. Do not convert a static fallback proof into a guest
  runtime or performance-equivalence claim.
- `demfile.c` replaces the retained current-PDB alias with a short numeric
  guest lease (DIV-006), resolves the SFT head from numeric 16:16 at the
  synchronous VDD boundary (DIV-007), and copies bounded NUL-terminated DOS
  paths before original open/create host operations (DIV-195/231). Read/seek,
  canonicalization and CF/AX error conversion remain in the original service.
  A failed path copy returns ERROR_INVALID_ADDRESS through `demClientError`.
  The original SFT/JFT helpers still use synchronous `Sim32GetVDMPointer`;
  do not describe them as exclusively lease-backed without tracing their
  pointer lifetime and the separate VDD shadow/commit callers.

## Registration reconciliation

The source-visible DIV-231 marker on `demCreateCommon` is not found in the
current README registration. README entries DIV-177 (one of several duplicate
177 uses), DIV-205 and DIV-217 refer to DEM diagnostic hooks at `demfile.c`,
but the current original/current re-diff and source search found no such
`mvdm_softpc_report` call in that file. These are candidates for stale or moved
records, not proof the historical diagnostics never ran. Reconcile each source
location and its approved cohort before updating metadata; do not silently
reuse numbers or delete other owners' records.

## Evidence gaps and scoped verification

Existing T409/T420/T427 reports support the source and historical build
boundary; they are not a fresh current-artifact test. A finite completion
matrix should exercise both fast selectors through immutable guest normal
read/write and observable offsets/error cases; independently exercise DEM
open/create path maximum, missing NUL, invalid/stale lease, and original CF/AX
error results; and verify SFT/JFT lookup, commit/discard and session-retirement
for both selected VDD callers. Root-directory provider missing/overlength and
WOW/non-WOW startup must be matched to the deployed image. These are specific
evidence gaps, not newly established production regressions or authorization
to rebuild guest. No tests were run or source behavior changed in this review.
