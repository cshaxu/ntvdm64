# M0 T199 S30 DEM Package Session Regression

## Question

Can the complete OpenNT DEM BOP family be executed or deferred through one
package session, instead of escaping through composition-local helpers?

## Inputs

- Original DEM ownership/order evidenced by the retained T199 S2--S4 map and
  `src/opennt/base/mvdm/dos/dem/` source.
- Existing bounded `bx-vdm` namespace/search, session, GSET, IOCTL,
  error-lock, DPB, boot-drive, DTA, raw-media, and compatibility providers.
- `src/bx-vdm/bx_ntvdm_dem_package_session_v1.{h,c}`.

## Implementation

The new DEM session owns copied DEM state (`gset` drive snapshot and hard-error
registration) and borrows the immutable namespace plane, which remains owner
of guest-file namespace and pending RAM-action state.  Composition now has
exactly one DEM call:

```text
common BOP ingress -> DEM package session -> selected DEM subprovider
```

The session owns package-level refusal as well: a recognized DEM subprovider
whose mode or state prerequisite is absent returns typed pass-through from the
session, rather than falling through to a second composition recognizer.

`50:36` (`vdd_create_user_notify`) was found to bypass the facade before
common ingress despite belonging to DEM miscellaneous service ownership.  It
is now dispatched inside the DEM session; it retains its original bounded
four-byte no-op result.

## Procedure and Observation

Ran:

```text
powershell.exe -ExecutionPolicy Bypass -File tools\Invoke-T198S16BootNamespaceVdmX64Probe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t199-s30-dem-package-r6 \
  -HostArchitecture x64
```

The MSVC x64 `/MT` source closure compiled, linked and exited zero.  Its DEM
route sweep creates a protected-mode synthetic event for each of the 73 legal
`50:00..48` identities.  This prevents any real-mode provider from acquiring
state while proving every identity enters `bx_ntvdm_dem_package_session_v1`
and returns a valid typed result.  Existing real-mode positive and negative
namespace, search, DTA, DPB, drive, error, and compatibility contracts remain
in the same fixture.  A dedicated `50:36` composition assertion verifies
four-byte resume through the session.

No native trace, Bochs object, OpenNT runtime link, GUI/device archive, host
filesystem discovery, or new device capability entered the procedure.

## Interpretation

This closes DEM package composition and routing, not all historical DEM host
capability.  Unsupported operations remain package-deferred; they do not
become DOS-kernel, filesystem, or Bochs behavior.  Confidence is high for the
source-built router and existing bounded provider preservation.

## Follow-up

Connect the already admitted selector-blind XMS capabilities (A20 and bounded
extended-memory aperture) to one XMS package provider, while keeping UMB and
INT15 lifecycle explicitly deferred.
