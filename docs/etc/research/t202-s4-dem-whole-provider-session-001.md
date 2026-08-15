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

## Interpretation and follow-up

This establishes one state/lifetime owner for the complete package; it does
not dispatch a BOP, perform guest-memory access, or claim an implemented
service.  The next implementation step is the provider's common pending
gather/write transaction ABI, followed by the plan's six source partitions
and one package regression before any old route is switched.
