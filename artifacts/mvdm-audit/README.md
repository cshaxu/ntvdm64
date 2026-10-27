# MVDM cross-audit: first parallel review

This is an in-progress S10 preparation record, not S9 closure or T acceptance.
The owner authorized four parallel roles; three agents and the primary session
used the disjoint frozen path shards described by `manifest.json`.

`frozen-file-reconciliation.csv` freezes the 4,684 unique MVDM union paths.
`combined-initial-status.csv` is the current merged index; detailed per-file
reasoning and limitations are in each shard directory. Initial counts are
205 partial (all raw mirror differences), 43 excluded (documented profile
or source roles) and 4,436 pending. None is classified full in this initial
pass. Exclusion of a provider does not prove its guest functionality works.

One frozen path, `src/mvdm/README.md`, changed after freezing to correct
obsolete publication wording and remove duplicate rows; see
`initial-summary.json`. No product source or guest media was changed in this
review. `frozen-external-audit-rows.csv` preserves 42,963 extra WOW audit
paths outside the MVDM union for relevance and dependency review.

Review breadth differs by shard. Shard 0 has metadata for 62 diff files, five
source-level detailed reviews and initial source-manifest scans of 43
unselected host files. Shard 1 has 48 differential owner initial reviews
and 34 unselected-role reviews. Shard 2 recovered its interrupted CSV and
has 23 of 48 differential files with detailed trace; the remaining 25
require renewed review. Shard 3 has 47 differential and 23 unselected-role
initial reviews. These are not equivalent to full semantic and test proof.

Read `findings.md` for actionable cross-group discrepancies. Source-line
matching percentages in the frozen input are textual heuristics, not the
requested functional restored-code percentage. Calculate and justify the
latter only when original owner, selected edition and actual behavior are
known. Regenerate the combined index after any shard changes and verify
uniqueness, shard ownership and frozen identity before relying on counts.
