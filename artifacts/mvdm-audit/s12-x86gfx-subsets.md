# S12 — per-source unselected X86GFX branches

DIV-510 marks 64 simple positive X86GFX branches in 13 product-selected MVDM
host C files whose actual observed commands do not define X86GFX. The selected
`video.c` does define X86GFX and remains untouched. Original conditions and
active alternatives remain; software CCPU/V86, active Video7 and input paths
are preserved. Original bodies match local OpenNT and their locations and
hashes are recorded in the [ledger](s12-x86gfx-subsets.jsonl). Existing project
markers and source-different/compound branches were held for further review.
The original mouse-coordinate observation fixture reuses host flags without
X86GFX; arbitrary external configurations are not covered.

Independent reconstruction verified 13 files and 64 branch replacements
against `8857768f1` with zero unrelated byte changes. Initial NTVDM comparison
showed changed original diagnostic line constants in `nt_fulsc.c` and
`nt_det.c`; registered `#line` restorations retain original line mappings,
including the earlier DIV-497 restore after the X86GFX boundary. Cached
NTVDM rebuild passed; after applying released identity, all ten images
matched the published set exactly or only in PE link timestamps. The original
mouse coordinate fixture passed 17 checks with mocked inputs; the Video7/DOS
source contract passed. These are not physical mouse or full guest acceptance.
No guest was changed, and no runtime suite or republish was required for
unchanged code. Other X86GFX contexts and compound conditions remain in scope.
