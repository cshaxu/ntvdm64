# M0 T282 S15 — COMMAND initial-environment bounded output binding

## Objective

Recover original `cmdGetInitEnvironment` after it has computed the exact
`cbComSpec + cchInitEnvironment` output, preserving its original `BX`
required-paragraph failure result and avoiding the historical early
`GetVDMAddr(ES, 0)` pointer materialization.

## Source-first boundary

The original `cmdenv.c` owns environment construction, COMSPEC prefix and
success/failure ordering. Adapter-softpc remains the sole owner of the exact
guest write lease; if one contiguous staged output needs material semantics,
it belongs in the private matching mirror overlay rather than in the original
body. No new mapper, host environment policy, BOP route or machine change is
admitted.

## Required audit before source change

Establish the smallest single-write staging form, including allocation and
lease failure disposition. If source lacks an observable failure result for a
new negative branch, stop and record it instead of inventing BX/CF semantics.

## Exit evidence

Preserve the original insufficient-BX result, prove the selected original
source composition and exact bounded-lease behavior on x86/x64, register every
mirror/overlay divergence, and distinguish source composition from a callable
`54:0F` route.
