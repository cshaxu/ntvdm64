# T484 S3 P30: native payload probe producers

Four interpreted producers are retired into CMD/native targets: native process
observation, Run16 unpaired CUI/GUI, mixed-batch CUI/GUI and its authored PIF
writer, and the private mouse hook/probe/Console snapshot tools. Both Windows
matrix callers build private-mouse-tools through an owned hidden CMD process
and use the canonical output directory. Test tools are never published.

The [old builders](../../../artifacts/build-migration/t484/s3-native-probes/baseline-result.json)
were run before retirement. All12 corresponding new images are
[identical except for identified PE timestamps](../../../artifacts/build-migration/t484/s3-native-probes/comparison.json).
The initial DLL comparison found an import-table order difference caused by
an unnecessary explicit kernel32 library. Keeping the original default-library
order removed it. No instruction or other data difference remains.

[Native builds](../../../artifacts/build-migration/t484/s3-native-probes/native-builds.json)
and [16 runtime checks](../../../artifacts/build-migration/t484/s3-native-probes/runtime-result.json)
pass: invalid argument rejection, x86/x64 CUI/GUI quoted argument/exit37/file
witnesses, PIF creation and rejection of overwriting an existing file. Unpaired
probes are checked only at their argument boundary; their actual launcher-loss
behavior is not reaccepted here. Mouse tools reject invalid input; no physical
mouse or Windows guest interaction is claimed by these checks.

No product, mirror or original guest source changes.
[Cached full build](../../../artifacts/build-migration/t484/s3-native-probes/build-result.json)
and [11 raw/22 published hashes](../../../artifacts/build-migration/t484/s3-native-probes/publication.json)
pass unchanged, so no publication recopy is needed. S3 remains active.
