# M0 T96 S3 OpenNT DEM Provider Map 001

## Reproducible evidence

The generator tools/governance/Export-OpenNtDemProviderMap.ps1 consumed the immutable BOP
provider/module map and the pinned OpenNT DEM source tree. Its accepted output
is artifacts/analysis/opennt-dem-provider-map-001-20260811-002/.

The mapping preserves demdisp.c table order and resolves every DEM service to
the source definition of its original handler. It is static source closure,
not a provider build or runtime assertion.

## Result

| Disposition | Count | Meaning |
| --- | ---: | --- |
| source-pending-composition | 65 | Original handler has a source definition; its historical CCPU/SAS/NT-host dependencies have not yet been made CLI-composable. |
| original-unavailable | 8 | The original dispatcher explicitly selects demNotYetImplemented. |
| not-callable-sentinel | 1 | SVC_DEMLASTSVC is a table boundary, not a callable service. |
| source-definition-unresolved | 0 | No dispatcher handler lacks a source definition. |

The original no-implementation services are 31 GETDRIVEINFO, 36 SETDPB, 38
SLEAZEFUNC, 40 EXTHANDLE, 43 GSETCDPG, 64 DRIVEFROMHANDLE, 66 FASTREAD, and
67 FASTWRITE. The historical demNotYetImplemented body is not an error path:
after optional debug output it executes setCF(0). They must not acquire a
new service implementation merely because an adapter endpoint happens to
exist, but their source-proven no-op/CF-clear contract may be rehosted.

## Consequences

The DEM provider has a closed source owner set but not a directly linkable
composition. demdisp.c itself and all 65 remaining source handlers compile
against the historical softpc/CCPU/SAS register and guest-memory environment.
S3 therefore proceeds by component-level dependency grouping and per-service
provider disposition, never by directly compiling the historical dispatcher
or adding a new runtime recognizer.

Existing adapter endpoints are evidence inputs only. They will be compared to
their original handler contract and later migrated through the DEM provider;
they do not become permanent dispatch exceptions.

## Existing adapter endpoint reconciliation

The early adapter contains recognizers for 12 DEM service values. The table
below is an audit result, not a migration admission.

| Service | Existing adapter role | Original handler | Audit disposition |
| ---: | --- | --- | --- |
| 00 | read-only file seek | demChgFilePtr | source-derived candidate; no migration yet |
| 02 | read-only file close | demClose | source-derived candidate; no migration yet |
| 13 | boot-drive fallback | demGetBootDrive | source-derived candidate; no migration yet |
| 15 | drive inventory | demGetDrives | source-derived candidate; no migration yet |
| 18 | read-only file open | demOpen | source-derived candidate; no migration yet |
| 22 | read-only file read | demRead | source-derived candidate; no migration yet |
| 27 | DTA registration | demSetDTALocation | source-derived candidate; no migration yet |
| 33 | IOCTL changeable media | demIOCTL | legacy AL=08 only; paired AL=08/09 source-contract slice is recorded, no migration yet |
| 50 | hard-error registration | demSetHardErrorInfo | source-derived candidate; no migration yet |
| 59 | non-debug result | demIsDebug | source-derived candidate; no migration yet |
| 66 | fast read | demNotYetImplemented | conflict: freeze and reject; the original slot is unavailable |
| 70 | DPB list materialization | demGetDPBList | source-derived candidate; no migration yet |

The FASTREAD conflict proves that historical reachability or an old endpoint
is not enough to admit a provider. No entry in the table currently reports
original-provider composability, and none may be added to the new DEM module
until its OpenNT contract and replacement blocker are recorded.

## Original no-implementation provider slice

The adapter-local DEM provider in bx_ntvdm_dem_provider_v1.c now rehosts only
the exact demNotYetImplemented contract for all eight table slots: validate
the common DEM selection, resume after the four-byte BOP, and clear CF. It
does not implement FASTREAD or FASTWRITE, access a file, parse guest memory,
or alter general-purpose registers. The focused test also proves that service
22 DEMREAD is not owned by this slice.
