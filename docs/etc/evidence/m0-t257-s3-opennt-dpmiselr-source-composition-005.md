# M0 T257 S3 — OpenNT `dpmiselr.c` Descriptor Source Composition

## Question

Can the original 486 `DpmiSetDescriptorEntry` ordering be formally composed
without importing an NT4 process LDT or introducing a descriptor cache outside
native Bochs?

## Inputs

- Original `src/opennt/base/mvdm/dpmi32/dpmiselr.c`.
- Original `src/opennt/base/mvdm/dpmi32/i386/dpmi386.c` as the historical
  `DpmiSetX86Descriptor` target.
- T257 S3 startup/table source composition and native GDT/LDT fixture 004.

## Source-derived boundary

The admitted `dpmiselr.c` body retains its original 486 branch, including the
descriptor-base/limit calculation, the high-limit repair, `DpmiSetX86Descriptor`
call and only-then `setAX(0)` success order.  Every source change is marked
`DIVERGENCE (T257 S3)`:

1. The original `Sim32GetVDMPointer(ES:BX)` becomes a bounded ES-range copy
   through the existing selector-blind protected-range action.
2. The original `NtSetLdtEntries` / `ProcessLdtInformation` effect becomes a
   bounded write at `selector_table_linear + AX`, where the table identity was
   already published by the original `DpmiPassTableAddress` source body.
3. The source's `FlatAddress[]` write is omitted only in the selected 486
   branch: it was an NT4 process-LDT host-address cache, while native Bochs
   reads the guest LDT directly. No equivalent cache is retained.
4. Generic legacy helper spellings such as `getAX` are translation-unit-local
   aliases, so this imported body cannot collide with other original owners in
   the combined bx-vdm link.

The seam owns no descriptor state. Its temporary fixed-size list lasts only for
the checked read and immediate checked write. Native Bochs remains the sole
descriptor consumer; fixture 004 proves that consumer path separately.

## Verification

- Fresh complete formal graph `build/M0-T257-S3/formal-r6/` built all
  **450/450** targets, including `dpmiselr.c` and
  `dpmi_descriptor_source_shim.c` as `bx-vdm` members.
- `t257-s3-dpmi-descriptor-source-fixture.exe` exited `0`.
- The fixture proves: a valid AX/CX list is read from ES:BX, its original
  high-limit repair changes `FFFFh` to `7000h`, the repaired entry is written
  to published table offset `AX=20h`, and AX becomes zero only on success.
- It also proves the original invalid-selector early return and a rejected
  bounded guest read both preserve AX and make no table write.

## Result

The `53:00` descriptor source is now a formally linked, locally exercised
partial composition. It is not active BOP ingress: protected-mode BOP staging,
table lifecycle, interrupt/frame composition and guest continuation remain
whole-package DPMI work. No Bochs intrusion is selected.
