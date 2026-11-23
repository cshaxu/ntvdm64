# T484 S3 P11: duplicate compilation entrypoints

The suballoc lifecycle fixture now builds through the x86 CMD/native fixture
target. Its original sources, defines, forced NT header and libraries remain;
the JS generator is removed. The five original success markers pass, and the
x64 invocation is rejected with exit2 rather than claiming a supported profile.

Four manual, compilation-only generators are retired:

- New-T279SupportLibraryNinja.ps1 duplicated RTL support, OEM file/process and
  suballoc archives already produced by the worker graph.
- New-T280S12DemObjectObservationNinja.ps1 compiled five DEM units for syntax.
- New-T280S20PdbSyntaxNinja.ps1 compiled demfile and the two host disk units.
- New-T281OriginalBopDispatchNinja.ps1 compiled nt_bop with an obsolete monitor
  profile and a missing command-register header.

None has an executable consumer or runtime assertion. Their13 distinct source
objects are present in the actual current worker graph and on disk, with
[compile edges and object identities](../../../artifacts/build-migration/t484/s3-compile-duplicates/production-coverage.json).
The supported production profiles take precedence over these historical
independent syntax islands. This does not claim preservation of their former
x64 syntax-only profiles. No original code is removed or rebuilt as guest.

Cached full host build PID12076 passed. All11 raw product images and22
published hashes remain unchanged, so republication is unnecessary. The
current package remains d8983d2b-3fe1-4d18-8ea9-64613c84346c at both locations.
[Verification receipt](../../../artifacts/build-migration/t484/s3-compile-duplicates/result.json)
records the actual lifecycle markers and architecture rejection; adjacent
product-build receipt records the full build. No product runtime or Full pass
is attributed to this batch. Its containing commit is the delivery identity.

Other producers, including the OEM family with runtime tests, are not removed
by this duplicate-only decision. S3 remains active; S4 and S5 remain pending.
