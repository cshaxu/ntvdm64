# S13: original debug guards in MVDM and non-MVDM

Six previously changed DEM, redirector and WOW files completed their
conditional review: all original condition groups are DBG guards, rejected
arms remain error-only, and retained alternatives match the original. See
[r647](s13-r647-dbg-only-review.json). Line counting uses CR/LF physical
lines; Python's broader text `splitlines` can incorrectly count historical
Latin-1 control characters as line breaks. A mistaken local `wcommdlg.c`
line adjustment detected during review was reverted before the build.

Ten further mirror files received original mappings for 177 existing rejected
DBG/DEBUG arms, with selected code retained. Other groups in these files remain
pending; see [r648](s13-r648-mixed-debug-guards.json). The mirror negative
test passed 181 retained controls. A combined negative probe initially failed
because MSVC stops at the first `#error`; independent negative invocations and
the combined positive/continuation control then passed all 177 checks.

Cached x86, x86-extra and x64 product builds passed. Two product images are
byte-identical and eight differ only in parsed PE/debug timestamps; see
[comparison](s13-r649-build-image-compare.json). Prior Full 17/17 is reused
per owner instruction, not rerun. All ten components were published and
hash-verified in both destinations. Guest files were not changed. Complete
supported-context and remaining file review remain open.
