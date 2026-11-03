# S13: MVDM and non-MVDM condition line mappings

Ten C/H files received original diagnostic mappings for 31 already excluded
arms. Three files have had their complete original conditional structures and
selected alternatives reviewed; the other seven retain pending groups.
See [guard evidence](s13-r662-guard-line-audit.json) and
[per-file review](s13-r662-flat-guard-review.json). Original paths follow the
per-file source inventory, including the OpenNT-4.5 DLC header baseline.
No selected code was replaced; full context enumeration remains open.

Cached x86, x86-extra and x64 builds passed. The x86 retained-guard controls
passed 181 checks; isolated MSVC controls passed for 31 original error,
selected and continuation line mappings. Six of ten product images are
byte-identical to the previous release; four differ solely in parsed PE
timestamps ([comparison](s13-r662-build-image-compare.json)). Previous Full
17/17 evidence is reused per owner instruction, not rerun. All ten images
were published and hash-verified at both destinations. No guest image was
modified or rebuilt. [Counts](s13-r662-counts.json) separate deleted files.
