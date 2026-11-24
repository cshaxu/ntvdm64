# T484 S3 P182 matrix profile producers

## Additional discovery and ownership

Tracing the P181 PIF consumers exposes two live callers outside the original
355-file lexical census: verify-win31-setup-modes.ps1 and
verify-win95-launch-matrix.ps1. Both still invoked NASM directly. Preserve the
frozen census (315 disposed,40 remaining), and register these two supplemental
caller decisions in the same dispositions.json. They are not whole-chain
complete: configuration/script materialization and staging remain. Thus355 is
not a claim that no additional build dependencies can exist.

One win31-profile target in the existing build-fixtures.cmd/CMake owner now
produces SCHED.COM, FULLKEY.COM, RUNA/B.COM and four prompt/scheduling/pair
controllers. It uses the exact independently authored sources and flags;
no original guest is compiled. Installation path is the bounded ASCII input,
with dependency-driven regeneration and the existing stable fixture cache.
Both runners copy the selected result and retain their old assertions,
blocking/nonblocking policy, process ownership and cleanup. Win95 builds this
group only for scheduling cases. No new build script or parallel toolchain.

## Verification

Background PID12764, build/t484-s3-matrix-profile, exits0 in56.8seconds.
Eight outputs with each of two installation paths (including a space) match
old direct NASM invocations byte for byte. Those old commands are test-only
comparison oracles, never producer inputs. Both repeated same-parameter builds
preserve output mtimes. Invalid relative installation paths fail. Both matrix
callers parse and contain no direct NASM invocation. Subsequent adjustment
moves the build inside their existing try/cleanup scope; final governance also
parses both final files. No new guest runtime acceptance is claimed.

Cached full and11 raw/22 published identity verification run in the same
owned directory; unchanged artifacts are reused only after identity checks.
Remaining S3 and S4/S5 obligations are not closed by this batch.

Final test also restores a valid cache after the rejected configuration.
PID29544 in build/t484-s3-matrix-profile-final exits0 in69.5seconds, including
all16 byte comparisons, no-op checks and final caller parsing. Cached full
PID24328 exits0 in63.8seconds;11 raw and22 published identities match.
Governance/parser/diff PID25288 passes in25.2seconds. No changed product image
requires republication; containing P182 commit is the pushed delivery.
