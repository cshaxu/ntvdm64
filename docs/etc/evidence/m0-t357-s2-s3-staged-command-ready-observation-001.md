# M0 T357 S2/S3 — staged COMMAND-ready observation

## Fixed product and media

The formal CPU40/x86 product SHA-256 remained
`c92e4cc21f872adc6abc766da94f583fdb751b416d1ed19ce73c457b8328fa65`.
Every valid stage used the same format-3 manifest and its 16 immutable media
hashes.

The first stage at `build/M0-T357/S1/runtime-stage` had a 58-character
`mvdm` root.  Its one observation timed out in app's path-capacity dialog.
The intermediate `O:\ntvdm64\mvdm` root was 15 characters and still exceeded
the original generated-shell buffer once its required NUL terminator was
included.  Neither result is a guest or SoftPC terminal.

The capacity-valid fresh stage is `O:\t357`, where `O:\t357\mvdm` is 12
characters.  It is not an alias and was created by the existing stage tool
from the same formal product and immutable media inputs.

## Final fixed observation

The final non-debug, console-owning observer timed out after eight seconds;
its watchdog exit is `0x53504354`.  Unlike the two capacity-invalid attempts,
the record contains source-owned progress reports:

- `MVDM-CONFIG-DONE al=00 cs=8E08 pass=02` and the copied generated command
  `O:\T357\MVDM\SYSTEM32\COMMAND.COM`;
- successful original DEM opens for `HIMEM.SYS`, `COUNTRY.SYS` and
  `COMMAND.COM` under the staged original system directory;
- source-owned BOP dispatch/return evidence for DEM selectors and COMMAND
  `54:05`, `54:04`, `54:02`, `54:0D` and `54:0F`.

This proves the formal CPU40 stage reaches the original COMMAND-ready
boundary.  The report does not show a completed `/C EXIT`, a guest process
return, or a DOS/Win16 execution result.  Those remain the next package's
separate workloads.

## Evidence paths

The disposable observation records are:

- `build/M0-T357/S3/observation-final/short-stage-command-exit`;
- its `.bop-return.txt`, `.config-done.txt`, `.config-command-store.txt`,
  `.dem-open.txt`, `.dem-seek.txt`, `.dem-ioctl.txt` and `.json` companions.

