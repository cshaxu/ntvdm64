# T484 S3 P43: serial discovery probe producer

B0041 now calls `tools/build-fixtures.cmd x86 serial-discovery <mask>` instead
of invoking NASM from PowerShell. The actual host QueryDosDevice observation
still selects the expected COM1-COM9 mask; no fixed machine expectation replaces
it. The native graph owns the NASM command, source dependency and cached output.
CMD rejects missing, nondecimal, oversized and wrong-architecture inputs before
configuration; CMake independently validates the0-511 numeric contract.

Five masks (0,1,255,256,511) each produce exactly the same4200 bytes as the
previous direct NASM command. A repeated511 build does no work and preserves the
output timestamp. Five invalid masks and the x64 selector are rejected with2.
The edited PowerShell runner parses. The new parameter is accepted only for
serial-discovery; existing fixture entries retain their two-argument contract.

[Verification](../../../artifacts/build-migration/t484/s3-serial-probe/verification.json)
records hashes, owned process and actual outcome. Cached full host build,
11 raw product identities,22 publication hashes and documentation governance
are separately recorded. No product or original guest code changes.

B0041 remains pending for its historical root-layout runtime copy/cleanup and
runtime helper review. No real serial-port guest acceptance is claimed for
that old path. This delivery proves construction equivalence and argument/cache
behavior, not complete integration-chain migration. S3 stays active.
