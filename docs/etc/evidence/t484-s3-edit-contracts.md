# T484 S3 P52 edit contracts

## Question and implementation

B0057 generated CL/LINK commands from PowerShell and defaulted to the retired
`wow32/obj/wmsgem.obj` layout. The new `wow-edit-contracts` native target links
the current `original_wmsgem` production object directly. It retains the two
existing fixture sources, their failure seams, `/O2 /MT` profile and all30
assertions. No mirror, product algorithm or original guest changes.

`tools/build-fixtures.cmd x86 wow-edit-contracts` owns production-object and
fixture construction. The PowerShell runner only delegates construction,
copies the resulting executables and evaluates cases. Each build/case records
an owned hidden process, deadline, exit code and stdout/stderr. An optional
Object argument must identify the current production object; stale or arbitrary
objects can no longer silently stand in for production.

## Evidence

The run under `build/t484-s3-edit/run` passes all24 tabstop combinations
(count, heap ownership, native result and cached callback) and all6 wordbreak
cases (zero, two guest pointer forms, native failure/success and cached native).
The selected production object's path and hash are recorded. This is bounded
native contract evidence, not a fresh Win16 guest integration pass.

Retained results are under `artifacts/build-migration/t484/s3-edit/`.
Cached full host build passed, with all11 raw product inputs and22 hashes at
assets/release and O:/winnt/system32 unchanged. No product recopy was needed.
Unchanged product identity reuses existing runtime acceptance, not a new run.
The native target's no-op rebuild leaves both test executables unchanged;
the CMD entry rejects x64 with exit2. The first cache-check invocation used
forward slashes in a CMD script path and failed before building; its retained
failure is superseded by the successful corrected invocation, not erased.
PowerShell parsing and documentation governance passed.

## Disposition

B0057 has no remaining interpreted producer: CMake owns its compiler/linker
commands; PowerShell remains a standalone assertion runner. Other test
producers, shared runtime staging and final interpreter-free clean acceptance
remain T484 obligations. Existing historical seam files are unchanged inputs;
this migration does not claim a test-source layout reorganization.
