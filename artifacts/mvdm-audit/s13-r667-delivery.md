# S13: conditional-tree review

Sixteen product-observed files were checked against their original conditional
trees. For each original group, the selector and alternatives match; an
existing error-only arm excludes its original nested contents, while retained
arms are recursively compared. Retained leaves match after trimming boundary
whitespace, and original diagnostic and continuation anchors are checked.
[Evidence](s13-r667-tree-review.json) records each original and current file
hash, group, arm, location and disposition.

The audit does not assert equivalence of local code outside conditional groups
or complete supported-context coverage. The survey conservatively held files
with unsupported parsing, unmatched structures, missing anchors, differing
retained leaves or retained-only groups requiring further context evidence.
Those remain pending. No product source or runtime image was modified in this
batch; the prior published build and Full test evidence remain unchanged.
[Counts](s13-r667-counts.json) report existing and deleted files separately.
