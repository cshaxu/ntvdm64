# T484 S3 P3: native guest-memory fixture production

Two runtime test runners now build through `tools/build-fixtures.cmd` and
native CMake/Ninja instead of generating compiler commands and calling the
Python guest-memory extractor:

- verify-guest-memory-lease.ps1: three existing fixtures, both x86 and x64.
- verify-redirector-async-publication.ps1: existing x86 async publication test.

The CMD entry selects a validated architecture and named group in a stable
per-architecture cache. It preserves each fixture's sources, definitions,
MSVC runtime and CL-driven versus explicit LINK library policy. Assertions
and evidence remain in the existing standalone test runners. No product
source, original guest, mirror diff or runtime ABI is changed.

SourceProjection.cmake contains the previously verified native projection
implementation. Worker and standalone fixture graphs now share it, including
the guest-memory preamble and original diagnostic line. The complete projected
source is byte-identical across all three graphs, SHA256
85B6B504C334177918B839C8069590D25A29858E7CA597CC96DDF90D73571F81.
This is a test projection, not a copied production implementation.

Run PID11860 passed all seven executable instances: three on each architecture
and the x86 async test's27 assertions. Final cached full build PID19252 passed.
Post-refactoring stability PID9488 confirmed all seven test image hashes and
mtimes unchanged, projection identity, both architecture graphs free of
forbidden interpreter commands, and invalid architecture/target rejection.
Existing host package identity is unchanged:11 raw inputs and22 destination
hashes are checked. No changed host image requires republication.

The [memory fixture evidence](../../../artifacts/build-migration/t484/s3-memory-fixtures/result.json)
contains execution, stability and build receipts plus individual results.
The containing commit delivers this bounded batch; S3 remains active.

The Python extractor is not yet deleted: wire-guest-memory-fixture.py still
serves six historical/manual generators (T280 location, T282 bounded copy,
T291 task frame, T322 XMS, T324 redirector and T335 callback frame). Their
sources still exist; lack of executable callers alone is not a deletion proof.
They require consolidation/migration and their own retained behavior checks.
Other S3 candidates, S4 publication and S5 clean acceptance remain unfinished.
