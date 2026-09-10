# M0 T336 S2 — CPU40 recursive-vector recovery

## Question

Which exact original CCPU40 edge produced the post-`54:05` null instruction
pointer, and can its smallest source-shaped initialization cohort be restored
without changing a BOP, guest image, loader, or executor?

## Inputs and procedure

- The corrected T336 S1 fixed-container report recorded `0xC0000005`,
  `IP=0`, and native return address `0x00e08ef8`.
- The x86 product map and disassembly resolve that return to the instruction
  immediately after `call dword ptr [_host_simulate_func]`.
- Original `softpc.new/base/disks/floppy.c:wait_int` saves the original CPU
  state, installs the INT 15h device-busy request at `int15_seg:int15_off`,
  then invokes `host_simulate()`.
- Original `softpc.new/host/src/nt_cprgs.c` owns
  `load_sw_cpu_access_functions()`, which installs the generated CCPU access
  vector and sets `host_simulate_func = c_cpu_simulate`.
- The historic source gates that body with `CPU_30_STYLE`.  The selected
  CPU40-only build already selects that same CCPU body through DIV-151, but
  lacked the former product-shell startup call.

The recovery adds one CPU40-only call to that existing original initializer,
immediately after the original `cpu_init()` in `base/support/main.c`.  It is
registered as `MVDM-HOST-DIV-182`; it adds no BOP route, loader, guest-media
change, second executor, adapter algorithm, or synthetic completion.

## Verification

- Fresh formal Ninja x86 compile/link: 419 actions, including
  `original-softpc-process.exe`.
- Fresh formal Ninja x64 compile/link: 419 actions, including
  `original-softpc-process.exe`.  This is compile/link evidence only.
- One fixed x86 console-owning, non-debug observation used immutable staged
  DOS/firmware media and the same ordinary-child arguments as S1.  It reached
  the existing original BOP records `50:11`, `50:3B`, `50:0F`, `50:1B`, and
  `54:05`, then remained live until the observer's fixed 8-second timeout.
  It produced no null-target exception report.

## Result and limit

The original null call is attributed to the uninitialized
`host_simulate_func` vector, not to guest loading, COMMAND dispatch, or the
NTDOS stack transition.  The CCPU40 recursive executor now remains installed
when original floppy `wait_int` requests its INT 15h re-entry.

The timeout proves the process no longer terminates at that null vector.  It
does not prove an FDC interrupt completed, a later BIOS/INT15 return occurred,
that NTDOS completed startup, or that DOS program execution works.  The next
owner cohort is the original FDC/INT15/ICA/heartbeat completion path and must
be admitted from source before another observation.
