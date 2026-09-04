# M0 T390 S4 — pure-DOS normal-child predecessor observation

## Result

The unchanged pure-DOS PIF profile reaches the original post-configuration
normal `COMMAND.COM` record. It does not yet prove that the second shell has
printed its banner or prompt, entered DOS `CON`, or consumed a Console key.

## Fixed observation

The non-debug console-owning container used the staged CPU40/x86 product,
`O:\\ntvdm64` media root and the explicit product target `command.com`.
The durable state sidecars record:

* `MVDM-CONFIG-DONE ... command=O:\\NTVDM64\\SYSTEM32\\COMMAND.COM`;
* original `54:01` continuation entry and successful return;
* pure-DOS opens of `HIMEM.SYS`, `COUNTRY.SYS` and `COMMAND.COM`, with no
  `REDIR.EXE` or `DOSX.EXE` open;
* a bounded timeout with no console bytes.

The later `MVDM-COMMAND-INPUT-READY` observation is intentionally not counted
as prompt evidence: source audit identifies it as the BIOS keyboard-idle
marker, not `Do16BitPrompt` or DOS `CON` completion.

## Disposition

This closes only T390's configuration/predecessor condition. The package stays
active for S5 so the product cannot be represented as interactive until the
second shell's source-defined banner/prompt/DOS `CON` path and normal keyboard
drain are observed.
