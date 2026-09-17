# M0 T420 S3 — Video package recovery

## Decision

The selected `softpc.new/base/video` unit is complete under the Win32/x86
CCPU40+C-VID product profile.  Its original `sources` manifest selects 19 C
translation units, and all 19 are built into `original-softpc-video.lib`.
There is no missing OpenNT video implementation to import and no duplicate
renderer to retire.

The original writers remain the authority.  `nt_cga.c`, `nt_ega.c` and
`nt_vga.c` publish changed text or graphics rectangles through the original
`InvalidateConsoleDIBits` seam.  The only modern-system implementation is the
existing worker-local public-Console binding: it owns the session-local
four-byte text cells and writes the invalidated rectangle with
`WriteConsoleOutputA`.  It is outside the mirror because NT4 Console Server
mechanics cannot be composed into a modern standalone worker; it neither owns
video mode policy nor reimplements the original writers.

## Frozen manifest and difference ledger

| Disposition | Files | Result |
| --- | ---: | --- |
| Byte-identical to matching OpenNT video source | 9 | Retained unchanged. |
| Source-shaped CCPU40/C-VID compile contract | 8 | Typed declarations and profile selections retain original execution owners. |
| CCPU40 writer-table carrier | 2 | Original storage is retained because the selected C-VID profile does not select legacy `A_VID` mover bodies. |

The current comparison has 10 changed source files, **+76/-165** lines versus
the selected OpenNT source.  S3 removes six duplicate divergence-comment
lines: its own source delta is **+0/-6**, and it adds no mirror file, overlay,
adapter runtime code or autonomous behavior.

Direct removal of the declaration cohort was tested using the generated x86
commands.  MSVC then emitted C4013 implicit-function diagnostics for
`setMarkPointers` and `host_stream_io_update`, plus C4113 callback-contract
mismatches.  Those lines are therefore required source-shaped declarations,
not removable x64 residue.  The explicit function table contracts prevent an
old C ABI from silently treating non-`int` C-VID callbacks as `int`.

## Selection and lifecycle proof

- `sources` selects: `ega_mode.c`, `ega_prts.c`, `ega_vide.c`, `gfx_updt.c`,
  `video.c`, `gvi.c`, `video_io.c`, `v7_ports.c`, `v7_video.c`, `vga_mode.c`,
  `vga_prts.c`, `vga_vide.c`, `ega_read.c`, `ega_writ.c`, `ega_dmmy.c`,
  `ega_trcr.c`, `egawrtm0.c`, `egwrtm12.c`, and `cga.c`.
- `ga_glue.c` remains physically present but is not named by the original
  manifest; it is not a selected substitute.
- Original C-VID `ev_glue.c::setWritePointers` supplies `EVID_WRT_POINTERS`.
  The old `mode0/mode1/mode2` tables are necessary original BSS carriers for
  shared selection code but their native-pointer movers are not selected under
  `CPU_40_STYLE`.
- `video.c` keeps the existing `MONITOR`/`X86GFX` standalone selection only to
  prevent original raw `B800` host alias access.  It routes through existing
  original SAS text writers; removing it would dereference absent standalone
  monitor memory, not restore an available OpenNT mechanism.
- Worker termination owns Console/text-cell teardown.  Video has no original
  reusable-session lifecycle to add.

## Verification

Fresh root: `build/M0-T420/S3/formal-x86-001`.

1. The generated x86 graph selected all 19 original video sources and rebuilt
   `original-softpc-video.lib` using the graph's real compiler commands.
2. The full generated four-program product graph was re-executed sequentially
   under the Visual Studio x86 environment after the ordinary Ninja launcher
   failed to spawn compiler children on this host.  This preserves compiler,
   command line, object and link inputs; only the scheduler changed.
3. The resulting `run16.exe`, `basesrv.exe`, `ntvdm.exe`, and `dtmgr.exe` were
   checked by the existing focused contract gates and the established direct
   and interactive `COMMAND`/`MEM`/`EDIT` matrix before publication.
4. `git diff --check` and documentation-governance checks passed.  CRLF
   notices are repository normalization notices, not whitespace failures.

## Residual boundary

This closes video presentation only.  Graphics display is bounded by public
Console capability: text rectangles are rendered; there is no admitted native
window or replacement renderer.  Keyboard/mouse, C-VID policy, CPU execution,
and WOW remain separate package owners.
