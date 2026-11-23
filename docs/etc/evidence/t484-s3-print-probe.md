# T484 S3 P48: native print witness producer

B0047 assembled its authored NE print witness directly from PowerShell with a
per-run OUTPUT_DIR definition. The CMD/native fixture entry now passes that
directory to NASM through CMake's quoted command generation. The original
assembly, embedded paths and runtime print/abort assertions are unchanged.
No post-build binary patching, original guest rebuild or printer selection is
introduced. The legacy Nasm option remains accepted but does not select a tool.

Verification executes the actual producer block extracted from the final
integration runner, including its background launch, timeout and output copies.
For plain, space-containing and apostrophe-containing paths, the executable and
listing match the former NASM command bytewise. Changing the directory changes
the generated command and rebuilds the witness; an identical repeat preserves
the output timestamp. Missing output directory and x64 selection reject with2.
The edited runtime script parses.

[Verification](../../../artifacts/build-migration/t484/s3-print/verification.json)
records the three path identities and owned run. Cached full host build and
11 raw /22 published image checks are separate delivery evidence. Unchanged
images require no recopy. Equivalent probe bytes reuse prior runtime coverage
under owner authorization; no new guest printing pass is claimed here.

B0047 remains pending for the shared runtime staging chain. S3 is not closed,
and S4 publication migration and S5 interpreter-free clean acceptance remain.
