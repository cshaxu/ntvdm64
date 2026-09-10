# T222 S2 Drive-View CWD Provider Migration

## Question

Can the existing source-derived DEM CWD service (`50:13`, `50:18`, `50:1A`)
move behind the shared drive-view provider without allowing a Direct fallback
for Readonly or changing retained Overlay/Virtual routing?

## Inputs

- OpenNT `src/opennt/base/mvdm/dos/dem/demgset.c` and `demdir.c`.
- Existing source-derived `bx_ntvdm_dem_cwd_service_v2` and session host
  context.
- T222 S1 source/ABI ledger and provider design.

## Procedure

`dem_drive_view_provider_v1` now owns the CWD prepare/complete wrapper. It
accepts only the shared `SESSION_CONTEXT` policy results `DIRECT_HOST` and
`REJECT_READONLY`; Overlay and Virtual decline the wrapper so their retained
boot-namespace route remains unchanged. The DEM package session retains the
only machine-facing gather/read and checked multi-write execution.

The focused provider fixture was compiled from the existing
`Invoke-T198S16BootNamespaceVdmX64Probe.ps1` x64 `/MT` source closure in
`build/M0-T222-S2/010-drive-view-provider`, then linked with the generated
response file under the Visual Studio x64 environment. The executable exited
zero.

## Observations

- Direct `50:1A` copies `C:\` through the checked gather ABI, calls the
  retained host-context implementation, returns resume with CF clear, and
  restores the test process CWD after the assertion.
- Readonly `50:1A` uses the same provider wrapper but returns the existing
  source-shaped `AX=5`/CF refusal.
- Overlay and Virtual make `prepare_cwd` decline; no new behavior or Direct
  fallback was introduced for either profile.
- The pre-existing all-DEM `dem-package` fixture exits `197` even when the
  package-session and provider header are restored to `HEAD`; it is therefore
  not evidence that this migration regressed Overlay. It remains an explicit
  separate baseline inconsistency and cannot satisfy T222 exit criteria.

## Interpretation

This is a CWD subfamily boundary checkpoint only. It establishes the
Direct/Readonly ownership seam and profile isolation, but does not close the
fifteen-identity package: `50:13/18/1A` still require normal Direct/Readonly
package-session regression, and checked DTA transport remains un-migrated.

## Follow-up

Add a normal package-session Direct/Readonly fixture for the CWD/default-drive
cases, then migrate the DTA service through the same provider before claiming
T222 closure.