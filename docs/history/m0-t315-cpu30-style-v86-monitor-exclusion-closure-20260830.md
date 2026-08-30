# M0 T315 closure — `CPU_30_STYLE` V86 monitor exclusion

## Result

Closed as a source-contract correction, not as a runnable profile recovery.

The owner confirmed that this product does not have and will not reconstruct
the NT4 kernel V86 execution facility.  Original `CPU_30_STYLE` therefore
cannot be a production machine configuration.

## Evidence

[T315 S1 source-contract audit](../etc/evidence/m0-t315-s1-cpu30-style-v86-monitor-contract-audit-001.md)
establishes that `base/mvdm/v86/monitor/i386/monitor.c` implements
`cpu_simulate()` by calling `NtVdmControl(VdmStartExecution, NULL)` for V86
execution, while `int.c` calls `VdmQueueInterrupt` and `fastpm.asm` performs
the monitor context transition.  This package is not a second C-language
CCPU interpreter.

## Product disposition

- Production compilation, linking, runtime and acceptance select only the
  original SoftPC `CPU_40_STYLE` + CCPU40 path.
- `CPU_30_STYLE`/`v86/monitor` remains historical source evidence only.
- No CCPU40 fallback is used to claim `CPU_30_STYLE` behavior.
- The next candidate is the ordered monitor/`NtVdmControl` user-mode contract
  package, restricted to source-shaped public/user-mode recoverable forms; it
  may not recreate kernel VDM execution.

## Verification

The source audit cross-checked the original OpenNT and OpenNT-src-2 monitor
trees.  Current build scripts already select `CPU_40_STYLE` and forbid the
CPU30/monitor/V86 sources.  Documentation governance and `git diff --check`
pass for this closure revision.
