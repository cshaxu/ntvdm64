# T484 S3 P42: COMMAND keyboard fixture producers

B0040 delegates its three CL invocations to `tools/build-fixtures.cmd x86
command-keyboard-shadow`. The native targets retain the authored PIF builder,
detached observer and original COMMAND reader fixture. They reuse current
cmdkeyb and shadow-registry product objects. The runner no longer imports the
entire compiler environment into its own runtime environment.

The first build exposed a stale fixture link: shadow-registry now calls
compat_product_path_a, absent from the old standalone command. The target links
the actual system_root implementation. Its overlapping NtvdmGetSystemDirectoryA
symbol is renamed only in this fixture compilation so the existing instrumented
Console/system-directory input remains intact. No path algorithm is copied,
no product implementation changes, and no new product callback is introduced.

The reader was copied to a fresh test-owned runtime/system32 directory, matching
the current package layout, then run with its registry trace. All four original
outcomes pass: unavailable without guest writes/environment queries, configured,
malformed and absent. The trace independently contains QUERY_OK, QUERY_MALFORMED
and QUERY_ABSENT. NTVDM.REG is removed after the run. The PIF helper generates
its file; both helpers reject missing arguments with64. The edited runner parses.

[Verification receipt](../../../artifacts/build-migration/t484/s3-keyboard-shadow/verification.json)
and adjacent records retain the initial failed link, corrected build and fixture
results. Cached full build,11 raw product hashes,22 published hashes and the
documentation gate are recorded separately. Product bytes are unchanged.

B0040 remains pending: its historical integration staging still assumes root
executables and NTVDM.REG, and its observer hardcodes that old runtime layout.
This batch does not run that stale staging/cleanup path, modify O:/winnt, or
claim real PIF/MEM integration acceptance. The native producer and bounded
reader test are verified independently; whole-chain completion requires the
remaining runtime layout migration. Serial probe migration is also still open.
