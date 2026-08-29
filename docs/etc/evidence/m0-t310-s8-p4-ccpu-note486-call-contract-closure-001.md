# M0 T310 S8 P4 — CCPU `note_486_instruction` call-contract closure

## Original contract

`softpc.new/base/ccpu386/popf.c` calls `note_486_instruction(buf)` after a
`POPFD` attempts to set reserved flag bits. The selected original
`c_main.c` defines that same-package routine as `VOID note_486_instruction`
with one `char *` argument. Its observable role is diagnostic only: it
optionally traces or enters Yoda according to the original environment policy;
it does not return a CPU value or alter the `POPFD` state transition.

All available OpenNT source baselines retain this definition and reached
caller. The source issue was the historical implicit declaration in `popf.c`,
which modern C interpreted as an `int` result on both x86 and x64.

## Minimal recovery

`popf.c` now declares the original same-package `void(char *)` contract next
to its existing include set. No function body, CPU state, 486 diagnostic
policy, environment behavior, adapter, mapping-manager boundary, or warning
suppression was added.

The mirror exception is registered as `MVDM-HOST-DIV-126` in
`src/mvdm-host/README.md`.

## Verification

The formal MSVC `/MT` Ninja graphs rebuilt the reached original object on both
targets:

```text
ninja -C build/M0-T310/S8/p1-machine-source/x64 obj/ccpu/popf.obj
ninja -C build/M0-T310/S8/p1-machine-source/x86 obj/ccpu/popf.obj
```

Both completed with no `popf.c` C4013 `note_486_instruction` diagnostic and
no compiler error. Other CCPU diagnostics remain visible for their owner
contract review.

## Scope boundary

This does not close the separate `EDL_fast_bop` reference in `c_main.c`.
That reference has no definition in the available OpenNT source baselines and
belongs to the distinct CCPU-to-machine/BOP extended-operation contract; it
must not be replaced by a local diagnostic declaration or a fabricated BOP
provider.
