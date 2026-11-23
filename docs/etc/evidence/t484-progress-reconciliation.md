# T484 original-candidate reconciliation

The [355-row snapshot](../../../artifacts/build-migration/t484/progress.json)
joins the original DIR-ordered inventory to current file bytes, delivery
commits and their evidence documents. Stable B identifiers follow original
file order. The snapshot records its HEAD; it is not a timeless count.
The read-only tools/audit/track-build-migration.py refreshes it, never as a
build input. [Explicit dispositions](../../../artifacts/build-migration/t484/dispositions.json)
separate reviewed decisions from mechanical file status.

The snapshot uses e82c74ff1 plus its explicitly recorded worktree changes:
113 original candidates are removed,29 changed and213 unchanged. Only112
removals have committed delivery evidence; the P33 deletion is still local
at snapshot time. Ten retained entries have native migration delivery, two
are reviewed declarative/audit-only inputs,13 still perform build/publication
work, and218 require disposition reconciliation. These218 are not218 proved
remaining producers. Similarly,207 interpreted candidates have command-related text,
including comments, queries and delegates; this is a review queue, not a live
call count or failure count.

Earlier modification does not close a producer: worker performance, WOW page
profiling, frontend-window library tests, two mouse-driver test producers and
the S4 compat95 publisher still contain actual interpreted production work.

This covers the original355 candidates only. New files, indirect calls,
current graphs and interpreter-free clean validation must also be reconciled
before closure. Batch counts alone establish no completion percentage.
