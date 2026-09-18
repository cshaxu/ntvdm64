# T420 S23 selected video capability baseline

## Frozen selected unit

The original `softpc.new/base/video/sources` manifest selects 19 C units:
`ega_mode`, `ega_prts`, `ega_vide`, `gfx_updt`, `video`, `gvi`, `video_io`,
`v7_ports`, `v7_video`, `vga_mode`, `vga_prts`, `vga_vide`, `ega_read`,
`ega_writ`, `ega_dmmy`, `ega_trcr`, `egawrtm0`, `egwrtm12` and `cga`.
The current formal x86 graph compiles all 19 into `original-softpc-video.lib`,
which is linked by `ntvdm.exe`. `ga_glue.c` is physically present but not in
the original selected manifest and is not a replacement implementation.

The `obj/` tree is ignored historical build output, not a source-mirror diff.
Normalized comparison against the pinned OpenNT video directory finds ten
changed selected source files: `cga.c`, `ega_prts.c`, `ega_read.c`,
`ega_vide.c`, `ega_writ.c`, `egawrtm0.c`, `egwrtm12.c`, `gfx_updt.c`,
`vga_prts.c` and `video.c`.

## Initial difference disposition

| Cohort | Files | Current source-shaped reason |
| --- | --- | --- |
| Same-package ABI declarations | `cga`, `ega_prts`, `ega_read`, `ega_writ`, `gfx_updt`, `vga_prts`, `video` | Original K&R/implicit declarations conflict with reached C-VID/video callbacks under current MSVC. The declarations preserve each original provider and do not add a replacement writer. |
| CCPU40 callback result/width contract | `ega_vide`, `gfx_updt` | `memset4` receives the selected native x86 word pointer type; original mark callbacks are declared `void` and callers use their side effect only. |
| CCPU40 table selection | `egawrtm0`, `egwrtm12` | CPU40 selects original C-VID EVID writer tables. The retained BSS table carriers avoid binding legacy `ULONG`-carried native-pointer movers that are outside the selected profile. |
| Standalone SAS video route | `video` | The finite `MVDM_STANDALONE_SAS_VIDEO` condition prevents the original MONITOR-only raw B800 host alias path and keeps original writes on the selected SAS route. Presentation itself remains in the worker-local public-Console binding. |

No changed body is yet accepted merely because it compiles. S23 must prove a
complete writer-to-guest-memory-to-Console chain, distinguish original C-VID
table publication (S22) from actual writer execution, and then re-evaluate
each row for removal or tighter source ownership.

## Acceptance matrix to complete

1. Text writes, clears, cursor update and upward/downward scroll: real DOS
   guest text witness plus SAS cell and Console rectangle observation.
2. C-VID EGA read/write/mark selection: a current x86 fixture executes the
   selected endpoint against guest video memory, not only checks that a pointer
   is non-null.
3. Mode/display transitions and geometry: positive text presentation witness;
   every graphics-only path is either observed or source-proven bounded by the
   public Console product contract.
4. Worker teardown and resize: the video binding may not retain a stale
   guest-memory or Console owner after a normal or abnormal worker exit.

The existing deployed `COMMAND/MEM/EDIT`, geometry, mouse and resize gates are
regression inputs. They do not substitute for the writer-level witnesses above.
