# T481 S20: test reduction and scope

Baseline: eb1acd5658950c1ab0afe58424c6302c7576c9b2 (S19). Owner approved
removing duplicate/meaningless tests and consolidating setup. Product and
original guest implementations are unchanged. This is a finite maintenance
review, not a claim that every retained assertion is irreducible.

## Dispositions

- Retired 95 legacy reconstruction/adapter fixture files: their old runner,
  monitor, transport or replacement interfaces are no longer production
  contracts. The one unique DTA layout fixture was preserved under
  test/mvdm-host/dos/dem/dos_find_layout_test.c and its x86 builder repaired.
- Retired 14 root test files for old WHPX/runner/bootstrap/source-count
  contracts, with no maintained build or test consumers.
- Retired 23 completed mirror-edit batch replay scripts: they assert historical
  migration counts, scratch paths and source snapshots rather than a current
  product contract. Existing audit artifacts and Git history retain evidence.
- Total retirement: 132 files, 12,238 LF-counted lines. Detailed paths, SHA-256,
  sizes and reasons are in build/t481-s20/retired.json. No failing current
  behavior test was removed to make a gate pass.
- Preserved pinned lib-nxvm duplicate fixture headers: upstream byte identity
  is deliberate, so local deduplication would violate the import contract.
- Moved 130 real package/guest runners to test/integration. Service and native-worker
  bounded fixtures remain unit tests in their owning component directories.
  Observer executables, fixture sources and analyzers remain shared support,
  not additional integration cases. DOS identity research remains explicitly
  unresolved; its integration runner is preserved, not promoted to passing.
- Three WOW palette/kerning/embedding runners now use one narrow
  test/integration/support/wow-guest-probe.ps1 helper for independent probe
  build, physical runtime stage, observer, environment restoration and cleanup.
  Case assertions remain in their runners. Embedding reads the copied report
  so the existing expected-stall contract is preserved.

Test tree footprint: 1,646 to 1,516 files; 194,502 to 182,295 LF-counted
lines (net -130 files, -12,207 lines, including the new README and helper).
These counts include fixture data and support files, not just executable tests.

The classification map is build/t481-s20/classification-moves.json. Active
callers and documentation links were updated; historical retired-source links
pin the original Git revision. See [test policy](../../../test/README.md).

## Verification

Owned hidden job logs: build/t481-s20/job. Cached x86/x64 builds passed;
DTA layout built with Ninja and ran successfully. Before/after palette,
kerning and embedding probes all passed (six actual WOW runs) using the same
frozen S19 package. Full completed 16 selected gates successfully in 332724 ms;
Console/Window workloads and lifecycle checks passed. External DOS4GW/NESticle
is an unavailable-fixture skip, never a passing result. Full details are in
build/t481-s20/full/timings.json and job/full.out. Syntax, documentation,
reference and diff checks passed before P delivery (job/final-checks4.out).

Package 7773a9b4-e360-4469-962a-df26bb16faf4 remains published: all 11 image
hashes match its frozen manifest at assets/release and O:/winnt/system32.
No runtime implementation changed, so no new package identity or installation
replacement is needed. No new Win3.1/Win95 manual acceptance is claimed.
winnt.zip and installed Windows profiles are unchanged.

Parallel QUEUE/Win95 Setup proposal edits and tools/win-multitask deletions
are not part of S20. Their remaining dirty status must not be reported as a
failure to commit S20, nor as a globally clean worktree.
