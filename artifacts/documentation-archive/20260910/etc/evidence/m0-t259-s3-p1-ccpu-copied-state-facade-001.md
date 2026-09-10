# M0 T259 S3 P1 — CCPU Copied-State Facade Recovery

## Question

Can the reached OpenNT CCPU accessor contract be recovered without adding a
second CPU frame subsystem, exposing a Bochs object, or changing an imported
DEM body?

## Original contract

- `src/opennt/base/mvdm/softpc.new/host/src/nt_aorc.c` maps `get*` and `set*`
  spellings to the selected CCPU register implementation.  Its callers may
  observe a register or flag immediately after updating it.
- `nt_cprgs.c` publishes the CCPU function-table form.  It is not a license
  for another CPU implementation; the relevant observable property here is a
  synchronous, mutable call-local register image.
- The imported `demdasd.c` saves registers, stages an INT 13 CS:IP pair,
  calls `host_simulate`, tests `getCF()`, and restores its saved image.

## Recovered facade

`demhndl_shim` already owns one call-private `bx_ntvdm_cpu_state_v1` copy and
the existing typed completion delta.  P1 extends that existing facade rather
than creating a new CCPU frame:

- adds historical-shape `CS`, `IP`, and `CF` getters plus scoped `CS`/`IP`
  setters;
- makes `setCF` and `setZF` update both the immediate copied image and the
  eventual typed result;
- keeps staged CS:IP local to the imported source call.  A normal BOP result
  still owns its outer resume RIP and cannot silently replace it.

`demdasd_ioctl_shim` deliberately rebinds only its private `CS:IP` staging
spellings after including the generic shim.  This avoids a macro collision
while preserving its original recursive-BIOS call-site form.  `ES` and flags
remain shared call-local CCPU state.

## Explicit remainder

This does **not** claim that `host_simulate()` is recovered.  Bochs invokes
the external BOP bridge from its #UD exception path and the bridge returns by
`longjmp` to the active CPU loop.  Directly calling a nested CPU loop from an
imported C body would overwrite that lifecycle and cannot safely preserve the
source call stack.  The next admissible work is a source-proven,
selector-blind suspend/continuation mechanical contract; it must retain the
OpenNT call shape but may not recreate CCPU or make bx-core/bx-mantle aware
of DEM, BOP, DOS, SoftPC, or CCPU.

## Verification

Fresh external formal Ninja graph:

```text
build/M0-T259-S3/formal-r5
```

The 286-object graph linked and executed:

```text
T230 S7 direct OpenNT DASD/IOCTL import: CCPU accessor, IOCTL, boundary and raw-drive failure contracts verified
```

The fixture proves `CS:IP:ES`, CF and ZF immediate copied-state behavior;
then verifies its typed ES/flags completion result and retained DEM IOCTL and
raw-drive failure paths.  It does not run a nested CPU loop.
