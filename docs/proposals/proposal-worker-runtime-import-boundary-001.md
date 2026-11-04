# Proposal — separate worker runtime imports

The owner directs this as a separate task from T475 S3, which addresses source,
header and static-link ownership. This candidate allocates no numeric task and
does not admit an implementation or reorder other queue entries.

WOW32 and VDMREDIR still import NTVDM executable entry points through their PE
tables. Audit the actual import names and ordinals at admission, distinguish
original external ABI obligations, and design a provider or binding that
removes these component-to-executable runtime imports while preserving a single
session, guest-memory and WOW state owner. Compare a shared runtime module with
an explicitly passed function table, including loader and CRT initialization,
x86 calling conventions, callback reentrancy, task restart, teardown and
failure cleanup. Do not choose either approach without evidence that it works
before all imported functions and DLL initialization can be reached.

Demonstrate the chosen boundary using actual PE import inspection, positive,
negative and lifecycle tests, real guest execution and coherent publication.
Retain any NTVDM exports still required by original or external consumers.
The [S3 design](../etc/operations/m0-t475-s3-worker-ntvdm-dependency-plan.md)
and [evidence](../etc/evidence/m0-t475-s3-worker-ntvdm-remediation.md) retain
the baseline and earlier ownership work.
