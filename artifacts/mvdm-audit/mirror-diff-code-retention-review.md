# Mirror-difference source retention estimate

The [per-file table](mirror-diff-code-retention.csv) checks every available
primary OpenNT mirror against current bytes, falling back to OpenNT-4.5 or the
registered OpenNT-src-2 supplement when needed, and verifies that each
different file remains partial in the current ledger. For C, header, assembly and
definition files it estimates the
percentage of original non-comment, nonblank source lines matched in the
current file, using the existing `EstimateOriginalCodeRetention.py` line
extractor and Python's sequence matcher. `obj.vdm/sources` is
a build input, for which a source-code retention percentage is not applicable.

Every row records the selected edition, both input hashes and the frozen current hash. The
current files whose hash has since changed are listed in the [summary](mirror-diff-code-retention-summary.json);
the estimate measures their actual current bytes, not the earlier snapshot.
Original paths with no current file are listed separately in that summary and
remain subject to their ledger dispositions; they have no current text to compare.
The [generator](../../tools/audit/EstimateMirrorDiffRetention.py) checks the
original hashes and current ledger before producing the table.
The supplemental generated NTVDM definition differs by an added export; its
provenance and profile boundaries are detailed in the [export review](export-source-recovery-review.md).

This is a textual estimate of retained original code, not an estimate of
executed branches, correctness or functional recovery. Comment stripping,
line edits, repeated lines and sequence matching affect the number; moved or
equivalent rewritten code may not match. Source selection, actual behavior,
failure and lifecycle evidence remain independent S10 and later-stage work.
