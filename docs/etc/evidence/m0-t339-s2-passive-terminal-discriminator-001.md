# M0 T339 S2 — Passive terminal discriminator observation

## Result

The one permitted unchanged short-root, console-owning observation did **not**
reach the previously recorded original `50:3D -> demExitVDM` terminal. Its
last emitted original BOP ingress record is `54:05` (`SVC_CMDSETINFO`), then
the fixed observer reaches its eight-second timeout. Consequently this
observation cannot select any of the S1 `50:3D` predecessor families, and it
does not authorize a file/EXEC, version, memory, configuration-device, drive,
or BOP-provider recovery.

The raw fixed-container report is
`artifacts/research/m0-t339-s2-passive-terminal-discriminator.txt`; its
paired console capture records exactly:

```
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
MVDM-BOP-DISPATCH 54:05
```

The observer reports `result=timeout`, `timeout-ms=8000`, and the stage
manifest hash `4ba66c24580eaf95b5d0be753760fc27e382fb80dbcd2c54ace3f076725fb88c`.
It records no `50:3D` line and therefore no copied `CS:IP` or `DS:DX` words.

## Passive discriminator

The existing registered `MVDM-HOST-DIV-164` hook remains at the original
`MS_bop_0`/`MS_bop_4` ingress: the original code has already decoded the
selector and service byte, but has not called `DemDispatch` or `CmdDispatch`.
For `50:3D` alone the hook copies the already-live 16-bit `CS:IP` and `DS:DX`
words to the adapter's observation output. It does not read guest memory,
route a service, mutate registers, advance IP, change a result, or alter the
guest/session lifecycle.

S1's source map establishes why those words would have been sufficient if the
terminal had been reached:

- COMMAND's `init.asm` direct service site has a distinct `CS:IP` from NTDOS
  `sysinit1.asm::stall`.
- NTDOS `mem_err` reaches `stall` after placing the original `badmem` string
  address in `DS:DX`.
- NTDOS command open/EXEC failure reaches `stall` after its `badfil` path
  leaves the original `crlfm` address in `DS:DX`.

This is a source-defined discriminator only, not a replacement terminal
classifier. The absence of its `50:3D` line in this single permitted run is
an observation limit, not evidence that any of those source families is
absent or repaired.

## `54:05` disposition

The observed `54:05` is the original COMMAND `cmdSetInfo` entry in
`dos/command/cmddisp.c`. Its selected original `cmdmisc.c::cmdSetInfo` body
records the DOS `SCSINFO` locations through the existing same-shaped guest
location binding. It is not a `demExitVDM` predecessor discriminator and does
not by itself select a COMMAND, BaseSrv, or guest EXEC recovery. The single
timeout therefore transfers no new business capability and does not reopen the
normal `config.nt` source path already proven by T338.

## S2 disposition

S2's only runtime observation has been consumed without reaching its target.
No S3 recovery cohort is selected. Any follow-on must be separately admitted
against the precise post-`54:05` original continuation/observer boundary; it
may not infer a `50:3D` repair from this timeout or repeat the container with
changed inputs.
