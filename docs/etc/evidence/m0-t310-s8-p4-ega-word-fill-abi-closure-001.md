# M0 T310 S8 P4 — EGA word-fill ABI closure

## Scope

This record closes one actual x86/x64 call-contract defect found by the
full formal SoftPC build: the selected EVID `Glue_writes` initializer in
`softpc.new/base/video/ega_writ.c`.

## Source finding

All available selected OpenNT baselines spell the forward declaration as
`fill_word_ev_glue(IU32, IU8, IU32)`.  The existing original implementation in
`softpc.new/base/cvidc/ev_glue.c` is instead
`fill_word_ev_glue(IU32, IU16, IU32)`, and the selected `MEM_HANDLERS`
word-fill table slot also requires the `IU16` form.  The historical
unprototyped table initializer accepted this mismatch; the x64 formal graph
reported it as C4113 in the generated selected EGA source.

## Resolution

`MVDM-HOST-DIV-074` changes only the forward declaration to `IU16` and adds a
local DIVERGENCE comment.  It does not alter the glue body, table order,
generated rule implementation, guest representation, or controller behavior.
No mapping-manager boundary is involved: this is an internal selected SoftPC
function-pointer contract.

## Verification

Both formal Ninja graphs rebuilt `obj/video/ega_writ.obj` successfully:

- x64: `build/M0-T310/S8/p1-machine-source/x64/ega-writ-abi.log`
- x86: `build/M0-T310/S8/p1-machine-source/x86/ega-writ-abi.log`

Each log contains zero errors and no C4113 diagnostic.  The full-build finding
is registered as `T310-S8-P4-CLASS-020049`; unrelated non-width diagnostics
remain visible and are not suppressed by this change.
