# Proposal: Native Console mouse-vector hypothesis withdrawal

## Decision

Withdraw P9's speculative vector derivation and restore the selected OpenNT
visible-pointer Console branch unchanged. Retain P8's separately evidenced
Console input-mode recovery.

## Evidence and disposition

P8 established that ordinary `cmd/conhost` `MOUSE_EVENT` records reach the
unchanged original event worker and callback return. P9 inferred a missing
relative vector from a Help-surface observation. The owner subsequently
identified EDIT.COM's independent startup welcome modal, which must be
dismissed with Escape, and reported that the temporary vector form makes the
apparent editor insertion point track mouse movement. That does not isolate a
vector defect.

The selected original `ScaleToWindowedVirtualCoordinates` visible-pointer
branch maps Console cells into callback `CX/DX`, updates its prior position,
and does not write the supplied `MOUSE_VECTOR`. A standalone inference must
not replace that source behavior without an isolated failing contract.

## Scope and acceptance

Remove the uncommitted P9 coordinate and diagnostic changes, including
`MVDM-HOST-DIV-265`. Do not change event dispatch, BOP routing, guest media,
the SoftPC mouse driver, or graphics/hidden-pointer behavior. Rebuild the
P8-only product and test EDIT.COM after dismissing its welcome modal.
