# M0 T258 S1 — Shared Guest-Pointer Mapping Manager Plan

## Question

Can the active OpenNT-shaped DEM, COMMAND and XMS mirrors retain their
historical guest-pointer calls through one bounded synchronous bx-vdm mapping
manager, without changing BOP semantics or exporting a host pointer?

## Source-first disposition

| Recovery rung | Result |
| --- | --- |
| Original source | Retain each original `GetVDMAddr`/`Sim32*` call and its surrounding ordering in the imported body. |
| Same-shaped facade | Required: the NT4 SAS/CCPU backing is unavailable, so named bx-vdm declarations preserve call shape while a session lease supplies the bounded implementation. |
| External intrusion | Rejected: no bx-core/bx-mantle/Bochs change is needed. |
| New behavior | Limited to the session lease implementation; it is required only because the original SAS pointer product shell is unavailable. |

## S1 sequence

1. Enumerate every active helper and each source call's address form, span,
   access, synchronous/asynchronous lifetime and current test.
2. Define one mapping-manager implementation and the session's three fixed
   instances: `guest_memory`, `host_handle`, and `session_data`.  The latter
   owns opaque data-ID publication/lookup/release even before a future owner
   selects a concrete payload type. Define its
   lease record and the historical-shaped facade declarations for both 16:16
   real-mode and checked 32-bit guest-linear addresses. A lease is direct only
   for stable synchronous RAM; otherwise it is copied/bounce or explicitly
   refused. Family call records must reference one of these fixed instances
   and may not embed, allocate or retain another mapper.
3. Implement manager lifetime/epoch, checked direct or bounce access, and
   teardown/write-back.  No native pointer leaves bx-vdm.
4. Migrate the smallest DEM, COMMAND and XMS callers whose exact span and
   synchronous lifetime are already proven; leave all other rows as explicit
   staged debt rather than silently changing their behavior.
5. Run focused lease and migrated-caller tests, the formal Ninja graph, the
   governance gate and a final independent diff review.

## Required negative proof

- Invalid, out-of-range and expired lease acquisition fails.
- A write bounce commits only at successful synchronous release.
- Pointer-valued data never reaches a fixed-width machine action, guest
  memory, opaque-handle manager or asynchronous child/event path.
- DPMI protected/persistent mappings are rejected rather than being routed
  through a real-mode convenience helper.

## Deferred owners

Redirector async paths, COMMAND child-worker retention and all DPMI protected
pointer paths remain with their owner packages.  This S does not use the
manager to re-admit them.
