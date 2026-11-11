# Test scope and dependencies

Tests are classified by what they establish, not the number of components or
processes they touch. Auxiliary files are not extra test cases.

## Unit tests

A unit test calls a selected provider or bounded component contract, optionally
with its inner dependencies, test doubles and owned native child processes.
It does not establish that a real DOS/WOW/Windows workload works end to end.

| Source/test family | Scope and dependency owner |
| --- | --- |
| common, lib-win32, lib-nxvm | Public library contracts; only their inner dependencies. Pinned shared-library fixtures retain upstream identity. |
| ntcon-exe, ntvwm-exe, run16-exe, ntvdm-exe, worker-base | The named product component and its inner providers. |
| adapter-basesrv | BaseSrv service/client contracts and explicit peer doubles. verify-service-fixtures starts in-process service fixtures; it is a unit runner. |
| adapter-mvdm-host-out, opennt-host, mvdm, mvdm-host, session | Original host and compatibility-boundary fixtures. Mocked guest state is not guest acceptance. Retained historical family names identify fixture ownership, not additional product layers. |
| app, broker | Native frontend/service contract fixtures. Real-package callers are integration entrypoints, even when they consume these fixture executables. |
| addon | Independently authored add-on logic/ABI fixtures; a host mock alone does not prove a working guest driver. |
| build, audit | Build/profile/source-boundary checks; no runtime acceptance claim. |
| observation | Shared observers, independent stimulus sources, fixture builders, diagnostic analyzers and remaining bounded provider-unit runners. Tools here are dependencies, not an additional acceptance tier. |

The BaseSrv unit runner lives in adapter-basesrv; native-worker fixture tests
live in ntvwm-exe. A unit may link multiple inner components. Production code
and generic libraries must never depend on test helpers or outer fixtures.

## Integration tests

`integration/` owns entrypoints that actually start the selected product,
exercise real RPC/worker/frontend/guest behavior and inspect its output or
completion. Original guest binaries remain immutable. Independent test guests
and observers are stimulus/measurement dependencies, not substitute providers.

A few examples of the distinction:

- `adapter-basesrv/verify-service-fixtures.ps1`: in-process provider unit checks.
- `integration/verify-s7-rpc-fixtures.ps1`: actual package RPC and participants.
- `integration/verify-wow-palette-guest.ps1`: actual Win16 GDI caller and report.
- `integration/verify-dos-identity-pair.ps1`: a diagnostic integration
  reproducer of the still-unresolved identity defect, not a passing gate.

`integration/support/wow-guest-probe.ps1` owns the common self-reporting WOW
setup: assembly, isolated package, environment, observer lifetime, report copy
and cleanup. Palette, kerning and embedding retain their own record assertions.
`observation/isolated_package_cleanup.ps1` remains the single package-scope
helper shared by runners; do not duplicate its path/identity/process checks.
Dependencies point from a case to helpers and selected providers. Shared setup
must not import individual cases or silently decide their expected result.

## Basic acceptance and selective execution

`tools/audit/Invoke-ProductVerification.ps1 -Suite Full` selects the maintained
real-product regression contract. Component CMake targets and focused runners
select unit contracts. Existing source/argument requirements are authoritative;
this directory is not a suite that should be executed by importing every file.

Keep happy paths, essential failure/lifecycle checks and regressions for known
fixed defects. Extend coverage for an actual uncovered contract, not to increase
file or assertion counts. Use the focused suite for changed test helpers; retain
normal product gates when product inputs or shared integration entrypoints change.
A missing external fixture is an explicit non-pass, never a fabricated success.

Retired runner/WHPX, replacement monitor/transport implementations and completed
batch-specific source-format replays were removed in T481 S20. Their exact source
and old evidence remain in Git at eb1acd565 and in indexed immutable audit receipts;
they must not be resurrected as current tests merely because a historical note
mentions them. No current product failure was removed to make a gate pass.
