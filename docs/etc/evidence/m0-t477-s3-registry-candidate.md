# T477 S3 registry candidate verification

## Question and scope

Does the initial executable-owned VM registry correctly isolate identity and
retirement while composing the real frontend-base service thread? The
[design](../operations/m0-t477-s3-vm-frontends-design.md) defines the admitted
contract. This is intermediate unit evidence, not S3 delivery or actual guest
VM integration.

## Inputs and procedure

On 2026-11-05 the owned background build runner used VS2022 x86 tools,
`cmake --preset msvc-x86-release-ccache`, then
`ninja -C build/.ninja/msvc-x86-release/worker ntvdm-vm-frontend-test.exe`.
Compiler commands fixed CCACHE_DIR, CCACHE_TEMPDIR and CCACHE_MAXSIZE to the
repository policy. The first graph attempt failed because its test edge
preceded the link rule declaration; moving the edge after the rule corrected
that build-only error. No runtime result was claimed for the failed attempt.

The corrected build supervisor PID was 20172, build cmd PID 27540, exit 0.
The owned unit supervisor launched fixture PID 6224 at
2026-11-05T21:48:24.1825087Z with a 60-second test-only diagnostic budget.
The fixture exited 0 and printed `PASS 38 registry assertions`.
Temporary stdout/stderr, exit records and runner scripts are in
`build/t477-s3`; the build tree remains the shared `.ninja` cache.

| Input/output | SHA-256 |
| --- | --- |
| src/ntvdm-exe/frontend/vm_frontend.c | f217f6cf009000a572ac63d253d1b751e46c7230704135c4c96710c46bb3ed73 |
| test/ntvdm-exe/frontend/vm_frontend_test.c | f6b44f4f650d8bc6effa016239e8c74fecc700c7a28401e4fe0af28579e518ab |
| ntvdm-vm-frontend-test.exe | 9594f4891f526cf9b4f9ce8b721c6190f0f6c74921a46612b0b306d6f38841ad |

## Observations and limits

The fixture checks duplicate VM rejection, two independent registered entries,
retirement of one while its sibling remains addressable, rejection of the old
identity, renewed incarnation on handle and payload-registry reuse, rollback
after registration followed by state-event failure, and broker-loss cleanup.
It uses the production registry and shared service thread, but a deterministic
BaseSrv control provider. It does not exercise an active I/O channel, actual RPC,
downstream task execution, real guest identity or the worker shutdown path.
Those remain mandatory S3 work, as do full regression, review, complete-package
publication, commit and push. No mirror or guest bytes changed in this batch.
