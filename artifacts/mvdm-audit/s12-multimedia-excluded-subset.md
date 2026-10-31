# S12 — inactive original multimedia branch subset

Owner instruction: retain the enclosing original conditional directives and
replace each verified excluded original branch body with a numbered explanatory
comment. This change covers only five host WOW32 original enclosing literal
`#if 0` blocks with no top-level `#else` or `#elif`. The active generic
`wmmedia.c::WMM32CallProc32` route is not changed. These branches cover older
Wave/MIDI and MCI conversion paths, not guest code; removing them does not
establish the multimedia capability is complete. Recorded divergence:
MVDM-HOST-DIV-495 in the MVDM register.

The source before this change, at commit `6780e9bfb`, matches local OpenNT
`base/mvdm/wow32` after newline normalization for all five files. Per-file
before/after hashes and net physical lines are in
[hashes](../../artifacts/mvdm-audit/s12-multimedia-subset-hashes.json).
The original source and complete text remain available in Git history and
pinned local source. The removed bodies contained 6,649 physical lines; five
markers replace them, for a net reduction of 6,644 physical lines.

The rest of the requested project-wide conditional audit and cleanup remains
open. Literal-zero selection of this bounded group does not prove other
macros are disabled in every supported translation unit or that an entire
no-code object has no data role. Guest sources and guest images remain
unchanged.

Verification for this bounded group: the cached x86 WOW32 target rebuilt
successfully, including NTVDM provider regeneration. After applying the
published package identity, all ten images matched the published images
exactly except for four COFF timestamp bytes each in NTVDM and WOW32. The
other eight were byte-identical. No runtime suite was repeated for these
link-timestamp-only changes, consistent with the approved unchanged-image
policy. No new package was published for this partial audit.
