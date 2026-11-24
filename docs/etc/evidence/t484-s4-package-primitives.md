# T484 S4 P1: native package primitives

S4 will retain CMD orchestration and reuse the existing HASH.EXE and
ntvdm-package-id tools. The existing package-id tool now provides --new
(nonzero canonical GUID) and --machine x86|x64 IMAGE (read-only bounded DOS,
PE, COFF and optional-header architecture inspection). It never executes the
input image. Existing stamp and --read behavior remains unchanged.

No generic JSON engine, script interpreter, duplicate hash implementation or
second packaging graph was introduced. These operations replace the GUID and
PE inspection responsibilities needed by current staging. The actual staging
and publication entrypoints are not yet migrated; no unresolved entry is closed
by this delivery. Frozen323/355 disposed,32 pending plus24 supplemental chains
remain unchanged.

## Verification

Test source: test/build/Test-PackageIdentityTool.ps1. Assertions cover generated
identity, real released x86/x64 inputs, opposite-machine rejection, stamped
copies and exact GUID bytes, source preservation, empty file, bad DOS signature,
out-of-file PE offset, invalid optional-header magic, absent input, unknown
architecture and zero GUID rejection. Only build-owned copies are stamped.

Owned background run: build/t484-s4-package-primitives/run.ps1; launch.json
records PID12764, host, exact command and start. Bootstrap and focused assertions
passed. The run also performs cached full host compilation and compares11 raw
inputs plus22 published hashes to the existing manifest; final result.json is
the terminal witness. Runtime binaries and original guest are not changed.
Governance is run separately before commit. No Full runtime claim is made.

Final result: PID12764 exited0 at2026-11-24 15:12:43Z; cached host build
and all11 raw/22 published hash checks passed. Governance PID22008 exited0
at15:12:50Z. Existing published product bytes remain exact; no recopy needed.
