# M0 T312 S2 — Original SoftPC binding transfer

Date: 2026-08-30  
Status: in progress

## Completed transfer

- `app/machine_shell.c` no longer constructs an alternate machine.  A selected
  session is SoftPC-only and invokes the original `host_cpu_init` and
  `host_cpu_reset` shapes defined by `softpc.new/host/src/nt_cpu.c`.
- The selected DEM/COMMAND `nt_bop` overlay now uses the original CCPU40
  `host_cpu.h` register vector and `sas.h` SAS interface.  It no longer uses
  the former self-authored register or SAS facade.
- `mvdm-host` and `mvdm-host-overlay` have no retired-machine include or
  terminology dependency.

## Remaining S2 disposition

The following files are not part of the selected formal CCPU40 graph.  They
are earlier retired-machine recovery experiments, not a second SoftPC
implementation.  S2 will either replace them with their named original owner
or move them out of production sources before S3 removes the retired tree:

- `adapter-mvdm-host-out/softpc/mvdm_command_registers.c` → original
  `nt_cprgs.c` register-vector owner.
- `mvdm_sas.c` → original CCPU SAS (`ccpusas4.c` plus the source-shaped
  CCPU-facing form); no self-authored RAM facade remains selected.
- `mvdm_vdm_stack.c`, `mvdm_realmode_stack.c`,
  `mvdm_protected_frame_transaction.c`, and `mvdm_protected_span.c` →
  respective original monitor/DPMI32 contract, or explicit unavailable
  disposition pending that owner package.

This is intentionally not a runtime-completion claim.  The next S2 step is
to remove the remaining direct retired-machine includes from production
sources, with an owner/disposition record for every removed experiment.
