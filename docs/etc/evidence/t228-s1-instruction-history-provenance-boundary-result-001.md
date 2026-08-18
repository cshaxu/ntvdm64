# T228 S1 Instruction-History Provenance Boundary Result

## Inputs

- Source: `src/bx-mantle/bx_ntvdm_instruction_history.cc`
- Fixture harness: `tools/probe/Invoke-T197S6MinimalMachineLinkProbe.ps1`, invoked with `-WholeCpu5Core -InstructionHistory -InstructionHistoryBoundaryFixture -RunLifecycle -HostArchitecture x64`.
- Toolchain/profile: MSVC x64 `/MT`, projected CPU5/Pentium-MMX, non-x86-64.
- Build roots:
  - `build/M0-T228-S1/history-base-20260818g`
  - `build/M0-T228-S1/history-provenance-20260818a`

## Procedure And Result

Both full CPU5 closures compiled, linked, and ran with recorded outer exit code zero.

| Configuration | Compile definitions beyond projected config | Verified result |
| --- | --- | --- |
| Base history | `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY=1` | Ring retained two copied positions; scalar CS transition returned exact previous/current records; provenance query returned unavailable before and after reset. |
| Explicit provenance | Base definition plus `BX_NTVDM_ENABLE_MANTLE_INSTRUCTION_HISTORY_PROVENANCE=1` | Same scalar assertions passed; the sole enabled provenance path copied and returned the known 15-byte predecessor pattern and 10-byte stack pattern through existing ordinary-RAM reads. |

The fixture uses a fixed external #UD bridge whose only result is decline so the pinned configuration can link its external bridge contract. It performs no #UD dispatch during this test and has no production membership.

## Interpretation

The prior implementation made an ordinary CS transition read guest RAM whenever history was enabled. The repaired implementation gates the memory header, real-address helper, provenance storage, and both `copy_from_ordinary_ram` calls behind the explicit provenance definition. Thus `BX-CORE-071`, `BX-MANTLE-072`, and `BX-CORE-074` retain their scalar-only contract; only the already registered `BX-MANTLE-075` path can copy guest RAM.

## Limitations

This is a mechanical diagnostic-boundary result, not a guest trace result and not a BOP, DOS, OpenNT-provider, CPU semantic, or machine-feature change. The separate untracked T225 S58 candidate remains outside this delivery.

## Default Formal Build

A fresh graph generated at `build/M0-T228-S1/formal-ninja-20260818b` from `tools/build/t225-s7-full-module-manifest.json` completed `ninja -C ... all` with exit code zero: 433 of 433 compile, archive, and link edges succeeded, including the default `bx-mantle` library, all local BOP owner-package fixtures, and `ntdos64-native.exe`. This default graph does not enable either history macro, so no provenance object path is activated.
