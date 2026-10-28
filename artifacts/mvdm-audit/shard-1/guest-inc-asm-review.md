# S10 shard 1: Win16 assembly declarations

Scope: `wow16/inc/gdidefs.inc`, `paswindw.inc`, `win3deb.inc`, `windefs.inc` pending when selected. Only read-only comparisons and textual build tracing were performed; no guest build, product execution or Ninja graph inspection.

All four current files match the corresponding files in both `O:/repos.external/OpenNT/base/mvdm` and `O:/repos.external/OpenNT-4.5/nt/private/mvdm` (respective SHA-256 prefixes `bebaf7a5e8fc3f44`, `e853182ddb41a397`, `71136d4520fd43d3`, `6b39e28aaf304051`). Use Get-FileHash for the full reproducible values.

`windefs.inc`: explicit MASM includes appear in `drivers/keyboard/keyboard.asm:24`, `kbdlocal.asm:28` and `drivers/sound/sound.asm:24`; the checked-in component makefiles build those objects and produce keyboard.drv and sound.drv. Source, ZIP and installed KEYBOARD.DRV all have SHA-256 `c34d7d8d71453dd7567c6db28958a0d4d11e4b198cfc436f4d430d93a5a3584c`; the same three SOUND.DRV copies hash `d5932cbd4e0155a07d4d0f64c0f8afcdc3aefe90e06e7265c6651b10c2f03801`. These identities do not prove how those binaries were built or that keyboard/sound works.

`win3deb.inc`: `kernel31/kernel.inc:56` has a commented-out include; kdata.asm and ripaux.asm define related macro names, which is not evidence that the include itself is processed. `gdidefs.inc` and `paswindw.inc`: no active include of either exact filename found by case-insensitive textual search in current WOW16 C, H, ASM, INC and makefiles. `print.h` and `printer.h` mention gdidefs.inc only in comments. This is a bounded negative search; generated includes, other extensions and external build procedures were not ruled out.

Reproduce using `rg -n -i 'gdidefs|paswindw|win3deb|windefs' src/mvdm/wow16`, inspect the keyboard and sound makefiles and hash their source, ZIP and installed artifacts. No driver load, input, audio, teardown, external build or end-to-end feature was tested.
