# M0 T269 S4 — OpenNT-host final conformance

## Question

Does the final `opennt-host` production tree satisfy the mirror-component and
private-overlay rules without leaving an unowned source body or changing the
already reached host/PIF behavior?

## Inputs and procedure

- Recomputed each live file identity against selected OpenNT revision
  `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`, rooted at `base/mvdm`.
- Reconciled all local `DIVERGENCE(...)` identifiers with
  `src/opennt-host/README.md`.
- Scanned source, tools and tests for external private-overlay implementation
  consumers and all source inclusions of an `opennt-host` C body.
- Ran the S3 formal Ninja graph incrementally and executed the direct PIF and
  host-mirror fixtures.

## Final inventory

| Classification | Count | Units |
| --- | ---: | --- |
| Exact selected original | 6 | `inc/oemuni.h`, `inc/vrnmpipe.h`, `softpc.new/host/inc/{error,nt_pif,nt_uis}.h`, `vdmredir/vrputil.h` |
| Registered true subset / same-shaped modification | 4 | `softpc.new/host/src/nt_error.c`, `vdmredir/{vrinit,vrnmpipe,vrputil}.c` |
| Private non-mirror binding fragment | 3 | `nt_error_bindings.h`, `vrinit_bindings.h`, `vrnmpipe_product_seams.h` below `opennt-host-overlay` |
| Unowned production source body | 0 | None. |

All live host divergence identifiers are source-local and README-registered:
`HOST-DIV-012` through `020`, `022` through `028`, with no invented `021`.
The four modified retained bodies remain below the 50-percent changed-retained
threshold recorded by S2.

## Boundary observations

- No non-host component includes or calls an `opennt-host-overlay` implementation.
  The only consumers are the matching host mirror bodies; the production-tree
  governance check is not an implementation consumer.
- No cross-component inclusion of an `opennt-host` C body remains.
- The exact original PIF parser now belongs solely to
  `opennt-bop/softpc.new/host/src/nt_pif.c`, because that COMMAND composition
  is its only reached consumer and contains the incompatible historical
  `PIF_DATA` layouts. Its destination SHA-256 is
  `4AE2C1D2C4A66DC63C9B2DDD673FCAE067F2FBDAB953DC8D96AECFCF03C262BC`,
  exactly matching the selected original. The component-local include is the
  registered `BOP-DIV-032` containment, not a host ownership violation.

## Verification result

- The formal MSVC x64 `/MT` Ninja graph reports `ninja: no work to do.`
- `t234-s2-command-pif-direct-fixture.exe` passed its original parser title,
  command, directory, target, close-on-exit and early CONFIG/AUTOEXEC checks.
- `t263-s8-opennt-host-mirror-fixture.exe` passed its utility and synchronous
  named-pipe route checks.
- Documentation governance and `git diff --check` are run at final delivery.

## Scope conclusion

T269 closes source identity, mirror registration, private overlay ownership
and the sole discovered cross-component owner edge. It does not claim recovery
of unadmitted asynchronous Redirector/VDD product paths or the separate
COMMAND lifecycle fixture that did not progress during S3.
