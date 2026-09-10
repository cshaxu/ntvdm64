# T222 S2 Drive-View CWD Normal-Session Regression

## Question

Does the Direct/Readonly drive-view provider handle the complete CWD group
through normal DEM package-session ingress without constructing the unrelated
file/search whole provider?

## Inputs

- OpenNT references: `src/opennt/base/mvdm/dos/dem/demdir.c` and
  `src/opennt/base/mvdm/dos/dem/demdisp.c`.
- Current CWD provider: `bx_ntvdm_dem_cwd_service_v2.c` and
  `bx_ntvdm_dem_drive_view_provider_v1.c`.
- Existing fixed-width mechanical action ABI and package-session executor.

## Procedure

Added `bx_ntvdm_dem_drive_view_session_v1_test.c`, using a 1 MiB fake guest
RAM backing for the existing mechanical read/write ABI.  It creates a normal
boot namespace plane and DEM package session, binds only the admitted C:
host namespace via `set_drive_view_host_namespace`, and asserts no whole
provider is constructed.  It enters BOP `50:13`, `50:18`, and `50:1A`
through ingress/registry/package-session dispatch under Direct and Readonly
profiles.

The fixture was compiled and linked with MSVC x64 `/MT` in the disposable
`build/M0-T222-S2/020-drive-view-session-verified` root.  Its executable exited
zero.

## Observations

- Direct `50:13` returns the checked 71-byte CDS layout for `C:\\`.
- Direct `50:18` and `50:1A` resume successfully after the bounded guest
  path read; `50:1A` uses `DL=2` for C:.
- Readonly `50:13` remains an observation and succeeds; Readonly `50:18`
  and `50:1A` return the source-shaped `AX=5`, `CF=1` refusal.
- The first normal-session run exposed an executor defect: `write_tx` tried
  to preflight every transaction before checking whether it had a guest-RAM
  write.  The multi-write ABI correctly rejects a zero-write record, but
  `demSetCurrentDir` and `demSetDefaultDrive` are valid CPU-only results.
  The executor now separately validates the transaction header, boundary,
  CPU state, result and forward resume RIP before accepting a zero-write
  transaction.  Transactions with writes retain the original preflight and
  checked mechanical write path.

## Interpretation

The CWD group now has one Direct/Readonly owner and one normal-session route:
the provider owns service semantics; package session owns bounded guest-memory
transport; CPU-only completions do not impersonate memory writes.  This does
not close T222: DTA and the remaining drive/volume/clock groups still require
normal-session family coverage.  Overlay and Virtual behavior remains
unchanged.

## Follow-up

Migrate and test the checked DTA group next, then combine all fifteen
identities in the required Direct/Readonly package regression.
