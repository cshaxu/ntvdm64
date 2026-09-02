# M0 T362 — Original COMMAND built-in/batch lifecycle closure

T362 closes by its declared dependency-first rule, not by claiming that a
COMMAND built-in, batch file, DOS child, native child or Win16 program ran.

## Delivered source recovery

- `cmdComSpec` now reads the original guest `DS:DX` COMSPEC text through one
  bounded session guest-memory lease while retaining the original prefix,
  length and `AL` contract.
- `cmdGetInitEnvironment` preserves the original environment construction and
  capacity gate, then writes its unchanged bytes through one bounded `ES:0`
  lease.
- `cmdGetNextCmd` snapshots/commits its original `CMDINFO` fields and performs
  command/environment transfers through short leases only.  No guest pointer
  survives the Base VDM call.
- The selected original COMMAND library and full CPU40/x86 product graph link
  successfully.  The source/build result is recorded in
  [T362 S2](../etc/evidence/m0-t362-s2-first-call-guest-memory-cohort-closure-001.md).

## Frozen runtime result

The one permitted fixed-container `/C EXIT` observation reached original
`54:05`, but not `54:01` or `54:00`.  Its complete map/source attribution is
recorded in [T362 S3](../etc/evidence/m0-t362-s3-frozen-command-observation-001.md):
the stopped execution stack is the original CCPU40
`c_IOVirtualised -> inb -> printer_io` conventional-device path.

## Disposition

This is not evidence that T362's COMMAND mapping changes caused the timeout.
It is one exact earlier source owner, so no COMMAND BOP leaf, guest change,
or second observation was admitted.  The queue now prioritizes the complete
SoftPC conventional-device/DOS workload package.  Once that package has a
source-shaped runtime closure, a new COMMAND lifecycle packet may resume from
the frozen `54:05` boundary.
