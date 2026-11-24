# T484 S3 P184: profile and DDE witness producers

Two retained focused integration tests now use the existing `win31-matrix`
CMD/CMake target. No new build entry or compiler recipe was introduced.

| Caller | Retained contract | Full relationship |
| --- | --- | --- |
| `test/integration/multitask/verify-win31-empty-profile.ps1` | Empty PROGMAN.INI profile reads/writes and written group persistence | Focused invocation, not a direct Full entry |
| `test/integration/multitask/verify-win31-progman-dde.ps1` | DDE transactions, four group files and program items; explicit historical unwritten-profile comparison | Focused invocation, not a direct Full entry |

The contracts differ and neither is an obsolete compiler-only probe. Existing
runtime assertions, copy cleanup, negative comparison and source ASM remain
unchanged. Each runner builds the shared target, checks failure, then copies
its specific output into the owned task. No original guest is rebuilt.

## Verification

Owned background run `build/t484-s3-profile-dde/run.ps1`, PID25320, completed
with exit0. Both real witness images are byte-identical to the previous NASM
command. A second CMD build preserves output modification times. Both callers
parse and contain no direct NASM invocation. The previous compiler command is
only an equivalence oracle, not an integration-test production path.

Cached full compilation passed (PID19376, exit0); all11 raw host hashes and22
existing published hashes match. No product republish was necessary.
Verification is recorded in
`build/t484-s3-profile-dde/delivery-result.json` and its delivery log. This batch
does not claim a fresh Windows runtime or Full acceptance run. Product sources,
ABI and runtime assertions are unchanged.

## Scope accounting

The fixed inventory remains316/355 disposed and39 pending. These two scripts
were outside that census and are now explicitly registered as supplemental
native producers pending shared staging, alongside the two matrix callers.
Of the18 additional direct-NASM candidates found in P183, two producer calls
are removed;16 still need retention/migration reconciliation. This does not
close their shared runtime preparation or the entire S3 packet.
