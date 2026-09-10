# M0 T198 S66: `BOP 17` Printer-Unavailable Contract Audit

## Observed input

The clean source-built run at
`artifacts/build/t198-s66-printer-observation-r1` reaches `C4 C4 17` at
`0203:02d5` with `AH=00`, `DX=0000` (LPT0), `BX=035c`, `DS=0070`, and
`FLAGS=00000046`.

## Original source map

The SoftPC BIOS table maps selector `17` to `printer_io`.  Its documented
AH=0 operation sends AL to the selected LPT port.  The entire body is guarded
by `#ifdef PRINTER`; without that product feature it performs no I/O and
returns without changing guest state.

## Existing provider

`bx_ntvdm_printer_unavailable_service_v1_dispatch` exactly validates the
three-byte real-mode `BOP 17` form and produces that empty three-byte resume.
It does not open an LPT port, create a printer device, invoke a spooler, or
access host resources.  S67 may compose this existing source-defined profile
disposition at the top-level boundary.

