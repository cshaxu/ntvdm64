# M0 T329 — CPU40 recursive-frame and guest re-entry contract closure

## Purpose

Recover or precisely dispose of the original CPU40 per-thread recursive
simulation-frame contract reached after the closed FDC/INT15 package. This
package owns `c_cpu_simulate`, `ccpu386SimulatePtr`, BOP-FE
`c_cpu_unsimulate`, `ccpu386Unsimulate`, their initializer and the original
guest re-entry callers. It is not a FDC, BIOS-vector or BOP-provider task.

## S plan

### S1 — Original CCPU recursive-frame lifecycle contract

Trace all selected definitions, initialization, TLS ownership, frame acquire,
frame release, exception return and original direct re-entry callers. Record
frame depth, ownership, expected return point and every base-level/null/failure
direction. Classify any missing link as an original body, a same-shaped existing
binding, a bounded source recovery candidate or a later owner. No behavior
change occurs here.

### S2 — Earliest complete source-shaped frame recovery

Recover only a statically proven missing original binding or lifecycle edge in
the complete CCPU frame cohort. Preserve original nested entry/return order and
failure behavior. Do not add an underflow guard as a substitute for proving the
call contract; do not change FDC, ROM, BOP, guest media or device result.

### S3 — Fixed-container frame result

Rebuild the selected product on independent x86 and x64 formal graphs. If S2
changed the product, run one bounded short-root immutable-media x86 observation
and record either a fixed original guest boundary or the next exact original
owner transfer.

## Exit

Close only with a source-shaped CCPU frame result and paired formal links. An
x64 link is not a guest runtime claim.

## T329 disposition

S1 found no missing CCPU lifecycle binding and its focused original nested
execution witness passes on both architectures. There is consequently no
admissible S2 behavior change. The successor is diagnostic-only capture at the
original unhandled-exception boundary, so a real context—not a speculative
base-level guard—selects the next source owner.
