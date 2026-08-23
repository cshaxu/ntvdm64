# M0 T257 S3 — Native Descriptor Lifecycle Ownership Map

## Question

Does completing `53:00` require a DPMI-aware Bochs/core intrusion, or can the
DOSX descriptor lifecycle remain ordinary guest code on native Bochs?

## Observations

1. In the selected DOSX `486/dxstrt.asm`, after the protected-mode switch,
   `selGDT` is assigned `SEL_LDT_ALIAS`; the guest clears the table through
   that alias and then issues `DPMIBOP PassTableAddress` (`53:0F`).  Later
   DOSX code modifies descriptor bytes through that guest alias and invokes
   `SetDescriptorTableEntries` (`53:00`).
2. OpenNT `dpmi32/dpmiselr.c` receives the entries through an NT4 pointer and
   calls `i386/dpmi386.c:DpmiSetX86Descriptor`.  The latter uses
   `NtSetLdtEntries`/`ProcessLdtInformation`: it exists because the old host
   process itself executed against a process LDT.
3. Native Bochs `BX_CPU_C::LLDT_Ew` in `src/bx-core/cpu/protect_ctrl.cc`
   already validates a guest GDT selector, fetches the LDT descriptor from
   guest memory, and installs the architectural LDTR cache.  Its ordinary
   descriptor fetches subsequently read the guest LDT.  No BOP, DOS or
   OpenNT identifier appears in that core path.

## Decision

`NtSetLdtEntries` must not be recreated.  The target design is a native guest
descriptor lifecycle:

```
DOSX guest writes LDT descriptor bytes
  -> native Bochs LLDT / segment-load mechanics consume guest table
  -> DPMI policy observes source-shaped completion in bx-vdm
```

`bx-vdm` may carry only the bounded `53:00` request/result and must not cache
or inject descriptors.  `bx-mantle` must not become DPMI-aware.  The sole
remaining proof needed before activation is a focused guest table/LLDT fixture
which demonstrates that the source-built DOSX table location is ordinary
guest memory visible to native Bochs.

## Consequence

No Bochs intrusion is selected by this map.  If the focused fixture disproves
the ordinary guest-table path, a separate registered mechanical exception
decision is required; no ad-hoc `FlatAddress[]`, host-LDT, or descriptor-cache
workaround is admissible.
