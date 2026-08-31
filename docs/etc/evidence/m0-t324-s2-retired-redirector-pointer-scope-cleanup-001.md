# M0 T324 S2 — Retired Redirector Pointer-Scope Cleanup

## Decision

The former `mvdm_redirector_pointer_scope` experiment remains retired.  It
had no selected production caller and represented an alternate lifetime model
for guest pointers.  It must not be restored merely to keep an old fixture or
Ninja generator buildable.

## Archived Material

The five T290 generators and four dependent fixtures were moved with Git
history to `docs/etc/legacy_code/retired-redirector-pointer-scope/`.  That
tree is explicitly outside every production include root, formal Ninja input,
runtime provider and test source root.

## Active Replacement

The selected local VDMREDIR cohort uses the source-shaped asynchronous
transaction in `mvdm-host-overlay/vdmredir`: it stages data while host work is
pending and obtains a fresh existing session lease for each guest-memory
crossing.  The original queue and ICA completion route remain the owner of
the observable Redirector sequence.

## Verification

The active T310 lease-caller ledger records `LEASE-002` as retired.  A source
search must find no remaining live `tools/` or `tests/` reference to
`mvdm_redirector_pointer_scope`.
