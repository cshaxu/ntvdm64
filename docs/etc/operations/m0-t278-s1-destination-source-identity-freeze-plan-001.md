# M0 T278 S1 — destination and source-identity freeze plan

This packet freezes the exact physical-re-root manifest before any `git mv`.
It derives all 1,689 paths from the committed T274 canonical source/hash union
and T277 final package audit.

Destination rules are mechanical and exclusive:

- `dirs`, `makefil0`, `inc`, `oemuni` and `suballoc` move to
  `opennt-mvdm-support`;
- `vdmutils` moves to `opennt-mvdm-tools`;
- only `softpc.new/base/bios`, `softpc.new/bios`, `softpc.new/roms` and
  `softpc.new/data` move to `opennt-mvdm-firmware`;
- every other selected source path remains below `opennt-mvdm-host`.

The ledger records the original source SHA-256 and its target. It is an input
to the later movement packet; it does not alter files or build inputs.
