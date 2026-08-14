# M0 T199 S4 DEM Package Facade Regression

## Question

Can the complete callable DEM family enter one classified `bx-vdm` package
facade, while retaining the prior provider fragments and without using a guest
trace to choose services?

## Inputs

- The T199 S2 DEM source/ABI/failure matrix and T199 S3 whole-package
  disposition.
- `src/bx-vdm/bx_ntvdm_dem_plane_v1.{c,h}`, the existing source-derived
  component classifier.
- Existing partial `bx-vdm` providers and the MSVC x64 adapter-only composition
  fixture.

## Procedure

1. Added `bx_ntvdm_dem_package_facade_v1`, a package-level route record over
   the existing DEM classification.  It has exactly four outcomes:
   `DEFERRED`, `ORIGINAL_NOOP`, `FASTREAD_COMPATIBILITY`, and
   `EXISTING_PROVIDER`.
2. Attached the facade immediately after common ingress/registry selection in
   the composition.  An existing DEM provider can therefore run only after the
   package route is available.  The facade contains no historical dispatcher,
   Bochs call, host filesystem access, or guest-memory action.
3. Preserved the seven original `demNotYetImplemented` members (`1F,24,26,28,
   2B,40,43`) through the retained exact provider: CF clear and a four-byte
   continuation.  `42` remains the separately registered, bounded fast-read
   compatibility exception and is never described as an original no-op.
4. Extended the source-built composition fixture to enumerate all `00..48`
   callable DEM values plus sentinel `49`; it checks every route disposition
   and the exact no-op CPU result.
5. Ran:

   ```powershell
   powershell.exe -ExecutionPolicy Bypass -File tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1 `
     -RepositoryRoot O:\repos.hobby\ntdos64 `
     -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t199-s4-dem-facade-r1
   ```

## Observation

The MSVC x64 `/MT` source closure compiled, linked, and the fixture returned
zero.  Its build record is
`artifacts/build/t199-s4-dem-facade-r1/t198-s16-bx-vdm-boot-namespace.json`.
The source list includes the facade and exact original-no-op provider; it
contains no bx-core, bx-mantle, Bochs product target, OpenNT runtime object, or
guest execution input.

## Interpretation and confidence

This closes the DEM package **implementation boundary**: every callable DEM
identity now crosses one package facade, and all retained partial providers are
behind that common admission point.  It does not close every DEM capability:
the route explicitly leaves unsupported members deferred, and the selected
fast-read rehost remains a recorded compatibility exception.  Confidence is
high for the compiled route/result contract; no native trace or DOS continuity
claim was made.

## Follow-up

T199 next maps COMMAND as a complete package before any further COMMAND
implementation.  A DEM native observation is deferred until the global
package-order gate permits integration verification.
