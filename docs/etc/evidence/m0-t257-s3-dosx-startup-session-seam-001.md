# M0 T257 S3 — DOSX Startup Session Seam

## Question

Can the original `dpmi32.c` DOSX startup/table data ordering be recovered
without carrying `Sim32GetVDMPointer` host pointers, `IntelBase`, `Ldt` or a
descriptor cache across the bx-vdm/machine boundary?

## Inputs

- Exact T257 S3 mirror: `src/bx-vdm/bop/opennt/dpmi/{data,debug,dpmi32,dpmiselr}.c`
  and the associated original headers.
- Original `DpmiInitDosx`, `DpmiInitApp` and `DpmiPassTableAddress` in
  `src/opennt/base/mvdm/dpmi32/dpmi32.c`.
- Selector-blind checked RAM API and T256 descriptor identity conclusions.

## Implementation

`bop/shim/dpmi_startup_session_shim.{h,c}` is a deliberately narrow
source-derived seam.  It reads the original 58-byte DOSX shared-data layout
through `bx_ntvdm_mantle_checked_ram_read_v1`, then retains each original
field as a fixed-width guest-linear value or scalar.  The fields are assigned
in the source order used by `DpmiInitDosx`.

The app initializer preserves the original `AX & DPMI_32BIT` flag extraction.
Selector-table publication records only the `selGDT` guest-linear identity.
It does not read/copy descriptors, set an LDT, expose a host pointer, or alter
native Bochs descriptor behavior.

## Verification

- Fresh formal graph: `build/M0-T257-S3/formal-r1/` (279 Ninja nodes).
- `bin/t257-s3-dpmi-startup-session-fixture.exe` built and exited `0`.
- The fixture writes all 58 bytes through checked RAM, confirms representative
  values from the beginning, middle and end of the original layout, verifies
  selector-table identity, protected-mode app flags, and rejects real-mode or
  null-input requests.

## Interpretation

This is partial source-composition evidence for `53:04`, `53:05` and `53:0F`,
not active BOP support.  The exact OpenNT translation units are staged but not
yet formal build members at the time of this record.  Its successor,
[source-composition evidence 002](m0-t257-s3-opennt-dpmi32-source-composition-002.md),
records the later formal `dpmi32.c` admission.  S4 remains owner of protected
interrupt/fault/IRET frames.
