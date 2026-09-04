# M0 T389 closure — explicit interactive COMMAND profile boundary

## Result

T389 closes its admitted source-audit cohort, not the interactive-product
feature. The original Base VDM record reaches NTDOS `$Exec`, `50:36`, and the
unchanged CPU40 `RETF` completes into the loaded child at `0713:0100`.
Accordingly, neither Base VDM delivery, BOP return mechanics, nor CPU40 far
return is the first missing transition.

The fixed staged first-shell profile still executes original `AUTOEXEC.NT`,
including `lh %SystemRoot%\\system32\\dosx`. Original DEM observations show
that `DOSX.EXE` is loaded and then performs its source-owned `SYSTEM.INI` and
`KRNL386.EXE` lookup; the latter exhausts the inherited host search path.
That DPMI/WOW bootstrap predecessor consumes the bounded observation before
the desired second-COMMAND `DoReEnter -> Do16BitPrompt -> DOS CON` state can
be proved.

## Accepted evidence

* [S8 boundary evidence](../etc/evidence/m0-t389-s8-child-command-reentry-boundary-001.md)
  records the original source contract, fixed observations, the completed
  `RETF`, and the DOSX/DPMI/WOW predecessor.
* The formal x86 CPU40 Ninja product links with the registered, default-off
  witnesses enabled. The fixed non-debug Console-owning run times out after
  the observed original lookup path; timeout is evidence, not success.

## Disposition

No guest source, `COMMAND.COM`, BOP provider, keyboard controller, or CPU
semantic was changed to force a prompt. The next candidate must first select
and prove either a source-shaped pure-DOS first-shell configuration that does
not launch DOSX, or a complete DPMI/WOW bootstrap recovery package. Only
after that decision can interactive second-COMMAND DOS-CON work be re-admitted.
