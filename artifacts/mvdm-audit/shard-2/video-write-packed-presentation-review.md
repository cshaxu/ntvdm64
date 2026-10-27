# S10 shard 2: video source writes and Video7 packed presentation

Frozen OpenNT/current SHA256 pairs were rechecked for selected changed
`base/video/video.c`, `base/video/egwrtm12.c` and `host/src/nt_vga.c`.
Each remains partial; selection or source equivalence of unaffected lines is
not proof of all guest VGA behavior.

## Source boundaries and current graph

`video.c` keeps original BIOS character/cursor routines and EGA service
provider. DIV-067 adds the original EGA function declaration. Three MONITOR
conditions additionally exclude `MVDM_STANDALONE_SAS_VIDEO` from the native
pointer shortcut, leaving the original SAS/BIOS writer path under that
configuration. The current x86 graph compiles this source under its C-VID
rule, which includes MONITOR, X86GFX, CPU_40_STYLE and
MVDM_STANDALONE_SAS_VIDEO. This establishes selection of the guarded route,
not by itself every generated writer's runtime behavior.

`egwrtm12.c` retains original mode-1/mode-2 byte/word providers; DIV-143
selects the CCPU40 EVID table/string mover route rather than uninstalled
legacy pointer-width-sensitive WRT string helpers, while DIV-136 makes the
original local declarations match their existing parameter definitions.
Do not reintroduce an unselected legacy host-pointer mover solely to reduce
textual diff. Required mode/plane masks and generated callback routes remain
separate behavioral obligations.

`nt_vga.c::nt_v7vga_hi_graph_std` retains the packed Video7 painter and its
DIB invalidation. The owner-approved VIDEO7-PACKED-SURFACE exception (README
opening entry) bounds x/y/width/height to screen dimensions and bounds source
extent using EGA plane size and offset-per-line, instead of rejecting valid
480-row updates. The corresponding source fixture directly links the actual
painter and checks 640x480, pitched source rows, clipped damage, source end,
invalid parameters and destination guards. It replaces the native invalidation
sink for observation; it is not an end-to-end guest display or device proof.
The test constructs matching `sc.PC_W_Width/Height` and DIB dimensions; the
source's actual bitmap allocation/stride relationship is a separate owner
invariant, not proved for a deliberately inconsistent DIB by that fixture.

## Evidence and finite next checks

T420 S23 records original text writer/mark presentation and mode-13 guest VRAM
write/read plus text recovery for its historical package; its then-current
text-only presentation limitation must not be treated as the current window
renderer verdict. T460 S3 records the actual packed painter fixture and source
contract checks; the README records the owner-approved SoftPC T60 lineage.
These are bounded and versioned evidence, not this review's fresh runtime
result. The VIDEO7 change does not establish all SVGA banks, chain modes or
unrelated SoftPC changes.

For current-package closure, identify the selected EVID table entries and
actual mode-1/2 write, fill, overlapping move and plane-mask behaviors, plus
mode switch/resize and invalidation/teardown in both directions. Verify the
producer maintains matching surface extents, pitch and buffer allocation
before the painter; separately test zero/edge damage and source exhaustion
without inventing a policy for malformed DIBs. Compare guest pixels and
published host frame for applicable modes; a BIOS return or a source-only
fixture is not full presentation proof. The three selected files remain
partial and no whole-file percentage is inferred.
