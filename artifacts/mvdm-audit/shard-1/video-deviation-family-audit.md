# VGA/Video7 source deviations: selected functional paths

Compared frozen OpenNT to the same current source used by the worker link,
using Python `splitlines()` (normalizing line endings) and
`difflib.SequenceMatcher(autojunk=False)` matching blocks. The number is
**original matching lines / original lines**, not a percentage of features,
coverage, behavior restored or physical-device compatibility:

| Source | Original/current lines | Matching original lines | Line similarity |
| --- | ---: | ---: | ---: |
| `v7_video.c` | 524 / 525 | 521 | 99.43% |
| `vga_prts.c` | 3315 / 3317 | 3299 | 99.52% |
| `egawrtm0.c` | 1366 / 1390 | 1356 | 99.27% |

All remain partial-mirror-diff. The selected x86 worker graph compiles each
into `original-softpc-video.lib`. `ninja -t commands` for egawrtm0 gives
NTVDM, CPU_40_STYLE, C_VID and V7VGA, no MONITOR or A_VID command-line
switch; source headers may add macros and must not be inferred solely from
command-line omission. Independent SHA-256 identities and initial line-count
records are in `selected-diff-register-audit.csv`.

## Video7 preserve-memory flag

`ega_vide.c` routes the Video7 extended mode service to
`v7vga_extended_set_mode`. OpenNT validates raw BL before removing bit 7;
current `VIDEO7-PRESERVE-MODE` first obtains `BL & 7Fh` and validates that
mode while preserving original clear/preserve use of bit 7. Other original
mode setup/bank operations remain. `src/mvdm/README.md` records owner-approved
SoftPC T70 S10 adoption and focused/all-mode BIOS witness: 67/E7 clear and
preserve, 16 legal modes with/without bit 7, and two invalid requests.
This is historical recorded testing, not a rerun of the S10 worker image;
its invalid-mode cases test the fixture's documented contract, not every
host side effect before rejection. Recovery gap is bounded: on a current
identified package exercise the same legal/invalid cases and original setup,
raw register/memory values and lifecycle, including no-reset and mode changes.
Do not overwrite it with a source-similarity percentage.

## VGA cursor and helper declarations

`vga_prts.c` adds the original EGA helper prototypes (DIV-067), then fixes
cursor bit-5-only writes and projects the raw start register's low six bits
rather than only its 5-bit geometry to `host_cursor_size_changed` (DIV-358).
`do_new_cursor` retains its geometry branches and applies the off bit after
shape selection; CRTC register writes now preserve the whole value.
`docs/etc/evidence/m0-t448-s11-window-startup.md` records a historical DOS
register probe and instrumented NTVDM publication path with 22h readback and
visible/on-off-on frames. That page also contains a later repeated pre-fix
problem description; only its explicit candidate witness is a pass, not the
repeated paragraph or the entire device. README records the later correction.
Bounded remaining work: verify register/frame transitions on current identity,
including shape-only, off-only, split/overflow and offscreen cases, plus
ordinary/graphics frontend changes; source diffs also include the explicit
`Glue_set_vid_wrt_ptrs` declaration covered by DIV-067. No defect in all VGA
modes is inferred from a single historical witness.

## CPU40 write-table selection and direct writers

`egawrtm0.c` has DIV-079 conditional selection of `CPU_40_STYLE` original
C-video tables and exclusion of legacy direct-pointer mover implementations;
DIV-136 annotates later-original callback parameter types in the *other*
preprocessor branch. The selected `ega_writ.c::SetWritePointers` retains
original chain-2 BIOS byte/word function selection then routes CPU40 to
original `SetWritePointers` / `cvidc/ev_glue.c` EVID selection; it bypasses
the legacy `Glue_set_vid_wrt_ptrs(handler)` branch. The current `egawrtm0.obj`
symbol table contains `ega_copy_b_write` and `ega_copy_w_write`, and common
storage for `mode0_copy_handlers` / `mode0_gen_handlers`; it does not define
the legacy `ega_copy_{b,w}_move*` names. This is build/object evidence that
precisely scopes the compile difference, not evidence all direct writers run.
T420 S22's historical original C-VID test proves publication of non-null
selected read/write/mark families, expressly not execution of every EGA
memory writer. Bounded recovery: tie each selected BIOS byte/word writer to
actual guest chain-2 memory effects across the relevant state/mode transitions,
compare original algorithm and readback, and exercise reset/ownership; retain
non-CPU40 mover behavior as a different profile rather than calling it fixed.

No new runtime/guest test, source mutation or new semantic exception occurred
in this review.
