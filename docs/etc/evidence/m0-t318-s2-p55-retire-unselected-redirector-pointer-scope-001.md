# M0 T318 S2 P55 — Retire unselected Redirector pointer scope

## Finding

The former `ADAPTER-SOFTPC-REDIR-001` implementation was a speculative
Redirector-specific guest-memory lease scope.  A complete source/build sweep
found no production caller of its begin/prepare/end API and no formal Ninja
source selection.  Formal dependency records for the selected original BOP and
DEM objects resolve the original `mvdm-host/inc/softpc.h`; neither reaches the
adapter header that redefined `GetVDMAddr` and `Sim32GetVDMPointer`.

Leaving it in the production adapter would create an unowned alternate pointer
contract, contrary to the source-shaped selected SoftPC route.

## Change

Removed the unused adapter header, scope API/body, its unselected test fixture,
and its README divergence registration.  Historical T290 evidence remains an
historical record only; it is not a build or runtime input.

## Verification

Both selected `M0 T318 S2` formal Ninja final-link rows were invoked after the
removal:

- x86: `original-softpc-process.exe` — `ninja: no work to do`;
- x64: `original-softpc-process.exe` — `ninja: no work to do`.

This proves the removed implementation had no selected-product dependency.  It
does not change media, firmware, product source inputs, or the fixed runtime
container, so no observation is permitted or needed.
