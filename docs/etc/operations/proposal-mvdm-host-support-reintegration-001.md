# MVDM host support reintegration

## Closed package

**MVDM host mirror-topology recovery** closed as `M0 T311`; its closure is
[recorded in history](../../history/m0-t311-mvdm-host-support-reintegration-closure-20260830.md).
This retained proposal is source-of-truth evidence for that completed move;
it is not a current candidate.

## Decision

`base/mvdm/inc`, `base/mvdm/oemuni`, and `base/mvdm/suballoc` are original
members of the MVDM host package. They are not a product-level support
component: current source scans show no direct guest-media, firmware, or tools
consumer. The final product tree therefore has no `mvdm-support` or
`mvdm-support-overlay` component.

Their destination is the original-relative MVDM host topology:

```text
src/mvdm-host/inc/
src/mvdm-host/oemuni/
src/mvdm-host/suballoc/
src/mvdm-host-overlay/
```

The last destination receives the existing `mvdm-support-overlay` material at
its corresponding original-relative paths. It remains an overlay, not a
second support implementation.

## Scope and S breakdown

### S1 — provenance and consumer closure

Reconcile every `mvdm-support` and `mvdm-support-overlay` file against the
selected OpenNT source path, current build graph, include consumer, and mirror
divergence register. Prove or record any non-`mvdm-host` consumer before a
move. A discovered genuine external consumer pauses the relevant move for an
owner decision; it does not justify retaining the old component by default.

### S2 — exact mirror re-root

Use `git mv` to move the complete original `inc`, `oemuni`, and `suballoc`
trees into `mvdm-host` at their original-relative names. Move every
`mvdm-support-overlay` file to its corresponding `mvdm-host-overlay` path.
Transfer all mirror-divergence registrations to `mvdm-host/README.md` or
`mvdm-host-overlay/README.md`, preserving identifiers, source purpose, reason,
implementation and file path.

### S3 — build and documentation closure

Update include roots, Ninja manifests, component inventories and current
architecture/proposal references to the new owner. Remove the old component
roots only after a complete source/build/document scan proves that no live
input names `mvdm-support` or `mvdm-support-overlay`.

### S4 — selected-profile mirror verification

Run the formal selected Win32/x86 CCPU40 affected build graph, compare every
moved mirror file with its selected original baseline, verify every overlay
registration, and run documentation governance plus `git diff --check`.

## Non-goals

- No BOP/provider, Base VDM, device, SoftPC, guest, firmware or adapter
  behavior change.
- No new compatibility shim, mapping manager, or adapter component.
- No source deletion other than the emptied obsolete component roots after
  verified `git mv` migration.
- No use of `src.old`.

## Exit criteria

The retired support roots no longer exist; every former file
has one verified destination or a recorded owner-approved exception; all
original `base/mvdm` relative topology is represented beneath `mvdm-host`; all
four previously registered width divergences remain registered at their new
owner; and the affected formal CCPU40 graph has no live old-root reference.
CCPU30 recovery is deliberately not asserted by this behavior-neutral move:
the immediately following CCPU30 package must establish its own original
source selection, formal graph and bounded execution evidence.
