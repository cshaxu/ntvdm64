# T202 S2: direct-composition DEM profile installation

## Question

Does the actual CLI composition install the shared capability profile before
DEM service dispatch, rather than limiting profile consumption to fixtures?

## Procedure

`bx_ntvdm_composition_runtime_v1` now constructs the default direct profile,
registers DEM's `session context` class, and installs the copied profile through
the existing composition setter before bind. DEM session installation creates
its CWD/default-drive context from that same profile.

The current product default remains direct selected-host integration. No
profile leaf chooses a mode; readonly, overlay and virtual stay represented by
the same installed ABI for their later CLI admission paths.

The full source closure now includes the CWD context, profile consumer,
overlay lifecycle and private host namespace. The latter uses the existing
adapter-local `NtQueryDirectoryFile`/`NtCreateFile` seam, so the retained
direct-composition and startup-plan probes explicitly link `ntdll.lib`.

## Observations

Fresh MSVC x64 `/MT` runs passed:

- `t202-s2-dem-package-bind-r2`: complete DEM package fixture source closure;
- `t202-s2-direct-profile-r4/s14-base`: direct composition installation and
  reset;
- `t202-s2-direct-profile-r4`: pointer-free direct startup plan.

The direct-composition probe writes Bochs diagnostic output to stderr during
its ordinary reset. Its runner now captures that output in `run.log` and
judges the declared process exit code, rather than treating diagnostic text as
a PowerShell exception.

## Interpretation

The profile/CWD seam is now present in the real product composition and not
merely a detached test facility. This does not yet connect the CWD context to
the `50:13`, `50:18`, and `50:1A` guest-memory ABI, and does not claim their
completion.

## Follow-up

Replace the retained root-only CWD/default-drive recognizers as one DEM family,
using the installed session context and source-shaped CDS/path failure rules.
