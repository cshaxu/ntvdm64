# S13: flat condition groups and WOW32 line mappings

Forty-seven product-observed files were reviewed against their originals in
full for conditional structure and selected alternatives: 42 before this
batch and five after its line updates. Every reviewed original group is flat;
the excluded arm is error-only, physical line mappings match, and retained
else text matches original. See [first evidence](s13-r659-flat-guard-review.json)
and [second evidence](s13-r660-flat-guard-review.json). These reviews do not
certify unrelated local adaptations or close complete context enumeration.

Ten WOW32 C/H files received original diagnostic mappings for 29 previously
excluded arms, preserving selected code. Five of those files are among the
fully reviewed group; other groups in the remaining files are still pending.
See [guard evidence](s13-r660-guard-line-audit.json).

Cached x86, x86-extra and x64 builds passed; 181 retained-guard tests and 29
line controls passed. Seven of ten product images are byte-identical and
three differ solely in parsed PE timestamps: [comparison](s13-r660-build-image-compare.json).
The prior Full 17/17 is reused per owner instruction, not rerun. Ten images
were published and hash-verified at both destinations; guest images were not
modified or rebuilt. [Counts](s13-r660-counts.json) keep deletions separate.
