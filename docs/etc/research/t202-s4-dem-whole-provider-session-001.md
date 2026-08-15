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

Revision r12 provides the direct-search fixture: two 8.3 files in a
self-created real-host directory prove admitted-host pathname first/next and
FCB-first results. Directory enumeration had falsely returned access denied
because its root-relative listing handle was then validated through
`GetFileInformationByHandle` without `FILE_READ_ATTRIBUTES`; r12 adds that
read-only validation right. No host path or handle crosses the search
token/DTA/SRCHBUF ABI. Guest-memory ingress remains unconnected.

Revision r26 makes that established search transaction a private lifetime
member of the single DEM whole provider.  The whole-provider source-built
fixture verifies initialization, validity and teardown with the complete
search source closure under MSVC x64 `/MT`.  This is state ownership only;
it does not switch a legacy BOP route or claim guest search completion.

The next `demsrch.c` partition now enables the existing search transaction's
direct-host first-search forms: it decodes the copied DOS request, enumerates
only the query's admitted root-relative directory, then passes the copied
entries to the established source-shaped search plan and session-owned
continuation table.  No directory handle, pointer or ambient path is carried
to the DTA/SRCHBUF.  `t202-s4-search-closure-r1` source-builds the retained
x64 boot-namespace closure after this change.  Direct-host search behavior
still needs a dedicated fixture before this is claimed as a completed search
partition.

Revision r27 establishes that dedicated `demsrch.c` partition without moving
any selector recognition into Bochs or switching an ingress route.
`bx_ntvdm_dem_search_partition_v1` owns the four related service forms as one
unit: pathname first/next (`50:09/0B`) and FCB first/next (`50:0A/0C`).  It
uses the original preparation layouts to form a checked gather, binds that
gather to the whole provider's copied continuation, and only then invokes the
provider-owned direct-host search transaction.  The DTA registration is copied
adapter state; it is neither a host pointer nor an ambient guest lookup.

`t202-s4-dem-search-partition-r3` source-builds with MSVC x64 `/MT` and
creates two 8.3 files in its own temporary admitted directory.  It exercises
pathname first/next and FCB first/next through the single partition, including
the copied continuation token placement between each pair.  The retained
whole-provider r27 closure compiles that partition and its source dependencies
with the other DEM partitions.  Neither fixture performs BOP ingress routing,
live Bochs memory access, or a native trace; those are separate package-level
steps after the remaining partition regressions and atomic legacy-route
migration.

The first route-composition increment retains the existing GSET-owned DTA
registration but creates the DEM whole provider only when the package session
receives its admitted host namespace.  In that installed state, all four
search identities are selected together before the legacy boot-plane fallback;
an unavailable provider precondition is routed to DEM's explicit unavailable
form rather than back to the boot snapshot.  The package teardown now releases
the whole provider before the host namespace is released, including failed
runtime installation cleanup.

`t202-s4-search-route-closure-r2` builds and runs the retained MSVC x64 `/MT`
boot-namespace composition fixture after adding the provider and partition to
its exact source closure.  This is compilation and lifecycle evidence only:
that retained fixture deliberately does not install an admitted host namespace,
so a separate host-installed route regression remains required before claiming
the direct search BOP route migrated.

Revision r28 additionally proves the provider's exact-continuation cancellation
path.  If a typed mechanical read cannot run, the route cancels only the
matching service/event/CPU/gather record; the fixture then successfully opens
and cancels a new gather.  Thus a mantle transport refusal cannot leave stale
search state that blocks a later BOP.

The host-installed route regression then exposed a genuine composition-state
gap: PATHFIRST reads a far pointer from the GSET registration and writes the
resolved DTA, while PATHNEXT must consume that resolved DTA rather than the
pointer variable.  The whole provider now retains only this bounded
guest-physical address for the matching pathname continuation.  It remains
below the adapter boundary and carries neither a host pointer nor Bochs state.

`t202-s4-search-route-behavior-r7` creates two 8.3 files in a temporary
admitted directory, installs the shared direct profile and host namespace in
the DEM package session, and invokes the four services through ordinary BOP
ingress plus the checked mechanical RAM seam.  It proves pathname first/next
(`50:09/0B`) and FCB first/next (`50:0A/0C`) produce `ALPHA` then `ZETA`
results.  The test preflights the same root-relative enumeration separately,
and never calls the legacy boot snapshot search transaction.  The related
whole-provider r30 and search-partition r4 MSVC x64 `/MT` regressions pass.
This closes the four-service route migration evidence only; it does not close
the remaining DEM identities, overlay behavior, historic hard errors, or a
native guest trace.

Revision r31 adds the corresponding `demhndl.c` route as one six-identity
partition: `50:00/02/08/16/1E/27`.  The route is selector-blind below the
common BOP ingress: it accepts only a provider-issued opaque token, translates
the two buffer forms through the existing copied mechanical read/write action,
and otherwise leaves a retained legacy fixture token with its existing owner.
This migration guard is necessary until the final one-provider switch; it does
not place a host `HANDLE`, DOS meaning, or BOP selector in bx-core or
bx-mantle.

`t202-s4-handle-route-closure-r1` source-builds the added route with the
complete whole-provider fixture under MSVC x64 `/MT`.  The host-installed
`t202-s4-handle-route-behavior-r1` fixture then adopts a temporary local file
inside the provider only, calls the six identities through ordinary BOP
ingress and checked RAM, and verifies seek, read, write, get-time, commit and
close results.  During that check the existing `50:42` legacy fast-read
fixture exposed a token-owner collision; the route now declines tokens it did
not issue, restoring the legacy result without treating it as a new provider
fallback.  This closes route evidence for this six-identity partition only.
It does not claim all 29 identities, readonly/overlay behavior, hard-error
delivery, Redirector pipes, or a native guest trace.

## Remaining route-order guard

The next namespace/metadata partition has no inbound opaque token by which a
transition route can distinguish a new direct-host request from the retained
boot fixture.  In particular, `50:12` is a pathname-only open entry; switching
it alone would force a choice between the profile's admitted host namespace
and the still-retained boot-file provider.  That is a package-composition
decision, not a per-service implementation detail.

Accordingly, no further pathname or FCB ingress route is admitted merely from
a focused fixture.  Their provider-internal direct/readonly/overlay regression
must close first.  The remaining legacy pathname/FCB leaves will then move in
one explicit final DEM package dispatch change, where no two providers can
claim the same untyped pathname.  The already migrated search and
provider-issued-token handle routes remain bounded evidence; they do not
authorize another partial switch.
