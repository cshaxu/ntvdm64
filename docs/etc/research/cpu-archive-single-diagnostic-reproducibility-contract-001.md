# CPU archive single-diagnostic reproducibility contract 001

Any future CPU archive observation derivative must retain the T130 machine,
adapter and profile inputs and rebuild `cpu/libcpu.a` from one pinned source
set only. Its compile manifest must explicitly set the following diagnostics
to zero: IRQ13 transfer, IRET-TF, far-jump and stack-transfer; it may set only
one separately registered target diagnostic to one. The existing machine
composition seam remains independently enabled only in `exception.cc`.

The derivative must prove every CPU object originates from that same pinned
source set, record all macro definitions, retain the non-CPU Bochs archives by
hash, and link the rebuilt archive once. No direct CPU object override, CPU
archive/object mixture, device rebuild, BOP branch, adapter semantic change or
second diagnostic is permitted.

Before any runtime observation, static verification must reject an omitted
macro, a mixed archive/object link, archive recursion outside `cpu/`, or a
changed profile/ROM/input identity. This establishes a comparable diagnostic
baseline rather than a collection of one-off trace patches.
