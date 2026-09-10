# M0 T318 S2 P28 — Passive dispatch observer rejection

## Purpose

P26 admitted exactly one attempt to distinguish original startup from reached
guest BOP dispatch without changing the fixed, non-debug, console-owning
runtime container.  The required result was evidence about the suitability of
that observation form, not an additional BOP implementation.

## Attempt

An x86/x64 CPU40 observation derivative added a bounded, stage-local record
immediately after the original `nt_bop.c` reads a DOS (`50h`) or COMMAND
(`54h`) service byte and before the unchanged original dispatcher call.  The
recorder had no guest-pointer, register, IP, dispatch-result, or lifecycle
authority.  The same fixed P22 short stage, real-console launcher, arguments,
media, firmware, timeout and working directory were retained; only the staged
product EXE changed.

Both derivative width products linked successfully.  The fixed x86 observation
then exited with `0xC0000005` before the eight-second timeout and produced no
dispatch record.

## Disposition

The derivative is rejected and removed rather than retained as a production or
diagnostic build option.  Its control-flow insertion changes the observed
product's startup behavior relative to P25's normal-product timeout and hence
cannot establish whether the original ingress was reached.  A zero record from
this perturbed binary is not evidence that the normal product stops before
BOP.

The selected route remains the complete original `nt_bop.c` path established
by P12/P26.  Subsequent recovery must use the static original startup owner
clusters already frozen in P24/P26—console/display, error termination, Base
VDM command acquisition, machine/media and CCPU/C-VID assembly—before another
runtime observation.  No BOP, DEM, COMMAND, NTDOS, EXEC or parent-return claim
is made by this record.
