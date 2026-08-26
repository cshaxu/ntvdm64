# M0 T273 S1 Bochs provenance and recovery audit

## Baseline and procedure

The only admitted baseline is the local Bochs 2.6 source tree:
`O:\repos.external\bochs-2.6-compat\bochs-2.6`. The recovery candidate is
the quarantined `src.old/bochs-core`, used read-only. For every non-README
candidate, the audit compared the identical relative path and SHA-256 bytes
against that baseline.

The candidate contains 205 upstream-relative files: **177 exact** and **28
different**; zero has no upstream counterpart. The baseline has 672 files, so
this is an audited minimal closure candidate, not a claim to mirror all of
Bochs. The independently recorded 2026-08-25 ledger confirms the same 177/28
result and that no retained delta exceeds the 50-percent mirror limit:
[`m0-t270-s1-p2-bochs-mirror-disposition-ledger-001.md`](../evidence/m0-t270-s1-p2-bochs-mirror-disposition-ledger-001.md).

## Difference inventory and disposition

- **Root/build subset:** `bochs.h`, `pc_system.cc`.
- **CPU boundary/profile/observation:** `cpu/access.cc`, `access32.cc`,
  `cpu.cc`, `cpu.h`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`, `exception.cc`,
  `icache.cc`, `init.cc`, `instr.h`, `io.cc`, `soft_int.cc`, `stack.cc`, and
  `string.cc`.
- **Toolchain/header spelling:** `cpu/i387.h`, `gui/paramtree.cc`, and
  `gui/siminterface.h`.
- **Native device subset/boundary:** `iodev/devices.cc`, `iodev/iodev.h`,
  `iodev/keyboard.cc`, `iodev/keyboard.h`, `iodev/pic.cc`, and `iodev/pic.h`.
- **Memory subset/boundary:** `memory/memory.cc`, `memory/memory.h`, and
  `memory/misc_mem.cc`.

All new executable bodies previously associated with those differences are
classified as private overlay candidates. The future `bochs-core-overlay`
implementation is compiled only as part of `bochs-core`, has no public ABI,
and may be called only from registered minimal boundaries in its parent
mirror. The relevant predecessor audits are
[`m0-t265-s4-bochs-core-overlay-audit-001.md`](../evidence/m0-t265-s4-bochs-core-overlay-audit-001.md)
and the T270 ledger above; their identifiers are evidence, not an automatic
admission of old code.

## Adapter recovery inventory

`src.old/adapter-bochs` contains 11 project-authored candidates:
`minimal_sim`, `minimal_pic`, `minimal_machine`, `minimal_product_shell`,
`machine_facade`, and `headless_8042`, each with its header where applicable.
The prior adapter provenance ledger confirms that no imported Bochs body
remains there; it is assembly-only:
[`m0-t265-s3-adapter-bochs-provenance-ledger-001.md`](../evidence/m0-t265-s3-adapter-bochs-provenance-ledger-001.md).
Their later admission requires a fresh dependency scan and vocabulary review.

## S1 conclusion

No candidate is copied in S1. This preserves the source-first ladder and
prevents `src.old/` from becoming an implicit build input. S2 may copy only
reviewed Bochs mechanical lifecycle candidates after defining the minimal
source set and build closure; it must not pull in product-shell, provider or
OpenNT semantics.
