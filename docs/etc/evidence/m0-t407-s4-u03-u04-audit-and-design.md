# M0 T407 S4 U03/U04 audit and implementation design

## Scope and stop point

This is the owner-directed audit/design stop point. No product source was
changed. The audit covers selected U03 effective-address resolution and U04
platform projections, rather than treating a successful DOS command as proof
of protected-mode platform correctness.

The paired local/original gross deltas are: `modesw.c` +311/-6,
`dpmi32.c` +78/-1, `dpmiselr.c` +11/-1, `data.c` +19/-0,
`dpmiint.c` +108/-17, `dpmidata.h` +28/-0, `dpmi32p.h` +6/-0, and
`c_seg.c` +48/-0. The effective-address adapter is an independently authored
101-line carrier, not a mirror-diff line count. These are scope measures, not
claims that all lines are removable.

## U03 effective-address ledger

| Unit | Original owner and current consumer | Finding | Disposition |
| --- | --- | --- | --- |
| SIM32 numeric translation | `base/mvdm/softpc.new/host/src/sim32.c::sim32_effective_addr`; callers through `Sim32pGetVDMPointer` | Original CPU30 route reads the descriptor table. CPU40 uses its original `selector_outside_GDT_LDT` and `read_descriptor_linear` through the adapter. | Retain one numeric facade, but make descriptor-table lookup the normal protected-mode rule after the U04 table split. |
| Current-segment preference | `mvdm_softpc_effective_address.c::current_segment_base` | Selector equality does not prove cache equality after descriptor mutation: CS/SS/DS may retain different hidden base/limit/AR snapshots. Choosing the first CS/SS/DS/ES/FS/GS match has no original SIM32 counterpart. | **Replace, not retain.** Delete the cache-preference helper and its private accessor declarations after separate GDT/LDT publication is proven. This removes the U03 policy rather than adding another decoder. |
| `0040h` protected-mode bypass | `c_seg.c::setSR` | It directly manufactures base `0400h`, limit `ffffh` and writable data AR, bypassing original bounds/present/type/accessed checks. DOSX does establish `SEL_BIOSDATA` through its original `NSetSegmentDscr`, so the special case is a publication workaround, not the owner. | **Delete after P1.** Restore original `c_seg.c` validation unchanged once the DOSX descriptor lands in the correct GDT. |
| low-CS observation | `c_seg.c` plus termination observer | On selector zero, argument evaluation performs SAS reads before the optional report gate. This is not semantically inert diagnostic code. | **Delete immediately with P1** (separate from descriptor correctness). It has one caller and no original owner. |

The directly removable `c_seg.c` additions are the 48-line low-CS observation
and `0040h` bypass, but only the observation is independently removable now.
The bypass must remain until P1 proves the descriptor table it currently
stands in for.

## U04 platform-projection ledger

