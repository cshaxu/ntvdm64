# S13 per-file checkpoint r1183

Ten mirror files changed: keyba.c, mouse_io.c, ios.c, main.c, ica.c,
gfx_updt.c, v7_video.c, vga_mode.c, cfpu_def.h and nt_emm.c. The inventory
links each original comparison and local binding analysis. The original
CPU30/CPU40 conditional frame around the selected keyboard reset adaptation
was restored. Unselected legacy alternatives in the keyboard and video code
were replaced with error guards; the product CCPU, PIC reset/EOI, Video 7
mouse and device bindings remain. Existing error anchors were normalized to
original physical LF line numbers. Guest source and media were not changed.

The [cached builds](s13-r1183-build-results.json) for x86, x86-extra and x64
succeeded. The [ten-image comparison](s13-r1183-build-image-compare.json)
found seven exact matches and three PE timestamp-only differences against the
preceding release, without substantive changes. Prior Full evidence
`s13-r682c-full-evidence.json` is reused under the owner's no-repeat rule;
no new runtime suite is claimed. The complete ten-image package was published
and hashes verified at `assets/release` and `O:/winnt/system32`.

The [census](s13-r1183-counts.json) lists 1,058 present C/H files: 181
reviewed identical to their original, 812 reviewed with differences, and 65
still incomplete. Rename-aware net 578 deleted C/H paths relative to
c97d4ee2c are separate. Final review must still validate supported contexts,
including non-Ninja addon consumers and shared guest headers.
