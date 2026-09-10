# M0 T354 S1 — permanent COMMAND bootstrap owner admission

## Inputs

- Closed [T353 continuation evidence](m0-t353-s4-fixed-config-prescan-observation-001.md).
- Original/mirrored `softpc.new/bios/sysinit1.asm`, `sysinit2.asm`, NTDOS
  `dos/file.asm`, DEM `demdisp.c`/`demfile.c`, and COMMAND `cmdconf.c`.
- Unchanged staged CPU40/x86 media manifest below `build/T353S/mvdm`.

## Source walk

`sysinit2.asm` initialises `commnd` to `\\COMMAND.COM`.  `sysinit1.asm`
passes that buffer to DOS `INT 21h AX=3D00h`; success then takes the original
size/close sequence and `AX=4B00h` handoff.  Any carry takes the same
`comerr -> badfil -> SVC_DEMEXITVDM` terminal.

The NTDOS file layer does not open that caller string directly: it uses the
canonical WFP string and invokes `SVC_DEMOPEN`; `demdisp.c` selects original
`demOpen`, whose original `DS:SI` pathname reaches `CreateFileOem`.  There is
therefore no source basis for a `50:12` success override.

The earlier `54:0C` cohort matters because `cmdconf.c::ExpandConfigFiles`
filters user `shell=` input and emits its own `shell=<system-root>\\system32\\command.com
/p ...` line.  NTDOS `sysconf.asm` is the original parser that can overwrite
`commnd` from that line.  Current package staging contains both
`mvdm\\COMMAND.COM` and `mvdm\\system32\\COMMAND.COM`, while the app-selected
system root is `<exe>\\mvdm`.

## Current conclusion

T353 proves that the first `3D` open reached `comerr`, but it does **not**
prove whether the live canonical WFP path was the default `C:\\COMMAND.COM`,
the source-generated `...\\mvdm\\system32\\COMMAND.COM`, or another original
parser result.  That question sits at the whole NTDOS configuration-to-file
handoff boundary.  It must be resolved before changing any file binding.

The full edge and disposition record is the adjacent
[bootstrap ledger](../operations/m0-t354-s1-permanent-command-bootstrap-ledger.tsv).

## Selected S2 boundary

S2 may add one default-off, copied-string observation at the already-existing
original `SVC_DEMOPEN` boundary: it records the bounded canonical DOS WFP
pathname and the original return carry/error scalar for the permanent-command
open only.  It may neither alter that string nor special-case the service.
The mapping manager must provide a synchronous copied read lease; no guest
pointer may survive the BOP call.
