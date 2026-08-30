# M0 T313 S2 P7 — Original CCPU40 lifecycle witness

## Decision

The selected source-shaped lifecycle has one original executor.  The current
Win32/x86 CCPU40 path is:

1. `softpc.new/obj.vdm/ntvdm.c` performs the original process entry sequence
   (`TimerInit`, `CpuEnvInit`, `nls_init`, then `host_main`).
2. `softpc.new/base/support/main.c::host_main` keeps the original ordering:
   host/application setup, `gfi_init`, virtual-driver initialization,
   `config`, `cmos_pickup`, `cpu_init`, then `InitialiseDosEmulation` and
   `host_start_cpu`.
3. Under the selected `CPU_40_STYLE + CCPU` profile, `cpu_init` selects the
   original `ccpu386/c_main.c::c_cpu_init`, which creates its original thread
   state and invokes `c_cpu_reset`.
4. `softpc.new/host/src/nt_msscs.c::InitialiseDosEmulation` performs original
   `io_init`, allocates its direct-error TLS, invokes the original BIOS
   `reset`, installs VDD/LIM state and enters its original SCS/DOS-image
   initialization sequence.
5. The original `softpc.new/base/bios/reset.c::reset` performs the selected
   BIOS reset ordering, including NPX/ICA setup and the NT host reset/q-event
   tail.  Those controller endpoint effects remain S3/S4 behavior work; they
   are not replaced by a second executor or a project-authored reset body.

No Bochs, CPU30, MONITOR, V86, `src.old`, or alternative emulator is an input
to this chain.

## Current bounded witness

`tools/build/New-T313CcpuLifecycleNinja.ps1` is the current, narrowly scoped
T313 build generator.  It is not the retired T310 script and selects:

- the original CCPU40 manifest (excluding only the mutually exclusive
  `ntstubs.c` FPU-stub carrier);
- original `nt_cprgs.c`, `nt_cpu.c`, `sim32.c`, and `nt_mem.c` host roots;
- the existing production execution/mapping/session adapters;
- the existing `tests/mvdm-host/ccpu_bounded_execution_fixture.*` witness.

Its only added test seam is the asserted-unreached original process entry
symbol needed because the execution adapter exports both the full process
entry and bounded `host_start_cpu` routes.  The fixture calls the latter; it
does not replace, select, or execute `obj.vdm/ntvdm.c`.

Fresh external x86 run from `build/M0-T313/S2/ccpu-lifecycle/r1` succeeded:

```text
ccpu386ThrdExptnPtr id:0x7a50 called with Bad Id
sas-init
cpu-init
access-init
[load_sw_cpu_access_functions] init READ/WRITE functions.
seed
start
returned-start
reenter
returned-recursive
```

The witness establishes the original SAS vector, CCPU initialization,
`host_start_cpu`, recursive original `host_simulate`, FPU execution and the
bounded original `D6 FE` return.  It stops before ROM media, BIOS BOP,
controller I/O, or SCS/DOS image loading.  It is therefore behavior evidence
for the CPU/SAS execution boundary, not a claim that a DOS boot completed.

## Fresh full formal selection

The current full selected CCPU40 candidate was generated and built externally
at `build/M0-T313/S2/r3`:

```text
ninja -C build/M0-T313/S2/r3 original-softpc-candidate
403/403 completed
```

The formal candidate includes the original BIOS, CCPU, host-root and selected
machine package archives.  Historic warnings remain visible; none was
suppressed or rewritten as part of this witness.

## Cleanup result

No non-original production form was found in the reached `mvdm-host` CPU/SAS
path during this work.  The new entry seam is test-only and cannot enter a
product library.  Reached original lifecycle divergences remain registered as
`MVDM-HOST-DIV-038`, `039`, `083`, and `084`; this P does not add a product
divergence.

## Consequence

S2 has source-order evidence and a bounded original CCPU40 behavior witness.
S3 owns controller/timer/input behavior; S4 owns video/storage/communications
behavior; S5 owns end-to-end finite lifecycle/teardown.  No later S is being
used to conceal an alternative CPU or reset implementation.
