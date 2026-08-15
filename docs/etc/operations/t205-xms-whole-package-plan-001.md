# T205 S1 — XMS whole-package re-admission and implementation plan

## Question

What must be completed, moved, or kept explicitly unavailable for the full
OpenNT XMS family `52:00..0B`, without turning partial routes or a native
trace into a substitute for the package?

## Inputs and method

The authoritative order is OpenNT `xmsdisp.c`
(`24B0FA2FE32BD7A94606D424A10B0961A9911190CB2684456AF04034E04BFF01`)
with `xmssvc.h`. The inspected handlers are `xmsa20.c`
(`7261B7064A089B655B8D41D3E4EF6887BE869AF2B7B62970389AEEE9A8ACC94`),
`xmsblock.c` (`BAD1DE87B499D3052E757D858B39437232F5653610DC5560C06AFCC5E05FD391`),
`xmsmisc.c` (`10AD205D9DEDE714749A9746DD99B3389546B8AAF3A63DD0236386A9D4CE5245`),
and `xmsumb.c` (`1608B70F239570F2CB5BEEAEEACA63065CC9550BD6BAD15B47F4160307B33DDC`).

The current comparison covers `bx_ntvdm_xms_package_session_v1` and the
selector-blind mantle A20/extended-memory capabilities. T200 and T199 are
evidence only; partial routes are not credited as a complete XMS provider.

The current inventory exporter produced
`artifacts/reports/t205-s1-bop-source-inventory-20260815.json`
(`30A44A4448AD261952BC73772C7F294C14820E77048D2BB2D5FCE44D3331D9E0`): 203 total entries and exactly
12 XMS entries in the dispatcher order below. This is a coverage check, not a
provider or runtime claim.

## Slot ledger

| Slots | OpenNT contract | Current result | Required owner disposition |
| --- | --- | --- | --- |
| `00` | AX `0/1/2` changes or queries A20 wrapping; AX/BL report source-defined result | Mantle A20 wrapper and adapter translation | Retain only after a source-register/A20-state regression proves no observable inversion. |
| `01` | `SS:BP` move record, word-count, two linear addresses, AX/BL failure | Typed stop | One copied move-record transaction over checked conventional/extended backing copies; no flat guest pointer or generic host memcpy. |
| `02`,`03`,`0B` | Allocate/free/reallocate by KiB base and size, with first 64-KiB reservation | Partial opaque allocator route | Reconcile zero-size, stale base/size, exhaustion, resize and source-register errors as one block-lifecycle group. |
| `04` | AX is historical `GetSystemInfo().dwPageSize` | Typed stop | Source-derived page-geometry contract; never ambient host RAM or invented Bochs paging. |
| `05` | AX is declared XMS extended KiB | Mantle capacity query | Tie it to admitted XMS allocator geometry, not ambient host memory. |
| `06`,`07`,`08` | Initialize, allocate and release UMB paragraphs; BL `B0/B1/B2`; coalesce | Typed stop | C3 real UMB map/reserve/release mechanical capability first; then XMS provider. No adapter UMB allocator. |
| `09` | Register INT15 vector and return XMS KiB in CX | Typed stop | C5 real-mode interrupt/firmware lifecycle first; no adapter callback, PIC or keyboard emulation. |
| `0A` | AX total free KiB; DX largest free KiB | Typed stop | Selector-blind mantle allocator-statistics request, then source register translation. |
| `0C` | `XMS_LASTSVC` sentinel | Rejected | Preserve non-callable rejection; never reproduce release-table out-of-bounds behavior. |

## Ownership decisions

- `bx-core` owns RAM translation, A20, memory map, firmware and interrupts;
  it never names XMS or BOP.
- `bx-mantle` may expose fixed-width selector-blind lifecycle requests for
  admitted A20, extended-memory, UMB-map or interrupt capabilities.
- `bx-vdm` owns copied layouts, source-derived register/failure translation
  and the XMS session, never raw mappings, RAM allocation or interrupt hooks.
- OpenNT source remains normative; SAS/SoftPC/CCPU calls are not modern link
  dependencies.

## Ordered later S work

1. **S2 — block/A20/geometry completion:** `00..05,0A,0B` together: copied
   move record, opaque lifecycle, statistics, page-size, register/error table
   and whole-group tests.
2. **S3 — C3 UMB capability admission:** actual memory regions,
   reserve/release lifecycle and negative no-map behavior before XMS code.
3. **S4 — UMB provider completion:** `06..08` together, including paragraph
   split/coalesce and `B0/B1/B2` errors.
4. **S5 — C5 INT15 capability and provider completion:** native default-off
   interrupt lifecycle first, then `09` with capacity regression.
5. **S6 — family convergence:** all `52:00..0B` through common ingress,
   exact `0C` rejection, obsolete partial-route cleanup and only then one
   bounded native integration observation.

## Exclusions

No native trace occurs before S6. DPMI is not a shortcut. Top-level `5E`
remains an XMS/DEM follow-up. No firmware/media import, host modification, raw
pointer, CCPU/SAS restoration or full Bochs product configuration is admitted.
