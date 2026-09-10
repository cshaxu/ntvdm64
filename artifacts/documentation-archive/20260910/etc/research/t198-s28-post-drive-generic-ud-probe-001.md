# T198 S28 Post-Drive Generic UD Probe 001

## Question

Can the existing selector-blind copied-event seam record the first generic
`#UD` after the `50:0F` drive-result resume, before the retained VGA-aperture
panic?

## Inputs And Procedure

The S26 exact source-built NTIO/NTDOS CPU5 fixture was rebuilt under MSVC x64
`/MT`.  Its external test bridge was temporarily changed only to copy the
already-provided fixed-width generic event and issue the existing typed STOP
after it had seen a `50:0F` resume.  It did not read guest memory, alter
production code, add a provider, or enable a device.

## Observation

The process did not reach that probe condition.  It emitted six instances of
`LSL: not recognized in real or virtual-8086 mode`, then:

```
prefetch: getHostMemAddr vetoed direct read, pAddr=0x00000000000a2e63
vga called in trigger_timer stub. you must not have loaded the vga plugin
```

and exited nonzero through the native panic path.  Therefore the test bridge
had not observed a preceding accepted `50:0F` resume in this unbounded run.
The temporary fixture change was removed rather than retained as a failing
test.

## Source Check

`bx-core/cpu/exception.cc` constructs and calls the mantle bridge for every
`BX_UD_EXCEPTION`; it does not test `C4 C4`.  Thus the failure is not a core
selector filter.  The probe's own post-drive condition was simply never true
before the sequence reached the prefetch panic.

## Interpretation

S26's controlled `50:0F` witness and this unrestricted trace are different
paths, not a proven single continuation.  The first needed datum is now the
first generic `#UD` in the unrestricted path, with a separate flag reporting
whether a drive BOP had occurred before it.  No VGA or other device conclusion
is licensed.

## Follow-Up

Admit a test-only selector-blind first-generic-`#UD` capture.  It may inspect
only the copied event and a local "drive resume previously seen" bit, then
issue typed STOP.  Its result will finally provide the CS:EIP/window necessary
to locate the invalid `LSL` or another prior fault.
