# M0 T396 S34 — WOW32 provider initialization frontier

## Question

Does the current fixed x86 stage merely map `wow32.dll`, or does the selected
original provider enter and return successfully from `W32Init` before the
first Win16 lifecycle/dispatch boundary?

## Inputs and procedure

The formal x86 parent was linked from `build/M0-T396/S20/formal-x86` and the
complete provider from `build/M0-T396/S32/wow32-x86`.  The fixed `O:\\t396`
stage was refreshed with those two artifacts and its manifest-pinned media.

`WOWEXEC.EXE` was launched by the existing Console-owning one-action observer
with the default high-frequency diagnostics disabled.  The sole optional
selector was `MVDM_WOW32_PROVIDER_REPORT_PATH`, consumed in the provider only
after original COMMAND had copied its guest environment.  It records two
fixed markers around original `W32Init`; it does not alter a thunk, register
frame, callback table, guest byte, or result.

## Observations

The provider report contains, in order:

```text
MVDM-WOW32-PROVIDER init-enter
MVDM-WOW32-PROVIDER init-success
```

The provider was rebuilt normally as `wow32.dll` (493,568 bytes).  This run
therefore advances beyond image loading and through the source `W32Init`
return.  The earlier generic BOP trace was deliberately not used as runtime
success evidence: its per-BOP file writes materially slow the interpreter and
change the 30-second observation position.

## Interpretation and next boundary

Confidence is high that the recovered optional registry/private-USER/C-VID
dependencies admit original provider initialization.  This is not yet a
Win16 lifecycle claim: retail FastWOW may dispatch through its assembly path,
and the current bounded run remains live rather than reaching a product exit.
S34 now owns the first low-perturbation original dispatch or task-creation
boundary after successful initialization.
