# T484 S3 P208: native product-object netlib contract

The maintained packstr test now uses tools/build-fixtures.cmd x86 netlib-packstr.
Its target belongs to the existing worker object-contract module and links the
unchanged test/opennt-host/netlib/packstr.c with the actual product-selected
obj/opennt-netlib/packstr.obj. It neither copies packing algorithms nor compiles
a different source subset. Dependency tracking includes that exact object.
The test runner no longer accepts a compiler script or arbitrary worker object
root; runtime output remains a fresh owned build directory.

Owned PID20224 runs build/t484-s3-netlib-contract/run.ps1. Actual fixture output
passes exact fit, overflow without writes, NULL terminator and no-room behavior.
The executable has an explicit10-second owned-process timeout. Its object is
built with the existing /MT x86 ABI. No product, original guest or mirror change.
The same delivery run requires cached full host build and11 raw/22 dual-published
image identity checks; identical publication is verified rather than overwritten.
This is an object-level fixture, not a fresh guest or Full runtime acceptance.
Documentation, parser and staged diff checks precede commit/push.

Three direct-command candidates remain: CCPU event consumption, modesw guards
and the old Run16 entry audit. Frozen316/355 and23 supplemental preparation/
staging chains are unchanged. S3 remains active and S4/S5 are not complete.
