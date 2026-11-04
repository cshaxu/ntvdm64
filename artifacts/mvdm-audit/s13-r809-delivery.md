# S13 ordered first-level audit checkpoint r809

This checkpoint follows r769 in inventory order and changes ten source files:
`suballoc.h`, `wow.h`, `reset.c`, `tape_io.c`, `c_main.c`, `c_xcptn.c`,
`printer_.c`, `trace.c`, `yoda.c`, and `base_def.h`. Previously excluded arms
were normalized with original line anchors and `#error`; selected product
logic is retained. No missing original first-level group was restored in this
batch. Per-file evidence is in the updated observed inventory and census.
Selected parent arms with children are recorded in
`s13-r809-depth2-parents.json` for the next depth, without classifying those
children.

Six existing files remain explicitly unresolved: `wowfax.h` and `wowinfo.h`
have original guest consumers; `c_main.h` and `base_def.h` have many product
include contexts not completely covered; the two C-VID generated headers
`cpu4gen.h` and `evidgen.h` share names with CCPU headers and require an
include-order-preserving overlay. They remain in the census and are not
claimed as finished. The `base_def.h` change only adds a diagnostic line
anchor to an already excluded branch; its ANSI alternatives are preserved.

Cached x86/x64 build succeeded. The coherent ten-image package was compared
against r769: six images are identical and four differ solely in independently
parsed PE/debug-directory timestamps, with zero substantive bytes changed.
See `s13-r809-build-image-compare.json`. In accordance with the owner rule,
prior `s13-r682c-full-evidence.json` remains the Full-test evidence; runtime
tests were not repeated. Ten images were installed and hash-verified in both
`assets/release` and `O:/winnt/system32`.

The current census in `s13-r809-counts.json` has 1,058 existing C/H files:
170 reviewed identical, 715 reviewed different, and 173 not yet complete
(167 unreviewed plus the six explicit unresolved files). These are first-depth
review statuses where so marked, not proof that every depth has been audited.
The 578 net deleted C/H paths versus rename-aware baseline `c97d4ee2c` are
separate and excluded from the 1,058. One current file is a documented
generator input outside observed build graphs; the census is not a claim that
all 1,058 are compiled mirror sources. Guest files were not modified or built.
