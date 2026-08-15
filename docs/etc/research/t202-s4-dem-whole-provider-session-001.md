# T202 S4: DEM whole-provider session

## Question

Can one DEM-owned provider state own the complete S3 29-identity membership,
the selected profile entry and the local file-token/backend lifetime without
yet switching any legacy BOP route?

## Procedure

Added `bx_ntvdm_dem_whole_provider_v1`.  It accepts only a valid copied
profile, admitted host namespace and DEM CWD context; initializes the existing
opaque file-token session and local-file backend as its private state; and
zeros/closes that state at teardown.  `owns_service` is the single explicit
membership guard for all 29 S3 identities, including the separately deferred
`50:47/48` boundary.

`Invoke-T202S4DemWholeProviderProbe.ps1` compiles the full provider substrate
and a focused fixture using MSVC x64 `/MT`.

## Observations

`artifacts/build/t202-s4-dem-whole-provider-r1` compiled and returned zero.
The fixture constructs an admitted real-host namespace, profile and CWD,
initializes the provider, verifies membership across `50:00..48`, tears it
down, and proves the cleared provider no longer validates.

Revision r7 adds and passes the provider-private copied gather lifecycle.  A
gather binds one owned service, copied exception/CPU boundary and bounded
range list; a competing prepare is refused without changing the first action.
Completion accepts only the exact action/boundary/CPU and byte count, copies
the bytes into caller storage, and clears the pending record.  A second
completion is refused.  The initial r5/r6 failures exposed an omitted source
closure and an action-clobber-on-refusal defect; r7 contains both corrections.

Revision r10 adds a single source-derived `demhndl.c` local-handle partition
behind that provider state.  It owns the complete local handle family
`50:00` (seek), `50:02` (close), `50:08` (file/device times), `50:16`
(read), `50:1E` (write/truncate), and `50:27` (commit).  Guest-visible
`AX:BP` remains the existing opaque token; the native `HANDLE` never crosses
the ABI.  The focused real-host-file regression passes seek, read, write,
zero-length truncation, file-time, handle-free device-time, commit, close and
stale-token rejection under MSVC x64 `/MT`.

The implementation deliberately preserves two source boundaries: `50:08`
device time does not resolve a token, and the historic `demClientErrorEx`
hard-error owner is still unavailable, so its known hard-error range returns
the recorded source-derived `AX=FFFF, CF=1` form.  `VDMREDIR` named-pipe
specialization remains outside this local-handle partition and is not
silently supplied.

## Interpretation and follow-up

This establishes one state/lifetime owner for the complete package plus one
complete source partition.  It does not dispatch a BOP or perform live
guest-memory access, and does not claim the 29-identity DEM package is
complete.  The next implementation steps are the remaining original source
partitions and one package regression before any old route is switched.

Revision r15 adds the namespace/metadata partition skeleton for the complete
`50:01/03/04/05/06/12/17/22/44` owner group. It centralizes membership,
copied OEM-path resolution and caller-selected DEM mutation-class policy;
the latter avoids treating the read/write-capable open operation as an
implicitly readonly or write-only service. r15 source-builds under MSVC x64
`/MT`. It is not a BOP route and does not yet implement the nine operations.

Revision r21 replaces that skeleton with one source-derived namespace/metadata
partition.  Its adapter-private operation ABI receives only the already copied
OEM pathname(s), copied CPU/event records and a typed CPU result; it performs
no guest-memory access and remains below BOP ingress.  It covers the original
`demfile.c`/`demdir.c` local forms for `50:01` attribute query/set,
`50:03/22` create/create-new, `50:04/05/06` directory/file mutation,
`50:12` ordinary file open and its DOS access/share decode, `50:17` rename,
and `50:44` path check.  Open/create returns only a session-owned opaque
`AX:BP` token; a native `HANDLE` remains private.

The r21 source-built MSVC x64 `/MT` fixture uses only its own temporary host
objects.  It proves attributes, open/close, create, rename, deletion,
directory create/remove and the `50:44` directory success form.  The
partition keeps named pipes/VDMREDIR deferred, maps a disallowed EA form to
the source-shaped failure result, and never substitutes an overlay or virtual
mode with a direct-host write.  This is one completed source partition, not
the complete 29-identity DEM provider, a BOP ingress route, or a native guest
run.

Revision r22 adds profile-negative coverage for this same partition.  A
readonly provider rejects both create and attribute mutation with the
source-shaped access-denied result, while an overlay-mode create reports its
current explicit unavailable form rather than falling through to a direct
host write.  The direct-host fixture object is reused only for read/query;
these negative checks make no additional host mutation.

Revision r25 adds the `demfcb.c` local-handle partition for the complete
`50:2C/2D/2E/2F/30/31` group.  It reuses the provider-owned opaque-token
session rather than serializing a host `HANDLE`; it preserves the historical
AX:SI FCB-close token form separately from AX:BP FCB I/O, returns FCB time,
date and size in their original register placements, and retains the distinct
partial disk-full form.  The r25 MSVC x64 `/MT` fixture exercises FCB open,
read, close, local date/time and file-information results against its
self-created file.  Create and write code are in the same source partition
but still need their dedicated mutation-profile regressions.  This remains a
provider-internal partition, not a guest-memory BOP route or full DEM claim.
