# S10 shard 3: DIB descriptor mapping retirement chain

Read-only follow-up to `wow-dib-selected-mapping-provider-review.md`. This is source-chain evidence, not a new test or a claim of a reproduced defect. The actual observed cached `wdib.obj` selects the non-i386 Vdm mapping calls, and published WOW32 code-section identity is established in the prior report; installed worker provider identity remains independently unbound.

## Creation and normal retirement

1. `VdmAddDescriptorMapping` (`dpmi32/dpmiselr.c:329-382`) allocates `DESC_MAPPING`, records selector range, Intel LDT base and host flat base, prepends the single `pDescMappingHead` list, and sets each `FlatAddress` entry. NTVDM's own `nt_mem.c::VdmAddVirtualMemory` owns the reserved emulated-address mapping, a separate resource.
2. `WOW16SetDibSel` (`wow16cal.asm:1017`) calls original `set_sel_for_dib` (`3protect.asm`). For a selector array with `csel=0`, it calls `IFreeSelector`; for allocation, `AllocSelectorArray`, limits and physical address setup occur in the guest. For an existing DIB selector, the guest updates its heap/selector association. The historical source does not prove the exact installed guest binary was built from it.
3. `IFreeSelector` delegates to `FreeSelArray`; the `free_sel` WOW branch clears the descriptor, then issues `WOW_DPMIFUNC_0C` when appropriate. `DpmiSetDescriptorEntry` processes the new base and calls `GetDescriptorMapping` (`dpmiselr.c:248`). The local DIV-310 publisher also calls it at line 156. `GetDescriptorMapping:410-444` returns remapped host base when selector and base still match; if selector is in a mapping but the LDT base differs, it unlinks and frees the mapping record. Thus the source has a *descriptor-update-driven invalidation path*, not direct removal by `VdmRemoveVirtualMemory`.
4. `VdmRemoveVirtualMemory` (`nt_mem.c:1011-1055`) restores physical-page translation, clears remap state and frees the reserved emulated address; it has no direct `pDescMappingHead` operation. `W32CheckAndFreeDibSectionInfo` sends selector-free callback before virtual-memory removal and then deletes bitmap/list node; `W32FreeDibInfoHandle` restores selector before its virtual-memory removal. These are distinct state transitions whose ordering matters.

## Remaining exact questions

* Confirm with a real callback/selector witness that each final selector-free or restore operation executes the DPMI update with a base different from the recorded one, so `GetDescriptorMapping` actually removes its node. A same-base replacement, selector reuse, multi-selector update order or failed/partial callback cannot be concluded from source inspection.
* `W32CheckAndFreeDibSectionInfo` ignores callback result and proceeds with `VdmRemoveVirtualMemory` and bitmap deletion. Verify actual callback failure/reentry and check selector/flat mapping before reuse; no actual failure is asserted.
* The path after map creation but before ownership-node insertion was recorded in the preceding report. Trace fault-injected mapping count, selector and host buffer after each failure; do not infer that removing emulated memory alone removes descriptor list state.
* The guest source says `IFreeSelector` in the selector-array free path, but source-to-installed-guest identity and per-selector DPMI calls still require independent runtime evidence. No alternate state registry is proposed.

Replay from repo root:

```powershell
rg -n 'pDescMappingHead|VdmAddDescriptorMapping|GetDescriptorMapping\(' src/mvdm/dpmi32/dpmiselr.c
rg -n 'VdmRemoveVirtualMemory|VdmFreeVirtualMemory' src/mvdm/softpc.new/host/src/nt_mem.c
rg -n 'WOW16SetDibSel|WOW16FreeDibSel|set_sel_for_dib|IFreeSelector|WOW_DPMIFUNC_0C' src/mvdm/wow16/kernel31/wow16cal.asm src/mvdm/wow16/kernel31/3protect.asm
rg -n 'W32CheckAndFreeDibSectionInfo|W32FreeDibInfoHandle|VdmRemoveVirtualMemory' src/mvdm/wow32/wdib.c
```
