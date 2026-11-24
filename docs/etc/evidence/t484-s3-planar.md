# T484 S3 P89 painter contract and duplicate entry retirement

B0145 remains a useful direct contract of actual CGA/EGA/munge objects:
complete pixels before publication, original object ownership, source and
frame bounds, mutex release, all three scales and a wrong-pixel negative.
Native worker target planar-painter now owns compilation/linking using the
actual nt_ega source profile; the assertion runner no longer parses an old
Ninja variable or emits a compiler batch. Original guest and product code
are unchanged.

The first owned run PID26204 failed1024 pixel comparisons. This is not a
passing migration receipt. The fixture predated commit151c01c1c, which changed
standard CGA to CHAIN2: paired source bytes use eight-byte planar strides,
not the old four-byte packed stride. The fixture still expected the old
layout. Correct only its standard-scale address oracle; scaled contracts
remain unchanged. This is an independent numerical oracle, not a call to the
production address macro. No failed cases were removed or relaxed.

Final owned PID18880 passed182801 checks,26114 cases/callbacks; wrong-pixel
negative exited1 as required. Map ownership and input hashes passed. Existing
selected-header warnings remain; no product source was altered to silence them.
These are controlled memory-DIB checks, not a guest rendering acceptance.

B0146 is retired: verify-qevnt-object-contract.ps1 was only a seven-line
forwarder to verify-softpc-object-contract.ps1 -Contract qevnt. It had no
additional assertions and no current test/tool callers. Preserve the shared
QEVNT contract and fixture; its remaining native migration is separately
tracked as B0153. Historical records remain evidence, not active dependencies.

[Receipts](../../../artifacts/build-migration/t484/s3-planar/result.json)
retain the failed and corrected outcomes. Raw logs remain build/t484-s3-planar.
This batch resolves two inventory entries:181 complete,174 remaining of355.
Retrospective retention review, S4 publication migration and S5 clean
interpreter-free acceptance remain open. Migration counts do not measure
retention-review completion.

Final gate PID11332 exited0: cached full build, unchanged native fixture
mtime/hash, unsupported-architecture rejection,11 raw host inputs and22
publication hashes passed. Existing published product bytes are unchanged;
no replacement package or guest was produced by this test-only batch.
