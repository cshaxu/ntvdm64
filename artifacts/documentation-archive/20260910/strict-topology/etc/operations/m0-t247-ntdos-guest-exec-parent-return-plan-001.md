# M0 T247 — NTDOS guest EXEC and parent-return owner-package plan

## Purpose

Recover the guest-owned NTDOS `EXEC` lifecycle that consumes a completed
COMMAND child result: original PSP, arena, JFN, environment, child entry and
ordinary parent restoration.  This package deliberately consumes the existing
`54:08/0A/0B` host-child contract; it must not recreate that host worker in
the guest or adapter.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Source/ABI/lifecycle map of original guest `EXEC`, PSP/arena/JFN/environment and parent-return paths against staged source-built NTDOS/COMMAND images and current `54:0B` seam. | One tracker/evidence map names the exact guest sources, call/return states, staging symbols, direct reusable pieces, and every machine/host owner transfer. |
| S2 | Compose an opt-in, fixed-width observation at the original `SVC_DEMENTRYDOSAPP` handoff that S1 proves follows `$Dup_PDB` and precedes guest child entry. | Existing checked-RAM observer records the source PDB prefix after imported `demEntryDosApp`; local positive/negative fixture proves no guest mutation, no selector widening and no host-side PSP/arena/JFN implementation. |
| S3 | Extend through the declared one-child ordinary-return profile, only if S2 proves the prerequisite path. | Source-built child → parent state restoration is observed with PSP/JFN/environment evidence; non-composable prerequisites fail/transfer explicitly. |
| S4 | Package regression, native checkpoint and closure transfer. | Formal package regression and bounded native evidence distinguish guest lifecycle completion from remaining machine/Redirector/WOW work. |

## Boundary and recovery ladder

NTDOS/COMMAND guest source owns process semantics.  `bx-vdm` can transport
only existing fixed-width BOP results and checked guest-memory effects;
`bx-core`/`bx-mantle` retain CPU, BIOS, interrupt and device mechanics.  S1
must apply the source-first ladder to each required source unit before any
new seam is proposed.  Missing device, BIOS, remote-stream, WOW or protected
mode behavior is a named owner transfer, not an adapter reimplementation.
