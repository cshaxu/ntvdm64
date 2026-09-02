# M0 T379 closure — Original VDMREDIR runtime product activation

## Closed result

T379 closes the original VDMREDIR product-activation package.

- S1 fixed the complete original source/export/import owner matrix and the
  immutable `REDIR.EXE` workload.
- S2 built the original `VDMREDIR.dll` through its original `.def` and kept
  its parent ABI as an import from the one CPU40/SoftPC process, never as a
  static Redirector provider or second machine.
- S3 closed bounded local handle, pipe, completion, cancellation and stale
  lifecycle behavior. Remote RAP/XACTSRV/private RPC routes retain their
  original unavailable disposition.
- S4 restored the parent module's original staged identity `ntvdm.exe`, put
  the original DLL beside it as hash-verified runtime companion, and verified
  the original dynamic-loader/export/failure contract.
- S5 made one bounded, non-debug frozen `REDIR.EXE` observation. The original
  guest executable was opened and the parent exited zero.

## Exact limit

The one run did not emit `MS_bop_7` or `57:xx`. It consequently does not prove
that `LoadVdmRedir`, a DLL export call, a named-pipe transaction or a remote
Redirector service executed. The package proves product composition and the
selected local workload's original file-open/return path only. No
trace-derived repair is admitted by this closure.

## Evidence

- [S1 admission](../etc/evidence/m0-t379-s1-original-vdmredir-product-admission-001.md)
- [S2 original DLL and parent loader](../etc/evidence/m0-t379-s2-original-vdmredir-dll-parent-loader-closure-001.md)
- [S3 local resource lifecycle](../etc/evidence/m0-t379-s3-local-resource-lifecycle-closure-001.md)
- [S4 formal activation matrix](../etc/evidence/m0-t379-s4-formal-vdmredir-activation-matrix-001.md)
- [S5 frozen observation](../etc/evidence/m0-t379-s5-frozen-original-redir-observation-001.md)
