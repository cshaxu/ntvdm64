# Mirror-difference source retention estimate

The [per-file table](mirror-diff-code-retention.csv) covers every one of the
197 frozen mirror-different roles and verifies that each remains partial in
the current ledger. For 196 C, header and definition files it estimates the
percentage of original non-comment, nonblank source lines matched in the
current file, using the existing `EstimateOriginalCodeRetention.py` line
extractor and Python's sequence matcher. The remaining `obj.vdm/sources` is
a build input, for which a source-code retention percentage is not applicable.

Every row records both input hashes and the frozen current hash. The four
current files whose hash has since changed are listed in the [summary](mirror-diff-code-retention-summary.json);
the estimate measures their actual current bytes, not the earlier snapshot.
The [generator](../../tools/audit/EstimateMirrorDiffRetention.py) checks the
original hashes and current ledger before producing the table.

This is a textual estimate of retained original code, not an estimate of
executed branches, correctness or functional recovery. Comment stripping,
line edits, repeated lines and sequence matching affect the number; moved or
equivalent rewritten code may not match. Source selection, actual behavior,
failure and lifecycle evidence remain independent S10 and later-stage work.
