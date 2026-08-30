# M0 T310 S9 — CCPU access-table contract review

## Scope

This record closes only `SPC-CCPU-ACCESS-TABLE`. It does not claim a CPU40
profile table, a hardware-CPU table, SAS monitor-vector closure, or a complete
guest machine.

## Selected original profile

The formal graph compiles original `nt_cprgs.c` with `CCPU` and
`CPU_30_STYLE`. The selected `load_sw_cpu_access_functions` body is therefore
the original `CPU_30_STYLE + CCPU` body: it assigns the common CCPU register,
flag and segment access slots and assigns `host_simulate_func` to
`c_cpu_simulate`.

The source's CPU40-style CCPU assignment body, A3/A2 alternatives and
`load_hw_cpu_access_functions` remain original source evidence but are not
selected in this product profile. S9 neither initializes them speculatively
nor supplies a substitute implementation.

## Contract disposition

- The generated `cpu4gen.h` declarations are the source authority for the
  selected CCPU providers: `IU8` for byte registers, `IU16` for word registers
  and MSW, `ISM32` for flag and segment-result carriers, `IBOOL` for flag
  setters, and `void` for normal setters and simulation entry.
- `host_cpu.h` and the global storage in `nt_cprgs.c` now expose those same
  selected CCPU function-pointer contracts. This is `MVDM-HOST-DIV-102`: a
  binding-only mirror correction, not a new table, adapter or semantic change.
- The original names, slots, assignment order and callers remain unchanged.
  Callers in BIOS, BOP, video and CCPU/SAS continue to use the original
  `getXX`, `setXX` and `host_simulate` access surface.
- `host_simulate_func` is a source-local table slot assigned to original
  `c_cpu_simulate`; its wider machine/stop semantics were separately closed by
  S8. No selector, BOP, DOS, Win32 or mapping-manager logic is inserted into
  this scalar access table.

## Focused behavior evidence

The existing bounded original-CCPU program verifies after
`load_sw_cpu_access_functions` that the selected AX, CF and simulation slots
refer to `c_reg.c`/`c_cpu_simulate`, then performs AX and CF updates through
the table before running its bounded instruction sequence. Its expected
completion remains the original `D6 FE` CCPU exit path. The x64 and x86 formal
build/run result is appended only after both fresh Ninja executions complete.

## Results

- Fresh MSVC `/MT` Ninja graph and bounded execution passed for x64.
- Fresh MSVC `/MT` Ninja graph and bounded execution passed for x86.
- Both executions observed original access-table initialization and completed
  their bounded `D6 FE` return path after table-mediated AX and CF checks.
- An incremental full-candidate forced-link retry reached an unrelated
  `base/support/time_day.c` compiler stall. It was terminated by exact process
  identity. S8's already-closed full-candidate forced-link evidence remains
  the composition baseline; S9 does not treat this later-owner stall as an
  access-table failure or modify it locally.
