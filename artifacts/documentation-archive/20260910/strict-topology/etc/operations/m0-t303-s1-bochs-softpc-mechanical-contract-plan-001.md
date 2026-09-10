# M0 T303 S1 — Bochs/SoftPC mechanical-contract admission plan

## Objective

Establish the finite original SoftPC/CCPU mechanical interface set required by
the first Base VDM → MVDM vertical slice, before selecting any implementation
body. The single executor remains Bochs through `adapter-bochs`; original
MVDM callers retain historical SoftPC/CCPU/SAS spellings through
`adapter-mvdm-host-out/softpc`.

## Method

1. Start only from the already selected first-slice MVDM call sites and list
   each reached `host_simulate`, register, stack, selector, memory mapping,
   A20 and controlled-stop form by original declaration and caller.
2. Classify each form as an existing same-shaped adapter binding, a missing
   adapter-softpc binding, a Bochs mechanical requirement, or an explicitly
   unavailable second-executor/product-shell operation.
3. Verify dependency direction: MVDM source → adapter-softpc → adapter-bochs
   → bochs-core; no Bochs component may know MVDM, BOP, DOS or Win32 terms.
4. Emit a source/ABI/failure ledger and a bounded implementation plan. Do not
   add a new instruction workaround, device, BOP route or guest trace repair.

## Exit

Each reached first-slice mechanical form has one owner, source declaration,
x86/x64 width/mapping treatment, failure behavior and implementation
disposition. The next S may then implement only the whole selected mechanical
contract and its focused tests.
