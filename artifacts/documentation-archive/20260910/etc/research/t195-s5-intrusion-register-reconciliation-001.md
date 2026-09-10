# M0 T195 S5: Intrusion Register And Build-Surface Reconciliation

## Scope

S5 reconciles the two semantic groups identified by the 58-entry S1 audit
after their S3/S4 source moves. It does not erase historical evidence or turn
historical derivative scripts into current build recipes.

## Register Changes

| Entries | Final T195 S5 disposition | Evidence |
| --- | --- | --- |
| BX-ABI-040; BX-TRACE-043, 044, 046, 059 | Migrated to adapter copied-fact observer | S3 source, static boundary test and linked adapter object |
| BX-TRACE-045 | Removed; no replacement | S3 deliberately rejects recreating its extra stack/frame RAM observation without a new bounded-read contract |
| BX-ABI-053, 054, 058; BX-TRACE-055 | Migrated to adapter machine-composition plane | S4 source, unit test and native map |
| BX-TRACE-060, 061 | Retained default-off generic diagnostics | They neither decode BOP bytes nor select a service/handler; S6 must verify the final map surface |

All other S1 dispositions remain unchanged: selector-blind mechanics and
embedding stay eligible for S6 verification; pending diagnostics remain
default-off; BX-MACH-009 and BX-TRACE-047 remain non-live history.

## Current Build Surface

`New-T98S1CurrentAdapterEngineDerivative.ps1` is the current derivative
generator. Its machine-composition form compiles:

- `cpu/exception.o`, solely for generic mechanics construction/result
  application;
- `adapter/bx_ntvdm_machine_composition_v2.obj`; and
- unchanged original `adapter/unexp_nt.c.obj` and
  `adapter/illegalp.c.obj`.

It has no `machine/` source directory, `bx_ntvdm_machine_bop_v1` object,
`bx_ntvdm_machine_composition_v1_probe`, or BOP catalogue-listener macro.
The generator keeps Bochs archives out of prerequisites.

Older scripts containing those names are retained chronological evidence under
their historical T records. They are not current recipes, are not referenced
by Status, and must not be repaired or used for T195/S6.

## Search And Boundary Evidence

On 2026-08-12 the current source and generator were searched for the removed
listener macro and legacy machine component names. No live source/generator
reference remained. Matches in historical tools and evidence records were
classified as retained history. The S3 observer and S4 mechanics boundary
tests pass; the fresh S4 closure links the adapter component and original
objects without an archive/device rebuild.

## Result

The register now distinguishes three states that must not be conflated:
historical evidence, retained selector-blind mechanics/default-off diagnostics,
and migrated adapter semantics. No unresolved semantic intrusion group remains
for S6; its remaining task is a finite source/map regression and final ledger
comparison.
