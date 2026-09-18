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

## Fresh formal-worker graph prerequisite

While preparing the required worker-local C-VID observation build, a clean
formal graph exposed that the generator created neither `obj/worker` nor
`obj/basesrv`.  Those are required by the generated MIDL service output and by
the `ntvdm.exe` worker objects; the fixture-only graph did not require them and
therefore masked the defect.  The generator now derives every `obj/` output
parent from the emitted graph, including nested owners such as
`obj/ntvdm/session`; no hand-maintained target directory list remains.  A new
`formal-x86-cvid-r7` root verified that every declared object-output parent is
pre-created.

On this host, native Ninja can remain at its initial dispatch lock even though
the generated graph is valid.  `Invoke-T310OriginalSoftpcSerial.ps1` is the
bounded execution fallback: it obtains only the selected target's command
order using `ninja -t commands`, then runs those exact commands under the
graph-declared x86 Visual Studio environment.  In a fresh
`formal-x86-cvid-r6` root it completed all 466 `ntvdm.exe` commands, produced
`ntvdm.exe` (3,204,608 bytes), and passed the link-time
`Verify-VdmTibStorage` owner/overlap check.  This is a build-graph and host
dispatch correction only: it changes neither the selected video sources nor
the product runtime semantics.

## Guest transcript regression gate

The shared ConPTY geometry runner now treats its raw guest Console transcript
as an acceptance input, rather than accepting its host observer exit alone.
Each geometry, mouse and resize row must contain the DOS COMMAND banner, the
MS-DOS Editor screen identity, and at least three `MEM` conventional-memory
reports (including the post-EDIT runs). It fails on `Bad command or filename`
or the native COMMAND command-resolution diagnostic.

The existing deployed 80x25 capture was checked against those predicates:
all three required markers were present, it contained 25 `MEM` reports, and
neither forbidden diagnostic occurred. A fresh single 80x25 ConPTY run also
captured all markers and passed the same predicates; its observer report was
`wait=0 exit=1`, the established guest COMMAND exit result. The observer
process itself returns zero when that report is a pass.

## INT 10 text-write and scroll witness

`tests/observation/video_int10_writer.asm` is a test-only DOS COM fixture.
It invokes the selected worker's original INT 10 `AH=09h` attributed-write
service, then `AH=06h` upward-scroll service over the same two-row rectangle,
and finally uses `AH=0Eh` teletype for a post-scroll video marker. It does not
link, replace or call a host-side video provider.

`Build-T420S23VideoGuestTest.ps1` assembled the fixture below the task-owned
build root; `VIDTST.COM` SHA-256 was
`bd02e4806e0a770e47798c230afd9f903a9167b562f8a0949147811e2069a0f4`.
The temporary `W:` mapping pointed only at that build root and was removed
after the run. The `terminal-observer.exe --video-int10` run captured
`O:\winnt\logs\t420-s23-video-int10-r3.raw` and its cell snapshots.

The transcript contains the DOS completion marker `S23_INT10_WRITER_OK`,
`VVVV`, the post-scroll `S23I`, and the subsequent `MEM` conventional-memory
report. It contains neither command-resolution diagnostic. Cell snapshots
first show `VVVV` on row 15 (the upper row after the intended upward scroll)
and then `S23I` on row 18. Thus this is a real guest INT 10 attributed-write
and scroll-to-public-Console witness, not a host exit-only observation.

This closes only the text-write/scroll/Console portion of the S23 matrix. The
separate SAS-cell attribution, C-VID EGA read/write/mark execution,
mode/display families and teardown/resize ownership remain required.

The reproducible focused gate is:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/audit/VerifyCvidIntegrated.ps1 `
  -FixtureRoot build/M0-T420/S23/console-regression-r3 `
  -LogPrefix t420-s23-video-int10-r7 `
  -VideoGuestFixturePath build/M0-T420/S23/video-int10-r3/VIDTST.COM -VideoOnly
```

It passed with `PASS video INT 10 guest write/scroll`; the child observer
recorded `video-int10 wait=0 exit=1`. `VideoOnly` deliberately avoids
re-running the broader geometry and EDIT matrix while iterating this one
writer witness; the full matrix remains a separate S23 closure requirement.

## Direct C-VID text-memory witness

