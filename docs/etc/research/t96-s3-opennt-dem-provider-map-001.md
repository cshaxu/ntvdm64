# M0 T96 S3 OpenNT DEM Provider Map 001

## Reproducible evidence

The generator tools/Export-OpenNtDemProviderMap.ps1 consumed the immutable BOP
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

The original unavailable services are 31 GETDRIVEINFO, 36 SETDPB, 38
SLEAZEFUNC, 40 EXTHANDLE, 43 GSETCDPG, 64 DRIVEFROMHANDLE, 66 FASTREAD, and
67 FASTWRITE. They retain an unavailable disposition until a separate
source-proven failure contract is introduced; they must not be implemented
because an adapter endpoint happens to exist.

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
