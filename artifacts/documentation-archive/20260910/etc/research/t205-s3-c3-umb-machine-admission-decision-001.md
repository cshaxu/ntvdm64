# M0 T205 S3 — C3 UMB machine-capability admission decision

## Question

Can the admitted no-device Bochs machine expose a selector-blind C3
upper-memory reservation lifecycle sufficient for the complete XMS UMB group
`52:06..08`, without inventing firmware, device, or historical product-host
state?

## Inputs

| Input | SHA-256 | Relevant fact |
| --- | --- | --- |
| `src/opennt/base/mvdm/xms.486/xmsumb.c` | `1608B70F239570F2CB5BEEAEEACA63065CC9550BD6BAD15B47F4160307B33DDC` | `xmsInitUMB` repeatedly claims `UMB_OWNER_XMS`; request/release operates only over that resulting map. |
| `src/opennt/base/mvdm/softpc.new/base/inc/umb.h` | `2461D7F74C3317A98D1456C238AD2F14BCDB2CBCD189D39836F24A353893E4B3` | The historical UMA begins at `0xC0000`, is bounded by `0x100000`, and has six distinct ownership states. |
| `src/opennt/base/mvdm/softpc.new/host/src/nt_umb.c` | `2A109F11985507463797E60B8B19FC12441BD18278B1444DDE581CC351DE72ED` | Reservation transitions include ROM, RAM, XMS, EMM and VDD ownership with SAS or NT section mapping. |
| `src/bx-mantle/bx_ntvdm_minimal_machine.cc` | `4A4388BE41F2E8F584C1DB7907116F78E7107336EE7DDC29BF6CCF25AE4A8C29` | The admitted machine initializes memory, empty ports and CPU only; it publishes no UMA owner map or firmware/device lifecycle. |
| `src/bx-core/memory/misc_mem.cc` | `EC9A98AF943ADA5E2B1DCF2EA4CF6B01C52D328E1806408C20E57420DD777B6D` | Low-ROM accesses remain special memory handling; backing bytes are not an allocation authority. |

The prior T199 C3 map is retained corroborating evidence, not a substitute for
this current-packet decision: [T199 S25](t199-s25-c3-umb-owner-abi-failure-map-001.md).

## Procedure

1. Read the complete XMS UMB provider and its dispatcher order.
2. Trace every `ReserveUMB` and `ReleaseUMB` transition to its historical
   owner and memory-side action.
3. Compare those prerequisites with the actual no-device machine lifecycle
   and the permitted mantle boundary.
4. Reject any proposal that derives allocatable UMB space from blank ROM,
   unspecified RAM, an adapter table, or a newly enabled device.

## Observations

`xmsInitUMB` obtains no address independently: it calls
`ReserveUMB(UMB_OWNER_XMS, &Address, &Size)` until that historical owner map
has no RAM-owned segment left.  `xmsRequestUMB` then splits/coalesces only
that XMS-owned list and exposes `B0` (too small), `B1` (no UMB), and `B2`
(unknown release segment) only after the list is real.

The original C3 provider is not a generic physical-RAM allocator.  Its owner
transitions require the original `InitUMBList` layout, historical ROM regions,
VDD/EMM interaction, and either `sas_connect_memory` or NT process section
mapping.  These are product-host and device/firmware composition facts, not
an XMS register contract.

The current mantle initializes no firmware, video/option ROM, EMM, VDD, SAS
or UMB layout.  It intentionally owns only an empty port space and bounded
ordinary/backing RAM mechanics.  The core's special treatment of the ROM
window does not identify a writable or allocatable region.  Treating erased
ROM bytes as free would manufacture both map ownership and historical errors.

## Decision

**C3 is not admitted for the current no-device profile.**  Therefore T205
does not create a C3 record, a synthetic aperture, or an adapter UMB
allocator.  XMS `52:06`, `52:07`, and `52:08` remain one explicit deferred
subgroup; no member receives a local workaround.

The retained source-derived package route must report its already documented
deferred outcome rather than claim `B0`, `B1`, or `B2`, because those results
presuppose an actual map.  This is a no-map/default-off decision, not a claim
that UMB semantics are unavailable in every later machine profile.

## Follow-up

T205 S4 is not admitted: its sole purpose was the conditional UMB provider
implementation.  A future machine-profile task may reopen it only after it
proves a first-profile UMA layout, its native owner, reset lifecycle and
negative absence test without importing the historical SoftPC allocator.

Continue T205 with the independently gated C5/INT15 assessment for `52:09`.
It must likewise remain machine-owned and may not be implemented by the VDM
adapter.
