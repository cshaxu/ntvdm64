# M0 T310 S5 P2 — CCPU profile path disposition

## Scope and method

This is a static disposition audit of the exact 133 source files selected by
the formal S5 x64 CCPU graph: 129 entries from the original
`softpc.new/base/ccpu386/sources` manifest and four original host roots
(`nt_cprgs.c`, `nt_cpu.c`, `sim32.c`, `nt_mem.c`).  The x86 graph has the same
source selection.

The audit distinguishes a historical **kernel/MONITOR V86 product path** from
the original CCPU interpreter's own guest virtual-8086 semantics.  The latter
is part of the selected pure-software CPU owner and is not permission to
reintroduce a kernel VDM or MONITOR executor.

## Physical input result

- `src.old`: zero selected source references; forbidden by the generated
  graph.
- `bochs-core` and `adapter-bochs`: zero selected source references; neither
  is a generated graph input.
- `kernel VDM` and `monitor.lib`: zero selected source references and zero
  generated graph inputs.
- `MONITOR`: five lexical occurrences, only as original conditional
  compilation in `nt_cpu.c` and `sim32.c`. The selected graph defines CCPU
  and never defines MONITOR, so it selects the original CCPU side of those
  conditions.
- `V86`: 38 lexical occurrences. They occur in original CCPU instruction,
  address/SAS and exception logic. They describe the emulated guest CPU's
  virtual-8086 behavior or comments; they are not a call, import, library or
  kernel-VDM implementation input.

## Reached-path disposition

| Form/path | Disposition | Reason |
| --- | --- | --- |
| `nt_cprgs.c::load_sw_cpu_access_functions` | still-required-with-owner | It installs the original CCPU access/entry functions selected by the S5 fixture. |
| `nt_cpu.c::{host_start_cpu,host_simulate}` | still-required-with-owner | Both original wrappers now execute the bounded CCPU return interval on x86/x64. Their BIOS recursive callers are not yet exercised. |
| `c_main.c::{c_cpu_simulate,c_cpu_unsimulate}` and `ntthread.c` | still-required-with-owner | They are the selected original execution/return frame mechanism. |
| CCPU guest-V86 terms in `call.c`, `ccpusas4.c`, instruction and exception sources | adapter-contract-retained | They remain original interpreter semantics. They must not acquire a kernel VDM/MONITOR implementation or a Bochs call. Their actual guest-mode/device coverage is S7 work. |
| `nt_cpu.c` and `sim32.c` `#ifndef MONITOR` forms | still-required-with-owner | These are original source guards which select CCPU behavior in this profile; they do not include a MONITOR executable input. |
| BIOS recursive calls from `ccpusas4.c` | still-required-with-owner | Original callers of `host_simulate`; no test seam may replace them. Their ROM/device lifecycle requires S7 source composition. |
| Test-only fixture seam carrier | tool-only relocation | It belongs only to `tests/mvdm-host`; it is not a product source, library or runtime provider. |

## Consequence

S5 P2 proves the current CCPU profile is mechanically isolated from the
historical MONITOR/kernel-VDM/Bochs product paths. It does **not** delete
original mirror source solely because it contains guest-V86 semantics, nor
does it claim that BIOS, devices or all virtual-8086 behavior have been
validated. S6 may remove only a physically selected, source-proven excluded
implementation input after its own per-path verification.
