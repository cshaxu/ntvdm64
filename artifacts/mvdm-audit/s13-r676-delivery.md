# S13: observed retained-arm contexts

The conditional trees in `nt_ega.c`, `nt_com.c` and `nt_thred.c` were
compared recursively against their originals. Each retained arm had a marker
selected by at least one of the actual x86 and x64-reference worker compiler
contexts; error-only arms keep original outer structure and line mappings.
[Evidence](s13-r676-witnessed-review.json) records every group, arm, source
hash, marker and context. Original and instrumented include traces matched
in order for all six file/context pairs.

The `floppy_i.c` candidate remains pending because an excluded arm's original
error-line anchor is missing. Temporary markers may still affect constructs
such as `__FILE__` or `__LINE__`; they establish observed selection, not
complete runtime equivalence or all supported contexts. No product source or
runtime image changed; prior published build and Full evidence remain
unchanged. [Counts](s13-r676-counts.json) keep deletions separate.