| Projection | Original owner / reached consumer | Current gap | Design disposition |
| --- | --- | --- | --- |
| Process LDT publication | `base/mvdm/dpmi32/i386/dpmi386.c::DpmiSetX86Descriptor`; consumer `dpmiselr.c`, CCPU LDT | Original publishes user descriptors through `NtSetLdtEntries` or `ProcessLdtInformation`. Current copies to a guest shadow and installs it as LDTR. | **P1 replace.** Rehost the original MVDM publication ordering in `mvdm-host`; retain only a CCPU table-binding seam. If validation is needed, import the kernel-only `base/ntos/ps/x86/psldt.c::PspIsDescriptorValid` into `opennt-host` as a separately audited finite helper. Do not use it for GDT/TSS entries. |
| GDT/IDT tables | DOSX `dpmi/486/dxboot.asm` lays out IDT then GDT and fills descriptors through `NSetSegmentDscr`; CCPU consumes GDTR/IDTR | Current uses the LDT shadow as GDTR, infers `IDT = AX:0 - 2048`, latches only the first address, and restores it on every protected interrupt. This conflates process LDT and DOSX GDT and has no reset proof. | **P1 replace.** Give GDT, LDT and IDT separate bounded guest-backed tables. Preserve DOSX layout/descriptor publication; make IDTR refresh follow actual publication and lifecycle, not an unreset file-static first-address latch. |
| Native task carrier | kernel VDM task setup plus CPU40 TR; consumer `modesw.c` | Current creates two busy TSS records in slots `01f0/01f8`, a backlink and I/O bitmap. No complete original standalone initializer was found. `KiInitializeGdtEntry` is only a field encoder, not the lifecycle owner. | **Retain as a registered minimal seam for P2.** First test selector slots, TSS layout, reset and port-I/O behavior. An optional `opennt-host` import of the finite encoder may replace field packing only after it reproduces the required system-descriptor values. |
| VDM_DPMIINFO / locked PM stack | `VdmTib.PmStackInfo` in original MVDM; DOSX `SEL_VDMTIB` consumer in `dxboot.asm`/`dxintr.asm` | Original guest gets a kernel VDM_TIB address. A modern host pointer cannot enter guest memory. Current allocates and fills a guest projection. | **Retain but reduce in P2.** Keep one guest projection with the original structure/layout and update ordering; bind it to session reset/teardown. Do not replace it with an adapter-owned alternate structure. |
| FastWOW TEB/TD prefix | `base/mvdm/wow32/i386/fastwow.asm`, `wow32.h` TD layout; consumer FastWOW | Original reads TEB `WOW32Reserved` at `0xc0`, then TD fields `vpStack`, `vpCBStack`, `FastWowEsp`. Modern TEB cannot provide the NT4 carrier to guest code. Current projects only the necessary 0x1000 page/prefix, but lacks proven initialization/reset/teardown and complete writer audit. | **Retain as P2 finite projection.** Keep guest values only; drive fields from original FastWOW/WOW32 writers and test first entry, callback, exit and reuse. Never publish a host TEB or host TD pointer. |

## Four-rung result

1. Direct original MVDM source is available for descriptor publication,
   descriptor layouts, DOSX table establishment and VDM_DPMIINFO/WOW field
   structures; those owners stay in `mvdm-host`.
2. CPU40 needs finite bindings for CCPU table registers and guest-addressed
   projections. They must preserve original ordering/layout and not create a
   mapping manager.
3. The kernel-only descriptor validator/field encoder, if used, belongs in
   `opennt-host` at its original-relative path after its own source audit.
4. No new autonomous table, TSS or TEB policy is justified. Existing
   autonomous portions must be deleted, replaced by a source-shaped carrier,
   or retained with a named, tested unavailable-kernel dependency.

## Recommended implementation sequence

### P1 — descriptor-domain separation (recommended first)

Rehost original `DpmiSetX86Descriptor` ordering through one CCPU LDT binding;
create separate GDT/IDT backing from the DOSX table layout; prove publication,
descriptor update, source-table reuse, reset and invalid-descriptor behavior.
Then delete the `c_seg.c` `0040h` bypass, U03 cache-preference policy and
low-CS observation. Expected direct cleanup is at least the 48 current
`c_seg.c` additions plus the cache-preference helper/diagnostic helper
surface; the larger U04 replacement is intentionally not estimated as a net
deletion until the new source-shaped table carrier is measured.

### P2 — finite kernel-carrier projections

Audit and bind TSS, VDM_DPMIINFO and FastWOW TEB/TD separately. Each needs
initialization, reset, failure, teardown and real-program hit evidence.
P2 must not use `PspIsDescriptorValid` for system descriptors or make a host
pointer visible to the guest.

## Verification required before P1 implementation

- fixture: descriptor publication then source-table mutation; old loaded
  selector must retain the published descriptor while new selector loads see
  the designated table;
- fixture: `SEL_BIOSDATA` must load through normal original CCPU validation,
  with no `0040h` condition;
- fixture: equal visible selector with distinct cached snapshot must select
  the original table rule, not arbitrary register order;
- integration: formal x86 link, DOSX 53:00/53:01 frontier, COMMAND/EDIT
  regression, and an explicit result for WOW/WRITE rather than an inference;
- lifecycle: failed allocation, second session/reset, table re-publication,
  stale guest address and teardown.

No S4 code implementation is authorized by this record.
