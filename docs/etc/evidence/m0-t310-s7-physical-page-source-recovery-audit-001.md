# M0 T310 S7 — Physical-page source-recovery audit

## Decision

`VdmSetPhysRecStructs` is an original SoftPC *interface* but not a recovered
original implementation. It must not receive a passive, bookkeeping-only, or
Bochs-backed definition merely to satisfy the link. The selected `i386 +
CCPU` profile requires a source-derived, same-shaped physical-page binding
only after the binding can perform a real selected-SoftPC memory operation.

This record corrects an earlier shorthand which described the selected
`ccpu386` profile as replacing `PhysicalPageREC.translation` entries. That
wording is present in the comment in `nt_mem.c`, but the selected profile's
`base/cvidc/prot_c.h` declaration has no `translation` member, and no selected
`ccpu386` source body consumes `GLOBAL_PhysicalPageRecords`. It is therefore
not evidence for inventing a table layout.

## Source inventory

- Original `softpc.new/host/src/nt_mem.c` has exactly two calls:
  `VdmAddVirtualMemory` calls `VdmSetPhysRecStructs(HostAddress,
  *IntelAddress, Size)` after allocation and `sas_overwrite_memory`; and
  `VdmRemoveVirtualMemory` calls it after cache invalidation and before free.
  Its declared contract is a void physical mapping update.
- Searches of the selected OpenNT, OpenNT 4.5, and `opennt-src-2` source
  editions find no definition of `VdmSetPhysRecStructs`.
- The imported NTVDMx64 evidence file `mvdm-softpc-patch/.../fmstubs.c` has a
  same-named empty TODO stub. It is not a recovered implementation and is
  not a legal product binding.
- `softpc.new/host/src/sim32.c` establishes `Start_of_M_area` from
  `InitIntelMemory`; `nt_mem.c` uses that historical process-local contiguous
  allocation for its private allocation list. It is not a fixed-width guest
  ABI and cannot be exported as a durable host pointer on x64.
- The selected `ccpu386/ccpusas4.c` resolves normal physical access directly
  as `Start_of_M_area + addr`; its `c_sas_connect_memory` changes only page
  type and its `c_sas_overwrite_memory` is intentionally a no-op. Therefore
  it has no source-defined external-page remap operation for this interface.
  An external DIB/host-page mapping cannot honestly be represented as an
  already-active CCPU mapping merely by publishing a surrogate.
- The old source-derived `adapter-mvdm-host-out/softpc/mvdm_sas.c` remains a
  pre-T310 Bochs test/recovery route. It is neither input nor fallback for
  the selected SoftPC graph and cannot satisfy this interface. Its eventual
  retirement remains the S9 owner decision.

## Required recovery shape

The one existing `session.guest_memory_mappings` instance is the only identity
authority for this boundary. A future same-shaped binding must:

1. Publish the native backing-object identity as one stable, session-local
   32-bit surrogate before it crosses a fixed-width SoftPC boundary, on both
   x86 and x64.
2. Preserve `VdmAddVirtualMemory` / `VdmRemoveVirtualMemory` names, parameter
   widths, alignment order, cache-invalidation order, and failure direction.
   A raw `ULONG` cast to `PVOID` is forbidden.
3. Resolve the surrogate only within a checked selected-SoftPC physical-page
   operation. The operation must establish/removes an observable mapping in
   the selected machine, not merely remember an ID.
4. Bound the span, reject overflow and stale IDs, and release/invalidate the
   mapping during the original remove/teardown sequence.
5. Keep `guest_memory_lease` out of durable page identity: it remains a
   synchronous copied-byte facility only.

The session mapping manager may be extended with span/lifetime metadata if
needed, but no second manager, pointer-split ABI, raw alias, or Bochs fallback
may be introduced.

## Recovered selected-SoftPC binding

The required same-shaped binding is now implemented below the named SoftPC
adapter, not in a mirror body or test seam:

- `mvdm_softpc_physical_mapping_publish` publishes the native backing range
  in the existing per-session `guest_memory` mapping-manager instance and
  returns its stable 32-bit identifier. This happens on both host
  architectures; x86 has no native-pointer identity shortcut.
- Original `VdmAddVirtualMemory` preserves its fixed-width call, alignment,
  Intel/SAS reservation and cache-invalidation ordering. Its only registered
  divergence is that `HostAddress` is the stable identifier and the adapter
  recovers the original native alignment before the original allocation.
- The exact historical `VdmSetPhysRecStructs(ULONG, ULONG, ULONG)` spelling is
  supplied by the named adapter. It binds the source span to the original
  Intel reservation and removes/releases it by that same Intel span when the
  original remove path calls the interface with zero host identity.
- The minimal `ccpusas4.c` hook asks the adapter first only when original CCPU
  needs a physical byte through `c_GetPhyAdd`; normal `Start_of_M_area + addr`
  behavior remains the unchanged fallback. The adapter returns a native byte
  address only for this immediate CCPU physical access, never as an MVDM ABI,
  session record, worker handoff or guest-memory lease.

