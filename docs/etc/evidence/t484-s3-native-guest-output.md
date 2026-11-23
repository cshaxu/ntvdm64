# T484 S3 P40: native guest-output probe producer

B0034 now delegates its x86 executable producer to the existing CMD fixture
entry and native CMake graph. The unchanged independently authored C source
retains /W4, /MT, user32 and non-incremental linking. No product, mirror or
original guest source changes. The runtime assertions and scenario remain
unchanged; the compiler child now has owned logs, PID, exit and timeout records.

The previous direct CL command and the native target both produce137728 bytes.
Only two bytes differ, both in PE timestamps; all remaining bytes are identical.
The comparison checks PE directory locations rather than ignoring arbitrary
header regions. A second native build preserves the output timestamp. The
edited PowerShell runner parses successfully. This is producer equivalence,
not a newly executed guest interaction pass; equivalent payload bytes reuse
the existing runtime coverage under the owner's no-repeat authorization.

The initial CMD receipt used an unspaced zero before a redirection operator,
so its exit file contained ECHO status rather than an exit code. The compiler
and linker logs completed; the subsequent owned verification records an actual
zero exit and no-op cache result. The malformed receipt is not passing evidence.

B0034 remains pending for runtime batch materialization and package staging
review. Moving its compiler invocation is not whole-chain completion. The
batch's cached host build and publication identity checks are recorded alongside
the comparison; unchanged product hashes require no recopy.

[Comparison and adjacent receipts](../../../artifacts/build-migration/t484/s3-native-guest-output/comparison.json)
retain the measured outputs. S3 remains active; S4 and S5 remain incomplete.
