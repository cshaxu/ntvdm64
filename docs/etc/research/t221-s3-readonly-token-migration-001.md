# T221 S3 P3: readonly namespace token migration

## Question

Can the retained immutable startup-image input travel through the same opaque
DEM session token table as Direct files, without placing its internal token or
any host handle in guest registers?

## Inputs and procedure

- T221 S2 file/handle view disposition and T202 S3 handle ABI map.
- Existing `readonly_namespace` lifetime and `dem_readonly_file_service`.
- Existing Direct token/PDB-owner evidence from T220.

Added `READONLY_NAMESPACE` as the first non-handle file-session backend kind.
On a declared-image `50:12`, the provider opens the immutable namespace only
inside bx-vdm, adopts its private token into the DEM session table, and returns
the new session token in AX:BP.  The handle route checks the session kind,
temporarily supplies the backend token only to the existing readonly service,
and releases the session slot only after the backend close succeeds.  The raw
readonly token never reaches guest state.

The Direct PDB-owner check was also moved behind the Direct view decision.
Readonly, Overlay and Virtual dispositions now reach the shared local backend
first; this fixes the prior Readonly create/open error from invalid-function to
the source-shaped access-denied result.  A Direct token still requires the
bounded copied PDB owner before host open/create.

## Observation

`Invoke-T202S4DemWholeProviderProbe.ps1` built and ran the complete current
whole-provider fixture under MSVC x64 C11 `/W4 /WX /MT` in
`build/M0-T221-S3/003` with exit code zero.  It exercises readonly open,
seek/read, write refusal, commit, close, Direct namespace/handle/FCB paths,
and each profile's no-fallback expectations.  This is source-built fixture
evidence (`guestExecution=false`), not a Bochs/native trace.

## Interpretation and limits

Direct and the retained startup readonly backend now share the opaque session
token boundary.  This does not yet make arbitrary host reads a readonly-image
view, and it does not implement Overlay or Virtual filesystem state, COW,
tombstones, merged search or FCB metadata.  The one-open readonly namespace is
retained historical fixture input until the package supplies broader readonly
view state.  `50:47/48` remain Redirector unavailable.

## Follow-up

Add package-owned overlay/virtual namespace state and its typed token kinds,
then migrate the pathname/FCB/search subfamilies through the same view.  Do
not use a session backend kind to create a new BOP-local service path.
