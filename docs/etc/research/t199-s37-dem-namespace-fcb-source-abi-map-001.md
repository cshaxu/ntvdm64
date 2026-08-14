# T199 S37 DEM Namespace and FCB Source/ABI Map 001

## Scope

This is the admission map for the next two S37 provider packages.  It derives
owner grouping from OpenNT's `demdisp.c` table and behavior from the original
component sources; it does not use native trace order.

| Package | Original source owners | BOP identities | CLI disposition |
| --- | --- | --- | --- |
| Readonly namespace handles | `demhndl.c`, `demfile.c`, `demdir.c` | `50:00-06`, `08-09`, `0B-0C`, `12-13`, `16-18`, `1E`, `22`, `27` | bounded immutable open/read/close/seek/search and root CDS; every mutation or writable-handle requirement is a contained source-derived terminal result |
| FCB/search | `demfcb.c`, `demsrch.c` | `50:07`, `0A`, `0C`, `20`, `2C-31` | bounded profile-entry first/next search and original null-close special case; host-handle/writable FCB operations are a contained terminal result |

## Immutable namespace mutation contract

`demfile.c` owns chmod, create/create-new, delete, file-times and rename;
`demdir.c` owns create/delete directory.  Each normal historical path requires
a writable NT host filesystem object.  The admitted CLI namespace deliberately
has neither writable storage nor ambient host path access.  The shared
`bx_ntvdm_dem_readonly_namespace_failure_provider_v1` therefore preserves the
selected provider route and returns AX=5/CF for:

`50:01`, `03`, `04`, `05`, `06`, `17`, and `22`.

This is a source-derived provider disposition, not an assertion that OpenNT
would always return DOS access denied for every historical host error.

`50:08 demFileTimes` is deliberately separate: the immutable namespace
supplies the recorded DOS date/time for the historical get branch (`BL=0`),
while its set branch (`BL=1`) remains AX=5/CF because this profile has no
writable metadata backend.  Invalid tokens return the source-shaped
no-handle result rather than joining the mutation fallback.

`50:27 demCommit` is in the adjacent `demhndl.c` handle family but has a
different source contract: the historical handler ignores `FlushFileBuffers`
failure and clears CF.  The same contained provider therefore returns a typed
CF-clear completion without creating a writable host handle.

`50:47` and `50:48` are the associated pipe-status pair.  The original
handlers call COMMAND's `cmdPipeFileDataEOF`/`cmdPipeFileEOF`, which require a
registered `cmdPipeList` entry, a projected NT read handle and synchronization
state.  The CLI profile creates no pipe SFT or host handle.  Both endpoints
therefore return the explicit source-derived no-projected-handle result
`AX=6`/`CF=1`; they do not query the host or fabricate pipe state.

`50:44 demCheckPath` has one self-contained historical branch: `\\DEV\\` is
always a valid DOS device directory, returns DX=0 and clears CF.  The other
historical branch creates `NUL` in a host directory to test it, which is not
admitted in the CLI profile.  The provider therefore obtains only a checked
128-byte DS:SI copy, accepts the case-insensitive `\\DEV\\` form, and returns
DX=0 plus AX=5/CF for every other path.  It performs no host path lookup,
device creation, or host filesystem write.

## Evidence

`artifacts/build/t199-s37-dem-package-r61-namespace/` is a source-built MSVC
x64 `/MT` all-DEM package regression.  It invokes every eight mutation
identity through the one DEM package session and asserts a typed resume,
AX=5, and CF set.  It simultaneously preserves the existing all-73 no-raw-
pass-through invariant.

## Remaining work

This does not close either package.  Namespace still needs one family review
of handle-token lifetime, DTA layouts and bounded successful open/read/seek
flows.  FCB/search still needs malformed SRCHBUF, continuation, foreign-PDB
and FCB-layout/host-handle negative coverage.  No trace is admitted until
those two packages and the remaining S37 packages have their final ledgers and
family regressions.
