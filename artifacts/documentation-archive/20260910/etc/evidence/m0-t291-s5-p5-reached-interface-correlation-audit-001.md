# M0 T291 S5 P5 — Reached-interface correlation audit

## Question

Which of the 8,500 static declaration/call correlations are actual current
host-runtime package-boundary interfaces, rather than original declarations,
package-internal definitions or calls from a profile-excluded historical path?

## Inputs and method

The T277 source correlation ledger supplies 4,803 declaration-only rows and
3,697 call correlations. Each caller path is joined to the reconciled T276
final disposition in the live file ledger. Declaration-only rows are retained
as source ABI evidence but cannot create a runtime dependency. A selected
definition-name correlation remains package-internal until the later static
closure proves its compilation unit; it does not create an adapter edge.

## Results

- 4,803 rows are declaration-only and not source-reached.
- 1,429 call rows resolve by name to a selected original MVDM definition.
  They are package-internal correlations, not outward boundary interfaces.
- 2,218 external-call rows originate in a caller already classified as
  `not-host-runtime`, `tool-only` or `firmware-only`; their profile exclusion
  is their final current-product result.
- 50 external-call rows originate in a potential host-runtime caller. They
  reduce to six original interfaces:
  - 40 `malloc` calls retain the original C allocation contract through the
    MSVC CRT; a null result remains the source failure result.
  - `ica_inb`, `ica_outb`, `ica_hw_interrupt` and `ica_clear_int` retain their
    SoftPC PIC spellings through the `softpc` family of
    `adapter-mvdm-host-out`, then typed `adapter-bochs` mechanics. Bochs owns
    PIC operation; no MVDM policy enters the machine layer.
  - `sim32_effective_addr` remains a SoftPC-shaped address operation through
    the same adapter family and the session-owned guest-memory mapping lease.
  - `setCPL` remains a DPMI32 SoftPC-shaped protected-context operation through
    that family; its later owner package is DPMI32, not a new CPU executor.

## Consequence

The live interface ledger is expanded with every correlation and marks the
twelve historical family summaries as expanded. This only completes boundary
classification. It does not enable any body, promise a compiled provider or
replace the later package-local ABI proof required by the static-closure and
owner-package tasks.