This is a selected CCPU physical-page operation, not a claim that the old
`PhysicalPageREC.translation` layout exists, that WOW DIB mapping is fully
enabled, or that any Bochs/MONITOR/kernel-VDM path has been selected.

## Verification

Fresh formal MSVC `/MT` Ninja graphs were generated under
`build/M0-T310/S7/physical-x64` and `build/M0-T310/S7/physical-x86` and built
outside the sandbox. Both fixtures exit zero. Before their existing original
SAS/CPU/FPU/recursive-`host_simulate` interval, they deliberately create a
non-DWORD-aligned external 4096-byte backing range, publish it, invoke the
original `VdmAddVirtualMemory`, write/read it through original
`c_GetPhyAdd`, invoke original `VdmRemoveVirtualMemory`, and verify the
session mapping-manager active count is zero. The test therefore proves live
external physical access and original removal release on both x86 and x64;
it does not merely prove a remembered token.

## Lease-caller closure

The production source sweep finds exactly three direct uses of
`session_guest_memory_acquire`: the generic 16:16 guest-location facade,
Redirector pointer scope, and WOW pointer scope. Their detailed owner and
lifetime dispositions are recorded in
[`m0-t310-s7-guest-memory-lease-caller-ledger.tsv`](../operations/m0-t310-s7-guest-memory-lease-caller-ledger.tsv).
Each starts a bounded synchronous lease and releases it in the same owner
operation; none publishes an external backing object, binds an Intel physical
span, or duplicates `VdmSetPhysRecStructs`. The new SoftPC physical mapping
does not acquire a guest-memory lease at all. Therefore S7 removes no neutral
lease mechanism and transfers all three retained callers unchanged to their
named owner packages.

## Admission consequence

No current caller exercises `VdmAddVirtualMemory` or
`VdmRemoveVirtualMemory` in the bounded S5 CCPU proof. The actual direct
caller found in the selected source is WOW DIB mapping (`wow32/wdib.c`);
the DPMI descriptor-mapping discussion is non-`i386` conditional. XMS, DPMI,
EMM and WOW remain potential lower-memory consumers but are not proof that
the selected i386 CCPU has an external-page remap implementation. S7 can
proceed only by first recovering the selected CCPU physical-page operation,
or by registering the external-page form as explicitly unavailable for this
profile with a source-shaped failure path. It cannot close by linking the
empty NTVDMx64 stub.

## Commands

```text
rg -n "VdmSetPhysRecStructs" <all three original editions> <ntvdmx64 evidence>
rg -n "PhysicalPageREC|GLOBAL_PhysicalPageRecords" src/mvdm-host/softpc.new
```

## Header-placement verification

`mvdm-host/softpc.new/host/src/nt_eoi.c` is byte-identical to the selected
`opennt-src-2` original. Its non-MVDM declaration carriers are source subsets
under `opennt-host/public/sdk/inc/{ntexapi,ntrtl,nturtl,ntpsapi}.h`, rather
than adapter-owned copies. The modern `nt.h` type/import binding remains in
`adapter-mvdm-host-out/win32`.

Focused formal Ninja recompilation of `obj/host/nt_eoi.obj` passed on MSVC
`/MT` x64 and x86 after this relocation. The only diagnostics were pre-existing
original warning forms (`NTVDM`/`CPU_30_STYLE` macro redefinitions,
unreferenced `TicCount`, and the historical `IS32 *` callback mismatch).

The complete selected `original-softpc-host-roots.lib` archive also rebuilt
successfully on both architectures after the same header move. It covers all
sixteen selected original host roots, including `nt_eoi.c` and `nt_timer.c`.
Its warnings are original-source diagnostics such as old-style declarators,
NT4 macro overlaps, and pre-existing pointer-width sites in unenabled device
paths; neither architecture reported a new missing declaration or compile
failure from the relocated carriers.

## Mirror-placement rule applied to EOI and timer roots

`nt_eoi.c` itself is an original MVDM SoftPC host root, not a non-MVDM
dependency. It therefore remains byte-identically at
`mvdm-host/softpc.new/host/src/nt_eoi.c`; moving it to `opennt-host` would
break the selected MVDM package mirror and obscure its original owner.

The rule applies at the original-file boundary:

- a non-MVDM original OpenNT file (or a source-identified true subset) is
  restored below `opennt-host` at its original relative name; and
- an interface whose original body cannot compose on the modern host is
  supplied only by the named same-shaped family below
  `adapter-mvdm-host-out`.

For EOI/timer, `ntexapi.h`, `ntrtl.h`, `nturtl.h`, and `ntpsapi.h` are the
first case and now live under `opennt-host/public/sdk/inc/`. Modern
type/import and unavailable-behavior bindings are the second case and remain
under the adapter. This keeps `nt_eoi.c` and `nt_timer.c` source-shaped while
preventing a compatibility reimplementation from entering either mirror.

## Superseded focused verification note

The earlier hard-fail `VdmSetPhysRecStructs` fixture seam was intentionally
removed once the real adapter binding above replaced it. The current focused
fixture now exercises that binding directly. The declaration-relocation result
remains valid; its earlier passive-stub guard is superseded by the live-page
verification above.
