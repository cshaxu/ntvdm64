# M0 T360 — Original SystemRoot package-layout recovery

## Purpose

Recover the app-owned installation layout that original MVDM expects before
interpreting any further CPU40/COMMAND observation. The current app invents
`<exe-directory>\\mvdm` as SystemRoot. Original `cmdconf.c` writes SystemRoot
twice into the 64-byte NTDOS `commnd` input:

```text
SHELL=<SystemRoot>\\system32\\COMMAND.COM /p <SystemRoot>\\system32
```

Even the user-selected `O:\\ntvdm64\\mvdm` spelling consequently produces 65
visible bytes and is rejected by app before original SoftPC or COMMAND runs.
The correct direction is not a path alias or COMMAND rewrite: restore an
original-shaped package where SystemRoot itself owns `system32`, configuration
media and the firmware `softpc` child.

## Sequence

1. **S1 — original root-consumer map.** Audit original MVDM configuration,
   NTDOS/NTIO loader and SoftPC firmware callers; record exact root-relative
   paths, buffer contracts and current app/staging mismatches.
2. **S2 — app/staging original-layout binding.** Make the smallest app and
   staging change that binds SystemRoot to the executable package root and
   firmware to its `softpc` child. Relocate package entries only by their
   original relative paths; do not alter bytes or original callers.
3. **S3 — package-layout conformance.** Add focused app/staging validation
   for root-relative `system32`, configuration and firmware inputs, including
   the exact 64-byte `shell=` arithmetic.
4. **S4 — formal product/package closure.** Rebuild the affected app product,
   create one hash-verified original-layout runtime container, and preserve
   explicit product/media identity.
5. **S5 — one frozen CPU40 observation.** Run T359's unchanged `/C EXIT`
   workload once from that fixed container. Its result resumes T359; it does
   not itself implement a COMMAND or BOP service.

## Exclusions

No guest, firmware or original MVDM source modification; no drive alias,
`subst`, junction, virtual boot volume, host C media copy, BOP/DEM result
special-case, CPU semantic work, CPU30, Bochs, x64, BaseSrv/CSRSS, WOW,
Redirector, EXEC or graphics recovery.

## Completion

T360 closes only when one immutable package at a capacity-valid installation
root has the original SystemRoot shape, all reached root consumers use
source-defined relative paths, and one valid frozen observation either reaches
T359's COMMAND-transient boundary or names a new exact source-owned
predecessor. Invalid observer-output or long-stage attempts are retained as
non-semantic evidence and do not consume that one valid observation.
