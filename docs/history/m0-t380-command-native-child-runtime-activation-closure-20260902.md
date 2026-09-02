# M0 T380 closure — COMMAND native-child runtime activation

## Closed result

T380 closes the bounded COMMAND native-child activation package.

- S1 re-established the complete original `54:07 -> /z -> 54:08/0A/0B`
  ownership and selected one public child workload.
- S2 confirmed the source-shaped original child lifecycle locally: creation,
  standard-stream endpoint binding, completion, cancellation, cleanup and
  re-entry balance.
- S3 confirmed the current CPU40/x86 formal product contains the original
  COMMAND archive and same-shaped Base VDM, SoftPC/Win32 and session bindings.
- S4 made the one permitted fixed observation.  It reached original `54:07`
  and original `54:08` stage 0 after DEM successfully opened `CMD.EXE`.

## Exact limit

The frozen observation timed out in original `54:08` before a return,
`54:0A`, `54:0B`, child completion or exit-37 marker.  Consequently this task
does not claim that the selected native child actually started, that it
returned to the guest, or that DOS PSP parent return is complete.

The exact remaining owner is not an app launcher or BOP leaf: it is the
original COMMAND `cmdExec`/`cmdExec32` stage-0-to-return lifecycle and its
original Base VDM re-entry interval.  Any later work must recover that cohort
as one owner package, rather than derive a patch from this trace.

## Evidence

- [S1 owner and predecessor rebaseline](../etc/evidence/m0-t380-s1-command-native-child-owner-predecessor-rebaseline-001.md)
- [S2 lifecycle confirmation](../etc/evidence/m0-t380-s2-command-native-child-lifecycle-confirmation-001.md)
- [S3 formal product confirmation](../etc/evidence/m0-t380-s3-command-native-child-formal-product-001.md)
- [S4 frozen observation](../etc/evidence/m0-t380-s4-frozen-native-child-observation-001.md)
