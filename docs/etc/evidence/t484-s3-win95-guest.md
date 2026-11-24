# T484 S3 P185: Win95 guest-only witness preparation

The retained focused regression checks internal DOS execution with DOSONLY=1,
absence of a native CUI worker, and optionally the exact DOS refusal stub of a
specified AMD64 PE image. It does not assume DOS EXEC rejects every PE file.
The existing stub-byte/entry checks and all runtime assertions are unchanged.

`verify-win95-guest-only.ps1` now uses the existing fixture graph through
`build-fixtures.cmd x86 win95-guest-only`. Its ONLY/REJECT COM variants preserve
original NASM input and the sole REJECT_NATIVE define. The target depends on
the existing native path binder. That binder now accepts the two explicit
128-byte slots @OUTPUT@ and @NATIVE@, reusing unique-marker, bounds, ASCII,
capacity and validate-before-save behavior. No new executable or state machine
is introduced. The runner no longer assembles or patches COM bytes itself.
Original guest and product sources are untouched.

## Evidence and limits

`build/t484-s3-win95-guest/run.ps1` (PID28644) passed with exit0:

- Both unbound COM variants match the previous assembler invocation byte for
  byte. The old command is used only as a comparison oracle.
- Existing shared binder tests pass: original Win31 paths, six real controllers,
  seven COM variants/both roles and18 existing rejection cases.
- Both Win95 templates preserve every byte outside their named slots; invalid
  second-field binding leaves the input unchanged.

The expanded checks reside in the existing `verify-win31-probe-paths.ps1`,
not a separate test framework. These are preparation/equivalence tests, not a
new Win95 desktop acceptance run. Shared patch configuration and runtime
staging remain explicitly pending.

Cached full compilation and11 raw/22 published host hash checks are recorded
by `delivery-result.json` in the same build directory. Unchanged product hashes
reuse the existing publication; this batch does not claim a new Full runtime.

The original census stays316/355 disposed,39 pending. This is a fifth
supplemental caller with native production complete but shared preparation
pending. Of P183's18 additional NASM callers, three are now migrated and15
remain for retention/migration reconciliation.
