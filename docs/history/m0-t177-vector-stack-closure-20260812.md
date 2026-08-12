# M0 T177 vector/stack attribution closure - 2026-08-12

## Outcome

T177 closes the direct-entry vector/stack question without a repair. T177 S1
proved that the earlier stack path followed a zero captured INT10 vector in a
direct pre-POST image. T177 S2 proved that the later machine-composition image
also retained direct startup and therefore could not test that predecessor.

T177 S3 admitted one bounded composition verification. T177 S4 built the
existing native-POST/machine-composition form with exactly two Bochs object
replacements and observed it once. The resulting native flow reached normal
NTIO/COMMAND behavior and `50:36`, with zero selector-`06h` and zero machine
composition outcomes. The direct aperture/stack failure was absent.

## Boundary result

- Bochs owns reset, POST, IVT and real-mode delivery.
- The adapter owns no vector synthesis, device operation, or machine BOP
  semantics.
- The original SoftPC `illegal_op_int` component remains linked but unobserved;
  no reachability repair is admitted.
- The native process exit is not a DOS or CLI terminal result.

The retained file `history/m0-t177-closure-20260812.md` predates this admitted
packet and remains an indexed historical locator; it is not overwritten or
reinterpreted by this closure.

## Transfer

The next high-value question is whether the source-owned `50:36` guest
transfer can ever reach a normal DOS-return signal suitable for a fixed-width,
engine-owned CLI result. That work must first attribute the original
continuation and failure/unavailable paths; it must not overload the Bochs
exit code or revive machine-BOP probing.
