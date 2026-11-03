# S13: accessfn non-PROD compilation context

`accessfn.c` is byte-identical to its original and contains one non-PROD
conditional group. The product build intentionally removes `/DPROD` and
`/DCCPU` for this translation unit. Actual x86 worker and x64 reference-x86
Ninja commands were preprocessed, and selected original vector facade
function definitions appeared in both. [Evidence](s13-r674-accessfn-review.json)
records hashes and observations. This is a per-file exception to the common
PROD profile, not a rule for unrelated sources.

The `ev_glue.c` candidate remains pending: its original/current condition tree
requires additional line-mapping review. No product source or runtime image
changed; prior published build and Full evidence remain unchanged.
[Counts](s13-r674-counts.json) keep deleted files separate. Complete supported
context enumeration remains open.
