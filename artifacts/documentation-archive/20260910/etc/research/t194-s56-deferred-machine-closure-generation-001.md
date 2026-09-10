# M0 T194 S56: Deferred Native-POST Machine Closure Generation

## Question

Can the existing generator express the S55-required combination of native
POST/deferred handoff and the original selector-06 machine island without a
new Bochs or adapter semantic change?

## Procedure

One generator invocation created the fresh, uncompiled root
`artifacts/build/current/t194-s56-deferred-machine-metadata-closure-r1`:

```text
New-T98S1CurrentAdapterEngineDerivative.ps1 -DeferredStartupPlan
  -MachineComposition
```

## Observation

The derivative record SHA-256 is
`538EC4D82C00FE182AE8BC7FC117692D8B2F45626DE2ADA1AD082A93819ED9C1`; the
generated makefile SHA-256 is
`68F8FE832E3BB01CCFB7CF2103833340D290DA446A87F5C0AF319D6B6F54F98E`.

Its record states `deferredStartupPlan: true`, `machineComposition: true`,
68 adapter sources, five CLI sources and two rebuilt Bochs objects (`main.o`
and `cpu/exception.o`). The makefile sets both deferred-startup and
machine-composition macros. Its final target adds only the existing machine
triple:

- `machine/bx_ntvdm_machine_bop_v1.obj`;
- `machine/unexp_nt.c.obj`;
- `machine/illegalp.c.obj`.

There is no legacy IOCTL source, host-namespace source, added archive
prerequisite, device target, compiler, linker or executable invocation.

## Disposition

The S55 composition requirement is expressible by the existing generator and
existing original-object island. A separate S must inspect the pre-existing
machine object inputs and build this one closure once; this generation makes
no runtime or selector-06 handling claim.
