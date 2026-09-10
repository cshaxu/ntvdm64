# OpenNT EMS Profile Gap

Status: direct source and object evidence, 2026-08-06. This is a
research-only record. It does not authorize an EMS implementation, a memory
alias, CCPU execution, or a runtime link.

## Question

The fixed NT4 CCPU source imports `LIM_b_write` and `LIM_w_write` when
writing a `SAS_MM_LIM` physical-memory page. Does an available historical
EMS source provide those imports for the selected `NTVDM + CCPU` profile?

## Source Results

The fixed NT4 input has:

- declarations in `base/mvdm/softpc.new/base/inc/emm.h`;
- the NT host lower layer in `host/src/nt_emm.c`;
- BIOS EMS entry handling in `base/bios/emm_fncs.c`;
- CCPU calls guarded by `#ifdef LIM` in `ccpu386/ccpusas4.c`.

It does not contain the original middle-layer source. The local NT4.5 source
snapshot contains it as the sole member of its historical `base/dos` library:

```text
base/mvdm/softpc.new/base/dos/sources: SOURCES=emm_mngr.c
```

The exact NT4.5 source is staged without modification under
`refs/opennt-45/base/mvdm/softpc.new/base/dos/emm_mngr.c`. Its SHA-256 is
`FD73CE7181F7DFB61432D63466C7641C3D010CA0C60BF446666B6C0118DDB8A9`.
The local XP source tree independently retains a later, non-identical version,
which confirms this is a real historical EMS layer rather than a fabricated
adapter.

## Compile Candidate

`ntdos64-opennt45-emm-recovery-objects` is a default-disabled compile-only
target. It selects a local compiler-compatibility overlay rather than the
original file. The overlay changes exactly one declaration:

```text
deallocate_altreg_set(short set)
  -> deallocate_altreg_set(unsigned short set)
```

This matches the unchanged historical `emm.h` prototype. Both types are
16-bit in the x86 target, so this is an ABI-preserving compiler repair, not an
EMS semantic change. The original source remains adjacent as provenance; both
hashes and the one-line change are in `refs/opennt-45/SOURCE-MANIFEST.json`.

The target builds successfully with the isolated x86 clang-cl gate.

## Critical Profile Result

The recovered `emm_mngr.c` defines `LIM_b_write` and `LIM_w_write` only
inside `#ifndef NTVDM`. Therefore the same file, compiled under the selected
`NTVDM + CCPU + LIM` profile, does **not** export either symbol. COFF
inspection instead shows ordinary EMS-manager exports such as
`init_expanded_memory`, `free_expanded_memory`, and
`lim_page_frame_init`.

Meanwhile CCPU's `phy_w8` and `phy_w16` call those two LIM write functions
when both `LIM` is enabled and a page is classified `SAS_MM_LIM`. The
historical NT host headers define `LIM` for the selected NTVDM profile.

This is a profile discontinuity:

```text
NTVDM + CCPU physical write
  -> LIM_b_write / LIM_w_write
  -> no provider in the recovered NTVDM EMS-manager compilation branch
```

It cannot be resolved by linking the NT4.5 source file, ordinary RAM, or a
success-returning stub. A valid next step requires either an original CCPU-era
NTVDM LIM-write provider with matching profile evidence, or a deliberate
decision that the CCPU profile cannot execute EMS-mapped writes. Until then,
the full CCPU link diagnostic remains correctly blocked.

## Boundary Consequence

The EMS finding reinforces the machine-boundary conclusion. EMS is neither a
CPU intrinsic nor a DOS SVC; it is a memory-mapping/device policy layer with
host allocation and page-alias semantics. A future neutral core capability may
support an explicitly configured expanded-memory mapper, but it must not
expose or emulate these historical CCPU private LIM callbacks as a public
contract.
