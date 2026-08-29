# M0 T310 S8 P4 C-video native-word shift closure

## Scope

This review covers the complete reached native-word bit-test form in the
selected C-video generated-rule package, restricted to:

- `mvdm-host/softpc.new/base/cvidc/SEVID019.c`
- `mvdm-host/softpc.new/base/cvidc/SEVID020.c`

The source files are byte-identical to the selected OpenNT source before the
registered minimal divergence.

## Source contract

Both rules store and test their condition through `IUH`, and both retain the
original guard `value >= 8*sizeof(IUH)`.  The prior expression used `1 <<
value`, leaving the left operand as a 32-bit `int`.  MSVC correctly reported
an x64-only C4334 because that 32-bit result was then consumed by the
native-word test.

`(IUH)1 << value` matches the existing `IUH` carrier and existing guard.  It
does not map a guest value, change a generated rule ID, add a wrapper, or
modify the rule's branch/result sequence.

## Verification

With warnings visible and no suppression, formal Ninja rebuilt the exact
original translation units successfully:

- x64: `obj/cvidc/SEVID019.obj`, `obj/cvidc/SEVID020.obj`
- x86: `obj/cvidc/SEVID019.obj`, `obj/cvidc/SEVID020.obj`

Both commands exited zero.  Their target-local logs contain no C4334 for
these sources.  Remaining generated C4702/C4715 diagnostics remain visible;
they are source-control-flow diagnostics, not evidence for changing this
native-word ABI repair.

## Disposition

`MVDM-HOST-DIV-123` is a two-site mirror divergence.  No overlay or adapter
is introduced.
