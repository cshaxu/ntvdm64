# T95 S6 Source-Built NTIO First-Prefix Observation 001

## Inputs

The only placed payload was the source-built Tools16 `NTIO.SYS`: 33,792 bytes,
SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.
The retained package manifest records NTDOS (27,858 bytes) and COMMAND
(50,384 bytes) as fixed identity-only inputs; neither is placed or started.
No guest byte was patched and no synthetic IVT/BDA/VDM state was introduced.

## Observation

The r2 companion logged `requested execution plan applied`, completed without
watchdog in 387 ms, and reached its normal benchmark stop at tick `10,000,004`
with native nogui exit `1`. After plan application, the trace emits repeated
Bochs CPU diagnostics `math_abort: MSDOS compatibility FPU exception`; the
terminal CPU snapshot is real-mode halted at `CS:IP = 0000:0001`.

## Classification

This is a successful verified source-built-NTIO **placement and entry** result,
followed by an early guest/entry-state stop. It is not an adapter rejection,
not an NTDOS/COMMAND load, and not a DOS or NTVDM runtime result. The trace
does not by itself prove whether the first FPU compatibility fault is caused by
the missing original VDM initialization state, the selected native CPU/profile,
or a separate reached guest dependency; those are the next read-only source
and trace questions. No extra guest input, BIOS emulation, host service, BOP
handler or retry is authorized by this observation.
