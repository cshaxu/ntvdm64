# T97 S2 Immutable Namespace and File-Token Contract

## Decision

The existing `50:12/00/16/02` readonly-file candidate is **withdrawn from the
adapter runtime** pending S3/S4. It remains a focused, adapter-local fixture
for the following narrow replacement design; it is not a current BOP provider.
This eliminates the former dual state where a partial source-derived helper
could resume real guest code while the DEM component and error contract were
still unclosed.

`50:42` is also absent at every layer. The prior T95 FASTREAD experiment is
superseded: direct `DemDispatch` evidence proves the original slot is
`demNotYetImplemented`, and the helper plus its test assertion have been
removed.

## Bounded Resource Contract

The proposed provider receives only immutable CLI/BYOB-admitted bytes and
profile declarations. Its initial namespace is exactly:

| Guest entry | Origin | Mutability |
| --- | --- | --- |
| `\COMMAND.COM` | profile-verified COMMAND image | immutable |
| `\CONFIG.SYS` | declared minimal materialization | immutable |
| `\AUTOEXEC.BAT` | declared empty materialization | immutable |
| `\TARGET.COM` or `\TARGET.EXE` | profile-verified target image | immutable |

All entries live on the single profile-declared DOS drive. They are bytes and
adapter-local metadata, never a host path, file descriptor, Win32 handle, or
Bochs object. There is no host directory scan, host current-directory call,
rescan, mutation, redirector, named pipe, or raw drive access.

The source-derived token design is a typed replacement for the historical
`HANDLE` split across `AX:BP`; it is not a host-handle encoding. A later
provider must issue opaque tokens only after an exact resource match, bind
seek/read/close to the same token, invalidate it on close, and reject stale or
out-of-profile tokens. It must preserve the copied BOP/event and checked
guest-memory transaction boundary already used by the adapter.

## Source Comparison and Gaps

OpenNT `demOpen` (`demfile.c`) accepts `DS:SI`, EA flag `AL`, access/share mode
`BL`, normalizes path syntax, returns a split handle plus file size/pipe state,
and reports source errors through CF/AX. `demChgFilePtr`, `demRead`, and
`demClose` (`demhndl.c`) consume that split handle and use the file-position,
ZF, `DS:DX`, `CX`, and CF/AX contracts.

The focused candidate preserves only these bounded facts:

- `50:12` consumes a copied fixed-size `DS:SI` pathname read and returns a
  private token and bounded size;
- `50:00` applies a checked signed seek to that token;
- `50:16` has a preflighted, bounded guest-RAM write and returns byte count in
  AX; and
- `50:02` invalidates the token and changes only documented result fields.

It does not yet close multiple simultaneous opens, the complete `BL` sharing
and access matrix, source pathname canonicalization/error mapping, hard-error
and extended-error data, or all original register side effects. Those are
functional provider requirements, not cosmetic refinements. Therefore runtime
dispatch was removed rather than treating `ERROR_ACCESS_DENIED` or a fixed
single-token namespace as OpenNT-equivalent behavior.

## Verification

`bx_ntvdm_dem_readonly_file_service_test` compiles under strict C11 and
verifies the isolated O/S/R/C contract: copied path input, opaque token,
checked seek, bounded read transaction, EOF, close, and stale-token rejection.
The compile uses `_CRT_SECURE_NO_WARNINGS` only to suppress existing UCRT
`wcscpy` deprecation diagnostics; no runtime behavior depends on that define.
`Test-BopLegacyEndpointRegister.ps1` remains the no-new-recognizer gate.

## Handoff

S3 must close the paired pathname-search/DTA contract and record the exact
path/mode/error matrix needed for one coherent namespace provider. S4 may then
replace the withdrawn candidate through the DEM plane, or retain unavailable
behavior. It may not re-enable the old helper directly.
