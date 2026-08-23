# Proposal: Selector-Blind Protected-Mode Substrate Recovery

## Purpose

Establish whether the existing Bochs protected-mode engine can expose the
small, typed machine operations required to compose the retained OpenNT DPMI
source mirror.  This is a prerequisite package, not a DPMI implementation.

## Owner boundary

- `bx-core` continues to own architectural protected-mode execution,
  descriptors, faults, paging and CPU caches.
- `bx-mantle` may assemble selector-blind operations around those existing
  mechanics.
- `bx-vdm` may adapt copied requests/results for an admitted original OpenNT
  owner, but may not own a DPMI server, descriptor table, CPU mode or guest
  pointer.

Neither bx-core nor bx-mantle may contain `BOP`, `DPMI`, `DOS`, `VDM`, OpenNT
provider, selector/service, or host-policy semantics.

## Evidence basis

`BOP-DEPENDENCY-117` and the [T254 S1 admission map](../evidence/m0-t254-s1-dpmi-profile-source-abi-admission-map-001.md)
prove that all original `53:00..18` services share these prerequisites:

1. bounded protected selector:offset read/write actions which resolve through
   the active CPU's existing descriptor state without leaking pointers;
2. one atomic, typed protected-context operation sufficient for original
   mode-transition and 16/32-bit IRET continuation needs; and
3. guest-linear allocation/mapping identity, not an NT host virtual address
   or host allocation pointer.

## Admission plan

1. **S1 — source/ABI map:** identify the exact existing Bochs/mantle call
   points, all current result-ABI omissions, the minimum testable operation
   shapes and whether they can stay in mantle with no bx-core patch.
2. **S2 — bounded mechanical composition:** implement only admitted generic
   operations and tests.  If a core patch is indispensable, register its
   exact upstream path, reason and negative test before editing it.
3. **S3 — regression/admission:** prove real-mode behavior is unchanged,
   prove protected descriptor/transition negative cases, and publish the
   resulting DPMI admission decision.  It must not import or dispatch `53:xx`.

## Non-goals

No OpenNT DPMI body, BOP selector recognition, DOSX guest patch, leaf stub,
LDT policy, VDD/WOW product shell, legacy `NtVdmControl` call, raw guest
pointer, or raw Bochs object crosses the boundary.

## Exit rule

The package either produces a narrow, tested generic substrate sufficient to
admit the original DPMI package, or records the precise missing machine
mechanic and leaves DPMI unadmitted.  It never replaces the original DPMI
lifecycle with a newly authored adapter implementation.
