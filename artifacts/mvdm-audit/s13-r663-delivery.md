# S13: non-PROD and non-NTVDM branch line mappings

Ten C/H files received original diagnostic mappings for 28 existing excluded
`#ifndef PROD` and `#ifndef NTVDM` arms. Selected code was retained. The
[per-file review](s13-r663-flat-guard-review.json) completes five flat-guard
files, including one touched in this batch; remaining groups are pending.
[Guard evidence](s13-r663-guard-line-audit.json) records the original paths,
hashes and physical lines. Missing nested directives under an already rejected
outer arm are intentionally not restored. Complete supported-context
enumeration remains open.

Cached x86, x86-extra and x64 builds passed; 181 retained-guard controls and
28 isolated MSVC line controls passed. Eight of ten images are byte-identical
and two differ only in parsed PE timestamps: [comparison](s13-r663-build-image-compare.json).
Previous Full 17/17 is reused under the owner's instruction, not rerun. All
ten components were published and hash-verified at both destinations; guest
images were not modified or rebuilt. [Counts](s13-r663-counts.json) report
deleted files separately.
