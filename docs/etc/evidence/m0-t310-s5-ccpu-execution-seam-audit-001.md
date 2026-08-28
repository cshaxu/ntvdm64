# M0 T310 S5 — original CCPU execution seam audit

## Purpose

Record the first source-shaped original `i386 + CCPU` execution workset before
claiming a runnable SoftPC backend.  The workset is deliberately independent
of Bochs, MONITOR, V86 and BOP-provider dispatch.

## Selected original path

1. `softpc.new/base/ccpu386/ccpusas4.c`: `sas_init` establishes original SAS
   state and obtains the original host backing store.
2. `softpc.new/base/ccpu386/c_main.c`: `c_cpu_init` establishes CCPU state;
   `c_cpu_simulate` enters the original executor through its original TLS
   simulation frame in `ntthread.c`.
3. `softpc.new/host/src/nt_cprgs.c`'s original
   `load_sw_cpu_access_functions` installs `c_cpu_simulate` in the historical
   `host_simulate_func` slot.  The fixture verifies that selection.
4. `softpc.new/host/src/nt_cpu.c`'s original `host_start_cpu` is the selected
   source-shaped host entry.  The fixture calls this wrapper rather than
   `c_cpu_simulate` directly.
5. The fixture writes `D6 FE` at the reset location using the original
   `c_sas_store` surface.  `c_main.c` decodes that historical direct CCPU
   exit, advances IP, calls `c_cpu_unsimulate`, and returns by the original
   simulation frame.  It does not enter BIOS BOP dispatch.
6. `sas_term` invokes original teardown.

The test-only seam carrier supplies only symbols proven unreachable for this
two-byte path; it is not linked into a product library and is not a runtime
provider.

## x86/x64 address correction

`IHPE` is documented by the original `insignia.h` as an integer the same size
as `IHP`, but was implemented as `unsigned int`.  Original `nt_mem.c` uses it
to retain addresses of its own backing allocation/header storage.  This
truncates x64 addresses during `InitIntelMemory`.

The mirror retains the spelling and changes `IHPE` to public Win32 `UINT_PTR`
under `MVDM-HOST-DIV-025`.  The matching CCPU definition of
`Length_of_M_area` now uses its existing header type under
`MVDM-HOST-DIV-026`.  Neither value crosses a component or guest ABI, so a
session mapping-manager identity would be semantically wrong; mapping leases
remain mandatory only when a historical native identity crosses such a
boundary.

Four additional, narrow source-preserving corrections were required for the
same execution interval:

- `MVDM-HOST-DIV-027` adds the existing `UTINY *host_sas_init(sys_addr)`
  declaration to `ccpusas4.c`.  The historical implicit declaration makes a
  pointer return an `int` in modern x64 C and corrupts the original backing
  address before CCPU begins.
- `MVDM-HOST-DIV-028` retains `nt_mem.c`'s internal allocation-header page
  calculations in pointer-sized `IHPE` temporaries.  It does not change the
  original shift/range/commit algorithm, and these values are neither guest
  addresses nor external identities.
- `MVDM-HOST-DIV-029` includes `<stdlib.h>` in `ckmalloc.h` so the existing
  `host_malloc` macro has its modern declaration.  This prevents the original
  FPU-stack allocation result from being implicitly narrowed to `int` on x64.

These corrections deliberately do **not** use the session mapping manager:
they repair transient native addresses wholly inside the original SoftPC
allocator/FPU implementation.  A mapping identity is required only when a
host object must cross a guest or component boundary.

## Build selection

`New-T310CcpuExecutionNinja.ps1` forms the dedicated formal graph for each
host architecture.  It selects the exact original CCPU manifest and direct
S5 host roots `nt_cprgs.c`, `nt_cpu.c`, `sim32.c`, and `nt_mem.c`, with `SIM32`
enabled.
It preserves all mirror sources.  The focused link selects original `fpu.c`
and excludes the alternate `ntstubs.c` carrier because both provide
`initialise_npx`/`npx_reset`; this is a static-link selection, not deletion or
source alteration.

## Explicit exclusions

- The fixture does not enter `nt_cpu.c`'s recursive `host_simulate` wrapper:
  its lock/TEB assertion and recursive BIOS semantics require later product
  composition. `host_start_cpu` alone is selected for this bounded host-entry
  proof.
- No `MONITOR`, V86, kernel VDM or Bochs source/type/global is a graph input.
- `fmstubs.c` retains its `__debugbreak` default and is not selected.

## Formal result

The generated Ninja graph completed and the exact same focused program
returned zero on both supported host architectures:

```text
sas-init
cpu-init
access-init
[load_sw_cpu_access_functions] init READ/WRITE functions.
seed
start
returned
fixture_exit=0
```

The checked postcondition is original `c_getIP() == 0xfff2`: the original
`nt_cprgs.c` selector was installed, original `nt_cpu.c` entered CCPU, and
CCPU consumed the two seeded bytes at
`0xffff0`, advanced its original IP, and returned via the original
`c_cpu_unsimulate` frame.  The formally generated graphs were run from
`build/M0-T310/S5/ccpu/x86` and a fresh
`build/M0-T310/S5/ccpu/x64-clean` root.  Both graphs select the original CCPU
and stated host roots only; neither takes a MONITOR, V86, Bochs or `src.old`
input.

This is a **bounded original-executor proof**, not a product boot claim.  The
fixture seeds `D6 FE`, whose original direct-unsimulate path returns before
BIOS BOP dispatch, device I/O, interrupt delivery, ROM media or BOP-provider
execution.  `rom_init`, `copyROM`, `host_simulate` and the other declared
forms in `ccpu_bounded_execution_fixture_seams.c` are test-only link seams,
except original `nt_cpu.c::host_simulate`, which is linked but not reached by
the direct branch. None of the test-only forms enters a product library.

## Remaining S5 work

- Select and prove the source-shaped recursive `host_simulate` lifecycle
  without the test-only `host_simulate` seam.
- Classify the remaining, unexercised `nt_mem.c` pointer-conversion paths
  (`VdmAllocate*`, `VdmCommit*`, `VdmDecommit*`, physical-record setup and
  related callers).  They are not covered by this fixture and may require a
  named adapter/mapping boundary rather than another native-width correction.
- Keep `nt_cpu.c::InitNtCpuInfo` outside the current runtime claim: its
  unexercised GDP/host-pointer reconstruction has x64 truncation diagnostics
  and belongs to a separate source-first adapter/overlay disposition, not the
  `host_start_cpu` wrapper proven here.
- Produce the required reached V86/MONITOR/scaffold disposition audit and
  connect a typed product stop/teardown result.  The direct CCPU return above
  is an executor return, not yet the session's typed stop contract.

## Evidence state

The previous x64 probe reached original `sas_init` and faulted in original
native-address width assumptions.  The five registered mirror divergences
above remove those blockers for this bounded interval, and formal x86/x64
build-and-run evidence is now complete.  S5 itself remains active because the
product lifecycle, typed session result and removal audit have not yet been
proved.
