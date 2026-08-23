# M0 T246 — SoftPC machine interrupt owner-package plan

## Purpose

Recover the complete first-profile ownership boundary shared by original
SoftPC `BOP 02` and `BOP 06`: unexpected/illegal operation interrupt handling
through the existing source-derived INT06 provider and selector-blind checked
RAM/port actions. This is a machine package, not a COMMAND/DEM trace patch.

## S sequence

| S | Scope | Exit evidence |
| --- | --- | --- |
| S1 | Read-only source/ABI/lifecycle map of `bios.c` dispatch table, `illegal_op_int`, `opennt_int06_provider_v1`, generic ingress, legacy `startup_machine_interrupt_v1`, and mantle action capabilities. | One route map names active versus dead composition, source owner, state/memory/port effects, exact `02/06` failure behavior, and whether a Bochs intrusion is genuinely required. |
| S2 | Attach the smallest source-shaped `02/06` provider route to the active generic ingress, if S1 proves its existing typed mechanics suffice. | Local source/provider fixture covers both selectors, preflight failure, source completion and typed resume/stop; no BOP/DOS vocabulary enters core or mantle. |
| S3 | Package regression and bounded native checkpoint. | Formal Ninja link and dedicated fixtures pass; one source-built run proves `06` no longer repeats and records the next distinct owner. |
| S4 | Closure/transfer. | Tracker rows, evidence and task closure distinguish completed machine sub-contract from all remaining device/firmware features. |

## Boundary

`bx-core` and `bx-mantle` retain only CPU, checked RAM, port and lifecycle
mechanics. `bx-vdm` owns selection of the recovered historical machine BOP
contract. Neither layer recognizes DEM, COMMAND, DOS files, or an OpenNT
service selector. The source-first ladder prohibits a Bochs patch unless the
existing typed mechanical actions demonstrably cannot express the original
provider's required effect.
