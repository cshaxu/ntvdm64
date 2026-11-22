# T484 S2 P1 native data generators

## Delivered boundary

This is the first S2 batch, not S2 or T484 closure. Two interpreted generators
are removed: Generate-FrontendFont.ps1 and GenerateCvidcCpuBinding.mjs.
Their production callers now use fixed native data transformers built by
tools/build/bootstrap.cmd. CMake/Ninja and MSVC compile these tools in the
stable build/.ninja/build-tools tree; no generator launches another process or
interprets build instructions. They are not extra released host components.

The font tool preserves the pinned V7 ROM hash and both256-glyph tables.
The C-VID tool derives public/private slots,197 selected CPU declarations,
the SAS vector,38 video offsets and the V7 plane constant from the existing
original metadata. No frozen ABI copy, original guest modification or guest
recompilation is introduced. The host source manifest retains its provenance,
and the native font directory and ROM invalidation dependency are explicit.

The original mirror code delta is zero points/zero lines. The MVDM README's
generator references change; no original C/assembly/header body changes.

## Evidence

Retained [font cases](../../../artifacts/build-migration/t484/s2-native-generators/font.json)
and [C-VID cases](../../../artifacts/build-migration/t484/s2-native-generators/cvidc.json)
identify the exact native tools and all five output hashes. These hashes equal
the pre-migration production-generated inputs. The font suite covers exact
bytes, unchanged mtime, wrong/truncated ROM, output failure and immutable input.
The C-VID suite covers four exact outputs, unchanged mtimes, rejection of a
changed declaration count and preservation of all prior outputs on rejection.
Sources live under test/tools/build; their PowerShell assertions remain
standalone tests, not prerequisites of source generation or compilation.

Final cached full build used the current successful x86/x64 product runners,
PID8968, 2026-11-22T16:15:09Z through16:18:12Z, exit0. It includes all eleven
host outputs and the existing x64 reference graph. This is cached full evidence,
not interpreter-free clean-host configuration: the remaining legacy graph
generators and post-link check still use interpreters.

[Raw image comparison](../../../artifacts/build-migration/t484/s2-native-generators/raw-comparison.json)
against the previous accepted build inputs found5 raw-identical images and6
with only PE timestamp-field differences. The comparator locates COFF,
export and debug timestamps from PE structures and requires every changed byte
to be within those fields. Each changed image has6 changed bytes. These are
not reported as equal raw hashes. Package stamping is a separate later step.

Publication used the existing staging/publishing implementation, which remains
S4 migration work. Package9459f9ca-4f2f-4643-bac7-e4c2b92149bb contains11 images;
assets/release and O:/winnt/system32 were both published and all22 hashes
verified. PID32188 exited0; no running installed instance needed termination.
No addon, installation, original guest or winnt.zip was changed.

Post-publication checks (PID18976, exit0) verified the actual linked C-VID GDP
contract (154 public/55 private slots) and strict library boundaries over9
graphs/434 A/B objects, then rechecked all22 published hashes. Prior Full20/20
runtime evidence is reused under the owner's raw-identical/verified-timestamp
authorization; no new Full runtime pass is claimed. New generator tests were
run and their exact executable hashes retained.

Early local candidates exposed CMD quoting and MSVC regex portability errors;
those were corrected before the final build. The first font mtime assertion
also compared local time with UTC; its corrected final suite passed. The final
candidate results above do not relabel those earlier attempts as passes.

## Remaining work

Documentation governance, relative links and diff checks passed in the owned
background run PID22456. The first launch PID9856 produced no completion
record and is not counted as a pass. The containing S2 P1 commit identifies
this delivered batch.

The four host graph generators, configure-time hashing, TIB post-link check,
test projections/producers and publication remain to migrate. Root CMake still
requires PowerShell/Node. tools/build.cmd is not yet the final entrypoint.
The [S1 worklist](../../../artifacts/build-migration/t484/files.csv) is a fixed
baseline; this batch closes two legacy-generator migrations only. S2 stays
active and S3-S5 are not admitted by this delivery.

The retained run roots are build/t484-s1-inventory and
build/t484-s2-native-generators. Current-task non-cache data is bounded below
128MiB; native binaries/objects belong only to the stable cache. Keep small
result/source-identity evidence, then dispose current-task copied runtime and
rollback trees after the containing commit/push under the existing cleanup rule.
