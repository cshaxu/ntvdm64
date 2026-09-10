# T187 S2 passive BOP DX snapshot 001

Date: 2026-08-12  
Packet: M0 T187 S2  
Disposition: source/static-test closed; no Bochs build or guest observation.

## Question

Can the existing default-off BOP catalog listener expose the low 16 bits of
the CPU snapshot's EDX without affecting the adapter route or Bochs mechanics?

## Change and boundary

`refs/bochs/cpu/exception.cc` now appends `dx=%04x` to its pre-existing
`ntdos64 adapter bop observed` line and supplies
`cpu_state->edx & 0xffffu` as its adjacent format argument.  The listener is
already protected by `BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER`, whose local
default remains zero.  It still classifies through the existing catalog
before the startup consumer and retains the same pre-existing optional stack
and frame reads.

No adapter or OpenNT source, typed ABI, exception disposition, CPU result,
guest-memory operation, provider, BOP branch, build recipe, Bochs build, or
guest run changed.  The exception is registered as BX-TRACE-059 before the
source patch.

## Verification

`tests/bx-ntvdm-bop-catalog-listener-boundary.ps1` passes.  It verifies the
default-off macro, required listener/catalogue terms, the new `dx=%04x`
field, retained ordering, and absence of adapter, OpenNT, DOS, DEM, result,
write, or selector-specific terms in the listener block. `git diff --check`
also passes.

## Interpretation and follow-up

This completes only the missing raw snapshot field.  It does not determine
why the T186 `54:0C` request passes through and does not authorize a new
observation.  A subsequent separately admitted package must decide whether a
fresh exact-engine observation is worthwhile and, if so, use the completed
`DS:DX` line solely to classify the existing checked destination predicate.

