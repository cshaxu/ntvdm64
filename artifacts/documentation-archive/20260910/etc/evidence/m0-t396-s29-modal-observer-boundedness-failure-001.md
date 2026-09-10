# M0 T396 S29 — modal observer boundedness failure 001

## Intended scope

S29 admitted one user-equivalent response to the exact original
`illegal_op_int` error-panel control: a visible same-process `#32770` dialog
with ID 102 and text `&Ignore`.

## Result

The current observer preserves the panel predicate, but its implementation
deduplicates only the same HWND.  In the 120,297-ms run it reported:

```text
observer-modal-continue=enabled
observer-modal-continue-selections=4
loaded-module-count=57
loaded-wow32=no
```

Thus it issued four matching-panel commands, not the admitted globally single
user decision.  No additional run was made.  The source records only an
aggregate count, so it cannot prove that the four panel instances represent
one intended original continuation rather than repeated error handling.

## Disposition

This is a **test-carrier boundedness failure**, not evidence that the product
can or cannot advance through the original `illegal_op_int` continuation.
The run cannot support a CPU, BOP, error-panel, protected-mode, WOW32-load, or
acceptance conclusion.  The S29 stop condition has been reached.

Any successor requires explicit admission and must first replace the
per-HWND behavior with a build-local observer that has a process-lifetime
global selection budget of exactly one, records the matched panel identity,
and stops observing rather than posting a second command.  It may not change
product source, original dialog semantics, guest state, or BOP/CPU behavior.