`tests/observation/video_direct_vram.asm` is the complementary test-only DOS
COM fixture.  It first asks the original INT 10 mode service for mode 3 so the
selected original text update state is established.  It then writes directly
to `B800:` with `REP STOSW`, reads every `VVVV` cell back through that same
guest mapping, writes a second `S23I` range, and reads its first and final
cell back before using DOS only for its completion string.  It does not use an
INT 10 text-write or scroll service as a substitute for those stores.

This is the selected path in the sources: `vga_mode.c` selects
`ega_text_update` for the text mode, `ega_writ.c` and `ega_read.c` publish the
original `Glue_writes` and `Glue_reads` providers, and `nt_vga.c` reaches the
single public-Console adapter only through `InvalidateConsoleDIBits`.  Thus
the witness exercises guest text-memory read/write followed by the selected
original update/presentation boundary.  It does not claim that every
unselected graphics provider is a text-console feature.

The `direct-vram` fixture SHA-256 is
`4d53200116fad6b2898a7eea36d559f59eb08bfeb256eb3f0db42a7f3b46a3fb`.
The fresh full run used `t420-s23-direct-vram-full-r4` under
`O:\winnt\logs\`: all 80x25, 45x34, 60x50, 120x30, mouse and resize
rows passed their raw guest transcript and cell assertions; the direct-VRAM
row recorded `VVVV`, `S23I`, `S23_INT10_WRITER_OK`, and a subsequent `MEM`
report; five short-history EDIT-to-MEM repetitions passed.  This closes the
real C-VID guest read/write/mark-to-presentation and text-mode/resize rows of
the matrix.  The remaining disposition is limited to graphics-only providers:
they are not a public text-Console presentation contract and must be recorded
as source-proven profile/null or a future separately admitted graphics
presentation capability, rather than fabricated as text output.

The reproducible full gate is:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File tools/audit/VerifyCvidIntegrated.ps1 `
  -FixtureRoot build/M0-T420/S23/console-regression-r3 `
  -LogPrefix t420-s23-direct-vram-full-r4 `
  -VideoGuestFixturePath build/M0-T420/S23/video-direct-vram-r3/VIDTST.COM `
  -VideoGuestRoute direct-vram
```

## Graphics-memory disposition

Graphics providers are selected, not profile-null: the formal video rule uses
`MONITOR`, `X86GFX` and `MVDM_STANDALONE_SAS_VIDEO`; original `vga_mode.c`
selects `ega_graph_update` and `vga_graph_update` with
`EGA_GRAPHICS_MARKING`.  A focused test-only mode-13 fixture then directly
wrote and read `A000:0000`, restored mode 3, and emitted
`S23_GRAPHICS_VRAM_OK`.  Its SHA-256 is
`eb0f9d1f7c20afeb8cabb98b321e0560e138b986632a5f7e7e67f8666ab9301f`;
the `t420-s23-graphics-vram-r1` guest transcript gate passed.

This proves selected graphics guest-memory execution and clean text recovery;
it intentionally does not claim a graphical pixel presentation in a terminal.
`Verify-T420S23GraphicsDisposition.mjs` verifies the precise current boundary:
the public Console adapter copies only the registered text surface, forwards
non-text invalidations in their original event shape, and returns
`ERROR_CALL_NOT_IMPLEMENTED` for a graphics palette rather than inventing a
Console palette.  The only `session_set_video_event_sink` occurrences are its
declaration and definition, so no product graphics sink is installed.  The
queued `kvm-window` proposal is therefore the one explicit future worker-local
graphics presentation owner.  It must not be silently implemented by S23 or
represented as a text-Console pass.

```powershell
node tools/audit/Verify-T420S23GraphicsDisposition.mjs .
powershell -NoProfile -ExecutionPolicy Bypass -File tools/audit/VerifyCvidIntegrated.ps1 `
  -FixtureRoot build/M0-T420/S23/console-regression-r3 `
  -LogPrefix t420-s23-graphics-vram-r1 `
  -VideoGuestFixturePath build/M0-T420/S23/video-graphics-vram-r1/VIDTST.COM `
  -VideoGuestRoute graphics-vram -VideoOnly
```

## S23 closure accounting

The selected-video mirror remains ten files different from pinned OpenNT,
totalling `+160/-76` physical normalized lines; S23 adds no mirror-side
provider replacement and no adapter graphics substitute.  Its additions are
three test-only guest fixtures and two verification gates.  The source-level
residual rows remain the prior declared ABI, CCPU40 table and standalone SAS
carrier boundaries; this acceptance pass neither hides nor expands them.
