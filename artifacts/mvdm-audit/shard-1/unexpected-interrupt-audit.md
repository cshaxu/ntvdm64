# Unexpected interrupt endpoint: shard 1

Frozen `softpc.new/base/system/unexp_nt.c` is byte-identical to OpenNT and
OpenNT45; selected worker build compiles `obj/system/unexp_nt.obj` into
`original-softpc-system.lib` and map `_unexpected_int` points to that object.
Map SHA-256 is `641E5C33430445ECC73885176250194BE936549654E14864074BD0B7D92D3CE2`;
worker image SHA-256 is `D3437091D920A74FF72FDC8ED023C6207642682A7739C45751E4F03A8C1D53E5`.

Original `base/bios/bios.c` maps BOP 02 to `unexpected_int`; `illegalp.c`
can also invoke it in illegal-op handling. It reads the master PIC ISR: zero
records software/unexpected cause as FF; otherwise it masks the relevant
master hardware ISR bits while preserving cascade IRQ2, reads and masks a
nonzero slave ISR and EOIs the slave, then masks and EOIs the master. It stores
the resulting cause at BIOS data area offset 6B. This is failure handling,
not an ordinary guest task termination API; actual behavior still depends on
PIC port dispatch and guest/ROM fault routing.

`docs/etc/evidence/m0-t420-s24-system-capability-baseline.md` explains this
source contract and explicitly says its other console and EDIT/MEM regressions
do not exercise this exceptional endpoint. The
`tests/component-integration/fixtures/machine_composition_test.c` file defines
its own simplified `unexpected_int` test double, so its passing test cannot
establish the original provider's hardware/software or cascade behavior.
No new host/guest fault injection was run, and no change or omission has been
proved merely from source identity. Keep this entry pending focused testing of
software case, master and slave interrupts, mask/EOI ordering, and final BDA
state against actual selected PIC provider; isolate destructive guest actions.
