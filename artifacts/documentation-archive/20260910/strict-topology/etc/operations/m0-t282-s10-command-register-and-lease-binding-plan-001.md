# M0 T282 S10 — COMMAND register and bounded-lease binding

Recover the minimum source-shaped SoftPC boundary needed before the original
COMMAND environment/current-directory group can be linked as behavior:
the reached 16-bit register accessors declared by original `softpc.h`, and
the already-admitted bounded `mvdm_guest_location` lease route.

## Objective

Audit and, where the current mechanical boundary permits it, provide the
original `getDS`, `getDX`, `getSI`, `getES`, `getBX`, `setAL`, `setBX` and
`setCF` call shapes for a synchronous BOP/service invocation.  Bind them only
through `adapter-softpc -> adapter-bochs`; use the existing session
`guest_memory` mapping-manager instance through `mvdm_guest_location` for
every guest-memory access.

## Scope

- Prove the exact original declarations and their historic monitor/CCPU
  ownership from `opennt-mvdm-support/inc/softpc.h`.
- Inventory the current `adapter-bochs` mechanical state surface and identify
  the smallest typed read/write additions required for the reached registers.
- Add only the source-shaped adapter-softpc functions and focused x86/x64
  fixture coverage if the typed machine surface can be kept selector-blind.
- Preserve 16-bit partial-register semantics and carry-flag behavior.

## Non-goals

- No generic CPU-frame ABI, BOP selector/route, mirror-body change, host
  environment/current-directory policy, unbounded `GetVDMAddr` facade, new
  mapping manager, CCPU executor, or Bochs-core source change.
- No persistent guest pointer, native pointer/HANDLE conversion, async lease,
  or register API beyond the reached COMMAND sites.

## Exit criteria

The reached original register forms have one verified, selector-blind typed
mechanical binding or a source-proven blocker.  The fixture proves low-word
and low-byte write preservation plus carry updates on both x86 `/MT` and x64
`/MT`; all guest memory remains an exact synchronous lease.  A later S may
then make only the ledgered minimal COMMAND mirror divergences.
