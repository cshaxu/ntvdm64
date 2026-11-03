# S13: CCPU non-PIG I/O selected-context review

The original conditional trees for CCPU `in.c` and `out.c` were checked,
including their retained non-PIG bodies and excluded SFELLOW alternatives.
Real Ninja compile commands for the x86 worker and x64 reference-x86 worker
were preprocessed: IN8/16/32 and OUT8/16/32 selected bodies were observed in
both graphs. [Evidence](s13-r672-pig-io-review.json) records each original and
current hash, condition group and preprocessing witness. These observations do
not prove unenumerated contexts or unrelated local adaptations.

The [retained-context survey](s13-r672-retained-context-survey.json) identifies
the first unproved retained-only condition encountered per pending file; it is
a triage list, not an exhaustive count of conditions or proof of exclusion.
No product source or runtime image changed; the prior published build and
Full evidence remain unchanged. [Counts](s13-r672-counts.json) keep deleted
files separate. Remaining contexts and files are still open.
