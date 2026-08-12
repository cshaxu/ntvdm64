# T95 S7 OpenNT BOP Catalogue Completeness 001

## Scope

This record closes the *definition and observation* part of the BOP work. It
does not claim an implementation for every selector or service, and it does
not add any OpenNT service behaviour to Bochs.

The pinned evidence input is
`artifacts/analysis/opennt-bop-inventory-005-20260811-001/opennt-bop-inventory.json`.
It records the source hashes and the definitions from
`upstream/opennt/base/mvdm/inc/bop.h`, `BOP.INC`, and the six service-family
headers.

The retained inventory has 16 OpenNT top-level selectors, 194 named service
definitions across DOS/COMMAND/XMS/DPMI/redirector/debugger, 41 named SoftPC
BIOS selector definitions, and 349 explicitly annotated historical BIOS-table
rows. The table rows cover all 256 selector byte values; the inventory records
their conditional historical owners without claiming that a particular owner is
enabled in the Bochs composition.

## Result

`bx_ntvdm_bop_catalog_v1_observe` recognizes every syntactic `C4 C4 xx`
sequence as an observable BOP identity. Recognition neither invokes a service
nor changes CPU or guest state. The native Bochs listener remains default-off,
copies its existing generic #UD evidence, calls that identity-only catalogue,
logs the result, and then continues through the existing dispatch path.

All sixteen OpenNT top-level definitions are explicitly covered by the
adapter unit test. Since the SoftPC table also has a row for every selector,
each is reported as overlapping provenance:

| Selector set | Classification |
| --- | --- |
| `50,51,52,53,54,56,57,59,5A,5B,5C,5D,5E,5F,FD,FE` | overlapping OpenNT top-level and SoftPC BIOS-table provenance |

The overlap is intentionally ambiguous. Three instruction bytes do not
establish whether execution reached an OpenNT entry or a historical SoftPC
BIOS-table entry. A later reached endpoint must supply its own control-flow
and caller evidence before any adapter service is admitted. Other selectors,
including `06`, are classified as SoftPC BIOS-table provenance rather than
being falsely labelled uncatalogued.

For each of the six selector families, the adapter also classifies the fourth
byte as `present`, `truncated`, or `undefined`. The pinned source headers have
continuous defined service ranges: DOS `00..49`, COMMAND `00..11`, XMS
`00..0B`, DPMI `00..18`, redirector `00..30`, and debugger `00..0F`. This is
observation provenance only. `undefined` does not suppress logging and does
not turn into a handler; the unit test covers both a defined endpoint and the
first undefined DOS value (`50:4A`).

## Boundary and next rule

The catalogue is not a dispatcher. It has no handler pointer, OpenNT object,
guest-memory operation, CPU result, or host capability. Unreached definitions
remain logged/pass-through. A reached BOP is admitted only after its OpenNT
caller, immediate continuation, ABI inputs, result state, and host-boundary
requirements have been recorded; the resulting implementation belongs to
`src/bx-ntvdm-adapter/`, except for already registered generic Bochs mechanics.
