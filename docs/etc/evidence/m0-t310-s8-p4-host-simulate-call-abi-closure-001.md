# M0 T310 S8 P4 — `host_simulate` call-ABI closure

## Source finding

The selected non-MONITOR profile makes `CpuH` name `cpu4.h`.  Unlike the
historical monitor `cpu.h` selection, that header does not publish the common
original `host_simulate` declaration.  Reached keyboard and mouse sources call
`host_simulate()` after including `CpuH`, so modern C inferred an `int` result
and emitted C4013 on both host architectures.

The selected source body is already supplied by the original SoftPC/CCPU path.
This was a missing declaration boundary, not a missing CPU implementation and
not a reason to add a controller shim.

## Recovery

`softpc.new/host/inc/host_def.h` now publishes the standard-C equivalent of
the original no-argument void form:

```c
extern void host_simulate(void);
```

It is registered as `MVDM-HOST-DIV-057`.  The declaration is intentionally in
the original common host carrier because it restores every non-MONITOR
controller caller consistently.  No call order, implementation, guest frame,
mapping-manager identity or controller state changed.

## Verification

The complete selected original SoftPC candidate rebuilt successfully on both
architectures:

- `build/M0-T310/S8/p1-machine-source/x64/host-simulate-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/host-simulate-x86-build.log`

Neither log contains a `host_simulate` diagnostic.  Other warning families
remain visible and are outside this exact ABI closure.
