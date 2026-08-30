# M0 T310 S16 P3 — original BIOS reset and dispatch boundary

## Manifest review

The original `base/bios/sources` manifest selects fourteen C members. Their
initial owner dispositions are recorded in the companion
[manifest ledger](m0-t310-s16-p3-bios-manifest-owner-ledger-001.tsv). The
ledger is intentionally source-unit scoped at this stage: it preserves the
entire original body and makes no table-entry-by-table-entry replacement.

## Critical original control forms

- `bios.c` owns the original `BIOS[]` BOP vector and `bop_dispatch` form. The
  vector reaches independent controller and host-BOP owners, so it is compiled
  but not enabled as a synthetic all-success dispatcher.
- `reset.c` owns `setup_ivt` and `reset`. It preserves original IVT order and
  calls SAS, CMOS, video, disk, keyboard, timer, DMA and other controller
  contracts. Those calls establish the actual dependency order for S17--S27;
  they are not a reason to reimplement reset in an adapter.
- `rom_basc.c` and `build_id.c` retain direct original source bodies. The
  former's error presentation is still subject to the host-platform owner; the
  latter has no new external callable dependency.
- `nt_rez.c -> host_find_file -> nt_unix.c` and `rom.c/cmos.c ->
  host_read_resource` form the selected immutable-input route. The current
  app/session location binding affects only lookup, not byte loading, checksums
  or SAS publication.

## Result

S16 has identified the complete original BIOS manifest and its out-of-package
owner edges. The next S16 work reads the ROM/CMOS loading contract and the
immutable-write failure disposition; controller enablement remains with its
named later S packet.
