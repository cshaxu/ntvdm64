# T221 S1 P5: DEM current workaround disposition

## Question

Which currently bound DEM seams, synthetic answers, and terminal fallbacks are
still legitimate profile backends, which must migrate into a complete owner
package, and which must be replaced before any package is called complete?

## Procedure

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/governance/Export-T221S1DemWorkaroundDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s1-dem-workaround-disposition-001.json
```

The exporter consumes the P4 73-row ledger and assigns exactly one action to
each current route/shim/fallback. It does not change routing or infer behavior
from a trace.

## Result

The [machine-readable disposition](t221-s1-dem-workaround-disposition-001.json)
covers 73/73 services:

| Action | Count | Meaning |
| --- | ---: | --- |
| migrate | 55 | Retain the existing seam only while it is moved below the complete owner package and shared profile ABI. It is not a permanent endpoint-local BOP implementation. |
| replace | 7 | Current generic/finite fallback conflicts with the source owner contract and must be replaced by an admitted whole-package disposition. |
| retain | 11 | Preserve the explicit original no-op or currently correct unavailable fence until its owning profile exists. |
| delete | 0 | No current bound path is proven redundant solely by this audit; deletion needs a later replacement and source/build reachability proof. |

The replacement set is intentionally small and explicit:

- `50:0E`, `50:10`: replace the generic fallback with source-shaped shared
  drive/volume behavior.
- `50:30`: replace generic fallback with the original FCB local-clock form.
- `50:32`, `50:33`, `50:3F`: replace generic unavailable responses with the
  error/lock owner contract.
- `50:3C`: replace finite boot cleanup as a lifecycle answer; it must not
  release direct tokens before the guest `DOS_ABORT` close path.

The retained set is deliberately narrow: seven original no-op services,
`50:29/2A` no-BDS raw-device unavailability, and `50:47/48` Redirector pipe
unavailability.  Retention does not grant device, pipe, or broad host access.

## T221 S1 conclusion

S1's audit exit criteria are met: every DEM service has one current bound
route, source owner, ABI/failure evidence family, recovery rung, profile
disposition, and current-workaround action.  The next S may therefore choose
one **whole owner package** to recover; it must not start with whichever BOP
appears next in a native trace.

The recommended next package is the 29-service file/handle/FCB/search group,
because it already has the most complete source/ABI map, all four profile
modes are defined at the shared ABI, and it contains the direct-token seam
that T220 corrected.  The selected S must still first define the package-wide
provider disposition and family regression; no individual service is admitted
by this recommendation.
