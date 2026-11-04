# S13 complete-file audit checkpoint r1082

Completed full conditional-tree comparison for `yoda.c`, `emm_mngr.c`,
`base_def.h`, `bios.h`, `cga.h`, `config.h`, `cpu4.h`, `cpu_vid.h`,
`debug.h`, `dma.h`, `egacpu.h`, and `egagraph.h`. The per-file ledger links
proof and remaining limits. Ten source files changed; `cga.h` and `dma.h`
needed no source edit.

Original selected branches and existing product-specific bindings remain.
New #error exclusions cover the unselected non-ANSI/LARGE_FRAME EMM branches,
non-NTVDM/SWITCHNPX config paths, the generic video callback and globals
alternatives, and three non-VGA/endian `egagraph.h` arms. Other changes
correct previously excluded branches' original #line anchors. In
`emm_mngr.c` the pinned original includes form feeds: original physical line
numbers were computed from LF bytes, not Python splitlines. The preceding three
commits' changed originals were checked and contain no form feeds.
The 110 `config.h` and 36 `egagraph.h` dependency compilation contexts are
recorded with command identities; no guest source or image was modified.

Cached x86/x64 builds succeeded. The ten-image comparison against the previous
release found six byte-identical images and four PE timestamp-only differences,
zero substantive changes; see `s13-r1082-build-image-compare.json`. In line with
the owner no-repeat rule, prior Full evidence `s13-r682c-full-evidence.json`
is reused; no fresh runtime suite was run. The coherent ten-image package was
published and hash verified in both `assets/release` and `O:/winnt/system32`.

The census records 1,058 existing C/H files: 169 reviewed identical, 727
reviewed different, and 162 incomplete. Historical net deletion is 578 C/H
paths versus rename-aware baseline c97d4ee2c, separate from the 1,058. Earlier
completed classifications remain subject to final evidence audit. One present
entry is a reviewed product-generator input outside observed graphs.
