# Shard 1: CCPU unsigned DIV decode and divide-error ownership

`softpc.new/base/ccpu386/div.c` is byte-identical to both frozen original sources and is selected by the current x86 worker build. The observer binds `_DIV8`, `_DIV16` and `_DIV32` object bytes to the released NTVDM code. It also binds the selected `_ccpu` decoder and resolves its three function-address relocations; `c_main.c:4148–4155,4235–4245` selects DIV8 for F6 /6 and DIV16 or DIV32 for F7 /6 according to operand size. These are image/source facts, not a guest-execution result.

All three leaf functions test zero divisor and quotient overflow before writing guest quotient/remainder registers; DIV32 delegates unsigned 64-by-32 arithmetic to `divu64`. For both fault checks in each leaf, the observer verifies the actual relative call to `_Int0` in the released image. The selected `Int0` object body also binds to release and contains a resolved call to `_c_cpu_continue`. Current `c_xcptn.c:437–482` restores the saved instruction pointer, gives the registered NTVDM host exception hook a protected-mode handling opportunity, otherwise enters original vector 0 and continues CPU dispatch. The source documents `c_cpu_continue` as not returning; this audit does not dynamically verify that property, exception-frame shape, or hook lifetime. The host hook is the separately registered `MVDM-HOST-DIV-268` divergence; `div.c` itself remains unchanged.

This evidence does not establish arithmetic correctness across operand combinations, divisor normalization, quotient-overflow boundary, guest register preservation during fault, real versus protected-mode delivery, nested exceptions, or 64-bit divide helper behavior. In particular, no arithmetic oracle or faulting guest is run, and undefined flags should not be treated as portable x86 guarantees. A finite behavioral follow-up can compare representative and boundary 8/16/32-bit quotient/remainder with a native oracle and use an isolated guest to distinguish divide-by-zero from overflow while checking saved IP and unchanged destination registers; retain the current selected profile and original guest policy.

Replay `python artifacts/mvdm-audit/shard-1/ccpu-div-observer.py`. No product or guest execution, product source/guest edit, shared ledger mutation or build-tree modification occurred.

## Targeted current fixture

The current cached x86 fixture was rebuilt and executed successfully; its
[receipt](ccpu-div-fault-fixture.json) records exact input hashes and output.
Its synthetic CCPU instruction stream checks one real-mode 8-bit divide by
zero with a pending data breakpoint: the divide handler runs once and the
pending debug event is cancelled. This is stronger than source binding, but
it does not prove quotient-overflow, all widths, protected-mode delivery,
register preservation in every branch, or an immutable live guest workload.
The existing fixture link uses `/FORCE:MULTIPLE` and emits linker warnings;
this result applies to that fixture, not an independently linked production
image or a complete CPU validation.
