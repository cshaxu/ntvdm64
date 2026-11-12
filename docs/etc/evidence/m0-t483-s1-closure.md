# M0 T483 S1 closure

## Scope and owner decision

Owner authorized closure after audit and verification. S1 completes mirror-diff
classification in fixed directory/file and line order. T483 remains open and no
next S is admitted. This is not closure of functional remediation.

## Results

The [classification summary](../../../artifacts/mvdm-audit/t483-s1/classification-summary.md)
and [ordered register](../operations/m0-t483-mirror-register.md) are the
repair handoff. Census: 1060 host C/H files, 491 identical, 259 exclusion-only,
302 other differences and 8 deletions. All 569 differing files are classified.
3042 decisions: retain 1767; restore 76; delete 120; minimize 117; correct 207;
pending evidence 755. The latter are unproved findings, not authorized repairs.
9596 lexical blocks comprise 9588 live blocks and 8 deletions, versus 9636 at
admission. Restored formatting: 365 sites / 249 files, including 122 C/H files.

Eight proved excluded carriers were deleted, with manifest/reference cleanup.
Selected generated graphs have no references to them. No functional repair was
implemented. Original guest and assets/winnt.zip were not changed. Old exception
records remain auxiliary evidence; incomplete alias associations are explicitly
unproved and do not change the new execution order.

## Verification and publication

Cached Ninja/ccache x86/x64 build passed after regenerating stale graphs. Formal
WOW graph and dynamic host-surface checks passed. Stale test expectations were
corrected to the selected direct OLE descriptor implementation and the two live
WINMM lazy exports, rather than dormant multimedia branches.

All 16 selected Full gates passed. DOS4GW/NESticle was explicitly skipped for
missing external fixtures. Retained WOW startup/frontier checks do not claim
complete gameplay coverage. Earlier package-stamp and guest-layout precheck
failures were corrected; historical failures are not counted as passes.

Package 6ace4e9a-48ea-48ac-abd9-94e966b67c78 contains 11 images, published and
hash-verified at assets/release and O:/winnt/system32. Raw build inputs are not
claimed equivalent to the previous package. See
[delivery receipt](../../../artifacts/mvdm-audit/t483-s1/delivery-verification.json),
[Full results](../../../artifacts/mvdm-audit/t483-s1/full-timings.json), and
[publication](../../../artifacts/mvdm-audit/t483-s1/publication-result.json).

## P delivery and limits

The S1 closure commit contains this record, audit artifacts, approved formatting
restoration/deletions, reference/test maintenance and the verified release.
Parallel QUEUE.md edits and the untracked win203 proposal remain untouched and
outside this commit. The repository commit and origin main identify the final
P revision without embedding a self-referential hash in this file.

Future repair follows stable T483-D0001 through T483-D3042. Do not restore an
original body merely because it was omitted: a modern API may correctly own that
boundary. Pending semantic evidence and diagnostic-line corrections remain
explicitly open. Compat and 11-component minimization are not completed by S1.
