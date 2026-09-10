# M0 T198 S64: `BOP C8` Headless Mouse-Vector Mapping Audit

## Observed contract

The clean source-built run at
`artifacts/build/t198-s64-mouse-install-audit-r1` reaches `C4 C4 C8` at
`8dc8:78cb` with `CS:BX=8dc8:77bf`, `DS=0809`, `AX=073b`, and `DX=8d8d`.

## OpenNT source map

`dos/v86/doskrnl/bios/spcmse_4.asm` constructs `sysinitgrp:mio_table`, puts
its offset in BX, and invokes `bop 0C8h`.  Its immediate continuation reads
the freshly written INT 33h IVT vector, uses it to patch guest mouse entry
addresses, then installs its own guest INT 33 handler.

The retained SoftPC BIOS table names `C8` `host_mouse_install1`.

## Existing finite provider

`bx_ntvdm_mouse_install1_mapping_service_v1` validates real-mode `C4 C4 C8`,
reads exactly four bytes from `CS:BX`, and writes exactly those bytes to IVT
INT 33 (`0xCC`).  It neither interprets the table nor adds mouse state, input,
IRQ, PIC, EOI, firmware, host integration, or Bochs behavior.  The following
guest code retains ownership of its own vector and handler installation.

S65 may compose this existing two-phase checked-RAM transaction directly
through the mantle's generic read/write mechanics.

