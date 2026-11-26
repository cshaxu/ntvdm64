# S10 Ordered Input and Video Repair Progress

Admitted after [S9 closure](s9-progress.md#s9-closure) under the existing
[repair plan](../../operations/m0-t485-h-repair-plan.md). Thirty assigned files,
1060 frozen snapshot points. Counts below are the frozen register population,
not current byte-sensitive diff counts. Measure each complete file at entry;
do not substitute normalized/token counts for original-relative byte differences.
P1 delivered the first file; 29 S10 files remain. Order is canonical relative path.
Whole-file handling and targeted published-package acceptance precede delivery.
Full remains S13; original guest remains immutable.

| Order | File below src/mvdm-host | Frozen points | State |
| ---: | --- | ---: | --- |
| 1 | softpc.new/base/cvidc/ev_glue.c | 4 | Delivered f3dda8496 |
| 2 | softpc.new/base/cvidc/evidgen.h | 3 | Not started |
| 3 | softpc.new/base/cvidc/sevid000.c | 64 | Not started |
| 4 | softpc.new/base/cvidc/sevid001.c | 25 | Not started |
| 5 | softpc.new/base/cvidc/sevid004.c | 3 | Not started |
| 6 | softpc.new/base/cvidc/sevid016.c | 4 | Not started |
| 7 | softpc.new/base/cvidc/sevid017.c | 11 | Not started |
| 8 | softpc.new/base/cvidc/sevid018.c | 9 | Not started |
| 9 | softpc.new/base/cvidc/sevid019.c | 3 | Not started |
| 10 | softpc.new/base/inc/gvi.h | 5 | Not started |
| 11 | softpc.new/base/keymouse/keyba.c | 44 | Not started |
| 12 | softpc.new/base/keymouse/keybd_io.c | 3 | Not started |
| 13 | softpc.new/base/video/cga.c | 157 | Not started |
| 14 | softpc.new/base/video/ega_prts.c | 30 | Not started |
| 15 | softpc.new/base/video/ega_read.c | 7 | Not started |
| 16 | softpc.new/base/video/ega_vide.c | 308 | Not started |
| 17 | softpc.new/base/video/ega_writ.c | 5 | Not started |
| 18 | softpc.new/base/video/egawrtm0.c | 20 | Not started |
| 19 | softpc.new/base/video/egwrtm12.c | 30 | Not started |
| 20 | softpc.new/base/video/gfx_updt.c | 94 | Not started |
| 21 | softpc.new/base/video/v7_ports.c | 17 | Not started |
| 22 | softpc.new/base/video/v7_video.c | 2 | Not started |
| 23 | softpc.new/base/video/vga_mode.c | 9 | Not started |
| 24 | softpc.new/base/video/vga_prts.c | 24 | Not started |
| 25 | softpc.new/host/src/nt_cga.c | 10 | Not started |
| 26 | softpc.new/host/src/nt_event.c | 50 | Not started |
| 27 | softpc.new/host/src/nt_graph.c | 91 | Not started |
| 28 | softpc.new/host/src/nt_hosts.c | 1 | Not started |
| 29 | softpc.new/host/src/nt_mouse.c | 23 | Not started |
| 30 | softpc.new/host/src/nt_vga.c | 4 | Not started |

## P1: ev_glue.c

[Whole-file review](s10-p1-ev-glue.md): 21/1663 ->5/102 (-16/-1561).
All four frozen points reviewed and residuals registered; zero complete frozen
point restorations. One of 30 files reviewed; 4/1060 points reviewed, 1056 remain.
No delivered S10 file yet. Entire 30-file baseline 201/102863 ->185/101302;
newline/EOF and unpaired added conditional guards are counted. The three initial
mapping failures were resolved explicitly, not dropped from the denominator.
Cached build/dual publication complete; final-package targeted tests pending.

P1 final verification: runtime1 cached build, dual publication (26 hashes),
actual C-VID fixture with 30 added dirty-span cases, live MEM/NTVDMD and
NESticle/Escape passed. All census hashes match. One file final-verified;
P1 commit/push pending. Four residuals remain owner-review items, not restored.

P1 delivery receipt: f3dda8496 committed and pushed to origin/main.
One of 30 files delivered; 4/1060 frozen points reviewed, 1056 remain.
Non-error total=185/101302, cumulative reduction=16 points/1561 lines.
Four evidence-backed residuals remain pending; restored frozen points=0.
Documentation governance and links passed. Next ordered file: evidgen.h.
