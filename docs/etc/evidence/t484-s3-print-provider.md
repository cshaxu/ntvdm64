# T484 S3 P113 native print provider producer

B0185 now uses the existing CMD/native fixture build. Retain the unchanged
C probe and runtime assertions: Escape SETABORTPROC registration, initiating
thread/TLS/DC identity, normal PDF output, drawing-phase cancellation,
EndPage rejection and EndDoc1 after cancellation, and no cancelled output.
This distinct current native provider contract justifies retention despite
having no current suite caller; migration effort alone does not.

The prior manual CL producer and CompilerDriver input are removed. The
native target retains /MT /O2 /W4 /WX and gdi32. No guest execution, foreground
operation, default/physical printer, dialog or new service double is involved.
The explicit Microsoft Print to PDF output stays in this test's directory.
The successful path and original return semantics remain documented in
[T447 provider evidence](m0-t447-s2-scope-freeze.md).

Owned PID18620 exits0:11 callbacks, foreign0/outside0/cancellations1; normal
output has a PDF signature, cancelled output is absent, and the queue is empty.
The complete cached host build, hash/mtime-stable target and x64 rejection2
pass. Eleven raw host inputs and22 published copies retain manifest hashes.
Existing publication verified, not republished. No product, mirror, original
guest or asset change. This native provider result is not a real WOW guest
printing acceptance claim.

[Receipt](../../../artifacts/build-migration/t484/s3-print-provider/result.json)
records actual run, cache, output and queue identities. Raw logs:
build/t484-s3-print-provider. Inventory215 complete/140 remaining of355.
Next ordered item B0186. S3 retrospective review, B0166/B0169 reconciliation
and S4/S5 remain incomplete.
