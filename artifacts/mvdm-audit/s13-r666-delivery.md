# S13: compound and profile branch line mappings

Ten C/H files received original diagnostic mappings for 29 existing excluded
compound and profile arms. Four completed original conditional structure
review; the remaining six retain pending groups. See [guard evidence](s13-r666-guard-line-audit.json)
and [per-file review](s13-r666-flat-guard-review.json). The 29 isolated MSVC
line controls substitute a selectable test condition for the original complex
selectors; they test line mapping, not all combinations of the original macros.
Complete supported-context enumeration remains open.

Cached x86, x86-extra and x64 builds passed; the existing x86 guard suite
passed 181 controls. Six product images are byte-identical to the prior release
and four differ solely in parsed PE timestamps; see [comparison](s13-r666-build-image-compare.json).
The prior Full selected gates and Console/Window 17/17 are reused under the
owner's instruction, not rerun. All ten images were published and hash-verified
at both destinations; guest images were not modified or rebuilt.
[Counts](s13-r666-counts.json) keep deleted files separate.
