# M0 T257 S3 — OpenNT `dpmi32.c` Source Composition

## Question

Can the exact OpenNT `dpmi32.c` startup/table source itself enter the formal
`bx-vdm` link on x64 without reviving NT4 VDM pointers, a process LDT, or a
Bochs/DPMI cross-boundary API?

## Inputs

- Exact mirror committed by `640bd090`.
- T257 S2 [import ledger](../operations/m0-t257-s2-dpmi-dosx-source-import-ledger-001.md).
- T257 S3 startup-session seam and fixture.

## Changes

`src/bx-vdm/bop/opennt/dpmi/dpmi32.c` is now a formal `bx-vdm` member.  The
original 25-slot `DpmiDispatchTable` remains in its original order.  The only
edited sites carry `DIVERGENCE (T257 S3)` comments:

1. `DpmiDispatch` receives an already copied staged selector, advances IP by
   one in the original order, and never reads a raw guest pointer.
2. `DpmiIllegalFunction` retains the invalid-service terminal through a local
   diagnostic record rather than importing old NT4 debug/window plumbing.
3. `DpmiInitDosx` preserves its original shared-data field order through the
   checked fixed-width startup session seam.
4. `DpmiInitApp` and `DpmiPassTableAddress` retain their source semantics
   through staged fixed-width app/table records; neither recreates `VdmTib`,
   `Ldt`, `IntelBase` or `FlatAddress[]` host identity.

All other selector-table bodies are explicit source-shaped deferred terminals
until their owner subpackages are admitted.  There is still no active `53:xx`
ingress.

## Verification

- Fresh formal graph: `build/M0-T257-S3/formal-r2/` (281 nodes).
- Formal compiler output contains `dpmi32.c` as
  `obj/bx-vdm/src_bx_vdm_bop_opennt_dpmi_dpmi32_c.obj`.
- `t257-s3-dpmi-startup-session-fixture.exe` links and exits `0`.
- The fixture invokes the imported C symbols `DpmiInitDosx`, `DpmiInitApp`
  and `DpmiPassTableAddress`, rather than calling the session seam directly.

## Result

The first original DPMI host translation unit is formally composable.  This
is a source-composition milestone, not DPMI runtime completion: descriptor
update (`dpmiselr.c`), protected frames/IRET, memory and DOS translation retain
their declared later owner packages.
