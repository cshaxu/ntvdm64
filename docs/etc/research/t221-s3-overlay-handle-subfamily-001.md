# T221 S3: Overlay Handle Subfamily

## Question

Can the existing DEM handle router move the coherent Overlay subset
`50:00`, `50:02`, `50:16`, `50:1E`, and `50:27` through one typed private
backend without a Direct `HANDLE` fallback or a BOP-specific host path?

## Inputs

- The OpenNT-derived local handle contract in
  `src/opennt/base/mvdm/dos/dem/demfile.c` (seek, close, read, write and
  flush ordering).
- T221 S3 P4--P6 Overlay store, private file object, resolver and typed
  session-token records.
- The existing Direct and readonly implementations in
  `bx_ntvdm_dem_handle_partition_v1` and
  `bx_ntvdm_dem_readonly_file_service`.

## Procedure

`bx_ntvdm_dem_handle_route_partition_v1` now recognizes the existing
`OVERLAY_FILE` token kind before considering the Direct handle lookup.  It
uses only `bx_ntvdm_dem_overlay_handle_backend_v1` for the selected subfamily:

- `00` seeks and returns the resulting 32-bit position in `DX:AX`.
- `02` applies the optional final seek and closes both private file and typed
  session token.
- `16` reads private bytes and emits the normal checked guest-RAM write
  action.
- `1E` keeps the existing provider-owned gather continuation, then writes the
  copied bytes to the private file; zero-length write truncates at the current
  private position.
- `27` is a successful volatile-COW flush with no host flush operation.

The same work corrected the private object so a short overwrite preserves the
remaining base-file bytes and advances the position only to the write end.

`50:08` is deliberately *not* included: the Overlay object has no admitted
file-metadata owner.  It returns the established explicit unsupported result
(`AX=1`, carry) rather than deriving timestamps or reaching Direct state.

The whole-provider fixture constructs a private Overlay file, adopts one typed
token, verifies seek, gather/write, readback, `08` refusal, flush, close and
stale-token rejection.  Its test link uses a four-megabyte stack because this
historical all-family fixture instantiates several bounded provider states in
one process; it does not alter runtime stack policy.

## Observation

The fixture source-built and passed with MSVC x64 `/MT`:

```text
powershell.exe -ExecutionPolicy Bypass -File \
  tools/probe/Invoke-T202S4DemWholeProviderProbe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T221-S3\027
```

The test initially exposed a private-file short-write length defect; the
corrected regression reads `COWe` after writing `COW` over the base `base`.
No host file is opened or modified in this subfamily regression.

## Interpretation

This closes only the Overlay *handle-operation* checkpoint.  Namespace
open/create, FCB, search, deletion/tombstones, metadata and Virtual remain
outside it.  Thus it is neither a 29-service provider closure nor evidence
that an Overlay guest path is admitted by an ordinary BOP open.

## Follow-up

Admit the namespace/FCB/search migration only as its own complete provider
subfamily with resolver adoption, source/failure map and all-mode regression.
