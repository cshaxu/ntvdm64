# M0 T312 S2 — Original SoftPC binding transfer

Date: 2026-08-30  
Status: closed

## Completed transfer

- `app/machine_shell.c` no longer constructs an alternate machine.  A selected
  session is SoftPC-only and invokes the original `host_cpu_init` and
  `host_cpu_reset` shapes defined by `softpc.new/host/src/nt_cpu.c`.
- The selected DEM/COMMAND `nt_bop` overlay now uses the original CCPU40
  `host_cpu.h` register vector and `sas.h` SAS interface.  It no longer uses
  the former self-authored register or SAS facade.
- `mvdm-host` and `mvdm-host-overlay` have no retired-machine include or
  terminology dependency.

## Archived former recovery code

The following files are not part of the selected formal CCPU40 graph. They
were earlier retired-machine recovery experiments, not a second SoftPC
implementation. They are now evidence only under
`docs/etc/legacy_code/retired-bochs/`, outside every production/build/runtime
input:

- `adapter-mvdm-host-out/softpc/mvdm_command_registers.c` → original
  `nt_cprgs.c` register-vector owner.
- `mvdm_sas.c` → original CCPU SAS (`ccpusas4.c` plus the selected narrow
  source-shaped overwrite form); no self-authored RAM facade remains selected.
- `mvdm_vdm_stack.c`, `mvdm_realmode_stack.c`,
  `mvdm_protected_frame_transaction.c`, `mvdm_protected_span.c`, and
  `mvdm_wow_pointer_scope.c` → their original monitor/DPMI32/WOW owner
  packages, or an explicit unavailable disposition when that package is
  admitted.
- `wow/mvdm_wow_callback_transaction.c` → original WOW callback owner; it
  cannot remain while its former frame facade is retired.

The only remaining retired-machine mention in a production root is explanatory
text in `adapter-mvdm-host-in/generic_ud_bridge.h`; it is not an include or a
call. S3 retires that generic #UD bridge with the rest of the retired machine
tree. This is intentionally not a runtime-completion claim.

## S2 conclusion

The selected CCPU40 lifecycle, register vector, and SAS owner now replace all
live retired-machine bindings. `mvdm-host` and its overlay no longer name or
include the retired machine route. The next packet step is S3 tree/build/test
retirement, followed by one fresh formal CCPU40 graph in S4.
