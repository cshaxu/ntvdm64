# T484 S4 P35: shared copied-guest path preparation

## Scope and design

Win31 and Win95 preparation duplicated Latin1 configuration replacement plus
PIF/GRP iteration in PowerShell. The replacement must preserve all other bytes,
including blank lines, mixed newlines and shell metacharacters. CMD FOR/F is
not an equivalent text editor. Reuse the existing PATH95 bounded byte engine
through explicit replace-text, disabling only its trailing path-boundary test
for this operation. CREG input is rejected; default replace and replace-equal
keep their prior rules. No new parser, executable or guest code change.

The shared tools/rebase-guest-paths.cmd verifies its copied directory owner,
replaces INI/CMD/NT text using a fresh candidate, and invokes released PIF/GRP.
Win31 retains recursive PIF selection; Win95 retains PATCH/WIN.PIF only.
The two integration preparers call this CMD entry. Their remaining driver
selection/configuration is still pending, not counted as migrated.

## Evidence and corrections

Evidence root: build/t484-s4-guest-paths. Baseline preparers are retained there
for comparison only, not current build dependencies.

- Cached full build and addon publication PID7672 completed21:22:21Z–21:23:39Z,
  exit0;11 raw/22 published host identities remain unchanged. Changed addon is
  PATH95 and its manifest, published only under assets/release.
- Native tests cover hive regression plus literal Latin1/NUL/high-byte text,
  blank/mixed lines, shell metacharacters, embedded-root suffixes and rejection
  of a hive by text mode.
- Initial CMD CALL label lookup failed with the authored LF file. Removing
  CALL exposed another negative-path GOTO label failure: the native rejection
  was correct but later copy/delete could run. These test failures are retained.
  The final CMD uses no subroutine or jump labels; failure gates stop subsequent
  operations and return nonzero outside loops.
- negative-linear/result.txt verifies missing-PIF preflight and stale-candidate
  rejection leave both input and existing candidate unchanged. This regression
  is retained in test/build/verify-guest-path-preparation.ps1.
- test-linear-result.json records the final two preparers' complete byte/hash
  comparisons, source identity and owned-copy cleanup. The preceding comparison
  also passed224 Win31 and1166 Win95 files but did not prove the error path;
  final linear version must pass both, not reuse only the earlier success.

This is actual preparation equivalence, not interactive Windows or Full
acceptance. Both authored drivers are copied from release; installed sources,
original guest binaries and winnt.zip remain unchanged. No mirror diff.

## Remaining work

Frozen census348/355 disposed,7 pending, plus five overlapping supplemental
chains. Required driver/profile preparation in the callers remains open.
S4 and final interpreter-free S5 acceptance are not complete.
