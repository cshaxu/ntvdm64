# M0 T383 closure — original COMMAND startup command-delivery recovery

T383 closed on 2026-09-02.

The selected original COMMAND/BaseClient/local-BaseSrv first-command cohort
was already complete in current source.  Its current x86 focused contract
build passed, proving first-record delivery, capacity/retry direction and
one-shot consumption.  The one admitted fixed pure-DOS `EXIT` observation did
not reach `54:01`; it instead reached the earlier original
`illegal_op_int -> host_error -> ErrorDialogBox` path after the second
`54:0F` return.  The fault bytes were `63 6f 64 65 78` at `03f4:2128`.

No provider, media, CPU or command-execution behavior was changed.  The
dependency-first successor is the CPU40 illegal-operation/error-path package;
T381 `cmdExec32` remains blocked until pure-DOS startup clears that owner.

Evidence: [S1/S2 owner ledger](../etc/evidence/m0-t383-s1-original-command-startup-owner-ledger-001.md),
[S3 formal proof](../etc/evidence/m0-t383-s3-original-command-delivery-formal-closure-001.md), and
[S4 fixed observation](../etc/evidence/m0-t383-s4-fixed-pure-dos-startup-observation-001.md).
