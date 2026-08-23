# M0 T260 S3 mantle purification

## Decision

`bx-mantle` is now limited to Bochs-native machine assembly.  Every former
`src/bx-mantle/bx_ntvdm_*` translation unit and header has been moved with
`git mv` to `src/adapter-softpc`, preserving history and content.  This is a
source-layout move, not a machine or BOP semantic change.

## Retained mantle inventory

| Area | Files | Reason |
| --- | --- | --- |
| Parameter/SIM/logging | `paramtree.*`, `logio.cc` | Native Bochs product-independent assembly. |
| Native port/PIC | `minimal_port_space.cc`, `pic.*` | Bochs port-handler and 8259 lifecycle only. |
| Native keyboard | `keyboard.*`, `scancodes.*` | Headless native 8042 mechanics; no guest service policy. |
| PC timing | `pc_system.*` | Native timing/lifecycle only. |

The former project-specific names `bx_ntvdm_mantle_pic_*` and
`ntdos64_port_is_registered` were replaced by neutral
`bx_mantle_pic_*` and `native_port_is_registered`.  They retain the same PIC
and port dispatch behavior; no OpenNT/VDM term remains in the mantle source
surface.

## Moved adapter-softpc inventory

The 42 moved files cover the copied `#UD` bridge, finite run/session, minimal
machine/SIM wrapper, state/action records, checked RAM and port actions, A20,
8042, IRQ, interrupt/segment/write observations, cancellation and instruction
history.  They are OpenNT-facing SoftPC/CCPU mechanical interfaces and now
belong to `adapter-softpc`; the component remains selector/service blind.

## Core exception reconciliation

`src/bx-core/README.md` now contains a component-local index for each active
Bochs source exception.  The detailed upstream/procedure/verification ledger
remains `docs/etc/research/adapter-external-intrusion-exceptions.md`; no
exception is created or broadened by this relocation.

## Deferred build-graph work

Formal Ninja source lists and fixture directory moves are intentionally S8
work.  Their current old path strings are recorded in the S1 manifest and are
not a claim that the old mantle remains a live owner.  Product source include
edges are repaired in this S3 change; S8 will perform the one coherent graph
switch and full build verification.
