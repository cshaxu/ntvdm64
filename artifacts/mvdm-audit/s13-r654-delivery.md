# S13: completed conditional reviews and platform line mappings

Twenty-six files with original DBG/DEBUG-only groups completed conditional
review; retained alternatives match original and excluded bodies remain
error-only. Twenty-one were checked with comment-aware directive screening;
physical line numbering uses CR/LF, not Python's broader text line splitting.
Two additional headers are byte-identical to the original: the CMOS NTVDM
branch was selected by actual MSVC preprocessing, and the other retains its
original include guard. See r650–r652 evidence. These are source-condition
reviews; registered nonconditional adaptations and unobserved contexts remain
separate.

Ten other files received original error, selected-arm and continuation line
mappings for 33 already excluded platform/device groups. Their remaining
condition groups are still pending; see
[r653](s13-r653-platform-guard-lines.json).

Cached x86, x86-extra and x64 builds passed, along with 181 retained guard
controls and 33 isolated MSVC line controls. Eight product images are byte
identical to the prior release and two differ only by PE timestamps; see
[comparison](s13-r654-build-image-compare.json). Prior Full 17/17 is reused
per owner instruction, not rerun. All ten components were published and
hash-verified in both destinations. Guest files were not modified; complete
supported-context and remaining file review remain open.
