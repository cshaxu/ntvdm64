# S13: host, redirector and WOW32 condition line mappings

Ten C/H files received original diagnostic mappings for 42 previously
excluded arms, preserving selected code. Seven of these files were checked
against their original conditional groups in full: see the [guard changes](s13-r661-guard-line-audit.json)
and [per-file review](s13-r661-flat-guard-review.json). Other groups in the
remaining three files and complete supported-context enumeration remain open.

Cached x86, x86-extra and x64 builds passed. The x86 retained-guard controls
passed 181 checks and MSVC line controls passed for all 42 groups. Six of ten
product images are byte-identical to the prior release and four differ solely
in parsed PE timestamps; see [comparison](s13-r661-build-image-compare.json).
The previous Full 17/17 is reused per owner instruction, not rerun. All ten
images were published and hash-verified at both destinations. No guest images
were modified or rebuilt. [Counts](s13-r661-counts.json) keep deleted files
separate from current files.
