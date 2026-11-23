# T484 S3 P12: native OEM and COMMAND fixtures

The two OEM generators are replaced by the existing native worker graph:
Generate-T420S17OemUniNinja.mjs and New-T282CommandOemSupportNinja.ps1 are
removed. `tools/build-fixtures.cmd x86 oem-tests` configures that graph and
builds17 opt-in tests. Actual production OEM, RTL, PIF and COMMAND objects are
reused; there is no second product source list. Additional test sources retain
their specific providers and use the current worker profile where required.

Five fixed projections select current DEM/COMMAND source bodies using native
CMake. Their selections match the previous generator on current sources,
after line-ending normalization. Sources are configure dependencies and their
hashes are emitted with the projections. This is test composition, not a
guest rebuild or alternate product implementation.

Migration exposed stale test assumptions:

- The first compile selected the DEM-only WIN_32 profile for a COMMAND caller.
  It correctly hit the excluded-profile error. Binding the test to the actual
  general worker profile fixes this without changing original headers.
- The font retry fixture still replaced two removed guest-location helpers.
  Current demCreateCommon uses original GetVDMAddr. The fixture now supplies
  that address interface. Five font retry cases remain and pass; the two
  assertions for the removed helpers are retired, not counted as passes.
- The PIF root link now needs the existing host-context and path-conversion
  objects after the prior ownership refactor. Those real providers are linked.

All17 targets compile. Sixteen first-run cases pass. The PIF caller first
rejects this batch's long working directory before exercising its normal path;
its stderr explicitly states the original64-byte limit. A short-directory run
then passes all normal/error/cleanup cases, and a separate long-directory run
passes the expected-rejection case. Final coverage is18 passing runtime cases
over17 targets, not18 different fixtures. No production boundary is relaxed.

Cached full host build PID23668 passes. All11 raw host images and22 publication
hashes remain unchanged; the existing package is still
d8983d2b-3fe1-4d18-8ea9-64613c84346c at both destinations. No changed product
requires republication. Original guest and non-error mirror diff changes are0.
This is targeted runtime validation, not a new Full run.

[Final receipt](../../../artifacts/build-migration/t484/s3-oem/final-result.json)
links by directory to initial runtime results, the two PIF reruns, source
projection identities, full build and publication checks. The containing
commit is P12 delivery. S3 remains active; further test producers, S4 release
production and S5 interpreter-free clean acceptance are outstanding.
