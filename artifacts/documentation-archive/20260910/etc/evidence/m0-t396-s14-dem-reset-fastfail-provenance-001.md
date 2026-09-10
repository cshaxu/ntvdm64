# M0 T396 S14 — DEM/reset fast-fail provenance 001

`BOP 50:36` is `SVC_DEMENTRYDOSAPP`, bound directly to original
`dos/dem/demmisc.c::demEntryDosApp`. It records NTDOS `$Exec`'s existing
`DS:SI`, `AX:DI`, and `DX` frame, optionally invokes the original VDD user
hook, then returns; it neither owns termination nor invents a next execution
target.

On the truthful S11 stage, the current product reaches DPMI `53h` services
and then still exits `0xc0000409`, 15 modules, no WOW32. Enabling the existing
default-off original BIOS reset report records all fourteen source-defined
phases entering and returning. The app CRT invalid-parameter report is empty
and the session-termination report is absent. Thus the current evidence rules
out a mid-reset failure, app invalid-parameter handler, known session exit,
observer, or dialog policy as the owner.

The remaining non-debug result is the pre-existing CRT fatal-app-exit family;
the earlier WER stack maps its CCPU dispatch table's index zero to original
BIOS `_reset`, but that observation cannot identify a current post-reset
source caller. No repair is selected from it. The next source-shaped proof
must use the actual declared Win16 `WOWEXEC.EXE` target, whose loader contract
is the only path that can legitimately reach BOP `51h` and `WOW32.DLL`.
